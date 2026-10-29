#pragma once

#include <Runtime/GuiControlResourceNative.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class RootRuntimeOwner;

class IGuiControlResourceErrorDisplay
{
public:
    virtual ~IGuiControlResourceErrorDisplay() = default;
    // 4801B2先取environment，4801BC才读text槽；不得在source回调前缓存chars。
    virtual void ShowAt4801B2(const RuntimeCString& text) = 0;
};

// 480000只测base并惰建；receiver已经由调用方捕获，不在本函数额外Ensure。
// false或control分配失败不回滚已发布槽；删除需外部scope，测试才显式回收原孤儿。
[[nodiscard]] bool LoadGuiControlSurfaceReferenceAt480000(YtlReferenceSlotRuntime& target,
    void* capturedReceiver, const char* resourceName, IGuiControlResourceLoadServices& loader,
    IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 4800A0直接写唯一24h正文；source每次在483D90取目录，errors仅在失败尾调用。
// 不借现代Surface中转，不以factory返回bool代替原reader.base门控。
[[nodiscard]] bool LoadGuiControlResourceIntoAt4800A0(GraphicsSurfaceNativeBody& destination,
    const char* resourceName, IArchiveResourceSource& source,
    const FileTransactionContext* transaction, IStartFileApi& files,
    const IEnvironmentKeyTableSource& key, IGraphicsSurfaceApi& surfaces,
    IGuiControlResourceErrorDisplay& errors, ArchiveCodecServices codecs,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

class IGuiControlResourceMessageApi
{
public:
    virtual ~IGuiControlResourceMessageApi() = default;
    virtual int MessageBoxAt4801C2(HWND window, const char* textCp932,
        const char* captionCp932, UINT type) = 0;
};
class GuiControlResourceMessageApi final : public IGuiControlResourceMessageApi
{
public:
    int MessageBoxAt4801C2(HWND window, const char* textCp932,
        const char* captionCp932, UINT type) override;
};

// 外部现代装配：借用真实root/environment/codec/API，不驻留任何原生正文或factory1h。
// 构造不Ensure；每次Load有独立reader factory，目录与错误窗口在原调用点晚取。
class GuiControlResourceLoadRuntime final : public IGuiControlResourceLoadServices,
    private IGuiControlResourceErrorDisplay
{
public:
    GuiControlResourceLoadRuntime(RootRuntimeOwner& root, EnvironmentStartupRuntime& environment,
        IStartFileApi& files, const IEnvironmentKeyTableSource& key, ArchiveCodecServices codecs,
        IGraphicsSurfaceApi& surfaces, IGuiControlResourceMessageApi& messages,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
        const FileTransactionContext* transaction = nullptr) noexcept;
    void* CaptureLoadReceiverAt413BE0() override;
    bool LoadIntoAt4800A0(void* capturedReceiver, GraphicsSurfaceNativeBody& destination,
        const char* resourceName) override;
private:
    void ShowAt4801B2(const RuntimeCString& text) override;
    RootRuntimeOwner& root_;
    EnvironmentStartupRuntime& environment_;
    RootArchiveResourceSource archiveSource_;
    IStartFileApi& files_;
    const IEnvironmentKeyTableSource& key_;
    ArchiveCodecServices codecs_;
    IGraphicsSurfaceApi& surfaces_;
    IGuiControlResourceMessageApi& messages_;
    IRuntimeStringManagerSource& strings_;
    const FileTransactionContext* transaction_;
};
}
