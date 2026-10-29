#pragma once
#include <Runtime/PropertyPageSoundNative.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/RuntimeCStringResult.h>
#include <Runtime/GameBuildProfile.h>
#include <bit>
#include <algorithm>
#include <exception>
#include <cstring>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime::PropertySoundDetail
{
using Page=PropertyPageSoundNativeBodyA8h;
using Services=PropertyPageSoundNativeServices;
struct ListItem3Ch final
{
    UINT mask;int item,subItem;UINT state,stateMask;char* text;int textSize,image;LPARAM data;
    int indent,group;UINT columns;UINT* columnIds;int* formats;int groupIndex;
};
static_assert(sizeof(ListItem3Ch)==0x3C&&sizeof(TVITEMA)==0x28);
template<class T> T Read(const T& value) noexcept {return *static_cast<const volatile T*>(&value);}
template<class T> T At(const void* p,unsigned offset) noexcept
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<DWORD>(p)+offset);}
inline DWORD Address(const void* p) noexcept {return reinterpret_cast<DWORD>(p);}
inline DWORD Delta(const void* end,const void* begin) noexcept {return Address(end)-Address(begin);}
inline DWORD Count12(const void* end,const void* begin) noexcept
{return static_cast<DWORD>(std::bit_cast<int>(Delta(end,begin))/12);}
inline DWORD Count8(const void* end,const void* begin) noexcept
{return static_cast<DWORD>(std::bit_cast<int>(Delta(end,begin))>>3);}
void* Resolve(const YtlReferenceSlotRuntime&,bool biasFirst=false,bool reloadBase=false) noexcept;
SaveDataEnvironmentArchive* Save(Services&,bool getter=false,bool inlineCapture=false,bool beginFirst=false,
    bool biasFirst=false,bool reloadBegin=false);
EnvironmentSoundParamsArchive* Params(Services&,bool getter=false,bool inlineCapture=false,bool beginFirst=false);
EnvironmentSoundParamsArchive* RegistrationParams(Services&,bool biasFirst,bool reloadBegin);
UnSaveDataRuntime* UnSave(Services&);
void* Root(Services&);
SoundManagerNativeBody50h* Sound(void*,Services&);
SoundManagerNativeBody50h* Sound(Services&);
SoundManagerNativeBody50h* SoundGetter(void*,Services&);
SoundManagerNativeBody50h* SoundGetter(Services&);
HWND Item(Page&,int,Services&);
LRESULT Send(HWND,UINT,WPARAM,LPARAM,Services&);
void Checkboxes(Page&,bool read,Services&);
int Number(Page&,int,Services&);
int TypeNumber(Page&,int,Services&);
int ParseText(const RuntimeCString&,Services&);
int TreeNumber(Page&,HTREEITEM,Services&);
DWORD TreeData(Page&,HTREEITEM,Services&);
PropertyPageSoundVoiceParam10h* Voice(Page&,DWORD);
void SetNumber(Page&,int,int,Services&);
void Slider(Page&,int,int,Services&);
void Enable(Page&,int,BOOL,Services&);
void Style(HWND,LONG,LONG,Services&);
void TreeState(Page&,HTREEITEM,UINT,Services&);
UINT TreeState(Page&,HTREEITEM,Services&);
HTREEITEM Next(Page&,UINT,HTREEITEM,Services&);
bool VoiceKey(const PropertyPageSoundVoiceParam10h&,const char*);
extern const char EffectKey[],VoiceKeyText[],AlternateKey[],NameKey[],SurnameKey[];
void RefreshMusic(Page&,Services&); // 413900
void RefreshEffects(Page&,Services&); // 437F10
void RefreshVoice(Page&,Services&); // 439CA0
void RefreshLower(Page&,Services&); // 420DA0
void RefreshTree(Page&,Services&); // 4165B0 / 4163A0
inline bool CategorizedVoiceUi() noexcept
{return CompiledGameProfile().soundSettings==GameBuildProfile::SoundSettings::Categorized;}
HTREEITEM CategorizedVoiceLeaf(Page&,HTREEITEM group,Services&);
void SynchronizeCategorizedTreeClick(Page&,HTREEITEM changed,Services&);
void RefreshVolume(Page&,int,Services&); // 437840
LRESULT Command(Page&,WORD,HWND,Services&);
void AdvancePreviewCursor(std::uint32_t*,const EnvironmentVectorStorage<RuntimeCString>&);
LRESULT Scroll(Page&,HWND,Services&);
LRESULT NotifyTree(Page&,NMTREEVIEWA*,Services&);
LRESULT CustomDraw(Page&,NMCUSTOMDRAW*,BOOL&,Services&);
void ToggleList(EditWindowEndpointNativeBody24h&,int,IPropertyPageSoundNativeApi&);
void AppendPair(Page&,PropertyPageSoundVolumePair8h,Services&);
void AppendVoice(Page&,const YtlReferenceSlotRuntime&,Services&);
void ReleaseVoiceRange(YtlReferenceSlotRuntime*,YtlReferenceSlotRuntime*) noexcept;
void FreeVector(void*,DWORD,Services&);
void MakeVoice(YtlReferenceSlotRuntime&,Services&);
void ReleaseLocal(YtlReferenceSlotRuntime&) noexcept;
RuntimeCString* ExpandName(Page&,RuntimeCString*,const RuntimeCString&,Services&);
void SheetButtons(HWND,Services&);
}
