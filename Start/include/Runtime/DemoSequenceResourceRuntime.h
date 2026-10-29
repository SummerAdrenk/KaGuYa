#pragma once
#include <Runtime/ProgramWindowFieldReferences.h>

#include <Runtime/DemoSequenceRuntime.h>
#include <Runtime/ArchiveEntryReader.h>
#include <Runtime/DrawManagerResourceRuntime.h>
#include <Runtime/SoundSlotManagerRuntime.h>
#include <Runtime/DemoSoundManagerReceiver.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class RootRuntimePayload;
class EnvironmentSoundParamsRuntime;

using IDemoResourceErrorDisplay = IGraphicsResourceErrorDisplay;

// 现代 SaveData registry 尚非原 +30 的完整 ABI。装配方必须按本次 receiver
// 解析实际 owner；不能忽略 receiver，再从固定 archive/名字镜像取值。
class IDemoSequenceEnvironmentArchives
{
public:
    virtual ~IDemoSequenceEnvironmentArchives() = default;
    virtual EnvironmentSoundParamsRuntime& ResolveSoundParamsAt4188C0(
        EnvironmentRootStorage& receiver) = 0;
    virtual RuntimeCString& ResolveMusicNameAt4BF74E(EnvironmentRootStorage& receiver) = 0;
};

class IDemoSequenceRuntimeSource : public IDemoSequenceEnvironmentArchives
{
public:
    virtual RootRuntimePayload* CaptureRootAt413BE0() = 0;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    virtual DemoSoundManagerReceiver ResolveSoundManagerAt4188F0(RootRuntimePayload* receiver) = 0;
};

// 两个 getter 独立调用；借用捕获正文，不锁住整段命令，也不增持 root 或 SoundParams。
class DemoSequenceRootSource final : public IDemoSequenceRuntimeSource
{
public:
    DemoSequenceRootSource(RootRuntimeOwner& root, EnvironmentRootOwner& environment,
        IDemoSequenceEnvironmentArchives& archives) noexcept;
    RootRuntimePayload* CaptureRootAt413BE0() override;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    DemoSoundManagerReceiver ResolveSoundManagerAt4188F0(RootRuntimePayload* receiver) override;
    EnvironmentSoundParamsRuntime& ResolveSoundParamsAt4188C0(EnvironmentRootStorage& receiver) override;
    RuntimeCString& ResolveMusicNameAt4BF74E(EnvironmentRootStorage& receiver) override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    IDemoSequenceEnvironmentArchives& archives_;
};

class Win32DemoResourceErrorDisplay final : public IDemoResourceErrorDisplay
{
public:
    Win32DemoResourceErrorDisplay(ProgramWindowHandleReference window, const std::string& gameTitleCp932) noexcept;
    void Show(const char* messageCp932) override;
private:
    ProgramWindowHandleReference window_;
    const std::string& title_;
};

// 480000/4800A0、4814F0 和 4BF5A0 的实际 VFS、图片解码、声音提交。
class DemoSequenceArchiveResourceServices final : public IDemoSequenceResourceServices
{
public:
    DemoSequenceArchiveResourceServices(IDemoSequenceRuntimeSource& source,
        IGraphicsSurfaceApi& graphics, IArchiveResourceSource& archives,
        const FileTransactionContext* transaction, IStartFileApi& files,
        const IEnvironmentKeyTableSource& key, IDemoResourceErrorDisplay& errors,
        ArchiveCodecServices codecs) noexcept;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    void* CaptureDrawManagerAt413BE0() override;
    bool LoadDemoSpriteAt4814F0(void* receiver, SpriteRuntime& target,
        const char* borrowedName, std::uint8_t flag) override;
    bool LoadDemoMaskAt480000(void* receiver, DemoSurfaceSlot& target,
        const char* borrowedName) override;
    void ExecuteDemoSoundAt4BF692(const ParamsDemoCmdSound& body) override;
private:
    IDemoSequenceRuntimeSource& source_;
    IGraphicsSurfaceApi& graphics_;
    IArchiveResourceSource& archives_;
    const FileTransactionContext* transaction_;
    IStartFileApi& files_;
    const IEnvironmentKeyTableSource& key_;
    IDemoResourceErrorDisplay& errors_;
    ArchiveCodecServices codecs_;
};
}
