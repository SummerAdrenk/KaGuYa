#include <Runtime/LayerWindowDrawStateRuntime.h>
#include <Platform/Input.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>
const T& RequireDrawArchive(const EnvironmentStartupRuntime& environment, const std::size_t index)
{
    // 482957/48296B各自解析正文，不检查原417990没有的类型标签。
    const auto* body = static_cast<T*>(environment.Archives().CaptureArchiveAt4180C0(static_cast<std::uint32_t>(index)));
    if (!body) throw std::logic_error("LayerWindow draw archive slot is empty (safe boundary)");
    return *body;
}
}

LayerWindowDrawStateRuntime::LayerWindowDrawStateRuntime(
    const Platform::IInputRuntime& input, const EnvironmentStartupRuntime& environment) noexcept
    : input_(&input), environment_(&environment) {}

LayerWindowDrawStateRuntime::LayerWindowDrawStateRuntime(
    Platform::KeyboardView keyboard, const UnSaveDataRuntime& unsaved,
    const SaveDataEnvironmentArchive& saved) noexcept
    : keyboard_(keyboard), unsaved_(&unsaved), saved_(&saved) {}

bool LayerWindowDrawStateRuntime::AnyActionSixCurrentState() const
{
    // 48290C..482932：每次解 Input+18 的当前 Keyboard，不附加 Mouse/Environment 就绪门控。
    const auto keyboard = input_ ? input_->KeyboardOrNull() : keyboard_;
    if (keyboard == nullptr)
        throw std::logic_error("LayerWindow draw requires the current Keyboard");
    // 原 44FCC2 固定创建 88 个容器。现代注入对象不足以容纳 action 6 时明确拒绝。
    if (keyboard->ActionCapacity() <= 6)
        throw std::out_of_range("LayerWindow keyboard state has no action six container");
    // 482932..482947：+5C/+60 是 action 6 的三字节绑定数组，+10 选择当前双缓冲。
    for (std::size_t index = 0; index < keyboard->BindingCount(6); ++index)
        if (keyboard->CurrentState(6, index) != 0)
            return true;
    return false;
}

std::uint32_t LayerWindowDrawStateRuntime::UnSaveFlagsAt14() const
{
    // 48294E..48295C：仅在调用本查询时读取 slot2；Draw 保持原 bit1 短路。
    return environment_ ? RequireDrawArchive<UnSaveDataRuntime>(*environment_, 2).FlagsAt14()
                        : unsaved_->FlagsAt14();
}

std::uint8_t LayerWindowDrawStateRuntime::SaveByteAt6F() const
{
    // 482962..482970：与 slot2 独立晚取，不跨查询保留 SaveData 引用。
    return environment_ ? RequireDrawArchive<SaveDataEnvironmentArchive>(*environment_, 1).valueAt6F
                        : saved_->valueAt6F;
}
}
