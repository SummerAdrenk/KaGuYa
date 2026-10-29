#pragma once

#include <Runtime/MusicDialogNativeConstruction.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/MusicResourceProfile.h>

namespace KaGuYa::Start::Runtime
{
// 4E3900只写bounds/events，不提前改写phaseAt5C、unwrittenAt60或resultAt80。
struct MusicDialogNativeBody84h final
{
    union { GuiDialogNativeBody5Ch common; };
    std::int32_t phaseAt5C;
    std::uint32_t unwrittenAt60;
    RECT boundsAt64;
    union { YtlReferenceSlotRuntime eventsAt74; };
    std::int32_t resultAt80;
    MusicDialogNativeBody84h() noexcept {}
};
static_assert(sizeof(MusicDialogNativeBody84h) == 0x84);
static_assert(alignof(MusicDialogNativeBody84h) == 4);
static_assert(offsetof(MusicDialogNativeBody84h, phaseAt5C) == 0x5C);
static_assert(offsetof(MusicDialogNativeBody84h, boundsAt64) == 0x64);
static_assert(offsetof(MusicDialogNativeBody84h, eventsAt74) == 0x74);
static_assert(offsetof(MusicDialogNativeBody84h, resultAt80) == 0x80);
static_assert(std::is_standard_layout_v<MusicDialogNativeBody84h>);
static_assert(std::is_trivially_destructible_v<MusicDialogNativeBody84h>);

// 仅借用正文外的系统叶；选择、构造与导航仍由真实GUI算法完成。
struct MusicDialogNativeServices final
{
    IGuiControlResourceLoadServices& pictures;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IGraphicsWindowSurfaceApi& drawing;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
    const MusicResourceProfile& resources = GameMusicResourceProfile();
};
class MusicDialogNativeScope final
{
public:
    explicit MusicDialogNativeScope(MusicDialogNativeServices&) noexcept;
    ~MusicDialogNativeScope() noexcept;
    MusicDialogNativeScope(const MusicDialogNativeScope&) = delete;
    MusicDialogNativeScope& operator=(const MusicDialogNativeScope&) = delete;
private:
    MusicDialogNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
MusicDialogNativeServices& RequireMusicDialogNativeServices();
GuiDialogNativeBody5Ch* ConstructMusicDialogNativeAt4E3900(void* storage84h);
MusicDialogNativeBody84h* DeleteMusicDialogNativeAt4FCC00(MusicDialogNativeBody84h*, std::uint32_t) noexcept;
const GuiDialogNativeVtable35& MusicDialogNativeVtable() noexcept;
void BeginMusicDialogNativeAt50D090(MusicDialogNativeBody84h&);
std::int32_t PollMusicDialogNativeAt52DD40(MusicDialogNativeBody84h&);
std::int32_t MusicDialogNativeResultAt50AB80(const MusicDialogNativeBody84h&) noexcept;
void ClearMusicDialogChecksAt532340(MusicDialogNativeBody84h&, std::int32_t invalidate);

class MusicDialogNativeConstruction final : public IMusicDialogNativeConstruction
{
public:
    explicit MusicDialogNativeConstruction(MusicDialogNativeServices& s) noexcept : services_(s) {}
    GuiDialogNativeBody5Ch* ConstructMusicDialogAt4E3900(void* storage84h) override;
    MusicDialogNativeServices& NativeDialogServices() noexcept override { return services_; }
private:
    MusicDialogNativeServices& services_;
};
}
