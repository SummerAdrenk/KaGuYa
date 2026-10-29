#include <Runtime/DrawManagerSpriteResourceNative.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
RootDrawManagerSpriteResourceNative::RootDrawManagerSpriteResourceNative(RootRuntimeOwner& root,
    IStartFileApi& files, const IEnvironmentKeyTableSource& key, IGraphicsResourceErrorDisplayNative& errors,
    AnimationFormatNativePictureServices& pictures, const FileTransactionContext* transaction) noexcept
    : root_(root), archives_(root), files_(files), key_(key), errors_(errors), pictures_(pictures), transaction_(transaction) {}

void* RootDrawManagerSpriteResourceNative::CaptureDrawReceiverAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.ReadPublishedAt6198C4()->PostWindow().DrawManagerAt18();
}

bool RootDrawManagerSpriteResourceNative::LoadAt4814F0(void*, SpriteNativeBodyA0h* target,
    const char* name, std::uint8_t restart)
{
    // 原4814F0不消费此前caller取得的Draw receiver，VFS另按现场来源捕获FileManager。
    return LoadDrawManagerSpriteNativeAt4814F0(target, name, restart, archives_, transaction_, files_, key_, errors_, pictures_);
}
}
