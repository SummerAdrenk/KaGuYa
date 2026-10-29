#pragma once
#include <Runtime/LikabilityDialogNative.h>
#include <Runtime/LikabilityResourceProfile.h>
#include <Runtime/LikabilityProfileRuntime.h>

namespace KaGuYa::Start::Runtime
{
class LikabilityProfileState;
// 旧94h前缀之后的单一现代owner持有state；不在共享Root或全局map维护旁路对象。
LikabilityProfileState* ConstructLikabilityProfileGui(LikabilityDialogNativeBody94h&,
    const LikabilityResourceProfile&, ILikabilityProfileHost&);
void DestroyLikabilityProfileState(LikabilityProfileState*) noexcept;
void RefreshLikabilityProfileGui(LikabilityProfileState&);
void TickLikabilityProfileGui(LikabilityProfileState&);
void DispatchLikabilityProfileEvent(LikabilityProfileState&, std::int32_t);
void CommitLikabilityProfileClose(LikabilityProfileState&);
bool LikabilityProfileHasCancelInput(const LikabilityProfileState&) noexcept;
}
