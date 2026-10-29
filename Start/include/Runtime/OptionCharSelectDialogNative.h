#pragma once

#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>

#include <span>

namespace KaGuYa::Start::Runtime
{
// 4E4A20/4FCEB0：正文只拥有原 Ref 和字段，不持现代状态镜像或服务指针。
struct OptionCharSelectDialogNativeBody8Ch final
{
    union { GuiDialogNativeBody5Ch common; };
    union { YtlReferenceSlotRuntime clickedManagerAt5C; };
    std::int32_t phaseAt68;
    std::int32_t pagingLatchAt6C;
    RECT boundsAt70;
    std::int32_t firstVisibleAt80;
    std::int32_t focusedAt84;
    std::int32_t selectionAt88;
    // 不启动common/+5C的默认成员初始化；new T/T()/T{}均不提前写原字段。
    // C++构造仅建立存储形状；仍须显式4E4A20，资源释放走原GUI+04而非普通delete。
    OptionCharSelectDialogNativeBody8Ch() noexcept {}
};
static_assert(sizeof(OptionCharSelectDialogNativeBody8Ch) == 0x8C);
static_assert(alignof(OptionCharSelectDialogNativeBody8Ch) == 4);
static_assert(std::is_standard_layout_v<OptionCharSelectDialogNativeBody8Ch>);
static_assert(offsetof(OptionCharSelectDialogNativeBody8Ch, common) == 0);
static_assert(offsetof(OptionCharSelectDialogNativeBody8Ch, clickedManagerAt5C) == 0x5C);
static_assert(offsetof(OptionCharSelectDialogNativeBody8Ch, phaseAt68) == 0x68);
static_assert(offsetof(OptionCharSelectDialogNativeBody8Ch, pagingLatchAt6C) == 0x6C);
static_assert(offsetof(OptionCharSelectDialogNativeBody8Ch, boundsAt70) == 0x70);
static_assert(offsetof(OptionCharSelectDialogNativeBody8Ch, firstVisibleAt80) == 0x80);
static_assert(offsetof(OptionCharSelectDialogNativeBody8Ch, focusedAt84) == 0x84);
static_assert(offsetof(OptionCharSelectDialogNativeBody8Ch, selectionAt88) == 0x88);

// PE 5BFECC，共38槽；+94接收借用Ref地址，不是按值消费参数。
struct OptionCharSelectDialogNativeVtable38 final
{
    GuiDialogNativeVtable35 common;
    std::int32_t (__thiscall* countAt8C)(GuiDialogNativeBody5Ch*);
    void (__thiscall* resetAt90)(GuiDialogNativeBody5Ch*);
    void (__thiscall* attachDirtyAt94)(GuiDialogNativeBody5Ch*, const YtlReferenceSlotRuntime*);
};
static_assert(sizeof(OptionCharSelectDialogNativeVtable38) == 0x98);
static_assert(offsetof(OptionCharSelectDialogNativeVtable38, common) == 0);
static_assert(offsetof(OptionCharSelectDialogNativeVtable38, countAt8C) == 0x8C);
static_assert(offsetof(OptionCharSelectDialogNativeVtable38, resetAt90) == 0x90);
static_assert(offsetof(OptionCharSelectDialogNativeVtable38, attachDirtyAt94) == 0x94);

// 非拥有来源：Manager工作归main；接口必须交付原0Ch三指针正文/Ref18，不能交付现代owner。
// 不把生产依赖缺口藏在modernObjectGraph中，也不改变null/异常/部分发布合同。
class IOptionCharSelectDialogNativeSource
{
public:
    virtual ~IOptionCharSelectDialogNativeSource() = default;
    // 4E4AAD..4E4AC6：new0Ch并清三个指针；new抛出时不得改外层+5C。
    virtual GuiClickedEventReferences* NewClickedEventManagerAt4E4AAD() = 0;
    // 4E4AD4 -> 4D7B90：接收owned；先发布base，再new18/发布control。
    // control失败不回收base；此函数自身不持额外owner或成功返回后的引用。
    virtual void AdoptClickedEventManagerAt4D7B90(YtlReferenceSlotRuntime& destination,
        GuiClickedEventReferences* owned) = 0;
    // 覆盖4E4B00..4E4C74的critical/singleton/root+30[1]/+1C/+84链。
    // 仅index0..3调用；返回原借用byte地址，紧接着由正文读取，不缓存可用性。
    virtual const std::uint8_t* AvailabilityAt4E4C74(std::int32_t index) = 0;
    // 52E123 -> 50FDB0；manager已按base+bias*stride解析，无额外Ref增持。
    virtual std::int32_t PollClickedEventManagerAt50FDB0(GuiClickedEventReferences* manager,
        std::int32_t clearMatched) = 0;
};

struct OptionCharSelectDialogNativeDependencies final
{
    IOptionCharSelectDialogNativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};

// 同源scope须覆盖构造正文/展开、业务及析构；正文与控制块均不保存该scope或dependencies。
class OptionCharSelectDialogNativeScope final
{
public:
    explicit OptionCharSelectDialogNativeScope(OptionCharSelectDialogNativeDependencies&) noexcept;
    ~OptionCharSelectDialogNativeScope() noexcept;
    OptionCharSelectDialogNativeScope(const OptionCharSelectDialogNativeScope&) = delete;
    OptionCharSelectDialogNativeScope& operator=(const OptionCharSelectDialogNativeScope&) = delete;
private:
    OptionCharSelectDialogNativeDependencies* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
OptionCharSelectDialogNativeDependencies& RequireOptionCharSelectDialogNativeDependencies();
const OptionCharSelectDialogNativeVtable38& OptionCharSelectDialogNativeVtable() noexcept;
OptionCharSelectDialogNativeBody8Ch* ConstructOptionCharSelectDialogAt4E4A20(
    OptionCharSelectDialogNativeBody8Ch* body);
OptionCharSelectDialogNativeBody8Ch* DeleteOptionCharSelectDialogAt4FCEB0(
    OptionCharSelectDialogNativeBody8Ch* body, std::uint32_t flags) noexcept;
void ApplyOptionCharSelectWindowAt53AF00(OptionCharSelectDialogNativeBody8Ch& body);

// 数据由原PE读取，描述符的null、重复指针及末条event3保持原样。
std::span<const GuiButtonDescriptorNative2Ch, 6> OptionCharSelectDialogNativeRecordsSample18() noexcept;
std::span<const POINT, 5> OptionCharSelectDialogNativePositionsSample18() noexcept;
}
