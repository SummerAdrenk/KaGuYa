#pragma once
#include <Runtime/LegacyItemAcquisitionResourceProfile.h>
#include <Runtime/GuiDialogIntoNative.h>
#include <Runtime/TransitRuntime.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
namespace KaGuYa::Start::Runtime
{
enum class LegacyItemAcquisitionAction { Save,Load,QuickSave,QuickLoad,Settings,Title,Likability };
class ILegacyItemAcquisitionSource
{
public:
    virtual ~ILegacyItemAcquisitionSource()=default;
    virtual int Read(const char*)=0;
    virtual void Write(const char*,int)=0;
    virtual UnSaveDataRuntime& UnSave()=0;
    virtual SaveDataEnvironmentArchive& SaveData()=0;
    virtual SIZE CanvasSize()=0;
    virtual RECT SnapshotBounds(bool maso)=0;
    virtual void SnapshotLayer(bool maso,GraphicsSurfaceNativeBody& target)=0;
    virtual void StartAudio(const char*)=0;
    virtual void SelectionCue(const char*)=0;
    virtual int Confirm(int mode,const YtlReferenceSlotRuntime& capture)=0;
    virtual int TransitionStep()=0;
    virtual bool AccelerateTransition()=0;
    virtual void RenderTransition(const TransitOperation&)=0;
    virtual void ResetScriptState()=0;
    virtual void ClearModalResult()=0;
    virtual void Command(const char*)=0;
    virtual bool Requested(LegacyItemAcquisitionAction)=0;
    virtual void CapturePreview()=0;
    virtual void Push(LegacyItemAcquisitionAction)=0;
    virtual void QuickSave()=0;
    virtual bool QuickLoad()=0;
    virtual bool ConfirmTitle()=0;
    virtual void QueueLoadAfter()=0;
    virtual void PostTitle()=0;
    virtual void Finish()=0;
};
struct LegacyItemAcquisitionServices final
{
    const LegacyItemAcquisitionResourceProfile* profile;
    ILegacyItemAcquisitionSource& source;
    IGuiControlResourceLoadServices& resources;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
};
std::size_t LegacyItemAcquisitionDialogStorageSize() noexcept;
GuiDialogNativeBody5Ch* ConstructLegacyItemAcquisitionDialog(void*,LegacyItemAcquisitionServices&);
void ReopenLegacyItemAcquisitionSystem(GuiDialogNativeBody5Ch&);
// 原0x5097F0保留的phase2退出转场；普通关闭确认接受直接到phase4。
void BeginLegacyItemAcquisitionExitTransition(GuiDialogNativeBody5Ch&);
struct LegacyItemAcquisitionScene;
struct LegacyItemAcquisitionSceneVtable final
{
    std::uint8_t(__thiscall* run)(LegacyItemAcquisitionScene*,int);
    LegacyItemAcquisitionScene*(__thiscall* destroy)(LegacyItemAcquisitionScene*,unsigned);
    void(__thiscall* update)(LegacyItemAcquisitionScene*);
    void(__thiscall* draw)(LegacyItemAcquisitionScene*);
};
struct LegacyItemAcquisitionScene final
{
    const LegacyItemAcquisitionSceneVtable* vptr;
    LegacyItemAcquisitionServices* services;
    GuiDialogNativeBody5Ch* dialog;
    unsigned savedSkip;
    int phase;
};
static_assert(offsetof(LegacyItemAcquisitionScene,vptr)==0);
LegacyItemAcquisitionScene* ConstructLegacyItemAcquisitionScene(void*,LegacyItemAcquisitionServices&);
void UpdateLegacyItemAcquisitionScene(LegacyItemAcquisitionScene&);
void DestroyLegacyItemAcquisitionScene(LegacyItemAcquisitionScene&) noexcept;
const char* LegacyItemAcquisitionSceneName() noexcept;
const char* LegacyItemAcquisitionActionName(LegacyItemAcquisitionAction);
}
