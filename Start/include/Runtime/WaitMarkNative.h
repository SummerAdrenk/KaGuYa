#pragma once
#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/DrawLayerReferenceArray.h>
#include <Runtime/DrawManagerResourceRuntime.h>
#include <Runtime/SpriteNativeDrawing.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/LayerListNative.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
class UnSaveDataRuntime;
struct WaitMarkNativeBody34h final
{
    LayerWindowNativeBody24h window;
    union { DrawLayerReferenceArray spritesAt24; };
    std::uint32_t completedAt30;
    WaitMarkNativeBody34h() noexcept {}
};
static_assert(sizeof(WaitMarkNativeBody34h) == 0x34);
static_assert(offsetof(WaitMarkNativeBody34h, spritesAt24) == 0x24);
static_assert(offsetof(WaitMarkNativeBody34h, completedAt30) == 0x30);
static_assert(std::is_trivially_destructible_v<WaitMarkNativeBody34h>);
// 4873C3测试整个DWORD，不沿用现代bool或其它窗口的BYTE参数类型。
struct WaitMarkNativeVtable final
{
    LayerItemNativeVtable base;
    RECT* (__thiscall* boundsAt0C)(LayerItemNativeBody14h*, RECT*);
    std::uint8_t (__thiscall* requestAt10)(LayerItemNativeBody14h*, std::int32_t, std::int32_t);
    void (__thiscall* advanceAt14)(LayerItemNativeBody14h*);
    void (__thiscall* resetAt18)(LayerItemNativeBody14h*);
};
static_assert(sizeof(WaitMarkNativeVtable) == 0x1C);
class IWaitMarkNativeStateSource
{
public:
    virtual ~IWaitMarkNativeStateSource() = default;
    virtual KeyboardManagerNativeBody438h* CaptureKeyboardAt4873CC() = 0;
    virtual UnSaveDataRuntime* CaptureUnSaveInlineAt47B034() = 0;
    virtual UnSaveDataRuntime* CaptureUnSaveVia417990At487410() = 0;
};
class EnvironmentWaitMarkNativeStateSource final : public IWaitMarkNativeStateSource
{
public:
    EnvironmentWaitMarkNativeStateSource(EnvironmentRootOwner&, InputRootOwner&) noexcept;
    KeyboardManagerNativeBody438h* CaptureKeyboardAt4873CC() override;
    UnSaveDataRuntime* CaptureUnSaveInlineAt47B034() override;
    UnSaveDataRuntime* CaptureUnSaveVia417990At487410() override;
private:
    EnvironmentRootOwner& environment_;
    InputRootOwner& input_;
};
struct WaitMarkNativeDescriptor final
{
    const char* name;
    std::uint8_t restart, position;
    std::int32_t x, y;
};
using WaitMarkNativeProfile = std::array<WaitMarkNativeDescriptor, 2>;
const WaitMarkNativeProfile& MamatomoWaitMarkNativeProfile() noexcept;
struct WaitMarkNativeServices final
{
    ISpriteNativeResourceSource& resources;
    IWaitMarkNativeStateSource& state;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
    // 两项分别由普通/自动状态选择；省略时保留18号原表。
    const WaitMarkNativeProfile& profile = MamatomoWaitMarkNativeProfile();
};
class WaitMarkNativeScope final
{
public:
    explicit WaitMarkNativeScope(WaitMarkNativeServices&) noexcept;
    ~WaitMarkNativeScope() noexcept;
    WaitMarkNativeScope(const WaitMarkNativeScope&) = delete;
    WaitMarkNativeScope& operator=(const WaitMarkNativeScope&) = delete;
private:
    WaitMarkNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    LayerListNativeScope layers_;
};
const WaitMarkNativeVtable& WaitMarkNativeMethods() noexcept;
WaitMarkNativeBody34h* ConstructWaitMarkNativeAt45E450(WaitMarkNativeBody34h&, LayerListNativeBody18h*, WaitMarkNativeServices&);
WaitMarkNativeBody34h* DeleteWaitMarkNativeAt469680(WaitMarkNativeBody34h&, std::uint32_t flags) noexcept;
SpriteNativeBodyA0h* SelectedWaitMarkSpriteNativeAt47B030(WaitMarkNativeBody34h&, IWaitMarkNativeStateSource&);
std::uint8_t RequestWaitMarkNativeAt4873C0(WaitMarkNativeBody34h&, std::int32_t requested, IWaitMarkNativeStateSource&);
void AdvanceWaitMarkNativeAt46B1F0(WaitMarkNativeBody34h&, IWaitMarkNativeStateSource&);
}
