#include "GraphicsState.h"
#include "GdiBackendState.h"

#include "RenderDxBridge.h"
#include "RenderGlBridge.h"
#include "Surface.h"
#include "TextRenderer.h"
#include <Logging/Logger.h>

#include <algorithm>
#include <bit>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Graphics
{
Runtime& GetRuntime() noexcept
{
    static Runtime runtime;
    return runtime;
}

SurfaceInfo* Runtime::Screen() const noexcept
{
    return renderer ? renderer->Screen() : nullptr;
}

RenderDxBridge* Runtime::Dx() const noexcept
{ return renderer && renderer->Kind() == 1 ? static_cast<RenderDxBridge*>(renderer.get()) : nullptr; }
RenderGlBridge* Runtime::Gl() const noexcept
{ return renderer && renderer->Kind() == 2 ? static_cast<RenderGlBridge*>(renderer.get()) : nullptr; }
GdiBackendState* Runtime::Gdi() const noexcept
{ return renderer && renderer->Kind() == 0 ? static_cast<GdiBackendState*>(renderer.get()) : nullptr; }
RendererReference<DirtyRectState>& Runtime::Dirty() const
{
    if (!renderer) throw std::logic_error("Graphics renderer is absent");
    return renderer->Dirty();
}

namespace
{
// 原10050348/10050390，GetErrorMessage保留CP932原字节，不按现代变量名调换两条错误。
constexpr char GdiDrawFailure[] =
    "\x83\x76\x83\x89\x83\x43\x83\x7D\x83\x8A\x83\x54\x81\x5B\x83\x74\x83\x46\x83\x43\x83\x58"
    "\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73\x81\x49 : [CRenderGDI - Initialize]";
constexpr char GdiPrimaryFailure[] =
    "\x83\x5A\x83\x4A\x83\x93\x83\x5F\x83\x8A\x83\x54\x81\x5B\x83\x74\x83\x46\x83\x43\x83\x58"
    "\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73\x81\x49 : [CRenderGDI - Initialize]";

int AddDword(int left, int right) noexcept
{ return std::bit_cast<int>(static_cast<std::uint32_t>(left) + static_cast<std::uint32_t>(right)); }
int SubDword(int left, int right) noexcept
{ return std::bit_cast<int>(static_cast<std::uint32_t>(left) - static_cast<std::uint32_t>(right)); }

int RectangleArea(const RECT& rect) noexcept
{
    const auto width = static_cast<std::uint32_t>(rect.right) - static_cast<std::uint32_t>(rect.left);
    const auto height = static_cast<std::uint32_t>(rect.bottom) - static_cast<std::uint32_t>(rect.top);
    return std::bit_cast<int>(width * height);
}

int MergePenalty(int area, int first, int second) noexcept
{
    return std::bit_cast<int>(static_cast<std::uint32_t>(area) -
        static_cast<std::uint32_t>(first) - static_cast<std::uint32_t>(second));
}

void RequireDirtyState(const Runtime& runtime)
{
    if (!runtime.Dirty()) throw std::logic_error("Graphics dirty rectangle owner is not initialized");
}

void MergeDirtyRectangles(Runtime& runtime)
{
    RequireDirtyState(runtime);
    // 原100074D0：包围面积不超过两面积之和才合并，以末项填洞并继续检查当前j。
    // IMUL/SUB保留低DWORD，最终按有符号值比较，不引入C++有符号溢出。
    for (int i = 0; i + 1 < runtime.Dirty()->count; ++i)
    {
        for (int j = i + 1; j < runtime.Dirty()->count;)
        {
            const auto& first = runtime.Dirty()->rectangles[i];
            const auto& second = runtime.Dirty()->rectangles[j];
            const RECT united{(std::min)(first.left, second.left), (std::min)(first.top, second.top),
                (std::max)(first.right, second.right), (std::max)(first.bottom, second.bottom)};
            const int area = RectangleArea(united);
            if (MergePenalty(area, runtime.Dirty()->areas[i], runtime.Dirty()->areas[j]) > 0)
            {
                ++j;
                continue;
            }
            runtime.Dirty()->rectangles[i] = united;
            runtime.Dirty()->areas[i] = area;
            --runtime.Dirty()->count;
            if (j != runtime.Dirty()->count)
            {
                runtime.Dirty()->rectangles[j] = runtime.Dirty()->rectangles[runtime.Dirty()->count];
                runtime.Dirty()->areas[j] = runtime.Dirty()->areas[runtime.Dirty()->count];
            }
        }
    }
}

void CloseBackend(Runtime& runtime)
{
    // 10002480 / 10001450仅关闭primary/draw或模块，不触及manager和文字。
    if (auto* dx = runtime.Dx()) dx->Shutdown();
    else if (auto* gl = runtime.Gl()) gl->Shutdown();
    else if (auto* gdi = runtime.Gdi()) gdi->Close();
    runtime.initialized = false;
}

void DestroyRuntimeContents(Runtime& runtime)
{
    // 原全局Ref先减计数，再由实际renderer析构按各自EH顺序退成员。
    runtime.renderer.RetireOwned();
    runtime.initialized = false;
    runtime.window = nullptr;
}
}

Runtime::~Runtime() noexcept(false)
{
    // 对应原atexit 1004D500；不用GetRuntime重入已在析构的函数static对象。
    std::scoped_lock lock(mutex);
    DestroyRuntimeContents(*this);
    // 关闭后仍可调用manager API；日志器随模块所有者结束，避免重复Close重新分配日志状态。
    Logging::Shutdown();
}

void SetError(std::string message)
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    runtime.errorMessage.Assign(message.c_str());
    Logging::Write(Logging::Level::Error, runtime.errorMessage.GetString());
}

void AppendNativeError(const char* message)
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    // 原10007B20/10007AC0：已有正文时先追加LF，再追加新正文；即使新正文为空也保留LF。
    if (!runtime.errorMessage.IsEmpty()) runtime.errorMessage.AppendChar('\n');
    runtime.errorMessage.Append(message);
    std::string utf8;
    // GetErrorMessage的原ABI保留CP932；仅日志转换，不能把UTF-8字节交给原Start。
    if (Logging::TryCodePageToUtf8(runtime.errorMessage.GetString(), 932, utf8))
        Logging::Write(Logging::Level::Error, utf8);
    else
        Logging::Write(Logging::Level::Error, "Native Graphics error: CP932 to UTF-8 conversion failed.");
}

bool InitializeRuntime(const int backendMode, const HWND window, const int width, const int height,
    const int option1, const int option2)
{
    auto& runtime = GetRuntime();
    std::unique_lock lock(runtime.mutex);
    // 1000338C先清全局错误文本；100033A5/100033F5/1000343F的主体new先于旧Ref释放。
    runtime.errorMessage.Empty();
    RendererState* next;
    if (backendMode == 1) next = new RenderDxBridge;
    else if (backendMode == 2) next = new RenderGlBridge;
    else next = new GdiBackendState;
    // 原100053B0/5410/5470先退旧Ref，再发布base和分配控制块。
    // 若控制块失败，旧资源已销毁，新body仍是未初始化状态，不能回滚成旧renderer。
    // 新体构造已完成；旧体退休或新control失败时，不能由unique_ptr补删新候选。
    runtime.renderer.Publish(next, backendMode == 1 || backendMode == 2 ? 0x3C : 0x50);
    runtime.backendMode = backendMode;
    runtime.initialized = false;
    runtime.window = window;
    runtime.width = width;
    runtime.height = height;
    runtime.option1 = option1;
    runtime.option2 = option2;
    const auto initializeResources = [&]()
    {
        const auto* directory = reinterpret_cast<const char*>(static_cast<std::intptr_t>(option2));
        if (auto* dx = runtime.Dx())
        {
            const auto* padding = reinterpret_cast<const std::uint8_t*>(static_cast<std::intptr_t>(option1));
            const auto pixels = dx->Initialize(directory, window, width, height, padding && *padding != 0);
            runtime.Dx()->Primary().Publish(new SurfaceInfo);
            BindBorrowedPrimarySurface(*runtime.Dx()->Primary(), width, height, pixels.pitch, pixels.bits);
        }
        else if (auto* gl = runtime.Gl())
        {
            const auto pixels = gl->Initialize(directory, window, width, height);
            runtime.Gl()->Primary().Publish(new SurfaceInfo);
            BindBorrowedPrimarySurface(*runtime.Gl()->Primary(), width, height, pixels.pitch, pixels.bits);
        }
        else
        {
            // 原100021AC/10002244先完成Draw，再100022BB发布primary，之后100022F4创建像素。
            if (!runtime.Gdi()->InitializeDraw(window, width, height)) throw const_cast<char*>(GdiDrawFailure);
            runtime.Gdi()->Primary().Publish(new Surface);
            if (!InitializeSurfacePixels(*runtime.Gdi()->Primary()->info, width, height, 0, 1))
                throw const_cast<char*>(GdiPrimaryFailure);
        }
        runtime.initialized = runtime.Screen() != nullptr;
        if (runtime.initialized)
        {
            // 原DX10001340/1368/13A3；每个base先发布再分配Ref control，失败不回滚前缀。
            runtime.renderer->Dirty().Publish(new DirtyRectState);
            runtime.renderer->Manager().Publish(new SurfaceManager);
            runtime.renderer->Manager()->text.Publish(CreateTextRenderer().release());
            if (backendMode != 1 && backendMode != 2)
            {
                // 原023FA..02413：所有初始化成功之后才发布七个viewport/cache字段。
                runtime.Gdi()->viewport = {0, 0, width, height};
                runtime.Gdi()->targetWidth = width;
                runtime.Gdi()->targetHeight = height;
                runtime.Gdi()->resizeMode = -1;
            }
        }
        return runtime.initialized;
    };

    bool succeeded = false;
    if (backendMode == 1)
    {
        // 原10001030/100013C6：DX局部CString跨越失败Close；Close再抛时才展开它。
        alignas(NativeAnsiString) std::byte storage[sizeof(NativeAnsiString)];
        auto* error = ::new (storage) NativeAnsiString;
        struct ErrorRetirement
        {
            NativeAnsiString* value;
            ~ErrorRetirement() noexcept(false) { if (value) value->Retire(); }
        } retirement{error};
        try
        {
            succeeded = initializeResources();
            // 原13AB正常退局部串仍在state1；退休失败可进入同一catch，不提前撤保护。
            if (succeeded) { error->Retire(); retirement.value = nullptr; }
        }
        catch (const char* text) { succeeded = false; error->Assign(text); }
        catch (...) { succeeded = false; error->Assign("syntax error"); }
        if (!succeeded)
        {
            {
                // 原100013DE..100013FF先销毁带后缀的临时串，再在1000140C调用Close。
                const NativeAnsiString report(*error, " : [CRenderDX - Initialize]");
                AppendNativeError(report.GetString());
            }
            CloseBackend(runtime);
            // 原1411退休期间state0仍保护局部串；只有正常返回后才撤本层资格。
            error->Retire();
            retirement.value = nullptr;
            return false;
        }
    }
    else
    {
        try { succeeded = initializeResources(); }
        catch (const char* text) { AppendNativeError(text); }
        catch (...)
        {
            // 原GL 10002EE1的字符串是CRenderDIB，不根据类名自行改写。
            AppendNativeError(backendMode == 2 ? "syntax error : [CRenderDIB - Initialize]" :
                "syntax error : [CRenderGDI - Initialize]");
        }
        if (!succeeded)
        {
            // 原10002EC9/10002442在Initialize捕获区外清理，异常仍能外传并保留库Ref。
            CloseBackend(runtime);
            return false;
        }
    }
    Logging::WriteFormat(Logging::Level::Info,
        "InitSystem: backend=%d hwnd=%p size=%dx%d option1=%d option2=0x%08X result=%d.",
        backendMode, static_cast<void*>(window), width, height, option1, option2, runtime.initialized ? 1 : 0);
    return runtime.initialized;
}

void ShutdownRuntime()
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    DestroyRuntimeContents(runtime);
}

void CloseRuntime()
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    CloseBackend(runtime);
    Logging::Write(Logging::Level::Info, "ExitSystem: 后端已关闭，manager资源保留至renderer销毁。");
}

const int* ResizeRuntime(const int width, const int height, const int option)
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    if (runtime.Dx()) return runtime.Dx()->Resize(width, height, option);
    if (runtime.Gl()) return runtime.Gl()->Resize(width, height, option);
    if (!runtime.Gdi()) throw std::logic_error("GDI renderer is absent");
    if (runtime.Gdi()->targetWidth == width && runtime.Gdi()->targetHeight == height && runtime.Gdi()->resizeMode == option)
        return runtime.Gdi()->viewport.data();

    // 原02984/02987先提交目标尺寸，两个Draw getter正常完成后才改viewport；option最后提交。
    runtime.Gdi()->targetWidth = width;
    runtime.Gdi()->targetHeight = height;
    const auto sourceWidth = static_cast<std::uint32_t>(runtime.Gdi()->Draw().Width());
    const auto sourceHeight = static_cast<std::uint32_t>(runtime.Gdi()->Draw().Height());
    const auto targetWidth = static_cast<std::uint32_t>(width);
    const auto targetHeight = static_cast<std::uint32_t>(height);
    runtime.Gdi()->viewport = {0, 0, std::bit_cast<int>(sourceWidth), std::bit_cast<int>(sourceHeight)};
    switch (option)
    {
    case 0:
        runtime.Gdi()->viewport[2] = width;
        runtime.Gdi()->viewport[3] = height;
        break;
    case 1:
    {
        // 原IMUL/DIV使用低DWORD与无符号除法；只将原除零危险域明确拒绝，不改成返回nullptr。
        if (!targetWidth) throw std::domain_error("Original GDI resize division by zero");
        const auto product = targetHeight * sourceWidth;
        const auto scaledHeight = product / targetWidth;
        if (scaledHeight < sourceHeight)
        {
            runtime.Gdi()->viewport[3] = height;
            if (!sourceHeight) throw std::domain_error("Original GDI resize division by zero");
            const auto scaledWidth = product / sourceHeight;
            runtime.Gdi()->viewport[2] = std::bit_cast<int>(scaledWidth);
            runtime.Gdi()->viewport[0] = std::bit_cast<int>((targetWidth - scaledWidth) >> 1);
        }
        else
        {
            runtime.Gdi()->viewport[2] = width;
            if (scaledHeight > sourceHeight)
            {
                if (!sourceWidth) throw std::domain_error("Original GDI resize division by zero");
                const auto scaled = targetWidth * sourceHeight / sourceWidth;
                runtime.Gdi()->viewport[3] = std::bit_cast<int>(scaled);
                runtime.Gdi()->viewport[1] = std::bit_cast<int>((targetHeight - scaled) >> 1);
            }
            else runtime.Gdi()->viewport[3] = height;
        }
        break;
    }
    case 2:
        runtime.Gdi()->viewport[0] = std::bit_cast<int>((targetWidth - sourceWidth) >> 1);
        runtime.Gdi()->viewport[1] = std::bit_cast<int>((targetHeight - sourceHeight) >> 1);
        break;
    default: break;
    }
    runtime.Gdi()->resizeMode = option;
    (void)InvalidateRect(runtime.Gdi()->Draw().Window(), nullptr, TRUE);
    return runtime.Gdi()->viewport.data();
}

void AddDirtyRect(const RECT* rect)
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    RequireDirtyState(runtime);
    RECT queued = rect == nullptr ? RECT{0, 0, runtime.width, runtime.height} : *rect;
    if (IsRectEmpty(&queued))
    {
        return;
    }

    const int slot = runtime.Dirty()->count;
    runtime.Dirty()->rectangles[slot] = queued;
    runtime.Dirty()->areas[slot] = RectangleArea(queued);
    if (slot != 5)
    {
        ++runtime.Dirty()->count;
        return;
    }

    // Graphics.dll:10007670 在第六个矩形进入暂存槽后，合并额外覆盖面积最小的一对。
    int bestPenalty = std::numeric_limits<int>::max();
    int mergeFirst = 0;
    int mergeSecond = 1;
    RECT merged{};
    int mergedArea = 0;
    for (int first = 0; first < 5; ++first)
    {
        for (int second = first + 1; second < 6; ++second)
        {
            const RECT candidate{
                (std::min)(runtime.Dirty()->rectangles[first].left, runtime.Dirty()->rectangles[second].left),
                (std::min)(runtime.Dirty()->rectangles[first].top, runtime.Dirty()->rectangles[second].top),
                (std::max)(runtime.Dirty()->rectangles[first].right, runtime.Dirty()->rectangles[second].right),
                (std::max)(runtime.Dirty()->rectangles[first].bottom, runtime.Dirty()->rectangles[second].bottom)};
            const int candidateArea = RectangleArea(candidate);
            const int penalty = MergePenalty(candidateArea,
                runtime.Dirty()->areas[first], runtime.Dirty()->areas[second]);
            if (penalty < bestPenalty)
            {
                bestPenalty = penalty;
                mergeFirst = first;
                mergeSecond = second;
                merged = candidate;
                mergedArea = candidateArea;
            }
        }
    }

    runtime.Dirty()->rectangles[mergeFirst] = merged;
    runtime.Dirty()->areas[mergeFirst] = mergedArea;
    if (mergeSecond != 5)
    {
        runtime.Dirty()->rectangles[mergeSecond] = runtime.Dirty()->rectangles[5];
        runtime.Dirty()->areas[mergeSecond] = runtime.Dirty()->areas[5];
    }
}

void ClearDirtyRects()
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    RequireDirtyState(runtime);
    runtime.Dirty()->count = 0;
}


bool PresentToWindow()
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    if (runtime.Dx() != nullptr)
    {
        if (!runtime.Dx()->IsActive()) return false;
        MergeDirtyRectangles(runtime);
        runtime.Dx()->Upload(runtime.Dirty()->rectangles.data(), runtime.Dirty()->count);
        // DX：10001F51 上传，10001F89 清队列，10001684 再 Present。
        runtime.Dirty()->count = 0;
        return runtime.Dx()->Present();
    }
    if (runtime.Gl() != nullptr)
    {
        if (!runtime.Gl()->IsActive()) return false;
        MergeDirtyRectangles(runtime);
        bool presented = false;
        if (runtime.Dirty()->count != 0)
        {
            runtime.Gl()->Upload(runtime.Dirty()->rectangles.data(), runtime.Dirty()->count);
            presented = runtime.Gl()->Present();
        }
        // GL：10003075 上传，10003137 Present，1000315E 最后清队列。
        runtime.Dirty()->count = 0;
        return presented;
    }

    MergeDirtyRectangles(runtime);
    const int count = runtime.Dirty()->count;
    bool updated = true, invalidated = true;
    if (count != 0)
    {
        const RECT* const rectangles = runtime.Dirty()->rectangles.data();
        for (int index = 0; index < count; ++index)
        {
            // 原02597每项重新取Draw和primary；矩形数组/数量则来自本轮合并后的快照。
            if (!runtime.Gdi()) throw std::logic_error("GDI renderer is absent");
            auto& draw = runtime.Gdi()->Draw();
            auto* primary = runtime.Screen();
            if (!primary) throw std::logic_error("GDI primary is absent");
            if (!draw.Copy(rectangles[index], *primary)) { updated = false; break; }
        }
        const RECT viewport{runtime.Gdi()->viewport[0], runtime.Gdi()->viewport[1],
            AddDword(runtime.Gdi()->viewport[0], runtime.Gdi()->viewport[2]), AddDword(runtime.Gdi()->viewport[1], runtime.Gdi()->viewport[3])};
        // 原0262D正常Copy失败仍投递失效；异常不走此尾，也不在finally清队列。
        invalidated = InvalidateRect(runtime.Gdi()->Draw().Window(), &viewport, FALSE) != FALSE;
    }
    RequireDirtyState(runtime);
    runtime.Dirty()->count = 0;
    return updated && invalidated;
}

bool PresentToPaint(const PAINTSTRUCT& paint)
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    if (runtime.Dx()) return runtime.Dx()->PresentPaint(paint);
    if (runtime.Gl()) return runtime.Gl()->PresentPaint(paint);
    if (!runtime.Gdi() || !runtime.Gdi()->HasDraw() || IsRectEmpty(&paint.rcPaint)) return false;
    // 原026D4/02701：宽相同才读高，比较后重新解析Blt接收者，不用primary尺寸或Ensure重建。
    if (runtime.Gdi()->viewport[2] == runtime.Gdi()->Draw().Width() &&
        runtime.Gdi()->viewport[3] == runtime.Gdi()->Draw().Height())
    {
        return runtime.Gdi()->Draw().Blt(paint.hdc, paint.rcPaint.left, paint.rcPaint.top,
            SubDword(paint.rcPaint.right, paint.rcPaint.left), SubDword(paint.rcPaint.bottom, paint.rcPaint.top),
            SubDword(paint.rcPaint.left, runtime.Gdi()->viewport[0]), SubDword(paint.rcPaint.top, runtime.Gdi()->viewport[1]), SRCCOPY);
    }
    // 原027B1..027EF先捕获三个接收者，再读高/宽，最后提交完整viewport而非rcPaint。
    auto& draw = runtime.Gdi()->Draw();
    const int sourceHeight = draw.Height();
    const int sourceWidth = draw.Width();
    return draw.Stretch(paint.hdc, runtime.Gdi()->viewport[0], runtime.Gdi()->viewport[1],
        runtime.Gdi()->viewport[2], runtime.Gdi()->viewport[3], 0, 0, sourceWidth, sourceHeight, SRCCOPY);
}

int GetFilteringCapabilities()
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    if (runtime.Dx() != nullptr)
    {
        return runtime.Dx()->FilteringCapabilities();
    }
    // CRenderGL::10003340检查库对象base；Close后renderer仍在而库对象已空。
    return runtime.Gl() && runtime.Gl()->IsActive() ? 0x11 : 0;
}

int GetMagFiltering()
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    return runtime.Dx() != nullptr ? runtime.Dx()->GetMagFiltering() : -1;
}

int GetMinFiltering()
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    return runtime.Dx() != nullptr ? runtime.Dx()->GetMinFiltering() : -1;
}

void SetMagFiltering(const int value)
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    // 原setter不追加getter，也不规范化后端返回值；GL/GDI为10002AA0空操作。
    if (runtime.Dx()) runtime.Dx()->SetMagFiltering(value);
}

void SetMinFiltering(const int value)
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    if (runtime.Dx()) runtime.Dx()->SetMinFiltering(value);
}

int CheckRendererDevice()
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    if (runtime.Dx() == nullptr)
    {
        return 0;
    }
    const int status = runtime.Dx()->TestCooperativeLevel();
    if (status != 0)
        Logging::WriteFormat(Logging::Level::Warning, "CheckDeviceLost: RenderDX status=%d.", status);
    // Graphics.dll:10001FA0：正常和 DEVICENOTRESET(2) 均返回 0，由 Start 随后调用 RenderReset。
    return status == 0 || status == 2 ? 0 : 1;
}

void ResetRenderer()
{
    auto& runtime = GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    if (runtime.Dx() != nullptr)
    {
        runtime.Dx()->Reset();
    }
    // CRenderGL::RenderReset 的虚表项是 nullsub_2（10003220）。
}
}
