#pragma once

#include <Runtime/GuiDialogIntoNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/InputRootNative.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsSurfaceCompositionApi;

// 4DB3D0 / 5BFF68：正文不存服务、现代GUI或另一份容器；+78/+7C构造未写。
struct CgOperationDialogNativeBodyA8h final
{
    union { GuiDialogIntoNativeBody68h into; };
    RECT boundsAt68;
    std::int32_t resultAt78;
    std::int32_t selectionAt7C;
    union { EnvironmentVectorStorage<std::int32_t> availabilityAt80; };
    union { GuiControlSurfaceReferences digitsAt8C; };
    RECT digitBoundsAt98;
    CgOperationDialogNativeBodyA8h() noexcept {}
};
static_assert(sizeof(CgOperationDialogNativeBodyA8h) == 0xA8);
static_assert(alignof(CgOperationDialogNativeBodyA8h) == 4);
static_assert(std::is_standard_layout_v<CgOperationDialogNativeBodyA8h>);
static_assert(std::is_trivially_destructible_v<CgOperationDialogNativeBodyA8h>);
static_assert(offsetof(CgOperationDialogNativeBodyA8h, into) == 0);
static_assert(offsetof(CgOperationDialogNativeBodyA8h, boundsAt68) == 0x68);
static_assert(offsetof(CgOperationDialogNativeBodyA8h, resultAt78) == 0x78);
static_assert(offsetof(CgOperationDialogNativeBodyA8h, selectionAt7C) == 0x7C);
static_assert(offsetof(CgOperationDialogNativeBodyA8h, availabilityAt80) == 0x80);
static_assert(offsetof(CgOperationDialogNativeBodyA8h, digitsAt8C) == 0x8C);
static_assert(offsetof(CgOperationDialogNativeBodyA8h, digitBoundsAt98) == 0x98);

// 仅原叶边界。Main还须提供已有GuiInput/GuiControlGraphics/GuiDialogScreen scopes；
// 本scope不代替这些根的生产绑定，也不拥有30h父窗口或B4h Scene。
struct CgOperationDialogNativeServices final
{
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsSurfaceCompositionApi& composition;
    InputRootOwner& input;
    IRuntimeStringManagerSource& strings;
    const RuntimeCString& popupButtonNameAt61A660;
};
class CgOperationDialogNativeScope final
{
public:
    explicit CgOperationDialogNativeScope(CgOperationDialogNativeServices&) noexcept;
    ~CgOperationDialogNativeScope() noexcept;
    CgOperationDialogNativeScope(const CgOperationDialogNativeScope&) = delete;
    CgOperationDialogNativeScope& operator=(const CgOperationDialogNativeScope&) = delete;
private:
    CgOperationDialogNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
};

// 37槽，复用Into的+8C/+90 ABI；子对象36槽仅在内部头公开。
const GuiDialogIntoNativeVtable37& CgOperationDialogNativeMethods() noexcept;
// 以下原生入口及虚槽执行期间必须绑定上面的叶scope。
// Main的new A8h -> 原thiscall构造等价口；raw分配保护仍归调用者，失败不伪成功。
GuiDialogNativeBody5Ch* ConstructCgOperationDialogNativeAt4DB3D0(void* raw);
CgOperationDialogNativeBodyA8h* ConstructCgOperationDialogNativeAt4DB3D0(
    CgOperationDialogNativeBodyA8h&);
// 4FBEE0内联完整析构段；不把不存在的独立导出地址命名为入口。
void DestroyCgOperationDialogNative(CgOperationDialogNativeBodyA8h&) noexcept;
CgOperationDialogNativeBodyA8h* DeleteCgOperationDialogNativeAt4FBEE0(
    CgOperationDialogNativeBodyA8h*, std::uint32_t flags) noexcept;
// 499CBA..499D45：旧槽先退役，新base先发布，control分配失败不补删新正文。
void AdoptCgOperationDialogNativeAt499CBA(YtlReferenceSlotRuntime&,
    CgOperationDialogNativeBodyA8h* ownedBody);
CgOperationDialogNativeBodyA8h* ResolveCgOperationDialogNative(
    const YtlReferenceSlotRuntime&) noexcept;

void BeginCgOperationDialogNativeAt50BFD0(CgOperationDialogNativeBodyA8h&);
std::int32_t PollCgOperationDialogNativeAt52C2D0(CgOperationDialogNativeBodyA8h&);
std::int32_t ResultCgOperationDialogNativeAt50AB30(const CgOperationDialogNativeBodyA8h&) noexcept;
void ResetCgOperationDialogNativeAt502110(CgOperationDialogNativeBodyA8h&);
void AppendCgOperationAvailabilityNativeAt4FEA00(CgOperationDialogNativeBodyA8h&,
    std::int32_t availability);
// 4A84E0写+7C后才调+44；4B7A42读同一字段。opaque值不是资源ID。
std::int32_t ReadCgOperationSelectionNativeAt7C(const CgOperationDialogNativeBodyA8h&) noexcept;
void WriteCgOperationSelectionNativeAt7C(CgOperationDialogNativeBodyA8h&, std::int32_t) noexcept;
void DrawCgOperationDialogNativeAt523370(CgOperationDialogNativeBodyA8h&, const RECT* clip);
void CompositeCgOperationDialogNativeAt523480(CgOperationDialogNativeBodyA8h&,
    YtlReferenceSlotRuntime ownedDestination, const RECT* clip);
}
