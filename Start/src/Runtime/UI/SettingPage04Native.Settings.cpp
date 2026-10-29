#include "SettingPage04Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage04NativeDetail
{
void* Registry(unsigned index)
{return ResolveEnvironmentArchiveAt417990(*RequireSettingPage04NativeServices().source.CaptureEnvironmentAt4180C0(),index);}

const SettingOtherBlockProfile& ProfileFor(Common* p)
{
    for(const auto& block:CurrentSettingOtherProfile().blocks)
        if(block.dialogId==p->idAt08)return block;
    throw std::logic_error("Unknown Other block dialog id");
}
Common* FindRadio(Common* p,int id)
{
    Fresh f;Into(p).findDialogAt90(p,&f.slot,id);RefLife life{f.slot};
    return Resolve<Common>(f.slot);
}
Control* FindControl(Common* p,int id)
{
    Fresh f;Read(p->vptrAt00)->findChildAt14(p,&f.slot,id);RefLife life{f.slot};
    if(auto* c=Resolve<Control>(f.slot))return c;
    // Only explicitly declared check owners are searched, not an untyped object map.
    const auto& block=ProfileFor(p);
    if(block.radios.empty())for(const auto& check:block.checks)
        if(check.eventAt00==id)if(auto* owner=FindRadio(p,id))
        {
            Fresh child;Read(owner->vptrAt00)->findChildAt14(owner,&child.slot,id);RefLife childLife{child.slot};
            return Resolve<Control>(child.slot);
        }
    return nullptr;
}
namespace
{
SaveDataEnvironmentArchive& Save(){return *static_cast<SaveDataEnvironmentArchive*>(Registry(1));}
std::uint16_t& Word(SettingOtherWord field)
{
    auto& words=static_cast<GlobalParamsArchive*>(Registry(7))->wordsAt0C;
    switch(field)
    {
    case SettingOtherWord::InsertionChoice:return words[2];
    case SettingOtherWord::OralChoice:return words[3];
    case SettingOtherWord::CountdownPosition:return words[5];
    case SettingOtherWord::CountdownSize:return words[6];
    case SettingOtherWord::CountdownColor:return words[7];
    }
    throw std::logic_error("Unknown Other word field");
}
std::uint16_t& Alpha(){return static_cast<GlobalParamsArchive*>(Registry(7))->wordsAt0C[8];}
bool IsCheck(const SettingOtherBoolBinding& b){return b.trueEvent<0&&b.falseEvent<0;}

void LoadBindings(Common* p)
{
    const auto& block=ProfileFor(p);
    for(const auto& b:block.booleans)
    {
        if(IsCheck(b))
        {
            if(auto* c=FindControl(p,b.controlOrRadioId))
                Field<int>(c,0x5C)=ReadBool(b.field)==b.checkboxStoredValueWhenChecked;
        }
        else if(auto* radio=FindRadio(p,b.controlOrRadioId))
            WriteRadio(radio,ReadBool(b.field)?b.trueEvent:b.falseEvent);
    }
    for(const auto& b:block.words)if(auto* radio=FindRadio(p,b.radioId))
    {
        const auto value=Word(b.field);int selected=b.otherReadEvent;
        for(const auto& choice:b.values)if(choice.value==value){selected=choice.event;break;}
        WriteRadio(radio,selected);
    }
    for(const auto& slider:block.sliders)if(auto* c=FindControl(p,slider.eventAt00))
        SliderTable(c).setValueAt64(c,std::bit_cast<int>(Save().valueAt70));
    for(const auto& indicator:block.backendIndicators)if(auto* c=FindControl(p,indicator.control))
    {
        std::uint32_t backend;std::memcpy(&backend,Save().bytesAt04.data()+4,sizeof backend);
        ControlTable(c).writeEnabledAt4C(c,backend==indicator.value);
    }
    Field<int>(p,0x68)=0;
}
void StoreBindings(Common* p)
{
    if(!Read(Field<int>(p,0x68)))
    {
        const auto& block=ProfileFor(p);
        for(const auto& b:block.booleans)
        {
            if(b.commit!=SettingOtherCommit::OnBlockEnd)continue;
            if(IsCheck(b))
            {
                if(auto* c=FindControl(p,b.controlOrRadioId))
                    WriteBool(b.field,(Read(Field<int>(c,0x5C))!=0)==b.checkboxStoredValueWhenChecked);
            }
            else if(auto* radio=FindRadio(p,b.controlOrRadioId))
            {
                // Original bool stores use != yes for skip and == yes for effects.
                const auto selected=ReadRadio(radio);
                WriteBool(b.field,b.field==SettingOtherBool::EffectsEnabled?selected==b.trueEvent:selected!=b.falseEvent);
            }
        }
        for(const auto& b:block.words)if(auto* radio=FindRadio(p,b.radioId))
        {
            const auto selected=ReadRadio(radio);
            for(const auto& choice:b.values)if(choice.event==selected){Word(b.field)=choice.value;break;}
            // Unrecognized selection deliberately leaves the persisted word untouched.
        }
        for(const auto& slider:block.sliders)if(auto* c=FindControl(p,slider.eventAt00))
            Save().valueAt70=static_cast<unsigned>(SliderTable(c).valueAt60(c));
    }
    Field<int>(p,0x68)=1;
}
}
bool ReadBool(SettingOtherBool field)
{
    switch(field)
    {
    case SettingOtherBool::SkipStartup:return Save().bytesAt04[0]!=0;
    case SettingOtherBool::SkipNewGameName:return Save().valueAt54!=0;
    case SettingOtherBool::EffectsEnabled:return Save().valueAt6F!=0;
    case SettingOtherBool::CountdownHalfAlpha:return Alpha()!=0;
    }
    throw std::logic_error("Unknown Other bool field");
}
void WriteBool(SettingOtherBool field,bool value)
{
    switch(field)
    {
    case SettingOtherBool::SkipStartup:Save().bytesAt04[0]=value;return;
    case SettingOtherBool::SkipNewGameName:Save().valueAt54=value;return;
    case SettingOtherBool::EffectsEnabled:Save().valueAt6F=value;return;
    case SettingOtherBool::CountdownHalfAlpha:Alpha()=static_cast<std::uint16_t>(value);return;
    }
    throw std::logic_error("Unknown Other bool field");
}
bool BoolSelection(Common* p,const SettingOtherBoolBinding& b)
{
    if(IsCheck(b))
    {
        auto* c=FindControl(p,b.controlOrRadioId);
        return c&&(Read(Field<int>(c,0x5C))!=0)==b.checkboxStoredValueWhenChecked;
    }
    auto* radio=FindRadio(p,b.controlOrRadioId);
    return radio&&ReadRadio(radio)==b.trueEvent;
}
bool NameAvailable()
{
    if(CurrentSettingOtherProfile().name.capability==SettingOtherCapability::AlwaysAvailable)return true;
    auto* root=RequireSettingPage04NativeServices().source.CaptureEnvironmentAt4180C0();
    return Resolve<EnvironmentPathRuntime>(root->pathsAt48)->nameChangeAt1C!=0;
}
void ApplyNameDisabled(Common* p,std::span<const int> ids)
{
    if(NameAvailable())return;
    const auto button=CurrentSettingOtherProfile().name.buttonEvent;
    for(int id:ids)
    {
        if(auto* radio=FindRadio(p,id))Read(radio->vptrAt00)->writeEnabledAt24(radio,0);
        else if(auto* c=FindControl(p,id))
        {
            if(id==button)ControlTable(c).writeEnabledAt4C(c,0);
            else ControlTable(c).writeStateAt44(c,0);
        }
    }
}
void __fastcall Load00(Common* p,void*){LoadBindings(p);}
void __fastcall Store00(Common* p,void*){StoreBindings(p);}
void __fastcall Load01(Common* p,void*)
{LoadBindings(p);ApplyNameDisabled(p,CurrentSettingOtherProfile().name.disabledControlsAtLoad);}
void __fastcall Store01(Common* p,void*){StoreBindings(p);}
void __fastcall Load02(Common* p,void*)
{LoadBindings(p);MTable(p).gateAtA0(p,!ReadBool(SettingOtherBool::EffectsEnabled),0);}
void __fastcall Store02(Common* p,void*){StoreBindings(p);}
void __fastcall Load04(Common* p,void*){LoadBindings(p);}
void __fastcall Store04(Common* p,void*){StoreBindings(p);}
void __fastcall Load05(Common* p,void*)
{
    LoadBindings(p);
    // 515840: unknown values select hidden but only raw FFFF initially disables dependents.
    MTable(p).gateAtA0(p,Word(SettingOtherWord::CountdownPosition)==0xFFFF,0);
}
void __fastcall Store05(Common* p,void*){StoreBindings(p);}
void __fastcall ContainerBegin(Common* p,void*){GuiDialogIntoBaseVtable().common.beginAt44(p);CTable(p).loadAt98(p);}
int __fastcall ContainerEnd(Common* p,void*){CTable(p).storeAt9C(p);return 1;}
RECT* __fastcall ContainerBounds(Common* p,void*,RECT* out){*out=Field<RECT>(p,0x6C);return out;}
}
