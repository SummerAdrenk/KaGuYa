#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/RuntimeCStringArray.h>

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// union 阻止编译器为失败的第二串补出首串析构；生命周期由原始入口逐字段管理。
struct ParamsPatternFileNameNative final
{
    explicit ParamsPatternFileNameNative(IRuntimeStringManagerSource& strings);
    ~ParamsPatternFileNameNative() noexcept(false);
    ParamsPatternFileNameNative(const ParamsPatternFileNameNative&) = delete;
    ParamsPatternFileNameNative& operator=(const ParamsPatternFileNameNative&) = delete;
    std::uint32_t kindAt00;
    union { RuntimeCString nameAt04; };
};

struct ParamsPatternFileGroupNative final
{
    explicit ParamsPatternFileGroupNative(IRuntimeStringManagerSource& strings);
    ~ParamsPatternFileGroupNative() noexcept;
    ParamsPatternFileGroupNative(const ParamsPatternFileGroupNative&) = delete;
    ParamsPatternFileGroupNative& operator=(const ParamsPatternFileGroupNative&) = delete;
    std::uint32_t kindAt00;
    union { RuntimeCString nameAt04; };
    union { RuntimeCStringArrayReference stringsAt08; };
};

struct ParamsPatternExcPositionNative final
{
    explicit ParamsPatternExcPositionNative(IRuntimeStringManagerSource& strings);
    ~ParamsPatternExcPositionNative() noexcept(false);
    ParamsPatternExcPositionNative(const ParamsPatternExcPositionNative&) = delete;
    ParamsPatternExcPositionNative& operator=(const ParamsPatternExcPositionNative&) = delete;
    std::uint32_t kindAt00;
    union { RuntimeCString nameAt04; };
    union { RuntimeCString subNameAt08; };
    std::uint32_t xAt0C;
    std::uint32_t yAt10;
};

struct ParamsPatternFileConvertNative final
{
    explicit ParamsPatternFileConvertNative(IRuntimeStringManagerSource& strings);
    ~ParamsPatternFileConvertNative() noexcept(false);
    ParamsPatternFileConvertNative(const ParamsPatternFileConvertNative&) = delete;
    ParamsPatternFileConvertNative& operator=(const ParamsPatternFileConvertNative&) = delete;
    std::uint32_t kindAt00;
    union { RuntimeCString nameAt04; };
    union { RuntimeCString subNameAt08; };
    std::uint32_t flagsAt0C;
};

struct ParamsPatternGroupNative final
{
    explicit ParamsPatternGroupNative(IRuntimeStringManagerSource& strings);
    ~ParamsPatternGroupNative() noexcept;
    ParamsPatternGroupNative(const ParamsPatternGroupNative&) = delete;
    ParamsPatternGroupNative& operator=(const ParamsPatternGroupNative&) = delete;
    union { RuntimeCString nameAt00; };
    union { YtlReferenceSlotRuntime indicesAt04; };
};

static_assert(sizeof(ParamsPatternFileNameNative) == 0x08);
static_assert(offsetof(ParamsPatternFileNameNative, kindAt00) == 0);
static_assert(offsetof(ParamsPatternFileNameNative, nameAt04) == 4);
static_assert(sizeof(ParamsPatternFileGroupNative) == 0x14);
static_assert(offsetof(ParamsPatternFileGroupNative, kindAt00) == 0);
static_assert(offsetof(ParamsPatternFileGroupNative, nameAt04) == 4);
static_assert(offsetof(ParamsPatternFileGroupNative, stringsAt08) == 8);
static_assert(sizeof(ParamsPatternExcPositionNative) == 0x14);
static_assert(offsetof(ParamsPatternExcPositionNative, kindAt00) == 0);
static_assert(offsetof(ParamsPatternExcPositionNative, nameAt04) == 4);
static_assert(offsetof(ParamsPatternExcPositionNative, subNameAt08) == 8);
static_assert(offsetof(ParamsPatternExcPositionNative, xAt0C) == 0x0C);
static_assert(offsetof(ParamsPatternExcPositionNative, yAt10) == 0x10);
static_assert(sizeof(ParamsPatternFileConvertNative) == 0x10);
static_assert(offsetof(ParamsPatternFileConvertNative, kindAt00) == 0);
static_assert(offsetof(ParamsPatternFileConvertNative, nameAt04) == 4);
static_assert(offsetof(ParamsPatternFileConvertNative, subNameAt08) == 8);
static_assert(offsetof(ParamsPatternFileConvertNative, flagsAt0C) == 0x0C);
static_assert(sizeof(ParamsPatternGroupNative) == 0x10);
static_assert(offsetof(ParamsPatternGroupNative, nameAt00) == 0);
static_assert(offsetof(ParamsPatternGroupNative, indicesAt04) == 4);

// 仅原位构造，不分配，不赋予失败正文任何额外清理资格。
[[nodiscard]] ParamsPatternFileNameNative* ConstructParamsPatternFileNameAt40C3D0(
    ParamsPatternFileNameNative* storage,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] ParamsPatternFileGroupNative* ConstructParamsPatternFileGroupAt40C380(
    ParamsPatternFileGroupNative* storage,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] ParamsPatternExcPositionNative* ConstructParamsPatternExcPositionAt41EB58(
    ParamsPatternExcPositionNative* storage,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] ParamsPatternFileConvertNative* ConstructParamsPatternFileConvertAt41ECFB(
    ParamsPatternFileConvertNative* storage,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] ParamsPatternGroupNative* ConstructParamsPatternGroupAt4124DD(
    ParamsPatternGroupNative* storage,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 四种工厂返回创建时缓存正文；不根据回调改过的 out.base 重新解析返回值。
// <4单文件与4..5.6单文件都没有raw正文构造清理；前者发布新typed槽，后者走407B20。
[[nodiscard]] ParamsPatternFileNameNative* CreateParamsPatternFileNameAt41E080(
    YtlReferenceSlotRuntime& out,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] ParamsPatternFileNameNative* CreateParamsPatternFileNameAt41E47C(
    YtlReferenceSlotRuntime& out,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// kind0/1 只在 raw ctor 展开时回收裸块，control 分配阶段已撤该资格。
// kind1/2/3 的 out 为原新局部槽，不释放旧引用；typed owner 资格由 reader 在返回后取得。
[[nodiscard]] ParamsPatternFileNameNative* CreateParamsPatternFileNameAt41E904(
    YtlReferenceSlotRuntime& out,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] ParamsPatternFileGroupNative* CreateParamsPatternFileGroupAt41E972(
    YtlReferenceSlotRuntime& out,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] ParamsPatternExcPositionNative* CreateParamsPatternExcPositionAt41EB42(
    YtlReferenceSlotRuntime& out,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] ParamsPatternFileConvertNative* CreateParamsPatternFileConvertAt41ECE8(
    YtlReferenceSlotRuntime& out,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 两个 Group 工厂入口均先释放旧 out，调用前已有的 owner 资格由 reader 持有。
// 第一表用缓存 control 写元数据；第二表 412490 每个后续字段重读 out.control。
[[nodiscard]] ParamsPatternGroupNative* CreateParamsPatternFirstGroupAt41F163(
    YtlReferenceSlotRuntime& out,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] ParamsPatternGroupNative* RebuildParamsPatternGroupAt412490(
    YtlReferenceSlotRuntime& out,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// Group 的零 wire count 跳过此调用；本叶 count=0 仍先释放旧 Ref。
[[nodiscard]] std::int32_t* RebuildParamsPatternGroupIndices(
    YtlReferenceSlotRuntime& out, std::uint32_t count,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// +10 外表的新局部 owner：不释放旧 out，不重写循环外已初始化的 bias。
// 正文无 cookie/不预清零，控制字段走缓存指针。
[[nodiscard]] std::int32_t* CreateParamsPatternOuterIntegersAt41EF45(
    YtlReferenceSlotRuntime& out, std::uint32_t count,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// raw 析构只销毁字段，不删除正文；只有两个含内 Ref 的正文整体 noexcept。
void DestroyParamsPatternFileName(ParamsPatternFileNameNative& body);
void DestroyParamsPatternFileGroupAt40F280(ParamsPatternFileGroupNative& body) noexcept;
void DestroyParamsPatternExcPosition(ParamsPatternExcPositionNative& body);
void DestroyParamsPatternFileConvert(ParamsPatternFileConvertNative& body);
void DestroyParamsPatternGroupAt40F7D0(ParamsPatternGroupNative& body) noexcept;

// 显式 Reset 可抛且 payload 后重读当前 control；reader 的正常缓存释放另由调用方实现。
void ReleaseParamsPatternItemAt43B810(YtlReferenceSlotRuntime& reference);
void ReleaseParamsPatternGroupAt43B850(YtlReferenceSlotRuntime& reference);
void DestroyParamsPatternItemReferenceAt40D590(YtlReferenceSlotRuntime& reference) noexcept;
void DestroyParamsPatternGroupReferenceAt40D670(YtlReferenceSlotRuntime& reference) noexcept;

// 按 base+bias*stride 解址，不检查 count/kind；typed 合法性由消费分支保证。
[[nodiscard]] ParamsPatternFileNameNative* ResolveParamsPatternFileName(
    YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] const ParamsPatternFileNameNative* ResolveParamsPatternFileName(
    const YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] ParamsPatternFileGroupNative* ResolveParamsPatternFileGroup(
    YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] const ParamsPatternFileGroupNative* ResolveParamsPatternFileGroup(
    const YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] ParamsPatternExcPositionNative* ResolveParamsPatternExcPosition(
    YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] const ParamsPatternExcPositionNative* ResolveParamsPatternExcPosition(
    const YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] ParamsPatternFileConvertNative* ResolveParamsPatternFileConvert(
    YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] const ParamsPatternFileConvertNative* ResolveParamsPatternFileConvert(
    const YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] ParamsPatternGroupNative* ResolveParamsPatternGroup(
    YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] const ParamsPatternGroupNative* ResolveParamsPatternGroup(
    const YtlReferenceSlotRuntime& reference) noexcept;
}
