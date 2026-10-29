#pragma once

#include <Runtime/ReadScriptEffectFactoryNative.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 全部借用。生产archives接RootArchiveResourceSource，program接同一Environment owner；
// copy接GraphicsSurfaceNativeApi，每块各取Graphics根，不缓存module或导出。
struct ReadScriptEffectMaskNativeDependencies final
{
    EnvironmentRootOwner& program;
    IArchiveResourceSource& archives;
    IStartFileApi& files;
    const IEnvironmentKeyTableSource& key;
    ArchiveCodecServices codecs;
    IArchiveReaderMemoryApi& readers;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsSurfaceRawCopyApi& copy;
    IRuntimeStringManagerSource& strings;
    decltype(&::MessageBoxA) messageBox = &::MessageBoxA;
    const FileTransactionContext* transaction = nullptr;
};

// 480B30：只生产原24h Surface/Ref12，不持有receiver、reader、CString或输出镜像。
// callee没有读ECX；caller仍须在调用前执行原Draw根捕获，不能以此省略caller步骤。
// 失败保留已发布output及Create/平铺前缀，不恢复旧图，不改成共享Surface。
// 零步长、DWORD回绕、空output解址均不追加guard；调用方不得把原非终止域当成功。
// 复用依赖的明确边界（本文件不改这些叶）：483D90的reader分配null仍转bad_alloc；
// 451500/407A80的control-null仍为显式终止；419D70的空reader/有界payload检查、
// RootArchiveResourceSource的缺root/manager检查仍存在。上述非法域不宣称原AV等价。
// ArchiveReaderFactoryRuntime为现代外部服务壳，reader正文14h/Ref12真实；其尾+10
// 未知DWORD不被本Producer读取，也不宣称该factory壳是原14h二进制正文。
// reader工厂内部CString仍用其现有默认进程来源；strings仅注入本入口与Surface。
// 生产应传同一默认来源；测试替换strings不代表全链417360已被替换。
class ReadScriptEffectMaskNative final : public IReadScriptEffectMaskNativeServices
{
public:
    explicit ReadScriptEffectMaskNative(ReadScriptEffectMaskNativeDependencies dependencies) noexcept;
    bool LoadMaskAt480B30(DrawManagerNativeBody2Ch* receiver,
        YtlReferenceSlotRuntime& output, const char* name) override;
private:
    ReadScriptEffectMaskNativeDependencies dependencies_;
};
}
