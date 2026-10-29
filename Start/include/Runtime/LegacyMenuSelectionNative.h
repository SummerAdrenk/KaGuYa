#pragma once
#include <Runtime/LegacyMenuSelectionResourceProfile.h>
#include <Runtime/GuiDialogIntoNative.h>
#include <Runtime/GraphicsTextRuntime.h>
#include <Runtime/MessageTextSelectionRuntime.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime
{
enum class LegacyMenuSelectionAction { Save, Load, QuickSave, QuickLoad, Settings, Title, Likability };
class ILegacyMenuSelectionSource
{
public:
    virtual ~ILegacyMenuSelectionSource() = default;
    virtual int ReadValue(const char*) = 0;
    virtual void WriteValue(const char*, int) = 0;
    virtual int ReadLabel(int, MessageFormatSelectionTarget) = 0;
    virtual void MarkLabelRead(int) = 0;
    virtual COLORREF ReadColor(const char*) = 0;
    virtual const char* FontFace() = 0;
    virtual UnSaveDataRuntime& UnSave() = 0;
    virtual SaveDataEnvironmentArchive& SaveData() = 0;
    virtual void SetCommand(const char*) = 0;
    virtual bool Requested(LegacyMenuSelectionAction) = 0;
    virtual void ClearCommand() = 0;
    virtual void ResetScriptMenuState() = 0;
    virtual void ClearModalResult() = 0;
    virtual bool ShouldAutoSave() = 0;
    virtual void CapturePreview() = 0;
    virtual void AutoSave() = 0;
    virtual void PushSystemScene(LegacyMenuSelectionAction) = 0;
    virtual void QuickSave() = 0;
    virtual bool QuickLoad() = 0;
    virtual bool ConfirmTitle() = 0;
    virtual void QueueLoadAfter() = 0;
    virtual void PostTitle() = 0;
    virtual void FinishScene() = 0;
    virtual bool SuppressBackgroundUpdate() = 0;
    virtual void DrawBackgroundAndMenu(GuiDialogNativeBody5Ch&) = 0;
};
struct LegacyMenuSelectionServices final
{
    const LegacyMenuSelectionResourceProfile* profile;
    ILegacyMenuSelectionSource& source;
    IGuiControlResourceLoadServices& resources;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IGraphicsTextApi& text;
    IGraphicsTextGlyphApi& glyphs;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
struct LegacyMenuSelectionDialog;
std::size_t LegacyMenuSelectionDialogStorageSize() noexcept;
GuiDialogNativeBody5Ch* ConstructLegacyMenuSelectionDialog(void*, LegacyMenuSelectionServices&);
void ReopenLegacyMenuSelectionSystem(GuiDialogNativeBody5Ch&);

struct LegacyMenuSelectionScene;
struct LegacyMenuSelectionSceneVtable final
{
    std::uint8_t(__thiscall* run)(LegacyMenuSelectionScene*, int);
    LegacyMenuSelectionScene*(__thiscall* destroy)(LegacyMenuSelectionScene*, unsigned);
    void(__thiscall* update)(LegacyMenuSelectionScene*);
    void(__thiscall* draw)(LegacyMenuSelectionScene*);
};
// 现代内部正文，不把原20h/EH尺寸带入新工厂。
struct LegacyMenuSelectionScene final
{
    const LegacyMenuSelectionSceneVtable* vptr;
    LegacyMenuSelectionServices* services;
    GuiDialogNativeBody5Ch* dialog;
    unsigned savedSkip;
    int phase;
    bool drawEnabled;
};
static_assert(offsetof(LegacyMenuSelectionScene, vptr) == 0);
LegacyMenuSelectionScene* ConstructLegacyMenuSelectionScene(void*, LegacyMenuSelectionServices&);
void UpdateLegacyMenuSelectionScene(LegacyMenuSelectionScene&);
void DestroyLegacyMenuSelectionScene(LegacyMenuSelectionScene&) noexcept;
}
