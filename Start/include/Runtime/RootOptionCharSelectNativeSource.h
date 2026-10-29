#pragma once

#include <Runtime/OptionCharSelectDialogNative.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class EnvironmentSoundParamsArchive;

// 正文外非拥有适配器：借同一 Environment 服务，不另建根、锁、manager 或可用性镜像。
// ctor 不 Capture；只有 Availability 在原每个 index 的读点进入同一根 owner。
class RootOptionCharSelectNativeSource final : public IOptionCharSelectDialogNativeSource
{
public:
    explicit RootOptionCharSelectNativeSource(EnvironmentStartupRuntime& environment,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;

    GuiClickedEventReferences* NewClickedEventManagerAt4E4AAD() override;
    void AdoptClickedEventManagerAt4D7B90(YtlReferenceSlotRuntime& destination,
        GuiClickedEventReferences* owned) override;
    const std::uint8_t* AvailabilityAt4E4C74(std::int32_t index) override;
    std::int32_t PollClickedEventManagerAt50FDB0(GuiClickedEventReferences* manager,
        std::int32_t clearMatched) override;

private:
    EnvironmentStartupRuntime& environment_;
    // 与既有 GuiClickedEventNative 相同：分配须兼容全局 sized delete。
    // 不将分配器塞进原0Ch正文或18h控制块；此端口不拥有任何已返回块。
    IEnvironmentRootMemory& memory_;
};

// 共享+84现已调用同一4787E0叶；此名称只作借用转发，不复制算法或旁路动态分派。
std::uint8_t* BorrowRootOptionCharDisplayAt4787E0(EnvironmentSoundParamsArchive& receiver,
    std::int32_t index) noexcept;
}
