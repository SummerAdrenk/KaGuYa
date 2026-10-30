#include "MainFrameSettingsNative.Internal.h"
#include <Logging/Logger.h>

namespace KaGuYa::Start::Runtime
{
namespace MainFrameSettingsNativeDetail
{
void* Registry(MainFrameSettingsNativeServices& services, unsigned index, unsigned site,
    RegistryOrder order, bool inlined)
{
    auto& source = services.apply.source;
    auto* const root = inlined ? source.CaptureEnvironmentInlineAt6198C8(site) : source.CaptureEnvironmentAt4180C0(site);
    Slot* begin;
    std::uintptr_t end;
    if (order == RegistryOrder::Function417990)
    { begin = Read(root->archivesAt30.begin); end = reinterpret_cast<std::uintptr_t>(Read(root->archivesAt30.end)); }
    else
    { end = reinterpret_cast<std::uintptr_t>(Read(root->archivesAt30.end)); begin = Read(root->archivesAt30.begin); }
    const auto count = static_cast<unsigned>(std::bit_cast<std::int32_t>(end - reinterpret_cast<std::uintptr_t>(begin)) / 12);
    if (index >= count) return nullptr;
    auto* const slot = reinterpret_cast<Slot*>(reinterpret_cast<std::uintptr_t>(begin) + index * 12u);
    return Resolve<void>(*slot, order != RegistryOrder::InlineStrideFirst);
}
void* Root(MainFrameSettingsNativeServices& services, unsigned site)
{
    services.apply.source.EnsureRootAt413BE0(site);
    return services.apply.source.RootAt6198C4(site);
}
}
namespace
{
using namespace MainFrameSettingsNativeDetail;
struct Retirement final
{
    Locals& values;
    MainFrameSettingsNativeServices& services;
    int state = -1;
    ~Retirement() noexcept
    {
        if (state != -1) KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Trace,
            "Start settings 800D unwind qualification=%d", state);
        // 5932C0..59331E：锁的1/3/8/10/12及Leave终止态由真实owner捕获核履行。
        // 本守卫只保存caller拥有对象的0/5/6/7资格，不能恢复IME、Root byte或声音。
        if (state == 7) DestroyGamePropertyNativeAt40E840(values.property, services.property);
        if (state == 6 || state == 7) DestroyRuntimeCStringAt40CD40(&values.second);
        if (state >= 5 && state <= 7) DestroyRuntimeCStringAt40CD40(&values.first);
        if (state == 0 || (state >= 5 && state <= 7))
            DestroyRuntimeCStringVectorAt40DD20(values.mappings, services.memory);
    }
};
void CloneInto(RuntimeCString& out, const RuntimeCString& source)
{
    auto* const header = reinterpret_cast<ATL::CStringData*>(reinterpret_cast<std::uintptr_t>(source.GetString()) - 16u);
    auto* const clone = CloneRuntimeCStringDataAt414540(header);
    const auto chars = reinterpret_cast<const char*>(reinterpret_cast<std::uintptr_t>(clone) + 16u);
    // Clone已取得引用；原始输出槽只接管指针，不触发CString赋值。
    std::memcpy(static_cast<void*>(&out), &chars, sizeof(chars));
}
}
void RunMainFrameSettingsNativeAt428E30(void* menu, MainFrameSettingsNativeServices& services)
{
    SettingDialogNativeApplyScope apply(services.apply);
    GamePropertyNativeScope property(services.property);
    SelectWindowNativeScope select(services.selectWindows);
    ToolWindowNativeScope tools(services.tools);
    Locals values;
    Retirement retirement{values, services};
    KaGuYa::Logging::Write(KaGuYa::Logging::Level::Trace, "Start settings 800D begin");
    static_cast<void>(services.apply.playback.StopAndClearAt4384D0(*RootSound(0x428E69)));
    auto* const initial = Registry(services, 2, 0x428E9A, RegistryOrder::InlineBiasFirst);
    const auto count = Count(Field<Vector>(initial, 0x30));
    ConstructRuntimeCStringVectorAt40B7A0(values.mappings, count, services.apply.strings, services.memory);
    retirement.state = 0;
    CaptureSnapshot(values, services);
    CloneInto(values.first, Field<RuntimeCString>(Registry(services, 1, 0x4292F3), 0x58));
    retirement.state = 5;
    CloneInto(values.second, Field<RuntimeCString>(Registry(services, 1, 0x42935A), 0x5C));
    retirement.state = 6;
    values.color44 = Read(Field<DWORD>(Registry(services, 1, 0x4293C1), 0x44));
    values.color40 = Read(Field<DWORD>(Registry(services, 1, 0x429415), 0x40));
    values.text60 = Read(Field<BYTE>(Registry(services, 1, 0x429469), 0x60));
    values.text61 = Read(Field<BYTE>(Registry(services, 1, 0x4294BD), 0x61));
    values.text64 = Read(Field<DWORD>(Registry(services, 1, 0x429511), 0x64));
    values.text68 = Read(Field<DWORD>(Registry(services, 1, 0x429565), 0x68));
    auto& selectedPage = Field<DWORD>(menu, 8);
    ConstructGamePropertyNativeAt40B860(values.property, Read(selectedPage), services.property);
    retirement.state = 7;
    auto* environment = services.apply.source.CaptureEnvironmentAt4180C0(0x4295D1);
    static_cast<void>(services.api.WinNlsEnableIME(reinterpret_cast<HWND>(Read(environment->windowAt2C)), TRUE));
    services.access.WriteMinimizeAt1B2(Root(services, 0x4295E5), 0);
    RunPropertySheetModalNativeAt4295F6(values.property.sheetAt00, services.property.sheet);
    services.access.WriteMinimizeAt1B2(Root(services, 0x4296FA), 1);
    environment = services.apply.source.CaptureEnvironmentAt4180C0(0x42970B);
    static_cast<void>(services.api.WinNlsEnableIME(reinterpret_cast<HWND>(Read(environment->windowAt2C)), FALSE));
    const auto closedPage = Read(values.property.lastPageAt334);
    if (closedPage >= 0) selectedPage = static_cast<DWORD>(closedPage);
    RestoreSounds(values, services);
    ApplyPolicyAndLayers(values, services);
    DestroyGamePropertyNativeAt40E840(values.property, services.property);
    // 42A916/42A931在每次正常Free前撤该串资格；错误只展开仍拥有的前缀。
    retirement.state = 5;
    DestroyRuntimeCStringAt40CD40(&values.second);
    retirement.state = 0;
    DestroyRuntimeCStringAt40CD40(&values.first);
    retirement.state = 14;
    DestroyRuntimeCStringVectorAt40DD20(values.mappings, services.memory);
    retirement.state = -1;
    KaGuYa::Logging::Write(KaGuYa::Logging::Level::Trace, "Start settings 800D complete");
}
void MainFrameSettingsCommandNativeRuntime::RunAt428E30(void* menu)
{ RunMainFrameSettingsNativeAt428E30(menu, services_); }
}
