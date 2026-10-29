#pragma once

#include <Runtime/SecretCgFlagRecords.h>
#include <Runtime/SecretCgMappingVector.h>

namespace KaGuYa::Start::Runtime
{
// 借用真实30h params根，不接收ParamsRuntimeProfile快照或复制后的CG表。
// callSite仅标明原捕获点，供根装配/验证核对；不属于原函数ABI参数。
class ISecretCgParamsRootSource
{
public:
    virtual ~ISecretCgParamsRootSource() = default;
    [[nodiscard]] virtual const void* CaptureAt418480(std::uint32_t callSite) = 0;
};

// 45BCA0正文的构造执行器；调用者提供未初始化的新对象字段，不能用于重置存活对象。
// 构造catch保留部分表和ready=0。虚表/外层正文分配由完整CSecretCG owner负责，
// SecretCgNativeArchive直接使用本执行体；输入必须是真实Params根或显式测试夹具。
void ConstructSecretCgTablesAt45BCA0(bool& readyAt04, std::uint32_t& baseKeyOffsetAt08,
    SecretCgReferenceVector& recordsAt0C, SecretCgMappingVectorStorage& mappingsAt18,
    ISecretCgParamsRootSource& params,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
