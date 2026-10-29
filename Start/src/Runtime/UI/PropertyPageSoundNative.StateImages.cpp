#include <Runtime/PropertyPageSoundNative.h>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr int CellSize = 13;
constexpr COLORREF Transparent = RGB(255, 0, 255);
constexpr UINT_PTR SubclassId = 1;
constexpr wchar_t OwnerProperty[] = L"KaGuYa.PropertyPageSound.StateImages.Owner.v1";

[[noreturn]] void Fail(const char* operation)
{
    const DWORD error = ::GetLastError();
    throw std::system_error(static_cast<int>(error ? error : ERROR_GEN_FAILURE),
        std::system_category(), operation);
}
void CleanupResult(BOOL ok, const char* operation) noexcept
{
    // 窗口退役不能让异常穿过系统回调；异常清理失败保留原生诊断。
    if (!ok) ::OutputDebugStringA(operation);
}
template<class T> T Resolve(HMODULE module, const char* name)
{
    auto address = ::GetProcAddress(module, name);
    if (!address) Fail(name);
    return reinterpret_cast<T>(address);
}
struct ControlsApi
{
    decltype(&::ImageList_Create) create;
    decltype(&::ImageList_AddMasked) add;
    decltype(&::ImageList_GetImageCount) count;
    decltype(&::ImageList_GetIconSize) size;
    decltype(&::ImageList_Destroy) destroy;
    decltype(&::SetWindowSubclass) subclass;
    decltype(&::RemoveWindowSubclass) removeSubclass;
    decltype(&::DefSubclassProc) next;

    explicit ControlsApi(HWND tree)
    {
        // 从实际树类解析，避免宿主同时载入 classic/v6 时混用列表与 subclass API。
        auto module = reinterpret_cast<HMODULE>(::GetClassLongPtrA(tree, GCLP_HMODULE));
        if (!module) Fail("Sound tree class module");
        create = Resolve<decltype(create)>(module, "ImageList_Create");
        add = Resolve<decltype(add)>(module, "ImageList_AddMasked");
        count = Resolve<decltype(count)>(module, "ImageList_GetImageCount");
        size = Resolve<decltype(size)>(module, "ImageList_GetIconSize");
        destroy = Resolve<decltype(destroy)>(module, "ImageList_Destroy");
        subclass = Resolve<decltype(subclass)>(module, "SetWindowSubclass");
        removeSubclass = Resolve<decltype(removeSubclass)>(module, "RemoveWindowSubclass");
        next = Resolve<decltype(next)>(module, "DefSubclassProc");
    }
};
HIMAGELIST CurrentList(HWND tree) noexcept
{
    return reinterpret_cast<HIMAGELIST>(::SendMessageA(tree, TVM_GETIMAGELIST, TVSIL_STATE, 0));
}
struct Drawing
{
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HBRUSH background = nullptr;
    HGDIOBJ previous = nullptr;
    ~Drawing() noexcept
    {
        if (previous) CleanupResult(::SelectObject(dc, previous) != nullptr, "Sound tree: restore DC failed\n");
        if (dc) CleanupResult(::DeleteDC(dc), "Sound tree: DeleteDC failed\n");
        if (bitmap) CleanupResult(::DeleteObject(bitmap), "Sound tree: DeleteObject(bitmap) failed\n");
        if (background) CleanupResult(::DeleteObject(background), "Sound tree: DeleteObject(brush) failed\n");
    }
};
struct Binding
{
    ControlsApi api;
    HIMAGELIST images = nullptr;
    HIMAGELIST automatic = nullptr;
    bool ready = false;
    bool internalSet = false;
    bool externallyReplaced = false;

    explicit Binding(const ControlsApi& functions) : api(functions) {}
    ~Binding() noexcept
    {
        if (automatic) CleanupResult(api.destroy(automatic), "Sound tree: automatic list cleanup failed\n");
        if (images) CleanupResult(api.destroy(images), "Sound tree: owned list cleanup failed\n");
    }
    HIMAGELIST Set(HWND tree, HIMAGELIST list) noexcept
    {
        internalSet = true;
        auto old = reinterpret_cast<HIMAGELIST>(::SendMessageA(tree, TVM_SETIMAGELIST,
            TVSIL_STATE, reinterpret_cast<LPARAM>(list)));
        internalSet = false;
        return old;
    }
};
LRESULT CALLBACK StateImagesProc(HWND tree, UINT message, WPARAM w, LPARAM l,
    UINT_PTR id, DWORD_PTR data)
{
    auto* binding = reinterpret_cast<Binding*>(data);
    if (binding->ready && message == WM_KEYDOWN && w == VK_SPACE)
    {
        // 共享功能扩展：只通知父级处理1/2及禁用态，阻止控件随后按五格列表默认循环。
        NMTVKEYDOWN notification{};
        notification.hdr.hwndFrom = tree;
        notification.hdr.idFrom = static_cast<UINT_PTR>(::GetDlgCtrlID(tree));
        notification.hdr.code = TVN_KEYDOWN;
        notification.wVKey = VK_SPACE;
        notification.flags = static_cast<UINT>(l);
        ::SendMessageA(::GetParent(tree), WM_NOTIFY, notification.hdr.idFrom,
            reinterpret_cast<LPARAM>(&notification));
        // 父通知可以同步销毁本树；此后不再访问binding，也不调用DefSubclassProc。
        return 0;
    }
    if (message == TVM_SETIMAGELIST && w == TVSIL_STATE && !binding->internalSet &&
        reinterpret_cast<HIMAGELIST>(l) != binding->images)
        binding->externallyReplaced = true;
    if (message != WM_NCDESTROY) return binding->api.next(tree, message, w, l);

    // 只摘下本绑定拥有的句柄；外部后来装上的列表既不接管，也不销毁。
    const auto current = CurrentList(tree);
    if (current && (current == binding->images || current == binding->automatic))
        binding->Set(tree, nullptr);
    CleanupResult(binding->api.removeSubclass(tree, &StateImagesProc, id),
        "Sound tree: RemoveWindowSubclass failed\n");
    CleanupResult(::RemovePropW(tree, OwnerProperty) == binding, "Sound tree: owner property changed\n");
    const LRESULT result = binding->api.next(tree, message, w, l);
    delete binding;
    return result;
}
void MakeImages(Binding& binding)
{
    Drawing drawing;
    drawing.dc = ::CreateCompatibleDC(nullptr);
    if (!drawing.dc) Fail("Sound tree CreateCompatibleDC");
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = CellSize * 5;
    info.bmiHeader.biHeight = -CellSize;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* pixels = nullptr;
    drawing.bitmap = ::CreateDIBSection(drawing.dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
    if (!drawing.bitmap) Fail("Sound tree CreateDIBSection");
    drawing.previous = ::SelectObject(drawing.dc, drawing.bitmap);
    if (!drawing.previous || drawing.previous == HGDI_ERROR)
    {
        drawing.previous = nullptr;
        Fail("Sound tree SelectObject");
    }
    drawing.background = ::CreateSolidBrush(Transparent);
    if (!drawing.background) Fail("Sound tree CreateSolidBrush");
    RECT strip{0, 0, CellSize * 5, CellSize};
    if (!::FillRect(drawing.dc, &strip, drawing.background)) Fail("Sound tree FillRect");
    // 原205不存在：明确采用标准checkbox，不声称与原资源像素等价；0格保持透明。
    const UINT states[] = {DFCS_BUTTONCHECK, DFCS_BUTTONCHECK | DFCS_CHECKED,
        DFCS_BUTTONCHECK | DFCS_INACTIVE, DFCS_BUTTONCHECK | DFCS_INACTIVE | DFCS_CHECKED};
    for (int i = 0; i < 4; ++i)
    {
        RECT cell{CellSize * (i + 1), 0, CellSize * (i + 2), CellSize};
        if (!::DrawFrameControl(drawing.dc, &cell, DFC_BUTTON, states[i])) Fail("Sound tree DrawFrameControl");
    }
    if (!::SelectObject(drawing.dc, drawing.previous)) Fail("Sound tree restore drawing DC");
    drawing.previous = nullptr;
    binding.images = binding.api.create(CellSize, CellSize, ILC_COLOR32 | ILC_MASK, 5, 0);
    if (!binding.images) Fail("Sound tree ImageList_Create");
    if (binding.api.add(binding.images, drawing.bitmap, Transparent) != 0 || binding.api.count(binding.images) != 5)
        Fail("Sound tree ImageList_AddMasked/five images");
}
void CheckImages(const Binding& binding)
{
    int width = 0, height = 0;
    if (binding.api.count(binding.images) != 5 || !binding.api.size(binding.images, &width, &height) ||
        width != CellSize || height != CellSize)
        throw std::logic_error("Sound tree owned image list was modified");
}
}

void IPropertyPageSoundNativeApi::InstallSoundTreeStateImages(HWND tree)
{
    DWORD process = 0;
    const DWORD thread = ::GetWindowThreadProcessId(tree, &process);
    char className[64]{};
    if (!tree || thread != ::GetCurrentThreadId() || process != ::GetCurrentProcessId() ||
        !::GetClassNameA(tree, className, sizeof(className)) || std::strcmp(className, WC_TREEVIEWA) != 0)
        throw std::invalid_argument("Sound tree installation requires a real current-thread SysTreeView32");
    const ControlsApi api(tree);
    // classic不保证按名称导出GetWindowSubclass；私有属性只登记本方法创建的绑定。
    if (auto* binding = static_cast<Binding*>(::GetPropW(tree, OwnerProperty)))
    {
        // 本树专属列表是借出句柄，外部不得销毁；被替换后不悄悄重新安装或接管外来列表。
        if (!binding || !binding->ready || binding->externallyReplaced || CurrentList(tree) != binding->images)
            throw std::logic_error("Sound tree binding is incomplete or was externally replaced");
        CheckImages(*binding);
        return;
    }
    const LONG style = ::GetWindowLongA(tree, GWL_STYLE);
    // 不能按数量/尺寸猜已有列表的来源；必须由本次样式转换建立自动列表的所有权证据。
    if ((style & TVS_CHECKBOXES) || CurrentList(tree))
        throw std::logic_error("Sound tree first installation requires no TVS_CHECKBOXES and no state image list");
    auto owner = std::make_unique<Binding>(api);
    MakeImages(*owner);
    CheckImages(*owner);
    ::SetLastError(0);
    if (!::SetPropW(tree, OwnerProperty, owner.get())) Fail("Sound tree SetPropW(owner)");
    if (!api.subclass(tree, &StateImagesProc, SubclassId, reinterpret_cast<DWORD_PTR>(owner.get())))
    {
        const DWORD error = ::GetLastError();
        CleanupResult(::RemovePropW(tree, OwnerProperty) == owner.get(), "Sound tree: rollback owner property failed\n");
        ::SetLastError(error);
        Fail("Sound tree SetWindowSubclass");
    }
    // 从此即使安装失败也由WM_NCDESTROY退役，失败绑定不能被重复调用伪装成成功。
    Binding& binding = *owner.release();
    ::SetLastError(0);
    const LONG previousStyle = ::SetWindowLongA(tree, GWL_STYLE, style | TVS_CHECKBOXES);
    if ((!previousStyle && ::GetLastError()) || !(::GetWindowLongA(tree, GWL_STYLE) & TVS_CHECKBOXES))
        Fail("Sound tree add TVS_CHECKBOXES");
    if (binding.externallyReplaced)
        throw std::logic_error("Sound tree state list replaced during style initialization");
    binding.automatic = CurrentList(tree);
    if (!binding.automatic) Fail("Sound tree automatic state image list unavailable");
    const auto old = binding.Set(tree, binding.images);
    if (old != binding.automatic || CurrentList(tree) != binding.images)
        throw std::runtime_error("Sound tree TVM_SETIMAGELIST installation failed");
    const auto automatic = std::exchange(binding.automatic, nullptr);
    if (!api.destroy(automatic)) Fail("Sound tree replaced automatic ImageList_Destroy");
    binding.ready = true;
}
}
