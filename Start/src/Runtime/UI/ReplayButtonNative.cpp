#include <Runtime/ReplayButtonNative.h>
#include <Runtime/ICgScButtonNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
ReplayButtonNativeBody84h* __fastcall Delete(ReplayButtonNativeBody84h* body,void*,unsigned flags)
{return DeleteReplayButtonNativeAt4A6D70(body,flags);}
}
const ReplayButtonNativeVtable15& ReplayButtonNativeVtable() noexcept
{
    // 原5B529C仅首槽不同；其余保持同一函数地址，不复制算法或把Replay正文强转CG。
    static const auto methods=[]
    {
        auto result=std::bit_cast<ReplayButtonNativeVtable15>(ICgScButtonNativeVtable());
        result.deleteAt00=reinterpret_cast<decltype(result.deleteAt00)>(&Delete);
        return result;
    }();
    return methods;
}
std::int32_t ProcessReplayButtonNativeAt4BEAF0(ReplayButtonNativeBody84h& body)
{return ProcessCgScButtonNativeAt4BEAF0(&body);}
std::int32_t ReadReplayButtonInputNativeAt4A9FC0(ReplayButtonNativeBody84h& body)
{return ReadCgScButtonInputNativeAt4A9FC0(&body);}
std::int32_t NavigateReplayButtonNativeAt4B4BC0(ReplayButtonNativeBody84h& body)
{return NavigateCgScButtonNativeAt4B4BC0(&body);}
void DrawReplayButtonNativeAt4ABE80(ReplayButtonNativeBody84h& body)
{DrawCgScButtonNativeAt4ABE80(&body);}
void CompositeReplayButtonNativeAt4A80B0(ReplayButtonNativeBody84h& body,GraphicsSurfaceNativeBody* target,const RECT* clip)
{CompositeCgScButtonNativeAt4A80B0(&body,target,clip);}
void ResetReplayButtonNativeAt4C1260(ReplayButtonNativeBody84h& body,std::int32_t invalidate)
{ResetCgScButtonNativeAt4C1260(&body,invalidate);}
}
