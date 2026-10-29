#include <Runtime/FileManagerRootRuntime.h>
#include <Runtime/FileInfoProgressRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/ParamsKeyInstallationRuntime.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
thread_local FileManagerInitializationProgressBorrow* FileManagerInitializationProgressBorrow::current_ = nullptr;
FileManagerInitializationProgressBorrow::FileManagerInitializationProgressBorrow(
    const FileManagerRootRuntime& owner, IFileInfoProgress& progress) noexcept
    : owner_(owner), progress_(progress), previous_(current_) { current_ = this; }
FileManagerInitializationProgressBorrow::~FileManagerInitializationProgressBorrow() noexcept { current_ = previous_; }
IFileInfoProgress* FileManagerInitializationProgressBorrow::Find(const FileManagerRootRuntime& owner) noexcept
{
    for (auto* at = current_; at; at = at->previous_) if (&at->owner_ == &owner) return &at->progress_;
    return nullptr;
}
IFileInfoProgress* FileManagerRootRuntime::InitializationProgress() const noexcept
{
    if (auto* borrowed = FileManagerInitializationProgressBorrow::Find(*this)) return borrowed;
    return progress_.get();
}
namespace
{
class NativeStartupFactory final : public IFileManagerStartupFactory
{
public:
    NativeStartupFactory(HINSTANCE instance, RuntimeStringBorrow title)
        : instance_(instance), title_(title) {}

    std::unique_ptr<IFileInfoProgress> CreateProgress() override
    {
        return std::make_unique<FileInfoProgressRuntime>(instance_, title_);
    }
    FileManagerNative28h* CreateManagerFromReference(
        const YtlReferenceSlotRuntime& progress) override
    {
        return NewFileManagerAt489E56(progress);
    }
private:
    HINSTANCE instance_;
    RuntimeStringBorrow title_;
};
}

void FileManagerRootRuntime::CreateFileManager(YtlReferenceSlotRuntime& selectedRootSlot,
    HINSTANCE resourceInstance, RuntimeStringBorrow title)
{
    NativeStartupFactory factory(resourceInstance, title);
    CreateFileManager(selectedRootSlot, factory);
}

void FileManagerRootRuntime::CreateFileManager(YtlReferenceSlotRuntime& selectedRootSlot,
    IFileManagerStartupFactory& factory)
{
    // 原局部progress不跨489C90调用复用；重复Begin而未结束是装配错误，明确拒绝。
    // 上轮control分配失败遗留的无control正文不回收；新调用重新初始化局部Ref。
    if (progress_.reference.controlAt08)
        throw std::logic_error("FileManager initialization progress is still active");
    auto createdProgress = factory.CreateProgress();
    if (!createdProgress) throw std::logic_error("FileManager startup factory returned no progress object");
    // 489DF5已撤去正文清理态；必须先release，再分配control，不能让unique_ptr回滚正文。
    progress_.AdoptAt489DF5(createdProgress.release());
    auto* next = factory.CreateManagerFromReference(progress_.reference);
    // 489E8A仍写调用入口已选的EDI根；安装失败不能回滚新正文或改写别代根。
    InstallFileManagerAt489E8D(selectedRootSlot, next);
}

void FileManagerRootRuntime::VerifyStartupFileSet(IFileManagerStartupErrorReport& report)
{
    auto* manager = CaptureManagerAt413BE0(); // 489F1D再次Ensure，不复用安装时的EDI。
    if (manager->scanStatusAt0C != 0) return;
    // 48AE9E重取progress HWND，48AEB4报告；报告正常返回才在48AEC9抛int1。
    auto* progress = InitializationProgress();
    if (!progress) throw std::logic_error("FileManager startup objects are not published");
    report.ReportScanFailure(progress->WindowAt47ADF0());
    throw 1;
}

void FileManagerRootRuntime::InstallEnvironmentKeyTable(ParamsRuntimeRootOwner& params,
    ArchiveCodecServices codecs)
{
    class RootSource final : public IParamsKeyNativeInstallationSource
    {
    public:
        RootSource(FileManagerRootRuntime& root, ParamsRuntimeRootOwner& params) noexcept
            : root_(root), params_(params) {}
        ParamsByteArray& CurrentBlobAt418480() override
        {
            auto& params = params_.GetOrCreateAt418480();
            const auto& game = params.GameSystem();
            if (!game) throw std::logic_error("Key installation requires the current GameSystem");
            borrowedRoot_ = &params;
            borrowedEpoch_ = params.GameSystemPublicationEpoch();
            borrowedIdentity_ = game.Native();
            borrowedBlob_ = &game->rawBlob;
            return game->rawBlob;
        }
        FileManagerNative28h& CurrentManagerAt479FF0() override
        {
            auto* const manager = root_.CaptureManagerAt413BE0();
            if (!manager) throw std::logic_error("Key installation requires the current FileManager");
            return *manager;
        }
        void ValidateCapturedManager(const FileManagerNative28h* captured) const override
        {
            if (root_.ManagerAt00() != captured)
                throw std::runtime_error("Params key destination manager changed (safe boundary)");
        }
        void ValidateCapturedInput(const ParamsByteArray* captured,
            const YtlReferenceSlotRuntime& identity) const override
        {
            // 原Ref无weak计数：先核对外部发布代次和当前槽，才允许解引用借用blob。
            // 这是明确更强的安全边界：换根即拒绝，即使另有原生持有者让旧正文存活。
            // 代次阻止正常重载时地址复用误判；不增持GS，也不读取已释放的control。
            const auto* const currentRoot = params_.Published();
            if (currentRoot != borrowedRoot_ || !currentRoot ||
                currentRoot->GameSystemPublicationEpoch() != borrowedEpoch_ ||
                currentRoot->GameSystem().Native().baseAt00 != borrowedIdentity_.baseAt00 ||
                currentRoot->GameSystem().Native().biasAt04 != borrowedIdentity_.biasAt04 ||
                currentRoot->GameSystem().Native().controlAt08 != borrowedIdentity_.controlAt08 ||
                captured != borrowedBlob_)
                throw std::runtime_error("Params key input owner expired (safe boundary)");
            const auto& current = captured->storage.RawSlot();
            if (current.baseAt00 != identity.baseAt00 || current.biasAt04 != identity.biasAt04 ||
                current.controlAt08 != identity.controlAt08)
                throw std::runtime_error("Params key input reference changed (safe boundary)");
        }
    private:
        FileManagerRootRuntime& root_;
        ParamsRuntimeRootOwner& params_;
        const ParamsRuntimeRoot* borrowedRoot_ = nullptr;
        std::uint64_t borrowedEpoch_ = 0;
        YtlReferenceSlotRuntime borrowedIdentity_;
        const ParamsByteArray* borrowedBlob_ = nullptr;
    } source(*this, params);
    InstallParamsKeyAt489F7C(source, codecs);
}

void FileManagerRootRuntime::ReleaseInitializationProgress() noexcept
{
    progress_.reset();
}

ByteArrayReferenceRuntime FileManagerRootRuntime::AcquireRawBytes() const
{
    auto* manager = CaptureManagerAt413BE0();
    if (!manager) throw std::logic_error("Archive key access has no published FileManager");
    return manager->KeyReferenceAt1C();
}

std::uint8_t FileManagerRootRuntime::NormalizedByteAt(const std::uint32_t index) const
{
    auto* manager = CaptureManagerAt413BE0();
    if (!manager) throw std::logic_error("Persistence key access has no published FileManager");
    return ReadNormalizedKeyByteAt47CB00(manager->KeyReferenceAt1C(), index);
}

std::size_t FileManagerRootRuntime::RawByteCount() const
{
    auto* manager = CaptureManagerAt413BE0();
    if (!manager) throw std::logic_error("Key length access has no published FileManager");
    // 48A1C0/48A217：manager已存在但key.control为空时长度为零。
    return manager->KeyReferenceAt1C().Size();
}

FileManagerNative28h* FileManagerRootRuntime::ManagerAt00() const noexcept
{
    const auto* slot = source_.PublishedFileManagerSlot();
    return slot ? ResolveFileManagerAt479FF0(*slot) : nullptr;
}

FileManagerNative28h* FileManagerRootRuntime::CaptureManagerAt413BE0() const
{
    return ResolveFileManagerAt479FF0(source_.CaptureFileManagerSlotAt413BE0());
}

void FileManagerRootRuntime::ReleaseFileManager(YtlReferenceSlotRuntime& selectedRootSlot)
{
    ReleaseFileManagerAt4777C9(selectedRootSlot);
}

FileManagerRootRuntime::~FileManagerRootRuntime() noexcept
{
    ReleaseInitializationProgress();
}
}
