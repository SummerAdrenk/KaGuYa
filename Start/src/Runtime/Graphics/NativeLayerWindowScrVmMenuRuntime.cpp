#include <Runtime/NativeLayerWindowScrVmMenuRuntime.h>

namespace KaGuYa::Start::Runtime
{
void NativeLayerWindowScrVmMenuRuntime::ApplyResultLayerList(std::int32_t index)
{
    // 4CEEF9调用4CB030，接收者是本次CScript，而不是再次查Root的Script。
    updates_.ApplyResultLayerList(index);
}
void NativeLayerWindowScrVmMenuRuntime::SelectEffect(std::int32_t selector)
{
    // 4CF27C与UPDATE共用4D6C10；保留其两次Root捕获及释放异常，不翻译异常。
    updates_.SelectLayerEffect(selector);
}
}
