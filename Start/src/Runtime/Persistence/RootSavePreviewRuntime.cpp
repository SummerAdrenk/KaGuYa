#include <Runtime/RootSavePreviewRuntime.h>
#include <Runtime/GraphicsSurfaceSaveApi.h>
#include <Runtime/SavePreviewCaptureApi.h>
#include <Runtime/PersistenceFileAccess.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class PreviewOutputFile final
{
public:
    PreviewOutputFile(IStartFileApi& files, const char* path) : files_(files)
    {
        SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
        const FileOpenRequest request{path, GENERIC_WRITE, 0, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, &security};
        handle_ = OpenFileUsingStartPolicy(request, nullptr, files_);
        if (handle_ == INVALID_HANDLE_VALUE)
        {
            // 42BAE0 只在 HRESULT 非零时清文件 Ref。句柄为零不等于 Ref 不存在。
            hasFileReference_ = files_.LastError() == ERROR_SUCCESS;
            handle_ = nullptr;
        }
    }
    // 原CStreamFile析构40ED70的EHFlags=5，Close异常不得被转成普通false。
    ~PreviewOutputFile() noexcept { if (handle_) files_.Close(handle_); }
    [[nodiscard]] HANDLE Handle() const noexcept { return handle_; }
    [[nodiscard]] bool HasFileReference() const noexcept { return hasFileReference_; }
private:
    IStartFileApi& files_;
    HANDLE handle_ = nullptr;
    bool hasFileReference_ = true;
};
}

RootSavePreviewRuntime::RootSavePreviewRuntime(RootSavePreviewSlots& slots, IGraphicsSurfaceApi& surfaces,
    IGraphicsWindowSurfaceApi& pixels, IGraphicsSurfaceSaveApi& saver,
    IStartFileApi& files, ISavePreviewCaptureServices& capture, SIZE thumbnailSize) noexcept
    : slots_(slots), surfaces_(surfaces), pixels_(pixels), saver_(saver), files_(files), capture_(capture),
      thumbnailSize_(thumbnailSize) {}

void RootSavePreviewRuntime::ReleaseAt46DC90()
{
    GraphicsSurfaceNativeScope scope(surfaces_);
    // 46DC93先缩略图，46DCD2后大图；回调后重读control，无control时不清base/bias。
    ReleaseYtlReference(slots_.thumbnailAt1D8);
    ReleaseYtlReference(slots_.captureAt1E4);
}

void RootSavePreviewRuntime::SetCaptureAt48CFF0(const YtlReferenceSlotRuntime& source)
{
    GraphicsSurfaceNativeScope scope(surfaces_);
    ReleaseAt46DC90();
    // 48CFF7先清双槽，48D005才判同址；不能提前复制或增持source。
    if (&slots_.captureAt1E4 == &source) return;
    ReleaseYtlReference(slots_.captureAt1E4);
    CopyGraphicsSurfaceReferenceAt47D740(slots_.captureAt1E4, source);
}

void RootSavePreviewRuntime::CaptureForSaveAt48DBE0()
{
    GraphicsSurfaceNativeScope scope(surfaces_);
    ReleaseAt46DC90();
    try
    {
        capture_.CaptureIntoAt48DBE0(slots_.captureAt1E4);
    }
    catch (...)
    {
        // FuncInfo5E6B44 -> try5E6B30(states0..5) -> catch-all5E6B20/48DFDC。
        ReleaseAt46DC90();
    }
}

void RootSavePreviewRuntime::GenerateThumbnailAt475FA0()
{
    GraphicsSurfaceNativeScope scope(surfaces_);
    // 01:44D8F0、09:44B720、18:475FA0；逐款尺寸绑定，余数/采样顺序不变。
    const std::int32_t PreviewWidth = thumbnailSize_.cx;
    const std::int32_t PreviewHeight = thumbnailSize_.cy;
    try
    {
        if (!slots_.captureAt1E4.baseAt00) throw 1;
        if (PreviewWidth <= 0 || PreviewHeight <= 0)
            throw std::invalid_argument("save preview dimensions are not bound");
        auto* const created = NewGraphicsSurfaceAt40C2E0(surfaces_);
        AdoptGraphicsSurfaceAt407A80(slots_.thumbnailAt1D8, created);
        if (!ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slots_.thumbnailAt1D8)->
            CreateAt415010(surfaces_, PreviewWidth, PreviewHeight, false, true)) throw 1;
        const auto* const dimensions =
            ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slots_.captureAt1E4);
        const auto width = dimensions->Width();
        const auto height = dimensions->Height();
        // 4760EC..476170每次查询前重新解槽，不跨Graphics回调保活或缓存正文。
        const auto* source = pixels_.GetSurfacePointerRaw(
            ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slots_.captureAt1E4)->Handle());
        const auto sourcePitch = pixels_.GetSurfacePitchRaw(
            ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slots_.captureAt1E4)->Handle());
        auto* destination = pixels_.GetSurfacePointerRaw(
            ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slots_.thumbnailAt1D8)->Handle());
        const auto destinationPitch = pixels_.GetSurfacePitchRaw(
            ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slots_.thumbnailAt1D8)->Handle());
        // 原过小尺寸产生零除/无效地址。保留查询前缀后明确拒绝，不制造插值结果。
        if (!source || !destination || width < PreviewWidth || height < PreviewHeight ||
            sourcePitch < static_cast<std::int64_t>(width) * 4 ||
            destinationPitch < static_cast<std::int64_t>(PreviewWidth) * 4)
            throw std::invalid_argument("save preview source has invalid dimensions or pitch");

        const auto baseWidth = width / PreviewWidth;
        const auto remainderWidth = width % PreviewWidth;
        const auto baseHeight = height / PreviewHeight;
        const auto remainderHeight = height % PreviewHeight;
        std::int32_t sourceY = 0, carryY = 0;
        for (std::int32_t y = 0; y < PreviewHeight; ++y)
        {
            const auto rows = baseHeight + carryY / PreviewHeight;
            carryY %= PreviewHeight;
            std::int32_t sourceX = 0, carryX = 0;
            for (std::int32_t x = 0; x < PreviewWidth; ++x)
            {
                const auto columns = (std::min)(baseWidth + carryX / PreviewWidth, width - sourceX);
                carryX %= PreviewWidth;
                std::array<std::uint32_t, 3> sums{};
                for (std::int32_t dy = 0; dy < rows; ++dy)
                {
                    const auto* row = source + static_cast<std::ptrdiff_t>(sourceY + dy) * sourcePitch;
                    for (std::int32_t dx = 0; dx < columns; ++dx)
                    {
                        std::uint32_t pixel;
                        std::memcpy(&pixel, row + static_cast<std::ptrdiff_t>(sourceX + dx) * 4, 4);
                        sums[0] += pixel & 0xffu;
                        sums[1] += (pixel >> 8) & 0xffu;
                        sums[2] += (pixel >> 16) & 0xffu;
                    }
                }
                // 476395..4763D7的DWORD和、无符号DIV与低byte截取；alpha输出固定零。
                const auto count = static_cast<std::uint32_t>(rows) * static_cast<std::uint32_t>(columns);
                if (!count) throw std::invalid_argument("save preview area is zero");
                const std::uint32_t output = (sums[0] / count & 0xffu) |
                    ((sums[1] / count & 0xffu) << 8) | ((sums[2] / count & 0xffu) << 16);
                std::memcpy(destination + static_cast<std::ptrdiff_t>(y) * destinationPitch + x * 4, &output, 4);
                sourceX += columns;
                carryX += remainderWidth;
            }
            sourceY += rows;
            carryY += remainderHeight;
        }
    }
    catch (...)
    {
        // FuncInfo5E67AC -> try5E6780 -> catch-all5E6770/47641A：清双Ref后返回。
        ReleaseAt46DC90();
    }
}

YtlReferenceSlotRuntime* RootSavePreviewRuntime::PreviewAt47D740(YtlReferenceSlotRuntime& out)
{
    if (!slots_.thumbnailAt1D8.baseAt00) GenerateThumbnailAt475FA0();
    if (!slots_.thumbnailAt1D8.baseAt00) throw 1;
    CopyGraphicsSurfaceReferenceAt47D740(out, slots_.thumbnailAt1D8);
    return &out;
}

GraphicsSurfaceReference RootSavePreviewRuntime::PreviewAt47D740()
{
    GraphicsSurfaceReference result(surfaces_);
    static_cast<void>(PreviewAt47D740(result.Slot()));
    return result;
}

bool RootSavePreviewRuntime::SavePreviewAt489420(const std::string& fullPath)
{
    return SavePreviewAt489420(fullPath.c_str());
}
bool RootSavePreviewRuntime::SavePreviewAt489420(const char* fullPath)
{
    const auto preview = PreviewAt47D740(); // 强Ref跨文件打开、导出调用和Close存活。
    // 489492..4894A0在Open前解局部Ref，48950A在Open后才读取这个正文的handle。
    const auto* const surface = preview.Get();
    PreviewOutputFile file(files_, fullPath);
    // 4894F1 / 4895A2：无文件 Ref 跳过 Graphics，局部 AL=0，最终返回 true。
    if (!file.HasFileReference()) return true;
    return saver_.SaveSurfaceIfLoaded(surface->Handle(), file.Handle()) == 0;
}

GraphicsSurfaceReference RootSavePreviewRuntime::ThumbnailAt1D8() const noexcept
{ return GraphicsSurfaceReference(slots_.thumbnailAt1D8, surfaces_); }
GraphicsSurfaceReference RootSavePreviewRuntime::CaptureAt1E4() const noexcept
{ return GraphicsSurfaceReference(slots_.captureAt1E4, surfaces_); }
const YtlReferenceSlotRuntime& RootSavePreviewRuntime::RawThumbnailAt1D8() const noexcept
{ return slots_.thumbnailAt1D8; }
const YtlReferenceSlotRuntime& RootSavePreviewRuntime::RawCaptureAt1E4() const noexcept
{ return slots_.captureAt1E4; }
}
