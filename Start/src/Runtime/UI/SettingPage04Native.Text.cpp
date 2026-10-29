#include "SettingPage04Native.Internal.h"

namespace KaGuYa::Start::Runtime
{
bool SettingPage04NativeTextApi::CreateFontAt4388B0(const GraphicsTextNativeFontParameters* p)
{return api_.CreateTextFontNativeAt4388B0(p);}
bool SettingPage04NativeTextApi::InitializeAt4389D0(const char* p,int x,int y)
{return api_.InitializeText(p,x,y);}
bool SettingPage04NativeTextApi::BlitAllAt53A980(int handle,RECT* out)
{return api_.BlitAllTextToSurface(handle,out);}
namespace SettingPage04NativeDetail
{
namespace
{
int Width(int length,int height) noexcept {return std::bit_cast<int>((static_cast<unsigned>(length/2)+1u)*static_cast<unsigned>(height));}
int Sub(int x,int y) noexcept {return std::bit_cast<int>(static_cast<unsigned>(x)-static_cast<unsigned>(y));}
int Add(int x,int y) noexcept {return std::bit_cast<int>(static_cast<unsigned>(x)+static_cast<unsigned>(y));}
int NameTextState(Common* p)
{
    const int id=CurrentSettingOtherProfile().name.textStateControl;
    if(id<0)return 1;
    auto* c=FindControl(p,id);
    if(!c)throw std::logic_error("Other name text-state control is missing");
    return ControlTable(c).readStateAt48(c);
}
void SetFont(GraphicsTextNativeFontParameters& p,const SettingOtherTextStyle& style,const char* face)
{
    if(!face||!*face||style.fontCp932.empty())
        throw std::logic_error("Other text font was not initialized");
    InitializeGraphicsTextFontNativeAt40C6E0(p);
    p.fontAt00.lfHeight=style.height;p.fontAt00.lfWeight=style.weight;
    ::lstrcpynA(p.fontAt00.lfFaceName,face,32);
    p.renderModeAt3E=style.renderMode;p.characterSpacingAt3C=style.characterSpacing;
    p.lineSpacingAt3D=style.lineSpacing;p.verticalAt55=style.vertical;
    p.textColorAt40=style.normalColor;p.shadowColorAt44=style.noShadowColor;
    p.shadowXAt4C=style.shadowOffset.x;p.shadowYAt50=style.shadowOffset.y;
}
void CloneText(Text& out,const RuntimeCString& source)
{
    auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(source.GetString())-16);
    auto* cloned=CloneRuntimeCStringDataAt414540(header);char* data=reinterpret_cast<char*>(cloned)+16;
    std::memcpy(&out.value,&data,4);out.active=true;
}
}
void BuildNameTextAt512200(Common* p,int mode,int shadow)
{
    auto& s=RequireSettingPage04NativeServices();GraphicsTextNativeFontParameters parameters;
    const auto& style=CurrentSettingOtherProfile().name.style;
    SetFont(parameters,style,s.source.FontFaceAt61A64C());
    const int height=-style.height;
    parameters.renderModeAt3E=mode?style.renderMode:1;
    // 原5122EB的00606060是COLORREF立即值；不把它解释为可解引用的原VA。
    parameters.textColorAt40=NameTextState(p)?style.normalColor:style.disabledColor;
    if(shadow){parameters.shadowColorAt44=NameTextState(p)?style.normalColor:style.disabledColor;parameters.shadowXAt4C=1;parameters.shadowYAt50=2;}
    try
    {
    if(!s.text.CreateFontAt4388B0(&parameters))throw 1;
    Text text;CloneText(text,static_cast<SaveDataEnvironmentArchive*>(Registry(1))->stringAt5C);
    AppendRuntimeCStringAt413550(text.value," ",1);
    auto& suffix=static_cast<SaveDataEnvironmentArchive*>(Registry(1))->stringAt58;
    AppendRuntimeCStringAt413550(text.value,suffix.GetString(),suffix.GetLength());
    const auto* captured=text.value.GetString();const int width=Width(text.value.GetLength(),height);
    auto* created=NewGraphicsSurfaceAt40C2E0(s.surfaces,s.strings);AdoptGraphicsSurfaceAt407A80(Field<Slot>(p,0x7C),created);
    if(!Resolve<Surface>(Field<Slot>(p,0x7C))->CreateAt415010(s.surfaces,width,height,1,1))throw 1;
    if(!s.text.InitializeAt4389D0(captured,0,0))throw 1;
    const auto handle=Read(Resolve<Surface>(Field<Slot>(p,0x7C))->handleAt04);
    if(!s.text.BlitAllAt53A980(handle,nullptr))throw 1;
    text.active=false;ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(captured)-16));
    }
    catch(...)
    {
        // FuncInfo5F99B0/try5F999C登记state4..6；51252F调用4155F0后正常返回。
        auto& slot=Field<Slot>(p,0x7C);
        if(Read(slot.controlAt08)){ReleaseOld(slot);slot.controlAt08=nullptr;slot.baseAt00=0;}
    }
}
bool BuildBlock03Text(TextBlock& p,RECT& bounds)
{
    auto& s=RequireSettingPage04NativeServices();GraphicsTextNativeFontParameters parameters;
    const auto& version=CurrentSettingOtherProfile().versions;
    if(!version.present)throw std::logic_error("Other version block has no text profile");
    SetFont(parameters,version.style,s.source.FontFaceAt61A650());
    const int height=-version.style.height;
    try
    {
    if(!s.text.CreateFontAt4388B0(&parameters))throw 1;
    for(unsigned i=0;i<2;++i)
    {
        const auto offset=i?0x18u:0x20u;
        const int width=Width(Field<RuntimeCString>(Registry(0),offset).GetLength(),height);
        auto* created=NewGraphicsSurfaceAt40C2E0(s.surfaces,s.strings);Fresh surface;
        ConstructGraphicsSurfaceReferenceAt451500(surface.slot,created,1);RefLife life{surface.slot};
        if(!Resolve<Surface>(surface.slot)->CreateAt415010(s.surfaces,width,height,1,1))throw 1;
        const auto anchor=version.anchors[i];
        auto* current=Resolve<Surface>(surface.slot);
        current->originXAt10=version.placement==SettingOtherPlacement::RightAligned?Sub(anchor.x,width):anchor.x;
        current->originYAt14=anchor.y;
        const auto* chars=Field<RuntimeCString>(Registry(0),offset).GetString();
        if(!s.text.InitializeAt4389D0(chars,0,0))throw 1;
        const auto handle=Read(Resolve<Surface>(surface.slot)->handleAt04);if(!s.text.BlitAllAt53A980(handle,nullptr))throw 1;
        AppendGuiControlSurfaceReference(p.surfacesAt80,surface.slot); // 53DEA0借Ref，不消费参数。
        current=Resolve<Surface>(surface.slot);
        const RECT rectangle{anchor.x,anchor.y,Add(anchor.x,Read(current->widthAt08)),Add(anchor.y,Read(current->heightAt0C))};
        ::UnionRect(&bounds,&bounds,&rectangle);
        if(i==0)life.active=false; // 第一张40D2F0前state降2，第二张仍在state6。
        ReleaseYtlReference(surface.slot);
        life.active=false; // 第二张正常释放失败仍保留局部EH资格，不共用第一张尾。
    }
    return true;
    }
    catch(...)
    {
        // 4E985B/5F9C90：只Clear元素，保留vector capacity；失败不发布bounds64。
        ReleaseGuiControlSurfaceRangeAt43EA10(p.surfacesAt80.begin,p.surfacesAt80.end);
        p.surfacesAt80.end=Read(p.surfacesAt80.begin);
        return false;
    }
}
}
}
