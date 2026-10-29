#include <Runtime/RootTitleSceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ReadScriptInputNative.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/InformationDisplayView.h>
#include <Runtime/RootScriptStartupNativeServices.h>
#include "TitleSceneNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
TitleSceneNativeManagerReceiver RootTitleSceneNativeSource::CaptureScenesAfter413BE0()
{
    dependencies_.root.EnsureCreatedAt413BE0();
    const auto selected=dependencies_.root.Published()->PostWindow().BorrowSceneManagerAt30();
    // Native选择由装配保证；不在原生分支失败后切受控manager。
    return {selected.NativeBody(),&selected.NativeServices()};
}
EnvironmentRootStorage* RootTitleSceneNativeSource::CaptureEnvironmentAt4180C0()
{return dependencies_.environment.CaptureAt4180C0();}
void RootTitleSceneNativeSource::ClearInformationDwordAfter413BE0()
{
    dependencies_.root.EnsureCreatedAt413BE0();
    // 默认+3C借同一个原14h父；不把现代Root按原偏移读取，不另建抑制状态。
    dependencies_.root.Published()->PostWindow().BorrowInformationDisplayAt3C().SuppressedAt10()=0;
}
bool RootTitleSceneNativeSource::EvaluateLoadPredicateAfter413BE0()
{
    dependencies_.root.EnsureCreatedAt413BE0();
    // 4BB00F先读当前发布根；47F650自身不消费该ECX，也不得额外Ensure。
    static_cast<void>(dependencies_.root.ReadPublishedAt6198C4());
    return ReadScriptLoadKeyAt47F650(dependencies_.environment,dependencies_.input);
}
void RootTitleSceneNativeSource::WriteRootByteAfter413BE0(std::uint32_t offset,std::uint8_t value)
{
    dependencies_.root.EnsureCreatedAt413BE0();auto* selected=dependencies_.root.Published();
    switch(offset)
    {
    case 0x1AF:selected->WriteByteAt1AF(value);return;
    case 0x218:selected->WriteByteAt218(value);return;
    case 0x219:selected->WriteByteAt219(value);return;
    default:throw std::logic_error("Title Root byte port requires a confirmed offset");
    }
}
void RootTitleSceneNativeSource::ClearReplayAt4A2DD6()
{
    auto* env=dependencies_.environment.CaptureAt4180C0();
    static_cast<void>(TitleSceneNativeDetail::InlineArchive(env,0));
    dependencies_.root.EnsureCreatedAt413BE0();auto* selected=dependencies_.root.Published();
    const auto index=ResolveScriptNamedValueNativeAt498020("replay",dependencies_.names,dependencies_.strings);
    WriteScriptValueAt4C8220(selected->ParamsAt47BDB0(),index,0,dependencies_.errors);
}
std::uint8_t RootTitleSceneNativeSource::LoadStartupScriptAfter413BE0(const char* name,std::int32_t argument)
{
    dependencies_.root.EnsureCreatedAt413BE0();
    auto& selected=*dependencies_.root.Published();auto* script=selected.ScriptAt47CD00();
    auto scope=dependencies_.execution.BindSelectedReceiver(selected,*script);
    // 每次绑定caller选中的publication；长期Title服务不得固定上一代Root的记录。
    RootScriptStartupNativeServices startup({dependencies_.root,dependencies_.environment,dependencies_.graphics,
        dependencies_.execution,dependencies_.errors,dependencies_.names,dependencies_.strings},
        dependencies_.resources,selected.ScriptPublication());
    return LoadStartupScriptNativeAt4D53D0(*script,name,argument,startup);
}
}
