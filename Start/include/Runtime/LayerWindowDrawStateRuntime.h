#pragma once
#include <Platform/InputViews.h>

#include <Runtime/LayerWindowEffectRuntime.h>

namespace KaGuYa::Start::Platform { class KeyboardView; class IInputRuntime; }

namespace KaGuYa::Start::Runtime
{
class SaveDataEnvironmentArchive;
class UnSaveDataRuntime;
class EnvironmentStartupRuntime;

class ILayerWindowDrawState
{
public:
    virtual ~ILayerWindowDrawState() = default;
    [[nodiscard]] virtual bool AnyActionSixCurrentState() const = 0;
    [[nodiscard]] virtual std::uint32_t UnSaveFlagsAt14() const = 0;
    [[nodiscard]] virtual std::uint8_t SaveByteAt6F() const = 0;
};

// 482900 的三个全局读取绑定真实对象，不能缓存为构造时的布尔快照。
class LayerWindowDrawStateRuntime final : public ILayerWindowDrawState
{
public:
    // 48A735 早于 48ABDA：仅借用稳定 owner，构造不解析尚未创建的 Keyboard/归档槽。
    // 两个 owner 必须长于本适配器；不持有旧槽的共享引用，也不缓存字段值。
    LayerWindowDrawStateRuntime(const Platform::IInputRuntime& input,
        const EnvironmentStartupRuntime& environment) noexcept;
    // 固定对象入口：保留既有受控调用，不模拟 Input 重建或 Environment 换槽。
    LayerWindowDrawStateRuntime(Platform::KeyboardView keyboard,
        const UnSaveDataRuntime& unsaved, const SaveDataEnvironmentArchive& saved) noexcept;
    bool AnyActionSixCurrentState() const override;
    std::uint32_t UnSaveFlagsAt14() const override;
    std::uint8_t SaveByteAt6F() const override;
private:
    const Platform::IInputRuntime* input_ = nullptr;
    const EnvironmentStartupRuntime* environment_ = nullptr;
    Platform::KeyboardView keyboard_ = nullptr;
    const UnSaveDataRuntime* unsaved_ = nullptr;
    const SaveDataEnvironmentArchive* saved_ = nullptr;
};
}
