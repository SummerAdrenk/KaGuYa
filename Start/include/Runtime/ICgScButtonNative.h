#pragma once
#include <Runtime/ModeButtonNative.h>

namespace KaGuYa::Start::Runtime
{
// 5B49D0公共15槽；Self是原ICgScBtnProc地址，不把兄弟Replay正文冒充CG派生对象。
struct ICgScButtonNativeVtable15 final
{
    void*(__thiscall* deleteAt00)(void*,std::uint32_t);
    std::int32_t(__thiscall* processAt04)(void*);
    std::int32_t(__thiscall* invalidateAt08)(void*,RECT);
    std::uint8_t(__thiscall* releaseAt0C)(void*);
    std::int32_t(__thiscall* inputAt10)(void*);
    void(__thiscall* resetAllAt14)(void*);
    std::int32_t(__thiscall* resetAt18)(void*,std::int32_t);
    std::int32_t(__thiscall* focusAt1C)(void*,std::int32_t);
    std::int32_t(__thiscall* pressAt20)(void*,std::int32_t);
    std::int32_t(__thiscall* commitAt24)(void*,std::int32_t);
    std::int32_t(__thiscall* selectAt28)(void*,std::int32_t);
    std::int32_t(__thiscall* currentAt2C)(void*);
    std::int32_t(__thiscall* navigateAt30)(void*);
    void(__thiscall* drawAt34)(void*);
    void(__thiscall* compositeAt38)(void*,GraphicsSurfaceNativeBody*,const RECT*);
};
static_assert(sizeof(ICgScButtonNativeVtable15)==0x3C);
const ICgScButtonNativeVtable15& ICgScButtonNativeVtable()noexcept;
void DestroyCgScButtonNativeAt4A4C60(void*)noexcept;
std::int32_t ProcessCgScButtonNativeAt4BEAF0(void*);
std::int32_t ReadCgScButtonInputNativeAt4A9FC0(void*);
std::int32_t NavigateCgScButtonNativeAt4B4BC0(void*);
void DrawCgScButtonNativeAt4ABE80(void*);
void CompositeCgScButtonNativeAt4A80B0(void*,GraphicsSurfaceNativeBody*,const RECT*);
void ResetCgScButtonNativeAt4C1260(void*,std::int32_t);
}
