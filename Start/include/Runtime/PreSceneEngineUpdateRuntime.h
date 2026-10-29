#pragma once

#include <Runtime/SoundMappingRestoreRuntime.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 46D420 使用的字段尚未全部获得业务名，因此接口保留描述项与对象 offset。
class IPreSceneEngineUpdateState
{
public:
    virtual ~IPreSceneEngineUpdateState() = default;

    [[nodiscard]] virtual std::uint32_t ConfigDescriptor2Field14() const = 0;
    // 46D547..46D558只捕获一次receiver，然后原地AND；不得拆成两次registry取源。
    virtual void MaskConfigDescriptor2Field14(std::uint32_t mask) = 0;

    [[nodiscard]] virtual bool InputDescriptor0Byte13() const = 0;
    [[nodiscard]] virtual bool InputDescriptor0Byte14() const = 0;
    [[nodiscard]] virtual bool InputDescriptor2Action(std::uint32_t action) const = 0;
    virtual void SetInputDescriptor0Byte14(std::uint8_t value) = 0;
    virtual void SetInputDescriptor0Byte13(std::uint8_t value) = 0;
    virtual void SetInputDescriptor0Field28(std::uint32_t value) = 0;

    [[nodiscard]] virtual bool ConfigDescriptor1Field38Bit20() const = 0;
};

class IPreSceneEngineUpdateServices
{
public:
    virtual ~IPreSceneEngineUpdateServices() = default;

    // 接收对象固定为 CDrawManager owning 向量索引 2，即 MesWinBtn::CMesWinBtn。
    virtual void InvokeMesWinButtons434FB0(std::int32_t argument) = 0;

    // 46D62E..46DB6B：读取 CUnSaveData+0x30 并恢复两组专用音效槽。
    virtual void RestoreSoundMappings() = 0;
};

class IMesWinButtonsUpdateBoundary
{
public:
    virtual ~IMesWinButtonsUpdateBoundary() = default;
    virtual void Invoke434FB0(std::int32_t argument) = 0;
};

// 把 46D420 的两个生产 receiver 固定到同一 services 对象：
// CDrawManager 索引 2 的 MesWinBtn，以及 CUnSaveData 音效映射恢复链。
class PreSceneEngineUpdateServicesRuntime final : public IPreSceneEngineUpdateServices
{
public:
    PreSceneEngineUpdateServicesRuntime(
        IMesWinButtonsUpdateBoundary& mesWinButtons,
        SoundMappingRestoreRuntime& soundMappingRestore) noexcept;

    void InvokeMesWinButtons434FB0(std::int32_t argument) override;
    void RestoreSoundMappings() override;

private:
    IMesWinButtonsUpdateBoundary* mesWinButtons_ = nullptr;
    SoundMappingRestoreRuntime* soundMappingRestore_ = nullptr;
};

// 主帧中位于输入更新之后、SceneController 更新之前的 46D420 前半段。
class PreSceneEngineUpdateRuntime final
{
public:
    PreSceneEngineUpdateRuntime(
        IPreSceneEngineUpdateState& state,
        IPreSceneEngineUpdateServices& services) noexcept;

    void Update();

private:
    IPreSceneEngineUpdateState* state_ = nullptr;
    IPreSceneEngineUpdateServices* services_ = nullptr;
};
}
