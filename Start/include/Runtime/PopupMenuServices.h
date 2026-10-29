#pragma once
#include <Runtime/UnSaveDataRuntime.h>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
// 原A4h之外的现代业务边界；实际字段归唯一root/environment/scene所有。
class IPopupMenuDialogServices
{
public:
    virtual ~IPopupMenuDialogServices() = default;
    virtual UnSaveDataRuntime& UnSaveAt4180C0() = 0;
    virtual std::int32_t TextGlyphCursorAt50() = 0;
    virtual bool ToolWindowEnabledAtSlot2() = 0;
    virtual void RefreshToolWindowAt434FB0(std::int32_t value) = 0;
    virtual void WriteScenePendingCommandAt1C(std::string_view cp932) = 0;
    virtual void ReplayEnvironmentVoiceBatchAt42C4F0() = 0;
    virtual void PopSceneAt5328E0() = 0;
    virtual std::int32_t ReadGuiWidthAt18() = 0;
    virtual std::int32_t ReadGuiHeightAt1C() = 0;
    virtual void MovePointerAt4512E0(POINT logical) = 0;
};
}
