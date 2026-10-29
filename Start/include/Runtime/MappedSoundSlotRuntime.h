#pragma once

#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScrVmSoundHandler.h>

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 0x4D6640/0x4D62A0 的共用生产实现；Text/Menu 必须调用同一映射状态和声音槽对象。
class MappedSoundSlotRuntime final
{
public:
    MappedSoundSlotRuntime(
        const IScrResourceNameSource& resourceNames,
        IScrVmSoundRuntimeBoundary& soundRuntime) noexcept;

    [[nodiscard]] std::size_t FirstCount() const;
    [[nodiscard]] std::size_t SecondCount() const;
    void ApplyFirst(std::int32_t resourceId, std::size_t groupIndex);
    void ApplySecond(std::int32_t resourceId, std::size_t groupIndex);

private:
    void Apply(
        ScrVmSoundSlotGroup group,
        std::int32_t resourceId,
        std::size_t groupIndex,
        std::size_t absolutePort);

    const IScrResourceNameSource* resourceNames_ = nullptr;
    IScrVmSoundRuntimeBoundary* soundRuntime_ = nullptr;
};
}
