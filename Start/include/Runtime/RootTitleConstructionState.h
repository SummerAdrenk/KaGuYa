#pragma once

#include <Runtime/TitleSceneProductionRuntime.h>
#include <Runtime/TitleMenuRuntimeServices.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class RootRuntimeOwner;

// 4A2850和标题菜单末段的真实状态；不缓存root、CParams或registry正文。
class RootTitleConstructionState final : public ITitleSceneConstructionState,
    public ITitleMenuRootState
{
public:
    RootTitleConstructionState(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
        ScriptNamedValueNativeTables tables, decltype(&::MessageBoxA) messageBox,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    void WriteSlot2DwordAt04(std::uint32_t) override;
    void WriteSlot2ByteAt24(std::uint8_t) override;
    void WriteSlot2ByteAt25(std::uint8_t) override;
    void WriteSlot2ByteAt26(std::uint8_t) override;
    void WriteSlot2ByteAt27(std::uint8_t) override;
    void WriteSlot2ByteAt08(std::uint8_t) override;
    void WriteSlot2ByteAt09(std::uint8_t) override;
    void WriteSlot2ByteAt0C(std::uint8_t) override;
    void WriteSlot2DwordAt18(std::uint32_t) override;
    void WriteSlot2DwordAt14(std::uint32_t) override;
    void WriteSlot2DwordAt1C(std::uint32_t) override;
    void WriteSlot2ByteAt29(std::uint8_t) override;
    void WriteSlot2ByteAt28(std::uint8_t) override;
    void ClearReplayAt4A2DD6() override;
    void WriteByteAt218(std::uint8_t) override;
    void WriteByteAt219(std::uint8_t) override;
private:
    EnvironmentStartupRuntime& environment_;
    RootRuntimeOwner& root_;
    ScriptNamedValueNativeTables tables_;
    decltype(&::MessageBoxA) messageBox_;
    IRuntimeStringManagerSource& strings_;
};
}
