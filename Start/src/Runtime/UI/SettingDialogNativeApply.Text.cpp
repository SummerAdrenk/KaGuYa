#include "SettingDialogNativeApply.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace SettingDialogNativeApplyDetail;
namespace
{
int Add(int x,int y) noexcept{return std::bit_cast<int>(static_cast<unsigned>(x)+static_cast<unsigned>(y));}
bool DefaultNamesDiffer(const RuntimeCString& current,const char* original)
{
    if(!original)throw ATL::CAtlException(E_FAIL);
    return _mbscmp(reinterpret_cast<const unsigned char*>(current.GetString()),reinterpret_cast<const unsigned char*>(original))!=0;
}
RuntimeCString* TextCommand(void* fixed,RuntimeCString* result)
{
    using Function=RuntimeCString*(__thiscall*)(void*,RuntimeCString*);
    const void* table;std::memcpy(&table,fixed,4);Function function;
    std::memcpy(&function,static_cast<const std::byte*>(table)+4,4);return function(fixed,result);
}
void BeforePlayback(TextWindowLayerNativeBodyE8h& b,SettingDialogNativeApplyTextServices s)
{
    auto* descriptor=s.layout.ReadDescriptor();if(!descriptor)throw 1;
    const auto color=s.layout.ReadBodyColor();static_cast<void>(s.layout.PrepareFont(descriptor->bodyFont,color,nullptr));
    // 字体叶返回后才读取旧descriptor的x/y与当前E8 offset；不提前快照坐标。
    const int dx=Read(descriptor->bodyX),dy=Read(descriptor->bodyY);
    const int x=Add(dx,Read(b.bodyOffsetXAtD8));
    const int y=Add(dy,Read(b.bodyOffsetYAtDC));
    if(!Read(b.bodyAt88.baseAt00))throw 1;
    auto* command=Resolve<void>(b.bodyAt88);TextStorage text;
    auto* returned=TextCommand(command,&text.value);
    struct Unwind final{RuntimeCString* p;~Unwind() noexcept{if(p)DestroyRuntimeCStringAt40CD40(p);}} unwind{&text.value};
    const bool success=s.text.InitializeText(returned->GetString(),x,y);
    // 433D09先撤CString EH，再按之前选中的chars/header正常Free；失败不重试。
    const char* captured=text.value.GetString();unwind.p=nullptr;
    ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(captured)-16));
    if(!success)throw 1;
}
void DuringPlayback(TextWindowLayerNativeBodyE8h& b,SettingDialogNativeApplyTextServices s)
{
    auto& services=RequireSettingDialogNativeApplyServices();
    try
    {
        b.shadowAt98=Read(Field<std::uint8_t>(Registry(1,0x433D3F),0x60));
        b.renderAt99=Read(Field<std::uint8_t>(Registry(1,0x433D56),0x61));
        ClearTextWindowPreviousRegionNativeAt418EE0(b,s.window);
        auto* fixedSave=Registry(1,0x433D74);
        auto* firstSystem=GameSystem(0x433D84,false);
        const char* defaultFirst=firstSystem->defaultGivenName.Native().GetString();
        bool changed=DefaultNamesDiffer(Field<RuntimeCString>(fixedSave,0x58),defaultFirst);
        if(!changed)
        {auto* secondSystem=GameSystem(0x433DC5,false);changed=DefaultNamesDiffer(Field<RuntimeCString>(fixedSave,0x5C),secondSystem->defaultFamilyName.Native().GetString());}
        auto* files=RootFiles(0x433E14);auto* message=Resolve<MessageOwnerNative34h>(files->messageAt10);
        MessageFormatSelectionRawTarget output{b.nameAt70,b.voicesAt7C,b.bodyAt88};
        const auto format=Read(b.formatIndexAt94); // 原在固定File/Message receiver之后读94。
        const auto read=message->ReadFormatRawAt417C40(format,output,changed?1u:0u,true);
        if(static_cast<unsigned>(read)>1u)throw 1;
        auto* unsaved=Registry(2,0x433E7F);AssignMessageReferenceAt410360(Field<Slot>(unsaved,0x3C),b.voicesAt7C);
        TextWindowNativeText layout(b,s.text,s.glyphs,s.layout,services.strings);
        static_cast<void>(layout.PrepareTextAt415150());
        const int available=s.text.GetTextLength();const int cursor=Read(b.glyphCursorAt50);
        const unsigned end=static_cast<unsigned>(cursor>=0?cursor:available);
        static_cast<void>(layout.BlitRangeAt419090(0,end));
        // 433C50不重置unread、playbackStarted、wait/cursor，不借4C1650的catch/重置行为。
    }
    catch(...)
    {
        auto* environment=services.source.CaptureEnvironmentAt4180C0(0x433ED0);
        services.source.MessageBoxA(reinterpret_cast<HWND>(Read(environment->windowAt2C)),
            SettingDialogNativeApplyTextErrorSample18(),environment->titleAt10.GetString(),MB_ICONERROR);
        // 原started区域catch显示错误后正常返回；错误叶自身失败仍传播，不改成成功stub。
    }
}
}
void RestoreSettingDialogTextNativeAt433C50(TextWindowLayerNativeBodyE8h& body)
{
    if(!Read(body.bodyAt88.baseAt00))return;
    const bool started=Read(body.playbackStartedAt54)!=0;
    auto bindings=RequireSettingDialogNativeApplyServices().textBindings.ForFixedBody(body);
    if(started)DuringPlayback(body,bindings);else BeforePlayback(body,bindings);
}
}
