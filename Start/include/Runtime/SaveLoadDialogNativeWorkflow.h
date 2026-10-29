#pragma once
#include <Runtime/SaveLoadDialogNative.h>
#include <Runtime/InputRootNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
// 四个独立原生子类构造边界；5330A0自己负责查找/分配/Ref/append/选择/失败资格。
class ISaveLoadDialogNativePageFactory
{
public:
    virtual ~ISaveLoadDialogNativePageFactory()=default;
    virtual GuiDialogNativeBody5Ch* ConstructPage00At4ED190(void* rawAC,GuiDialogNativeBody5Ch* parent,std::int32_t id)=0;
    virtual GuiDialogNativeBody5Ch* ConstructPage01At4EE390(void* rawAC,GuiDialogNativeBody5Ch* parent,std::int32_t id)=0;
    virtual GuiDialogNativeBody5Ch* ConstructSystemAt4F0C80(void* raw80,GuiDialogNativeBody5Ch* parent,std::int32_t id)=0;
    virtual GuiDialogNativeBody5Ch* ConstructPopupAt4EF660(void* rawC0,GuiDialogNativeBody5Ch* parent,std::int32_t id)=0;
};
class ISaveLoadDialogNativeBeginSource
{
public:
    virtual ~ISaveLoadDialogNativeBeginSource()=default;
    virtual YtlReferenceSlotRuntime* CaptureBackgroundAfter413BE0(YtlReferenceSlotRuntime* fresh)=0;
    virtual InputRootNativeBody24h* CaptureInputAt418210()=0;
};
// 原441980/4425C0的handle门控及捕获发生在此精确图形叶，caller不提前读句柄。
class ISaveLoadDialogNativeDrawingSource
{
public:
    virtual ~ISaveLoadDialogNativeDrawingSource()=default;
    virtual void DrawCapturedAt441980(GraphicsSurfaceNativeBody*,const RECT*)=0;
    virtual void BlitCapturedAt4425C0(GraphicsSurfaceNativeBody* destination,GraphicsSurfaceNativeBody* source,const RECT*)=0;
};
class NativeGraphicsRuntimeLibrary;
class SaveFileStateNativeArchive;
class ISaveLoadDialogNativeCatalogSource
{
public:
    virtual ~ISaveLoadDialogNativeCatalogSource()=default;
    virtual SaveFileStateNativeArchive* CaptureCatalogAt4180C0()=0;
    virtual void RequestCloseApplication() { throw std::logic_error("save/load close application source is not bound"); }
    virtual void ShowPageLimitMessage() { throw std::logic_error("save/load page limit source is not bound"); }
};
class SaveLoadDialogNativeGraphicsSource final:public ISaveLoadDialogNativeDrawingSource
{
public:
    explicit SaveLoadDialogNativeGraphicsSource(NativeGraphicsRuntimeLibrary& graphics) noexcept:graphics_(graphics){}
    void DrawCapturedAt441980(GraphicsSurfaceNativeBody*,const RECT*) override;
    void BlitCapturedAt4425C0(GraphicsSurfaceNativeBody*,GraphicsSurfaceNativeBody*,const RECT*) override;
private:
    NativeGraphicsRuntimeLibrary& graphics_;
};
void InitializeSaveLoadDialogPageNativeAt5330A0(SaveLoadDialogNativeBody184h&,std::int32_t,ISaveLoadDialogNativePageFactory&);
void OpenSaveLoadDialogChildrenNativeAt532560(SaveLoadDialogNativeBody184h&);
void BeginSaveLoadDialogNativeAt50E8B0(SaveLoadDialogNativeBody184h&,ISaveLoadDialogNativeBeginSource&,ISaveLoadDialogNativePageFactory&);
void DrawSaveLoadDialogNativeAt524800(SaveLoadDialogNativeBody184h&,const RECT*,ISaveLoadDialogNativeDrawingSource&);
void CompositeSaveLoadDialogNativeAt524860(SaveLoadDialogNativeBody184h&,YtlReferenceSlotRuntime ownedByValue,const RECT*,
    ISaveLoadDialogNativeDrawingSource&);
void FlushSaveLoadDialogDirtyNativeAt525FB0(SaveLoadDialogNativeBody184h&,IGuiDialogNativeScreenSource&);
void ProcessSaveLoadDialogEventsNativeAt522350(SaveLoadDialogNativeBody184h&,ISaveLoadDialogNativeCatalogSource&,
    ISaveLoadDialogNativePageFactory&);
// 四个分立业务内核的必填链接合同，不是父Poll的替身。未完成的原命令不得挂默认成功叶。
using SaveLoadDialogNativeCommandEntry=std::int32_t (*)(SaveLoadDialogNativeBody184h&,void* externalServices);
struct SaveLoadDialogNativeCommandEntries final
{
    SaveLoadDialogNativeCommandEntry returnTitleAt51E320;
    SaveLoadDialogNativeCommandEntry deleteAt51B8F0;
    SaveLoadDialogNativeCommandEntry saveAt525480;
    SaveLoadDialogNativeCommandEntry loadAt520AB0;
    void* externalServices;
};
std::int32_t PollSaveLoadDialogNativeAt52F7A0(SaveLoadDialogNativeBody184h&,ISaveLoadDialogNativeCatalogSource&,
    ISaveLoadDialogNativePageFactory&,ISaveLoadDialogNativeTransitionSource&,IGraphicsTransitApi&,
    const SaveLoadDialogNativeCommandEntries&);
}
