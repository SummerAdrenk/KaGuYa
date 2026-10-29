#pragma once
#include <Runtime/CgUiButtonReaderNative.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>

namespace KaGuYa::Start::Runtime
{
std::span<const CgUiButtonRecordNative28h,12> ModeSelectButtonNativeRecordsSample18() noexcept;
std::span<const CgUiButtonRecordNative28h,12> GameModeSelectButtonRecords() noexcept;
class RootModeSelectButtonNativeSource final:public IModeSelectButtonNativeSource,
    private ICgUiButtonResourceErrorDisplay
{
public:
    RootModeSelectButtonNativeSource(RootRuntimeOwner&,EnvironmentStartupRuntime&,IStartFileApi&,
        const IEnvironmentKeyTableSource&,ArchiveCodecServices,IGraphicsSurfaceApi&,
        IGuiControlResourceMessageApi&,IRuntimeStringManagerSource& =DefaultRuntimeStringManagerSource(),
        const FileTransactionContext* =nullptr) noexcept;
    void* CaptureDrawAt45AD92() override;
    const CgUiButtonRecordNative28h* RecordAt601658(std::uint32_t) override;
    std::uint8_t LoadAt480930(void*,CgUiButtonNativeBody6Ch*,const CgUiButtonRecordNative28h*,std::int32_t) override;
private:
    void ShowAt480A2D(const RuntimeCString&) override;
    RootRuntimeOwner& root_;
    EnvironmentStartupRuntime& environment_;
    RootArchiveResourceSource archives_;
    IStartFileApi& files_;
    const IEnvironmentKeyTableSource& key_;
    ArchiveCodecServices codecs_;
    IGraphicsSurfaceApi& surfaces_;
    IGuiControlResourceMessageApi& messages_;
    IRuntimeStringManagerSource& strings_;
    const FileTransactionContext* transaction_;
};
}
