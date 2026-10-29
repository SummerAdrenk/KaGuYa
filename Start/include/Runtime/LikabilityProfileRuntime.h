#pragma once
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 02只借用主线已有archive/codec/图形服务；不是另一套decoder或虚拟文件目录。
struct LikabilityAnimationServices final
{
    IArchiveResourceSource& archives;
    IStartFileApi& files;
    IArchiveReaderMemoryApi& readers;
    const IEnvironmentKeyTableSource& key;
    ArchiveCodecServices codecs;
    IGraphicsWindowSurfaceApi& graphics;
    const FileTransactionContext* transaction = nullptr;
};
class ILikabilityProfileHost
{
public:
    virtual ~ILikabilityProfileHost() = default;
    virtual std::int32_t ReadLikabilityPage() = 0;
    virtual void StoreLikabilityPage(std::int32_t) = 0;
    virtual IGraphicsSurfaceRawCopyApi& LikabilityCopyApi() = 0;
    // 不需要动画的款不查询；02缺绑定明确throw，不能退成静态图。
    virtual LikabilityAnimationServices& LikabilityAnimation() = 0;
};
}
