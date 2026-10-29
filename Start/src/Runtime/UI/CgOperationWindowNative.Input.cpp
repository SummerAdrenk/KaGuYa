#include <Runtime/CgOperationWindowNative.h>
#include "CgOperationDialogNative.Internal.h"
#include <Runtime/GuiDialogIntoStateNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <mbstring.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=CgOperationWindowNativeBody30h;
bool Compare(ICgOperationWindowNativeSource& source,unsigned site,const char* expected)
{
    auto& text=source.CaptureScenesAt413BE0(site)->commandAt1C;
    auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(text.GetString()))-1;
    auto* clone=CloneRuntimeCStringDataAt414540(header);
    const bool equal=::_mbscmp(reinterpret_cast<const unsigned char*>(clone+1),
        reinterpret_cast<const unsigned char*>(expected))==0;
    ReleaseRuntimeCStringHeader(clone);return equal;
}
constexpr char menu[]="\x83\x81\x83\x6A\x83\x85\x81\x5B\x95\x5C\x8E\xA6";
constexpr char next[]="\x8E\x9F\x82\xD6";
constexpr char previous[]="\x91\x4F\x82\xD6";
constexpr char hide[]="\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x8F\xC1\x8B\x8E";
constexpr char back[]="\x96\xDF\x82\xE9";
bool HideOrMouse(CgOperationWindowNativeServices& services)
{
    auto& text=services.source.CaptureScenesAt413BE0(0x4BD7B1)->commandAt1C;
    auto* clone=CloneRuntimeCStringDataAt414540(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(text.GetString()))-1);
    union Text{RuntimeCString value;Text(){}~Text(){}} temporary;
    auto* chars=reinterpret_cast<char*>(clone+1);std::memcpy(&temporary.value,&chars,4);
    struct Cleanup{RuntimeCString& value;bool active=true;~Cleanup() noexcept{if(active)DestroyRuntimeCStringAt40CD40(&value);}} cleanup{temporary.value};
    bool result=::_mbscmp(reinterpret_cast<const unsigned char*>(temporary.value.GetString()),
        reinterpret_cast<const unsigned char*>(hide))==0;
    if(!result)
    {
        auto* root=services.source.CaptureEnvironmentInlineAt6198C8(0x4BD812);
        auto& archives=root->archivesAt30;
        const auto count=std::uint32_t(std::bit_cast<std::int32_t>(std::uint32_t(
            reinterpret_cast<std::uintptr_t>(archives.end)-reinterpret_cast<std::uintptr_t>(archives.begin)))/12);
        void* archive=nullptr;
        if(count>5)archive=ResolveEnvironmentObjectReference<void>(archives.begin[5]);
        // 原无可用slot5仍读null+14；不把坏对象图当成“鼠标没有按下”。
        const auto selector=*reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(archive)+0x14);
        result=ReadMouseSelectorAt4B0630(services.input,selector);
    }
    cleanup.active=false;DestroyRuntimeCStringAt40CD40(&temporary.value);return result;
}
GuiDialogNativeBody5Ch* Dialog(Body& body) noexcept
{return &ResolveCgOperationDialogNative(body.dialogAt24)->into.common;}
void ResetPointer(CgOperationWindowNativeServices& services)
{
    auto* cursor=ResolveEnvironmentObjectReference<CursorNativeBody10h>(services.input.CaptureAt418210()->cursorAt0C);
    using Reset=std::uint8_t(__thiscall*)(CursorNativeBody10h*);
    Reset reset;std::memcpy(&reset,reinterpret_cast<const std::byte*>(cursor->vptrAt00)+8,4);
    InputNativeScope scope(services.inputServices);(void)reset(cursor);
}
int WindowResult(int result)
{
    switch(result)
    {
    case -1:return -1;
    case 0:return 4;case 1:return 3;case 2:return 1;case 3:return 5;case 4:return 6;
    default:throw std::logic_error("CG operation dialog returned an unknown action");
    }
}
int LegacyRequest(Body& body,CgOperationAction action)
{
    // 01原4A7BD0返回0/2后由4A5630跳可用差分；共享Scene不消费0/2，先适配为选择变化6。
    auto* dialog=ResolveCgOperationDialogNative(body.dialogAt24);
    if(!dialog)throw std::logic_error("CG operation request has no dialog");
    return WindowResult(CgOperationNativeDetail::DispatchAction(*dialog,action));
}
}
std::int32_t PollCgOperationWindowNativeAt4BD590(Body& body,CgOperationWindowNativeServices& services)
{
    CgOperationDialogNativeScope scope(services.dialog);
    std::int32_t result=-1;
    const bool legacy=CurrentCgOperationResourceProfile().encoding==CgOperationVisualEncoding::NativeSevenState;
    bool consumed=false;
    if(!ReadFirstNestedGuiInactiveAt5103C0(*Dialog(body)))
    {
        if(Compare(services.source,0x4BD5F7,menu))
        {consumed=true;result=legacy?LegacyRequest(body,CgOperationAction::OpenPopup):1;}
        else if(Compare(services.source,0x4BD6BE,next))
        {consumed=true;result=legacy?LegacyRequest(body,CgOperationAction::Next):2;}
        else if(Compare(services.source,0x4BD739,previous))
        {consumed=true;result=legacy?LegacyRequest(body,CgOperationAction::Previous):0;}
        else if(HideOrMouse(services)){consumed=true;result=3;}
        else if(Compare(services.source,0x4BD99F,back)){consumed=true;result=4;}
    }
    // 旧请求已消费但仅剩一个可用差分时可不产生选择变化；命令仍须清空。
    if(consumed)
        SetRuntimeCStringAt437680(services.source.CaptureScenesAt413BE0(0x4BD66D)->commandAt1C,"",0);
    else
    {
        auto* dialog=Dialog(body);(void)dialog->vptrAt00->pollAt48(dialog);
        dialog=Dialog(body);const auto value=dialog->vptrAt00->resultAt50(dialog);
        result=WindowResult(value);
    }
    SynchronizeCgOperationWindowDirtyNativeAt4AB980(body);return result;
}
std::uint8_t HitCgOperationWindowNativeAt4C2690(Body& body,CgOperationWindowNativeServices& services)
{
    CgOperationDialogNativeScope scope(services.dialog);
    auto* mouse=ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(services.input.CaptureAt418210()->mouseAt00);
    // 原先固定mouse receiver，随后才解析dialog；Hit内部可重入，成功后独立Capture cursor。
    if(!HitNestedGuiControlsAt50BAB0(*Dialog(body),mouse->logicalAt18))return 0;
    ResetPointer(services);return 1;
}
}
