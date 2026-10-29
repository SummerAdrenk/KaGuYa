#pragma once

#include <Runtime/ArchiveEntryReader.h>
#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/SpriteRuntime.h>
#include <Runtime/LoadSpriteErrorEnvironment.h>

#include <array>
#include <string>

namespace KaGuYa::Start::Runtime
{
class IRuntimeStringManagerSource;
IRuntimeStringManagerSource& DefaultRuntimeStringManagerSource() noexcept;
// 18 号 PE 的 6014D0/601470 字节表；不从本机文件名猜测资源。
struct CountDownWindowResourceProfile final
{
    std::array<std::string, 60> numberNamesCp932;
    std::array<std::string, 12> suffixNamesCp932;
    std::array<POINT, 6> numberPositions;
    std::array<POINT, 6> suffixPositions;
};

[[nodiscard]] CountDownWindowResourceProfile BuildMamatomoCountDownWindowResourceProfile();

class ICountDownWindowMessageApi
{
public:
    virtual ~ICountDownWindowMessageApi() = default;
    virtual void MessageBoxAt481615(HWND window, const char* textCp932,
        const char* captionCp932, UINT type) = 0;
};

class Win32CountDownWindowMessageApi final : public ICountDownWindowMessageApi
{
public:
    void MessageBoxAt481615(HWND window, const char* textCp932,
        const char* captionCp932, UINT type) override;
};

// 4814F0 的实际 reader/decoder/error 分流。外部依赖均借用，须长于使用者。
class CountDownWindowResources final
{
public:
    CountDownWindowResources(const ResourceCatalog& catalog,
        const FileTransactionContext* transaction, IStartFileApi& files,
        const IEnvironmentKeyTableSource& key, ILoadSpriteErrorEnvironmentSource& environment,
        ICountDownWindowMessageApi& messages, ArchiveCodecServices codecs,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    [[nodiscard]] bool LoadSpriteAt4814F0(SpriteRuntime& sprite,
        const std::string& nameCp932, bool playAnimationFromStart);

private:
    const ResourceCatalog& catalog_;
    const FileTransactionContext* transaction_;
    IStartFileApi& files_;
    const IEnvironmentKeyTableSource& key_;
    ILoadSpriteErrorEnvironmentSource& environment_;
    ICountDownWindowMessageApi& messages_;
    ArchiveCodecServices codecs_;
    IRuntimeStringManagerSource& strings_;
};
}
