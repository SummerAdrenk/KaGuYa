#include "QuickOperationsNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace QuickOperationsNativeDetail;
bool QuickLoadNativeAt485CE0()
{
    auto& source = RequireQuickOperationsNativeServices().source;
    auto* save = static_cast<std::byte*>(CaptureInlineArchive(1));
    if (std::uint8_t(save[0x7A]) && !Confirm(4)) return false;

    QuickOperationsNativeFactoryBody10h factory;
    ConstructQuickOperationsNativeFactory(factory, 1);
    FactoryLifetime lifetime{factory.base};
    std::int32_t error = 1;
    if (CaptureLatestInline() >= 0)
    {
        // 485EEC/485EF3独立Env与417990，不复用前一有符号测试的catalog/序号。
        const auto slot = Increment32(CaptureLatestVia417990());
        error = LoadFileFactoryNativeAt480830(factory.base, slot, source) != 0 ? 2 : 0;
    }
    if (error != 0)
    {
        auto* env = source.CaptureEnvironmentAt4180C0();
        const char* caption = env->titleAt10.GetString();
        HWND window = reinterpret_cast<HWND>(env->windowAt2C);
        static_cast<void>(source.MessageBoxA(window, error == 1 ?
            QuickOperationsNativeNoSaveMessage() : QuickOperationsNativeLoadErrorMessage(), caption, 0x10));
    }
    lifetime.Finish(); // 正常Free先撤资格；首次异常外传，不能转false。
    return error == 0;
}

void __stdcall QuickSaveNativeAt485FC0(std::uint32_t preview)
{
    auto& deps = RequireQuickOperationsNativeServices();
    auto* save = static_cast<std::byte*>(CaptureInlineArchive(1));
    if (std::uint8_t(save[0x79]) && !Confirm(3)) return;

    QuickOperationsNativeFactoryBody10h factory;
    ConstructQuickOperationsNativeFactory(factory, static_cast<std::uint8_t>(preview));
    FactoryLifetime lifetime{factory.base};
    auto* params = CaptureParams();
    SetRuntimeCStringAt437680(params->ResolveSaveParamsAt08()->saveDescriptionAt04, "", 0);
    if (RollingQuickSaveNativeAt4888C0(factory) == 0)
    {
        const auto item = CaptureLatestVia417990();
        RefreshSaveFileCatalogItemNativeAt48E5F0(-1, item, deps.catalog, deps.memory);
    }
    lifetime.Finish();
}
}
