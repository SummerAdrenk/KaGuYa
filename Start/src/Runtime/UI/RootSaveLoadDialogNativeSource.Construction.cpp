#include <Runtime/RootSaveLoadDialogNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/SaveFileStateNativeArchive.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <stdexcept>
#include <bit>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootSaveLoadDialogNativeSource::CaptureEnvironmentAt4180C0()
{ return dependencies_.saveLoad.CaptureEnvironmentAt4180C0(); }

SaveFileStateNativeArchive* RootSaveLoadDialogNativeSource::CaptureCatalogAt4180C0()
{
    auto* env = CaptureEnvironmentAt4180C0();
    return static_cast<SaveFileStateNativeArchive*>(ResolveEnvironmentArchiveAt417990(*env, 6));
}

void RootSaveLoadDialogNativeSource::RequestCloseApplication()
{
    // 01原492D90返回6调用416B40发WM_CLOSE；不是返回标题的确认命令。
    auto* environment=CaptureEnvironmentAt4180C0();
    (void)::PostMessageA(reinterpret_cast<HWND>(environment->windowAt2C),WM_CLOSE,0,0);
}

void RootSaveLoadDialogNativeSource::ShowPageLimitMessage()
{
    auto* environment=CaptureEnvironmentAt4180C0();
    const auto* message=CurrentSaveLoadResourceProfile().deletion.pageLimitMessage;
    if(!message)throw std::logic_error("save/load page limit message is not bound");
    ::MessageBoxA(reinterpret_cast<HWND>(environment->windowAt2C),message,
        environment->titleAt10.GetString(),MB_ICONERROR);
}

std::int32_t RootSaveLoadDialogNativeSource::ReadInitialPageFromEnvironmentAt4EBC35()
{
    // 4EBC35..49只读slot6+14，不是+0C/+10；DWORD位模式直接写父+E4。
    auto* catalog = CaptureCatalogAt4180C0();
    return std::bit_cast<std::int32_t>(*static_cast<const volatile std::uint32_t*>(&catalog->regularPageStepAt14));
}

void* RootSaveLoadDialogNativeSource::CaptureDrawAfter413BE0()
{
    dependencies_.saveLoad.EnsureRootAt413BE0();
    auto* root = static_cast<RootRuntimePayload*>(dependencies_.saveLoad.RootAt6198C4());
    // RootRuntimePayload不是原21Ch；getter只借本次根的唯一+18 Ref，不按原offset强转。
    const volatile auto& slot = root->DrawManagerSlotAt18();
    const auto base = slot.baseAt00;
    if (!base) return nullptr;
    const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot.controlAt08);
    const auto stride = control ? control->strideAt0C : 0u;
    const auto bias = slot.biasAt04;
    // 50E8F5..909先control/stride再bias；volatile保留4ACDD0不消费ECX时的原读点。
    return reinterpret_cast<DrawManagerNativeBody2Ch*>(base + bias * stride);
}

bool RootSaveLoadDialogNativeSource::LoadIntoAt4800A0(void* draw, GraphicsSurfaceNativeBody* surface,
    const char* name)
{
    // 4800A0不消费ECX，但仍传原caller选定的receiver；不得让loader再次Capture。
    return dependencies_.pictures.LoadIntoAt4800A0(draw, *surface, name);
}

std::uint8_t RootSaveLoadDialogNativeSource::LoadSurfaceAt480000(void* draw, YtlReferenceSlotRuntime* out,
    const char* name)
{
    // 480024固定EDI；只在base为空时分配，失败不补清输出或换loader。
    const volatile auto& slot = *out;
    auto base = slot.baseAt00;
    if (!base)
    {
        AdoptGraphicsSurfaceAt407A80(*out,
            NewGraphicsSurfaceAt40C2E0(dependencies_.graphics, dependencies_.saveLoad.Strings()));
        base = slot.baseAt00;
    }
    GraphicsSurfaceNativeBody* surface = nullptr;
    if (base)
    {
        const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot.controlAt08);
        const auto stride = control ? control->strideAt0C : 0u;
        const auto bias = slot.biasAt04;
        surface = reinterpret_cast<GraphicsSurfaceNativeBody*>(base + bias * stride);
    }
    // 480081先push name再push surface；空正文也交原Load，不新增E_INVALIDARG门控。
    return LoadIntoAt4800A0(draw, surface, name);
}

GuiDialogNativeBody5Ch* RootSaveLoadDialogNativeSource::ConstructSlotConfirmationAt4EACA0(void* raw)
{
    SaveLoadConfirmDialogNativeScope scope(*NativeSlotConfirmationServices());
    return ConstructSaveLoadConfirmDialogNativeAt4EACA0(raw);
}

GuiDialogNativeBody5Ch* RootSaveLoadDialogNativeSource::ConstructDeleteConfirmationAt4EC8A0(void* raw)
{
    SaveLoadMessageBoxNativeScope scope(*NativeDeleteConfirmationServices());
    return ConstructSaveLoadMessageBoxAt4EC8A0(raw);
}

SaveLoadMessageBoxNativeServices* RootSaveLoadDialogNativeSource::NativeDeleteConfirmationServices() const noexcept
{ return &dependencies_.deleteConfirmation; }

SaveLoadConfirmDialogNativeServices* RootSaveLoadDialogNativeSource::NativeSlotConfirmationServices() const noexcept
{ return &dependencies_.saveLoad.ConfirmationServices(); }
}
