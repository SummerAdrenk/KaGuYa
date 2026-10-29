#include <Runtime/ReadScriptTransitionMaskLoader.h>

#include <Runtime/ArchiveEntryReader.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/DrawManagerResourceRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/TransitRuntime.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原 PE 5B340C / 5B3430 / 5B3510；保持 CP932，不能改成 UTF-8 消息。
char readFailure[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
char loadFailure[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x83\x8D\x81\x5B\x83\x68\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
char createFailure[] = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
}

ReadScriptTransitionMaskLoaderRuntime::ReadScriptTransitionMaskLoaderRuntime(
    const std::uint32_t& programWidthAt18, const std::uint32_t& programHeightAt1C,
    const ResourceCatalog& catalog,
    const FileTransactionContext* transaction, IStartFileApi& files,
    const IEnvironmentKeyTableSource& key, IGraphicsSurfaceApi& surfaces,
    IGraphicsTransitApi& transit, IGraphicsResourceErrorDisplay& errors,
    ArchiveCodecServices codecs) noexcept
    : programWidthAt18_(programWidthAt18), programHeightAt1C_(programHeightAt1C),
      catalog_(catalog), transaction_(transaction), files_(files),
      key_(key), surfaces_(surfaces), transit_(transit), errors_(errors), codecs_(codecs) {}

bool ReadScriptTransitionMaskLoaderRuntime::LoadMaskAt480B30(
    std::shared_ptr<RuntimeGraphicsSurface>& output, const char* nameCp932)
{
    std::string error;
    try
    {
        ArchiveReaderMemoryApi memory;
        ArchiveReaderFactoryRuntime factory(catalog_, transaction_, files_, memory, codecs_);
        if (!factory.Load(std::string(nameCp932), key_)) throw readFailure;

        auto source = std::make_shared<RuntimeGraphicsSurface>(surfaces_);
        bool loaded = false;
        try
        {
            loaded = source->LoadArchiveReaderAt419D70(factory.Reader(), 0u);
        }
        catch (const StaticSurfaceResourceException&)
        {
            // 41FCB0 的格式识别失败在原 419D70 以 AL=0 返回。
        }
        if (!loaded) throw loadFailure;

        const auto width = static_cast<std::uint32_t>(source->Width());
        const auto height = static_cast<std::uint32_t>(source->Height());
        // 480C5B/480C69：先比较 width，仅相等才读 height；两项是 DWORD 比较。
        if (width == programWidthAt18_ && height == programHeightAt1C_)
        {
            // 465885..4658B2：旧 Ref release 完毕之后才发布新 source。
            output.reset();
            output = source;
            return true;
        }

        // 480CF4..480D68：先发布空 owner，再读取 height/width，Create 失败不回滚。
        if (!output) output = std::make_shared<RuntimeGraphicsSurface>(surfaces_);
        const auto targetHeight = std::bit_cast<std::int32_t>(programHeightAt1C_);
        const auto targetWidth = std::bit_cast<std::int32_t>(programWidthAt18_);
        if (!output->Create(targetWidth, targetHeight, true, false)) throw createFailure;

        // 480E73/480F81 用 JNB，每轮重读 Program；ADD 按 DWORD 回绕。
        // 无缩放、无源原点修正、无空矩形过滤；零步长的原始非终止边界见审计。
        for (std::uint32_t y = 0; y < programHeightAt1C_; y += height)
        {
            for (std::uint32_t x = 0; x < programWidthAt18_; x += width)
            {
                const auto sourceHandle = source->Handle();
                if (!output)
                    throw std::logic_error("480B30 output reference was removed during tiling");
                const auto destinationHandle = output->Handle();
                static_cast<void>(transit_.TransitCopyBlit(destinationHandle, sourceHandle,
                    std::bit_cast<std::int32_t>(x), std::bit_cast<std::int32_t>(y),
                    nullptr, nullptr, nullptr, 0));
            }
        }
        return true;
    }
    catch (const char* text)
    {
        // 5E9ED8 的 adjectives=1/type=.PAD；包含 const char*，非仅 char*。
        error = std::string(nameCp932) + " " + text;
    }
    catch (...)
    {
        error = "syntax error";
    }

    // 两个 handler 都返回到 481137，48115D MessageBoxA 在装载 try 之外。
    error += " [ CDrawManager::LoadEffect ]";
    errors_.Show(error.c_str());
    return false;
}
}
