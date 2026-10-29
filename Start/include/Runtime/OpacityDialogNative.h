#pragma once

#include <Runtime/GuiDialogNative.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class IOpacityDialogResetStateServices;
class IOpacityDialogUpdateServices;

// 18号607054与Baku 56EADC仅滑轨不同；资源槽及行程修正继续共用已核对的数据。
struct OpacityDialogNativeProfile final
{
    RECT track;
    std::optional<POINT> backgroundPosition;
};
const OpacityDialogNativeProfile& MamatomoOpacityDialogNativeProfile() noexcept;

// 4E4780：首5Ch为真实基类，唯一派生字段是5C处RECT；默认构造不提前写Ref。
struct OpacityDialogNativeBody6Ch final
{
    union { GuiDialogNativeBody5Ch common; };
    RECT boundsAt5C;
    OpacityDialogNativeBody6Ch() noexcept {}
};
static_assert(sizeof(OpacityDialogNativeBody6Ch) == 0x6C);
static_assert(alignof(OpacityDialogNativeBody6Ch) == 4);
static_assert(offsetof(OpacityDialogNativeBody6Ch, common) == 0);
static_assert(offsetof(OpacityDialogNativeBody6Ch, boundsAt5C) == 0x5C);
static_assert(std::is_standard_layout_v<OpacityDialogNativeBody6Ch>);
static_assert(std::is_trivially_destructible_v<OpacityDialogNativeBody6Ch>);

const GuiDialogNativeVtable35& OpacityDialogNativeVtable() noexcept;
OpacityDialogNativeBody6Ch* ConstructOpacityDialogNativeAt4E4780(
    OpacityDialogNativeBody6Ch& body, IGuiControlResourceLoadServices& loader,
    IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
OpacityDialogNativeBody6Ch* ConstructOpacityDialogNativeAt4E4780(
    OpacityDialogNativeBody6Ch& body, IGuiControlResourceLoadServices& loader,
    IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings,
    const OpacityDialogNativeProfile& profile);
OpacityDialogNativeBody6Ch* DeleteOpacityDialogNativeAt4FCE80(
    OpacityDialogNativeBody6Ch* body, std::uint32_t flags) noexcept;

// 完成正文由调用者传入；control失败保留已发布base，不增加raw正文owner。
void InstallOpacityDialogReferenceAt45AF17(
    YtlReferenceSlotRuntime& slot, OpacityDialogNativeBody6Ch* completedBody);
OpacityDialogNativeBody6Ch* ResolveOpacityDialogReference(
    const YtlReferenceSlotRuntime& slot) noexcept;

// 只借用50D730/52DF20的现场来源；服务不进入6Ch正文或18h control。
// 输入/Graphics继续使用既有Gui原生scope，缺来源不回退现代Dialog。
class OpacityDialogNativeScope final
{
public:
    OpacityDialogNativeScope(const IOpacityDialogResetStateServices& reset,
        IOpacityDialogUpdateServices& update) noexcept;
    ~OpacityDialogNativeScope() noexcept;
    OpacityDialogNativeScope(const OpacityDialogNativeScope&) = delete;
    OpacityDialogNativeScope& operator=(const OpacityDialogNativeScope&) = delete;
private:
    const IOpacityDialogResetStateServices* previousReset_;
    IOpacityDialogUpdateServices* previousUpdate_;
};
}
