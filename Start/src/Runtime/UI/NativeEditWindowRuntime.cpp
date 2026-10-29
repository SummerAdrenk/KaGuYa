#include <Runtime/NativeEditWindowRuntime.h>

#include <Platform/PendingWindowCreation.h>
#include <Platform/WindowCreation.h>
#include <Platform/WindowHost.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

#include <bit>
#include <cstring>
#include <mbctype.h>
#include <new>
#include <stdexcept>
#include <vector>

namespace KaGuYa::Start::Runtime
{
namespace
{
LONG Add32(LONG left, LONG right) noexcept
{
    return std::bit_cast<LONG>(std::bit_cast<DWORD>(left) + std::bit_cast<DWORD>(right));
}

LONG Sub32(LONG left, LONG right) noexcept
{
    return std::bit_cast<LONG>(std::bit_cast<DWORD>(left) - std::bit_cast<DWORD>(right));
}

LOGFONTA EditFontAt48CF30(LONG height)
{
    LOGFONTA font{};
    font.lfHeight = Sub32(0, height);
    font.lfWeight = 700;
    font.lfCharSet = 0x80;
    font.lfQuality = 2;
    font.lfPitchAndFamily = 0x30;
    // 原PE 5ADC0C：全角ＭＳ ゴシック的CP932字节，不依赖宿主源码编码。
    constexpr char face[] = "\x82\x6C\x82\x72\x20\x83\x53\x83\x56\x83\x62\x83\x4E";
    std::memcpy(font.lfFaceName, face, sizeof(face));
    return font;
}

std::vector<char> AllocateCStringBytes(std::size_t count)
{
    // 4314F2/40B4F8 -> 438AC0以HRESULT抛出CString分配失败，不改变其他异常。
    try
    {
        return std::vector<char>(count, '\0');
    }
    catch (const std::bad_alloc&)
    {
        throw std::bit_cast<std::int32_t>(0x8007000Eu);
    }
    catch (const std::length_error&)
    {
        throw std::bit_cast<std::int32_t>(0x8007000Eu);
    }
}

// 469F70是先清旧引用再分配新正文/控制块，不是唯一引用时原地写的COW。
// 分开保存base与control，保留控制块分配抛出前base已经发布的原中间态。
template<class T>
struct NativeOwnedReference
{
    struct Control final : YtlReferenceControlRuntime
    {
        explicit Control(T* value) : payload(value) {}
        void DestroyPayloadAtVslot00() override { delete payload; }
        void DeleteControlAtVslot04(std::uint32_t flags) override
        {
            if ((flags & 1u) != 0) delete this;
        }
        std::uint32_t strideAt0C = sizeof(T);
        std::uint32_t countAt10 = 1;
        T* payload;
    };
    static_assert(sizeof(Control) == 0x18);
    T* base = nullptr;
    Control* control = nullptr;

    NativeOwnedReference() = default;
    NativeOwnedReference(const NativeOwnedReference&) = delete;
    NativeOwnedReference& operator=(const NativeOwnedReference&) = delete;
    ~NativeOwnedReference() noexcept { Clear(); }

    void Clear() noexcept
    {
        if (control == nullptr)
        {
            return;
        }
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08 != 0) control->DestroyPayloadAtVslot00();
            if (control != nullptr) control->DeleteControlAtVslot04(1);
        }
        control = nullptr;
        base = nullptr;
    }

    void AllocateNew()
    {
        Clear();
        T* created = new T{};
        // 469FDC..469FE7：正文分配后重读槽；重入发布的control只删控制块，不再减计数。
        if (control != nullptr) control->DeleteControlAtVslot04(1);
        base = created;
        // 46A007/50D4BA用本轮保存在EDI的正文，不能在控制块分配后重读base。
        control = new Control(created);
    }
};

struct NativeBrush
{
    HBRUSH handle = nullptr;
    ~NativeBrush() noexcept
    {
        if (handle != nullptr)
        {
            DeleteObject(handle);
        }
    }
};
}

struct NativeEditWindowRuntime::State
{
    struct Endpoint
    {
        State& state;
        bool child;
        HWND window = nullptr;
        WNDPROC previous = DefWindowProcA;
        Platform::ExecutableWindowThunk thunk;
        Platform::PendingWindowCreateRecord record{};
        struct MessageContext { MessageContext* previous; };
        MessageContext* current = nullptr;
        bool destroyPending = false;

        Endpoint(State& stateValue, bool isChild) : state(stateValue), child(isChild) {}
        ~Endpoint() noexcept
        {
            // 原失败分支保留节点；仅现代对象结束寿命时防止全局链悬空。
            if (record.object != nullptr)
                Platform::PendingWindowCreateRecords().Remove(record);
        }

        static LRESULT CALLBACK Initial(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            auto* endpoint = static_cast<Endpoint*>(
                Platform::PendingWindowCreateRecords().PopForCurrentThread());
            if (endpoint == nullptr)
            {
                return 0;
            }
            endpoint->window = hwnd;
            // 创建前已申请thunk；首次消息按48E090重新写入对象与实际窗口过程。
            // 原首次过程没有分配失败退路，不能替换为Initialize的额外Raise策略。
            static_cast<void>(endpoint->thunk.TryInitialize(endpoint, reinterpret_cast<std::uintptr_t>(&Bound)));
            const WNDPROC procedure = endpoint->thunk.Procedure();
            SetWindowLongA(hwnd, GWL_WNDPROC,
                static_cast<LONG>(reinterpret_cast<LONG_PTR>(procedure)));
            return procedure(hwnd, message, wParam, lParam);
        }

        static LRESULT CALLBACK Bound(Endpoint* endpoint, UINT message, WPARAM wParam, LPARAM lParam)
        {
            return endpoint->Dispatch(message, wParam, lParam);
        }

        static Platform::WindowClassRegistrar& Registrar(bool child)
        {
            if (child)
            {
                static Platform::WindowClassRegistrar edit([] {
                    Platform::WindowClassProfile profile;
                    profile.style = 0x0B;
                    profile.windowProcedure = &Initial;
                    profile.superclassName = "EDIT";
                    return profile;
                }());
                return edit;
            }
            static Platform::WindowClassRegistrar outer([] {
                Platform::WindowClassProfile profile;
                profile.style = 0x0B;
                profile.windowProcedure = &Initial;
                profile.backgroundBrush = reinterpret_cast<HBRUSH>(6);
                profile.cursorResource = MAKEINTRESOURCEA(32512);
                profile.cursorIsSystemResource = true;
                return profile;
            }());
            return outer;
        }

        void Create(DWORD style, HWND parent)
        {
            const HINSTANCE module = GetModuleHandleA(nullptr);
            const ATOM atom = Registrar(child).Register(module, module, &previous);
            // 46FC13/48566A：申请失败只写ERROR_OUTOFMEMORY，不Raise，也不提前删对象。
            if (!thunk.TryInitialize(nullptr, 0))
            {
                SetLastError(ERROR_OUTOFMEMORY);
                return;
            }
            if (atom == 0)
            {
                return;
            }
            record.object = this;
            record.threadId = GetCurrentThreadId();
            Platform::PendingWindowCreateRecords().Push(record);
            // 601054 RECT为{INT_MIN,INT_MIN,0,0}，原减法的宽高bitpattern也是INT_MIN。
            static_cast<void>(CreateWindowExA(0, MAKEINTATOM(atom), nullptr, style,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                parent, reinterpret_cast<HMENU>(child ? static_cast<void*>(this) :
                    static_cast<void*>(&state.owner)), module, nullptr));
            // 原返回HWND只由Initial写入。失败不自动Remove未消费创建记录。
        }

        LRESULT Forward(UINT message, WPARAM wParam, LPARAM lParam)
        {
            return CallWindowProcA(previous, window, message, wParam, lParam);
        }

        LRESULT Dispatch(UINT message, WPARAM wParam, LPARAM lParam)
        {
            MessageContext context{current};
            current = &context;
            LRESULT result = 0;
            const bool handled = child
                ? state.DispatchControl(message, wParam, lParam, result)
                : state.DispatchWindow(message, wParam, lParam, result);
            if (!handled)
            {
                if (message == WM_NCDESTROY)
                {
                    // 439FF1/48F0D1保存调用原过程之前的WNDPROC，不能改为与thunk比较。
                    const LONG before = GetWindowLongA(window, GWL_WNDPROC);
                    result = Forward(message, wParam, lParam);
                    if (previous != DefWindowProcA && GetWindowLongA(window, GWL_WNDPROC) == before)
                    {
                        SetWindowLongA(window, GWL_WNDPROC,
                            static_cast<LONG>(reinterpret_cast<LONG_PTR>(previous)));
                    }
                    destroyPending = true;
                }
                else
                {
                    result = Forward(message, wParam, lParam);
                }
            }
            if (destroyPending && context.previous == nullptr)
            {
                destroyPending = false;
                window = nullptr;
                current = nullptr;
                // 两个原+0C均为retn4；不在此处删除root集合引用。
            }
            else
            {
                current = context.previous;
            }
            return result;
        }
    };

    NativeEditWindowRuntime& owner;
    explicit State(NativeEditWindowRuntime& value) : owner(value) {}
    Endpoint outer{*this, false};
    Endpoint edit{*this, true};
    std::optional<std::uint32_t> eventAt48;
    RECT logical{};
    RECT defaultMargins{1, 2, 1, 2};
    NativeOwnedReference<RECT> margins;
    std::optional<std::uint32_t> identifier;
    std::int32_t byteLimit = 0;
    std::uint32_t alignment = 0;
    COLORREF textColor = 0;
    COLORREF backgroundColor = 0x00FFFFFF;
    NativeOwnedReference<NativeBrush> brush;

    ~State() noexcept
    {
        // 468810：DestroyWindow先于brush、边距Ref和子/父thunk释放。
        if (outer.window != nullptr)
        {
            DestroyWindow(outer.window);
        }
        brush.Clear();
        margins.Clear();
        // 成员逆序析构最终为edit thunk、outer thunk；不追加原本没有的字体Delete。
    }

    bool DispatchControl(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        if (message == WM_KEYDOWN)
        {
            if (wParam == VK_TAB) eventAt48 = 3;
            else if (wParam == VK_RETURN) eventAt48 = 1;
            else if (wParam == VK_ESCAPE) eventAt48 = 2;
            result = edit.Forward(message, wParam, lParam);
            return true;
        }
        if (message != WM_SIZE)
        {
            return false;
        }
        RECT rectangle{};
        if (GetWindowRect(edit.window, &rectangle))
        {
            const LONG height = Sub32(rectangle.bottom, rectangle.top);
            if (height != 0)
            {
                const HFONT oldFont = reinterpret_cast<HFONT>(SendMessageA(edit.window, WM_GETFONT, 0, 0));
                bool create = oldFont == nullptr;
                if (oldFont != nullptr)
                {
                    LOGFONTA oldDescription;
                    if (GetObjectA(oldFont, static_cast<int>(sizeof(oldDescription)), &oldDescription) ==
                        static_cast<int>(sizeof(oldDescription)) &&
                        oldDescription.lfHeight != height)
                    {
                        DeleteObject(oldFont);
                        create = true;
                    }
                }
                if (create)
                {
                    const LOGFONTA description = EditFontAt48CF30(height);
                    const HFONT font = CreateFontIndirectA(&description);
                    SendMessageA(edit.window, WM_SETFONT, reinterpret_cast<WPARAM>(font), 1);
                }
            }
        }
        result = 0;
        return true;
    }

    void ApplyByteLimitAt4824A0(WPARAM wParam)
    {
        if (!identifier.has_value())
        {
            throw std::logic_error("CEditWindow +78 has not been published by WM_CREATE");
        }
        const auto bits = static_cast<std::uint32_t>(wParam);
        if ((bits & 0xFFFFu) != *identifier || (bits & 0xFFFF0000u) != 0x03000000u || byteLimit <= 0)
        {
            return;
        }
        const int requested = GetWindowTextLengthA(edit.window);
        if (requested < 0)
        {
            throw std::bit_cast<std::int32_t>(0x80070057u);
        }
        auto text = AllocateCStringBytes(static_cast<std::size_t>(requested) + 1);
        GetWindowTextA(edit.window, text.data(), Add32(requested, 1));
        const auto length = static_cast<std::int32_t>(std::strlen(text.data()));
        if (length <= byteLimit)
        {
            return;
        }
        const auto selection = static_cast<std::uint32_t>(SendMessageA(edit.window, EM_GETSEL, 0, 0)) & 0xFFFFu;
        std::int32_t position = 0;
        std::int32_t previous = 0;
        for (;;)
        {
            const auto saved = previous;
            if (position < 0 || position > length)
            {
                throw std::bit_cast<std::int32_t>(0x80070057u);
            }
            // 原4825D4为movsx，保持当前CRT多字节locale，不改成硬编码CP932判定。
            const int byte = static_cast<signed char>(text[static_cast<std::size_t>(position)]);
            if (_ismbblead(static_cast<unsigned int>(byte)) != 0)
            {
                ++position;
            }
            const auto beforeIncrement = position;
            ++position;
            if (beforeIncrement >= byteLimit)
            {
                position = saved;
                break;
            }
            previous = position;
            if (position >= byteLimit)
            {
                break;
            }
        }
        // 419D00产生独立CString正文，不能在首个读取缓冲中原地截断后冒充第二次分配。
        auto prefix = AllocateCStringBytes(static_cast<std::size_t>(position) + 1);
        std::memcpy(prefix.data(), text.data(), static_cast<std::size_t>(position));
        SetWindowTextA(edit.window, prefix.data());
        SendMessageA(edit.window, EM_SETSEL, selection, selection);
        SendMessageA(edit.window, EM_SCROLLCARET, 0, 0);
    }

    bool DispatchWindow(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        switch (message)
        {
        case WM_CREATE:
        {
            DWORD style = 0x50000080u;
            if (alignment == 1) style = 0x50000081u;
            else if (alignment == 2) style = 0x50000082u;
            edit.Create(style, outer.window);
            identifier = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(GetMenu(edit.window)) & 0xFFFFu);
            result = 0;
            return true;
        }
        case WM_SIZE:
        {
            RECT rectangle{};
            if (edit.window != nullptr && GetClientRect(outer.window, &rectangle))
            {
                const RECT inset = margins.base != nullptr ? *margins.base : defaultMargins;
                MoveWindow(edit.window, Add32(rectangle.left, inset.left), Add32(rectangle.top, inset.top),
                    Sub32(Sub32(Sub32(rectangle.right, rectangle.left), inset.right), inset.left),
                    Sub32(Sub32(Sub32(rectangle.bottom, rectangle.top), inset.bottom), inset.top), TRUE);
            }
            result = 0;
            return true;
        }
        case WM_ERASEBKGND:
        {
            RECT rectangle{};
            if (brush.base != nullptr && GetClientRect(outer.window, &rectangle))
            {
                const HDC dc = reinterpret_cast<HDC>(wParam);
                const HGDIOBJ previous = SelectObject(dc, brush.base->handle);
                PatBlt(dc, rectangle.left, rectangle.top, Sub32(rectangle.right, rectangle.left),
                    Sub32(rectangle.bottom, rectangle.top), PATCOPY);
                SelectObject(dc, previous);
                result = 1;
            }
            else
            {
                result = outer.Forward(message, wParam, lParam);
            }
            return true;
        }
        case WM_CTLCOLOREDIT:
            if (brush.base != nullptr)
            {
                const HDC dc = reinterpret_cast<HDC>(wParam);
                SetTextColor(dc, textColor);
                SetBkMode(dc, TRANSPARENT);
                result = reinterpret_cast<LRESULT>(brush.base->handle);
            }
            else
            {
                result = outer.Forward(message, wParam, lParam);
            }
            return true;
        case WM_COMMAND:
            ApplyByteLimitAt4824A0(wParam);
            result = 0;
            return true;
        default:
            return false;
        }
    }
};

NativeEditWindowRuntime::NativeEditWindowRuntime() : state_(std::make_unique<State>(*this)) {}
NativeEditWindowRuntime::~NativeEditWindowRuntime() noexcept = default;

std::shared_ptr<NativeEditWindowRuntime> NativeEditWindowRuntime::CreateAt46FA30(
    std::uint32_t disableDword, std::uint32_t alignmentDword,
    std::uint32_t borderFlagDword, HWND environmentParent)
{
    // 独立直测入口；collection通过friend先建立自己的桥接control，再调用CreateWindows。
    auto object = std::shared_ptr<NativeEditWindowRuntime>(new NativeEditWindowRuntime());
    object->CreateWindows(disableDword, alignmentDword, borderFlagDword, environmentParent);
    return object;
}

void NativeEditWindowRuntime::CreateWindows(std::uint32_t disableDword,
    std::uint32_t alignmentDword, std::uint32_t borderFlagDword, HWND environmentParent)
{
    state_->alignment = alignmentDword;
    state_->outer.Create(borderFlagDword != 0 ? 0x40800000u : 0x40000000u, environmentParent);
    EnableWindow(Window(), disableDword == 0);
}

HWND NativeEditWindowRuntime::Window() const noexcept { return state_->outer.window; }
HWND NativeEditWindowRuntime::EditControlWindow() const noexcept { return state_->edit.window; }
RECT& NativeEditWindowRuntime::LogicalRectAt4C() noexcept { return state_->logical; }
const RECT& NativeEditWindowRuntime::LogicalRectAt4C() const noexcept { return state_->logical; }
RECT& NativeEditWindowRuntime::DefaultMarginsAt5C() noexcept { return state_->defaultMargins; }
const RECT& NativeEditWindowRuntime::DefaultMarginsAt5C() const noexcept { return state_->defaultMargins; }
const RECT* NativeEditWindowRuntime::AppliedMarginsAt6C() const noexcept { return state_->margins.base; }
std::optional<std::uint32_t>& NativeEditWindowRuntime::EventAt48() noexcept { return state_->eventAt48; }
const std::optional<std::uint32_t>& NativeEditWindowRuntime::EventAt48() const noexcept { return state_->eventAt48; }
std::optional<std::uint32_t> NativeEditWindowRuntime::ControlIdentifierAt78() const noexcept { return state_->identifier; }
std::int32_t& NativeEditWindowRuntime::TextByteLimitAt7C() noexcept { return state_->byteLimit; }
COLORREF& NativeEditWindowRuntime::TextColorAt84() noexcept { return state_->textColor; }
COLORREF NativeEditWindowRuntime::BackgroundColorAt88() const noexcept { return state_->backgroundColor; }

void NativeEditWindowRuntime::ApplyLayoutAt4880F0(RECT target, const RECT* margins)
{
    RECT current{};
    if (!GetWindowRect(Window(), &current) || EqualRect(&current, &target))
    {
        return;
    }
    if (margins == nullptr)
    {
        state_->margins.Clear();
    }
    else
    {
        state_->margins.AllocateNew();
        state_->margins.base->left = margins->left;
        state_->margins.base->right = margins->right;
        state_->margins.base->top = margins->top;
        state_->margins.base->bottom = margins->bottom;
    }
    MoveWindow(Window(), target.left, target.top, Sub32(target.right, target.left),
        Sub32(target.bottom, target.top), TRUE);
}

void NativeEditWindowRuntime::SetBackgroundColor(COLORREF color)
{
    auto& brush = state_->brush;
    if (brush.base == nullptr)
    {
        brush.AllocateNew();
    }
    if (brush.base->handle != nullptr && DeleteObject(brush.base->handle))
    {
        brush.base->handle = nullptr;
    }
    brush.base->handle = CreateSolidBrush(color);
    state_->backgroundColor = color;
}
}
