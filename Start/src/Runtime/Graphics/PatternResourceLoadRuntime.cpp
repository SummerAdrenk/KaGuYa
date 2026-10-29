#include <Runtime/PatternResourceLoadRuntime.h>
#include <Runtime/RuntimeCStringException.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原 PE 5B5234/5B5258 的 CP932 字节，包括开头空格；不经过格式化或现代编码转换。
constexpr char ReadFailureSuffix[] =
    "\x20\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x93\xC7\x82\xDD\x8D"
    "\x9E\x82\xDD\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
constexpr char LoadFailureSuffix[] =
    "\x20\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x83\x8D\x81\x5B\x83\x68"
    "\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";

[[noreturn]] void ThrowFailure(const IPatternResourceFailureName& failureName,
    const char* suffix, IRuntimeStringManagerSource& strings, PatternResourceAccess access)
{
    const auto* const prefix = failureName.ResolveFailurePrefix();
    if (access == PatternResourceAccess::Native)
        ThrowRuntimeCStringConcatAt466760(PatternResourceFailurePrefixAt466797(prefix), suffix, strings);
    if (!prefix) throw std::logic_error("Pattern resource failure prefix is unavailable");
    ThrowRuntimeCStringConcatAt466760(*prefix, suffix, strings);
}
}

std::uint8_t PatternResourceNativeInitializationSource::ReadRegistrySlot1ByteAt6D() const
{
    auto* const root = owner_.CaptureAt4180C0();
    // 4CCB8D..BD4：Leave后使用当前根；只按vector数量选index1，不retain或检查ready。
    const auto begin = reinterpret_cast<std::uintptr_t>(root->archivesAt30.begin);
    const auto bytes = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(root->archivesAt30.end) - begin);
    const auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
    const SaveDataEnvironmentArchive* archive = nullptr;
    if (count > 1u)
        archive = ResolveEnvironmentObjectReference<SaveDataEnvironmentArchive>(
            *reinterpret_cast<const YtlReferenceSlotRuntime*>(begin + 12u));
    return archive->bytesAt6D[0];
}

void ExecutePatternResourceLoad(const PatternResourceLoadRequest& request,
    const IPatternResourceFailureName& failureName, IPatternResourceLoadAction& action,
    const PatternResourceLoadServices& services)
{
    const bool cg = request.path == PatternResourceLoadPath::CgDirect ||
        request.path == PatternResourceLoadPath::CgExc || request.path == PatternResourceLoadPath::CgGroup;
    const bool direct = request.path == PatternResourceLoadPath::NormalDirect ||
        request.path == PatternResourceLoadPath::CgDirect;
    ArchiveReaderFactoryRuntime factory(services.source, services.transaction, services.files, services.memory,
        services.codecs);
    if (cg)
        static_cast<void>(factory.LoadBorrowedAt483D90(request.capturedInputCharacters, services.key));
    else
        static_cast<void>(factory.LoadBorrowedAt4D44C0(request.capturedInputCharacters, services.key));

    // 4CCA6A/4A8D35 等只检查 base，不消费 AL，也不把缺 control 擅自归入读取失败。
    if (!factory.Reader().Slot().baseAt00)
        ThrowFailure(failureName, ReadFailureSuffix, services.strings, request.access);

    if (request.path == PatternResourceLoadPath::CgDirect)
        action.CaptureCgDirectOwnerAfterRead();
    const auto initialization = services.initialization.ReadRegistrySlot1ByteAt6D();
    if (direct && !action.ResolveDirectTargetAfterInitialization())
        ThrowFailure(failureName, LoadFailureSuffix, services.strings, request.access);
    if (!action.LoadFromReader(factory.Reader(), initialization, request, services.codecs))
        ThrowFailure(failureName, LoadFailureSuffix, services.strings, request.access);

    // 480650/4B0840 的异常不翻译为 false；错误串构造和成功提交时 factory 均仍存活。
    // CG 的外层 catch 属于 4A84E0 caller，不在单次装载范围中提前吞掉。
    action.CommitWhileFactoryAlive();
    if (request.path == PatternResourceLoadPath::NormalDirect)
    {
        // 4CCCDD 在提交之后才缓存 control；特殊正常尾撤 member 析构资格。
        // 其余路径和到达这里之前的异常展开仍走 factory 的 current Ref 清理。
        factory.ReleaseReaderNormalAt4CCCDD();
    }
}
}
