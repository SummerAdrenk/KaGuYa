#include <Runtime/RootGraffitiPageNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneManagerView.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
ScriptParamsNativeBody& ParamsAt47BDB0(void* receiver)
{
    // 49DDF5保存的ESI根：498020之后才读取这代root的当前+1C0 Ref。
    // 回调不捕获owner，不再次413BE0，不缓存此前的CParams正文。
    return static_cast<RootRuntimePayload*>(receiver)->ParamsAt47BDB0();
}
}
RootGraffitiPageNativeSource::RootGraffitiPageNativeSource(
    RootRuntimeOwner& root, EnvironmentRootOwner& environment) noexcept
    : root_(root), environment_(environment) {}

EnvironmentRootStorage* RootGraffitiPageNativeSource::CaptureEnvironmentAt4180C0()
{
    // 49DCFA/49DD4B/49DDA4、4B6185逐次执行；返回owner解锁后重读的发布值。
    return environment_.CaptureAt4180C0();
}
DrawManagerNativeBody2Ch* RootGraffitiPageNativeSource::CaptureDrawManagerAfter413BE0()
{
    // 4B620A、46A980各槽、4B5872/4B58A8/4B58EF及4AA9F0各原点。
    root_.EnsureCreatedAt413BE0();
    // 与RootDemoSequenceNativeSource相同，直接取得DrawLayers所借的原2Ch。
    return root_.Published()->PostWindow().DrawManagerAt18();
}
GraffitiPageNativeParamsReceiver RootGraffitiPageNativeSource::CaptureParamsRootAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    // 49DDEB只选root；47BDB0延迟到scene的498020完成后才执行。
    return {root_.Published(), ParamsAt47BDB0};
}
SceneManagerNativeBody48h* RootGraffitiPageNativeSource::CaptureSceneManagerAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    // 4B6152借原+30正文；不调用现代SceneManager，不在Source里推进或补建。
    return root_.Published()->PostWindow().BorrowSceneManagerAt30().NativeBody();
}
}
