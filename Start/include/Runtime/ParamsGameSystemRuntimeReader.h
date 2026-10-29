#pragma once

#include <Runtime/ParamsRuntimeProfile.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeReadCursor;

// +10：byte数量在重建成功后消费；每轮从当前control读取signed count。
void ReadParamsStartupBytesAt41AE52(ParamsRuntimeReadCursor& cursor, ParamsByteArray& bytes);
// +AC：DWORD数量先消费，零保留旧Ref；非零重建后晚取输入，正文复制成功才提交。
void ReadParamsRawBlobAt41CD56(ParamsRuntimeReadCursor& cursor, ParamsByteArray& bytes);

// +A0：先清活动范围；四字节槽只以低BYTE计数，逐DWORD重新解析目标begin。
// rawCountMetadata仅供当前现代profile快照比较，不是原GS正文的额外字段。
void ReadParamsTriplesAt41CCCE(ParamsRuntimeReadCursor& cursor, ParamsTripleTable& triples,
    std::uint32_t& rawCountMetadata,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 41D023..41D08C：+C4 原 CString 数组Ref；零count保留旧表，正数先发布整数组。
// 循环使用缓存的线上count，每项再解析当前Ref；失败保留已写前缀。
void ReadParamsStringListAt41D023(ParamsRuntimeReadCursor& cursor, ParamsCStringArray& strings,
    IRuntimeStringManagerSource& managers = DefaultRuntimeStringManagerSource());

// 41D08F..41D1EE：+D0 place数组；name返回后重读Ref定位value，写前才检查空目标。
void ReadParamsPlacesAt41D08F(ParamsRuntimeReadCursor& cursor, ParamsPlaceArray& places,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 41D21F..41D3FE：不清旧表；非零inner count构造临时串/内vector，完成后复制追加。
void ReadParamsNestedListsAt41D21F(ParamsRuntimeReadCursor& cursor, ParamsNestedStringTable& lists,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 41D411..41D74C：真实thumbnail Ref表；目标表先清，完成一项才追加。
void ReadParamsThumbnailsAt41D411(ParamsRuntimeReadCursor& cursor, ParamsThumbnailTable& thumbnails,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 41D74C..41D854：真实CString vector，非零count只构造一个跨项复用的临时串。
// 负signed count仍构造/释放该临时串；clear保留容量，失败不撤销已追加前缀。
void ReadParamsSceneNamesAt41D74C(ParamsRuntimeReadCursor& cursor, ParamsSceneNameTable& names,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 41D857..41DBF6：真实group/item Ref表；完成一组才发布，失败保留已发布前缀。
void ReadParamsRegistCgAt41D857(ParamsRuntimeReadCursor& cursor, ParamsRegistCgTable& groups,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 41DBF7..41DF15：真实两层Ref与CString vector；局部格式按 >=5.8 分支。
// 旧版每item先读唯一scene再读name；此入口不开放整份旧版GameSystem。
void ReadParamsRegistSceneAt41DBF7(ParamsRuntimeReadCursor& cursor, ParamsRegistSceneTable& groups,
    double version = 5.8,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 41ADE0：按原版本比较读取字段；版本头准入由434940负责，不额外增加reader门禁。
// 按原表级发布顺序读取，失败不回滚；Cursor 的安全截断不等价于原版越界访问。
// 接收唯一11Ch正文；完整根Ref拥有关系另外验收，不以布局正确替代工厂闭合。
// 40C410 仅将 +08/+0C 尺寸置零，并初始化空字符串和空容器；未写 +00、
// +04 marker、+2C defaultNameMode、+44/+48/+4C/+50 soundSlotCounts、+54 nameSelectionMode。
// marker/nameSelectionMode是历史成员名；+04/+54业务用途未证，不能据名称新增分派。
// 生产使用RawAt40C410，默认构造的数值零值仅属现代fixture；reader不整对象重置。
// 两处byte数组已使用真实Ref及未初始化正文；不以vector零尾部代替部分发布。
// place数组已保留40C930仅构造name、未写value的原状态，不再先把value补零。
// Demo已为原生命令Ref，不能套用严格工具DecodeParamsDemoCommand的拒绝条件。
// 41CD6E：负 signed blob 长度的空目标 memmove 无安全等价表示；实际字节可读时
// 抛 InvalidUnitCount，不可读时仍由 Cursor 抛 TruncatedInput；旧 blob 已清空。
void ReadParamsGameSystemAt41ADE0(
    ParamsRuntimeReadCursor& cursor,
    ParamsGameSystem& gameSystem,
    double version,
    std::uint32_t* rawTripleCountMetadata = nullptr);
}
