#pragma once
#include <Runtime/ProgramProfileDialogNative.h>
#include <Runtime/ProgramMain00DialogNative.h>
#include <Runtime/ProgramSystemDialogNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include "ProgramMain01DialogNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime::ProgramProfileDetail
{
using Slot = YtlReferenceSlotRuntime;
using Common = GuiDialogNativeBody5Ch;
using Child = GuiControlNativeBody50h;
using Surface = GraphicsSurfaceNativeBody;
using ProgramMain01NativeDetail::Copy;
using ProgramMain01NativeDetail::Resolve;
using Leaves = ProgramMain01NativeDetail::LeafScope;
struct Ref final
{
    Slot slot{};
    ~Ref() noexcept { ReleaseYtlReference(slot); }
    Ref() = default;
    Ref(const Ref&) = delete;
};
enum class Role { Main, System, Confirmation };
struct Top;
struct Panel final
{
    ProgramMain01DialogNativeBody88h base;
    Top* owner;
    const ProgramPanelProfile* data;
    Role role;
};
struct Top final
{
    ProgramSelectDialogNativeBodyECh base;
    const ProgramModeProfile* mode;
    Slot confirmation;
    const ProgramChoiceProfile* pendingChoice;
    bool modal;
};
inline Common& Base(Top& p) noexcept { return p.base.into.common; }
inline Panel& AsPanel(Common* p) noexcept { return *reinterpret_cast<Panel*>(p); }
inline Top& AsTop(Common* p) noexcept { return *reinterpret_cast<Top*>(p); }
class ModeScope final
{
public:
    explicit ModeScope(const ProgramModeProfile&) noexcept;
    ~ModeScope() noexcept;
private:
    const ProgramModeProfile* previous_;
};
const ProgramSelectDialogNativeVtable42& TopTable() noexcept;
const GuiDialogNativeVtable35& PanelTable() noexcept;
void DestroyTop(Top&) noexcept;
void NewPanel(Slot&, Top&, const ProgramPanelProfile&, Role, int id);
Panel& ChildPanel(Top&, int id);
void Load(Slot&, const char*);
int Value(const char*);
void Rules(Panel&, std::span<const ProgramControlRule>);
void BeginPanel(Panel&);
int PollPanel(Panel&);
void UpdateTop(Top&);
void OpenConfirmation(Top&, const ProgramChoiceProfile&);
void CloseConfirmation(Top&, int outcome);
void Reopen(Top&);
void Calendar(Panel&);
void DrawPanel(Panel&, const RECT*);
void CompositePanel(Panel&, Slot owned, const RECT*);
void Redraw(Top&);
}
