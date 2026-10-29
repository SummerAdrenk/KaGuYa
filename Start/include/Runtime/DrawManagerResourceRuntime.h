#pragma once

#include <string>
#include <cstdint>
#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/RuntimeCString.h>
#include <Runtime/RootArchiveResourceSource.h>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentKeyTableSource;
struct FileTransactionContext;
class IStartFileApi;
class ResourceCatalog;
class RuntimeGraphicsSurface;
class SpriteRuntime;
struct SpriteNativeBodyA0h;
struct AnimationFormatNativePictureServices;
class RootRuntimeOwner;

class IGraphicsResourceErrorDisplay
{
public:
    virtual ~IGraphicsResourceErrorDisplay() = default;
    virtual void Show(const char* messageCp932) = 0;
};

// 4800A0 原地修改传入的 Surface；引用的创建、保留和释放由调用者控制。
[[nodiscard]] bool LoadDrawManagerStaticInto(
    RuntimeGraphicsSurface& target,
    const std::string& nameCp932,
    const ResourceCatalog& catalog,
    const FileTransactionContext* transaction,
    IStartFileApi& files,
    const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs);

// 现代名称持有适配；生产 Demo 使用下方捕获 chars 的入口。
[[nodiscard]] bool LoadDrawManagerSprite(
    SpriteRuntime& target,
    const std::string& nameCp932,
    const ResourceCatalog& catalog,
    const FileTransactionContext* transaction,
    IStartFileApi& files,
    const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs);

[[nodiscard]] bool LoadDrawManagerStaticBorrowedAt4800A0(
    RuntimeGraphicsSurface& target, const char* nameCp932,
    IArchiveResourceSource& source, const FileTransactionContext* transaction,
    IStartFileApi& files, const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] bool LoadDrawManagerSpriteBorrowedAt4814F0(
    SpriteRuntime& target, const char* nameCp932, std::uint8_t restart,
    IArchiveResourceSource& source, const FileTransactionContext* transaction,
    IStartFileApi& files, const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
// 原34h/44h等caller直接提交A0h，保留同一factory/reader/错误分类，不构造现代Sprite。
[[nodiscard]] bool LoadDrawManagerSpriteNativeAt4814F0(SpriteNativeBodyA0h* target,
    const char* nameCp932, std::uint8_t restart, IArchiveResourceSource& source,
    const FileTransactionContext* transaction, IStartFileApi& files, const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplay& errors, AnimationFormatNativePictureServices& pictures);

class ISpriteNativeResourceSource
{
public:
    virtual ~ISpriteNativeResourceSource() = default;
    virtual void* CaptureDrawReceiverAt413BE0() = 0;
    virtual bool LoadAt4814F0(void* receiver, SpriteNativeBodyA0h* target,
        const char* name, std::uint8_t restart) = 0;
};
// receiver只供原caller捕获时点，4814F0本身不消费它；资源读取仍另行晚取FileManager。
class DrawManagerSpriteNativeResourceRuntime final : public ISpriteNativeResourceSource
{
public:
    DrawManagerSpriteNativeResourceRuntime(RootRuntimeOwner&, IStartFileApi&, const IEnvironmentKeyTableSource&,
        IGraphicsResourceErrorDisplay&, AnimationFormatNativePictureServices&,
        const FileTransactionContext* = nullptr) noexcept;
    void* CaptureDrawReceiverAt413BE0() override;
    bool LoadAt4814F0(void*, SpriteNativeBodyA0h*, const char*, std::uint8_t) override;
private:
    RootRuntimeOwner& root_;
    RootArchiveResourceSource archives_;
    IStartFileApi& files_;
    const IEnvironmentKeyTableSource& key_;
    IGraphicsResourceErrorDisplay& errors_;
    AnimationFormatNativePictureServices& pictures_;
    const FileTransactionContext* transaction_;
};
}
