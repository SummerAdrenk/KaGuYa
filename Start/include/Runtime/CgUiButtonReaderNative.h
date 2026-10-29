#pragma once
#include <Runtime/CgUiButtonNative.h>
#include <Runtime/ModeSelectButtonNative.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ICgUiButtonResourceErrorDisplay
{
public:
    virtual ~ICgUiButtonResourceErrorDisplay()=default;
    virtual void ShowAt480A2D(const RuntimeCString&)=0;
};
std::uint8_t LoadCgUiButtonReaderNativeAt48CAE0(CgUiButtonNativeBody6Ch&,std::uint32_t,
    const ArchiveReaderReference&,IGraphicsSurfaceApi&,
    IRuntimeStringManagerSource& =DefaultRuntimeStringManagerSource());
std::uint8_t LoadCgUiButtonRecordNativeAt480930(CgUiButtonNativeBody6Ch*,
    const CgUiButtonRecordNative28h*,std::int32_t,IArchiveResourceSource&,
    const FileTransactionContext*,IStartFileApi&,const IEnvironmentKeyTableSource&,
    IGraphicsSurfaceApi&,ICgUiButtonResourceErrorDisplay&,ArchiveCodecServices,
    IRuntimeStringManagerSource& =DefaultRuntimeStringManagerSource());
}
