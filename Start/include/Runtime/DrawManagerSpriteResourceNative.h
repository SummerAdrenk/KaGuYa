#pragma once

#include <Runtime/DrawManagerResourceRuntime.h>
#include <Runtime/GraphicsResourceErrorNative.h>

namespace KaGuYa::Start::Runtime
{
// 明确选择原生CString错误域；与旧IGraphicsResourceErrorDisplay的重载不作失败切换。
[[nodiscard]] bool LoadDrawManagerSpriteNativeAt4814F0(SpriteNativeBodyA0h* target,
    const char* name, std::uint8_t restart, IArchiveResourceSource& source,
    const FileTransactionContext* transaction, IStartFileApi& files, const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplayNative& errors, AnimationFormatNativePictureServices& pictures);

class RootDrawManagerSpriteResourceNative final : public ISpriteNativeResourceSource
{
public:
    RootDrawManagerSpriteResourceNative(RootRuntimeOwner& root, IStartFileApi& files,
        const IEnvironmentKeyTableSource& key, IGraphicsResourceErrorDisplayNative& errors,
        AnimationFormatNativePictureServices& pictures, const FileTransactionContext* transaction = nullptr) noexcept;
    void* CaptureDrawReceiverAt413BE0() override;
    bool LoadAt4814F0(void* receiver, SpriteNativeBodyA0h* target, const char* name, std::uint8_t restart) override;
private:
    RootRuntimeOwner& root_;
    RootArchiveResourceSource archives_;
    IStartFileApi& files_;
    const IEnvironmentKeyTableSource& key_;
    IGraphicsResourceErrorDisplayNative& errors_;
    AnimationFormatNativePictureServices& pictures_;
    const FileTransactionContext* transaction_;
};
}
