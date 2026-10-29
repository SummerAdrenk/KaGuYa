#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/ScriptDispatchNative.h>
#include <Runtime/ScrVmScriptCommandState.h>

#include <atlbase.h>
#include <mbstring.h>

#include <exception>
#include <limits>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char LoadScriptPrefix[] = "LoadScript : ";
constexpr char MissingResourceNameCp932[] =
    "\x83\x74\x83\x40\x83\x43\x83\x8B\x96\xBC\x82\xAA\x82\xA0"
    "\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49";
constexpr char LeftDoubleCornerBracketCp932[] = "\x81\x77 ";
constexpr char RightDoubleCornerBracketCp932[] = " \x81\x78";
constexpr char ResourceLoadFailedCp932[] =
    "\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\x82\xC9\x8E\xB8"
    "\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
// 原PE 5B5D30的格式串后缀；不是现代解析器的what()诊断。
constexpr char NotScriptFileCp932[] =
    "\x82\xCD\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67\x83\x74"
    "\x83\x40\x83\x43\x83\x8B\x82\xC5\x82\xCD\x82\xA0\x82\xE8"
    "\x82\xDC\x82\xB9\x82\xF1\x81\x49";

[[nodiscard]] std::string PrefixMessage(const std::string& detail)
{
    return std::string(LoadScriptPrefix) + detail;
}

[[nodiscard]] std::string ResourceFailureDetail(
    const std::string& resourceName, const char* suffix)
{
    std::string message(LeftDoubleCornerBracketCp932);
    message += resourceName;
    message += RightDoubleCornerBracketCp932;
    message += suffix;
    return message;
}
}

ScrScriptRuntimeState::ScrScriptRuntimeState()
    : ScrScriptRuntimeState(CreateScrVmParamsGenerationSlot())
{
}

ScrScriptRuntimeState::ScrScriptRuntimeState(
    ScrVmParamsGenerationBinding generationSlot)
    : generationSlot_(std::move(generationSlot))
{
    if (!generationSlot_)
        throw std::invalid_argument("SCR script params generation slot is null");
    fixtureBody_ = std::make_unique<ScriptNativeBody>(ScriptNativeBody::PrefixOnlyTag{});
    body_ = fixtureBody_.get();
}

ScrScriptRuntimeState::ScrScriptRuntimeState(ScriptNativeBody& body,
    ScriptNativePublication& publication, ScrVmParamsGenerationBinding generationSlot)
    : generationSlot_(std::move(generationSlot)), body_(&body), publication_(&publication)
{
    if (!generationSlot_)
        throw std::invalid_argument("SCR script params generation slot is null");
}

ScrScriptRuntimeState::ScrScriptRuntimeState(ScrScriptRuntimeState&& other) noexcept
    : generationSlot_(std::move(other.generationSlot_)),
      fixturePublication_(other.fixturePublication_), fixtureBody_(std::move(other.fixtureBody_)),
      body_(std::exchange(other.body_, nullptr)),
      publication_(other.publication_ == &other.fixturePublication_
          ? &fixturePublication_ : other.publication_)
{
    // 自有夹具的记录随对象迁移；借用正文仍使用调用方传入的同一记录。
    other.publication_ = &other.fixturePublication_;
}

ScrResourceNameTable::ScrResourceNameTable(std::vector<std::string> names)
    : names_(std::move(names))
{
}

const std::string& ScrResourceNameTable::NameAt(const std::int32_t scriptId) const noexcept
{
    static const std::string Empty;
    if (scriptId < 0 || static_cast<std::size_t>(scriptId) >= names_.size())
        return Empty;
    return names_[static_cast<std::size_t>(scriptId)];
}

std::int32_t ScrResourceNameTable::FindId(
    const std::string& resourceName) const noexcept
{
    // 显式工具入口保证非null；不能先构造std::string再模拟原nullable反查。
    return FindResourceIdAt47A020(resourceName.c_str());
}

std::int32_t ScrResourceNameTable::FindResourceIdAt47A020(const char* const nameCp932) const
{
    for (std::size_t index = 0; index < names_.size(); ++index)
    {
        if (!nameCp932) throw ATL::CAtlException(E_FAIL);
        if (_mbsicmp(
                reinterpret_cast<const unsigned char*>(names_[index].c_str()),
                reinterpret_cast<const unsigned char*>(nameCp932)) == 0)
        {
            if (index > static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
                return -1;
            return static_cast<std::int32_t>(index);
        }
    }
    return -1;
}

std::size_t ScrResourceNameTable::Size() const noexcept
{
    return names_.size();
}

ScrResourceNameReceiver ScrResourceNameTable::CapturePatternNames() const noexcept
{
    return {this, &FindName};
}

ScrResourceNameRootReceiver ScrResourceNameTable::CaptureParamsForNames() const noexcept
{
    // 显式固定表没有另一层Params对象；两阶段均借同一表，不复制条目。
    return {this, &CaptureRootPattern};
}

ScrResourceNameReceiver ScrResourceNameTable::CaptureRootPattern(const void* receiver) noexcept
{
    return static_cast<const ScrResourceNameTable*>(receiver)->CapturePatternNames();
}

const char* ScrResourceNameTable::FindName(const void* receiver, const std::int32_t id) noexcept
{
    if (id < 0) return nullptr;
    const auto& names = static_cast<const ScrResourceNameTable*>(receiver)->names_;
    const auto index = static_cast<std::size_t>(id);
    return index < names.size() ? names[index].c_str() : nullptr;
}

const ScrContainer* ScrScriptRuntimeState::LoadedScript() const noexcept
{
    const auto& parser = NativeParserAtA0();
    return parser.bytesAt04.RawSlot().baseAt00 ? &parser : nullptr;
}

std::int32_t ScrScriptRuntimeState::LoadedScriptId() const noexcept
{
    return NativeParserAtA0().ScriptId();
}

std::int32_t ScrScriptRuntimeState::EnvironmentScriptId() const
{
    // 不把原未初始化字段伪造成 -1，更不能借用 CParams+60 的 format ID。
    return generationSlot_->Current().SaveParams2At30().scriptFileIdAtSaveParams04;
}

std::uint32_t ScrScriptRuntimeState::ProgramCounter() const
{
    return generationSlot_->Current().programCounter;
}

std::uint32_t ScrScriptRuntimeState::StagedProgramCounter() const noexcept
{
    return NativeParserAtA0().stagedPcAt10;
}

std::string_view ScrScriptRuntimeState::CurrentTitle() const
{
    return RuntimeStringView(generationSlot_->Current().SaveParamsAt08().saveTitleAt08);
}

RuntimeCString& ScrScriptRuntimeState::NativeTitleAt48CF90()
{
    return generationSlot_->Current().SaveParamsAt08().saveTitleAt08;
}

bool ScrScriptRuntimeState::SaveCommandGate() const
{
    // 4C8CF0未初始化+25；4D0B38实际发布之后才能读取，CParams换代不重置它。
    if (!IsSaveCommandGatePublished())
        throw std::logic_error("CScript+25 read before publication");
    return body_->commandAt24.bytes[1] != 0;
}

bool ScrScriptRuntimeState::IsSaveCommandGatePublished() const noexcept
{
    return (publication_->commandMask & ScrVmScriptCommandState::SaveGateAt25Mask) != 0;
}

std::optional<bool> ScrScriptRuntimeState::SaveCommandGateState() const noexcept
{
    if (!IsSaveCommandGatePublished()) return std::nullopt;
    return body_->commandAt24.bytes[1] != 0;
}

std::int32_t ScrScriptRuntimeState::CurrentSaveSlot() const
{
    return generationSlot_->Current().SaveParams2At30().instructionIndexAtSaveParams08;
}

bool ScrScriptRuntimeState::UsesParamsGenerationSlot(
    const ScrVmParamsGenerationBinding& generationSlot) const noexcept
{
    return generationSlot_ == generationSlot;
}

void ScrScriptRuntimeState::SetProgramCounter(const std::uint32_t programCounter)
{
    generationSlot_->Current().programCounter = programCounter;
}

void ScrScriptRuntimeState::SetStagedProgramCounter(
    const std::uint32_t programCounter) noexcept
{
    NativeParserAtA0().stagedPcAt10 = programCounter;
}

void ScrScriptRuntimeState::CommitStagedProgramCounter()
{
    generationSlot_->Current().programCounter = NativeParserAtA0().stagedPcAt10;
}

void ScrScriptRuntimeState::ApplyProgramCounterTargetAt4D6E90(std::int32_t target)
{
    ApplyScriptProgramCounterTargetAt4D6E90(NativeParserAtA0(), target);
}

void ScrScriptRuntimeState::SetCurrentTitle(std::string_view title)
{
    // 48CF90 接收 C 字符串：首 NUL 截断。此有界 view 入口不读取调用方边界以外。
    const auto nul = title.find('\0');
    if (nul != std::string_view::npos) title = title.substr(0, nul);
    if (title.size() > static_cast<std::size_t>(INT_MAX))
        throw std::length_error("CSaveParams title exceeds signed CString length");
    SetRuntimeCStringAt437680(generationSlot_->Current().SaveParamsAt08().saveTitleAt08,
        title.data(), static_cast<int>(title.size()));
}

void ScrScriptRuntimeState::SetSaveCommandGate(const bool enabled) noexcept
{
    body_->commandAt24.bytes[1] = static_cast<std::uint8_t>(enabled);
    publication_->commandMask |= ScrVmScriptCommandState::SaveGateAt25Mask;
}

void ScrScriptRuntimeState::SetCurrentSaveSlot(const std::int32_t slot)
{
    generationSlot_->Current().SaveParams2At30().instructionIndexAtSaveParams08 = slot;
}

void ScrScriptRuntimeState::ReplaceParamsGeneration()
{
    generationSlot_->Replace();
}

ScrScriptSwitchResult ScrScriptRuntimeState::LoadScript(
    const std::int32_t scriptId,
    const IScrResourceNameSource& resourceNames,
    const ResourceCatalog& catalog,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api,
    const IEnvironmentKeyTableSource& environmentKeyBytes,
    ArchiveCodecServices codecs)
{
    ScrScriptSwitchResult failure;
    std::string failureDetail = "syntax error";
    std::string resourceName;
    try
    {
        // 4D3D0F..42在同ID比较之前取得Pattern并复制首NUL正文；副本跨资源I/O存活。
        const auto names = resourceNames.CapturePatternNames();
        const char* const rawName = names.FindNameAt4AD1B0(scriptId);
        resourceName.assign(rawName ? rawName : "");
        if (resourceName.empty())
        {
            failure.error = ScrScriptSwitchError::MissingResourceName;
            failureDetail = MissingResourceNameCp932;
            throw 1;
        }

        if (LoadedScriptId() != scriptId)
        {
            // 4D3D64..4D3E31局部工厂跨parser和环境ID发布存活；原Load内部catch清reader。
            // 不能用工具LoadOrThrow把工厂吞掉的bad_alloc等改成外层syntax分支。
            ArchiveReaderMemoryApi memory;
            ArchiveReaderFactoryRuntime factory(catalog, transactionContext, api, memory, codecs);
            (void)factory.Load(resourceName, environmentKeyBytes);
            auto* const reader = factory.Reader().Get();
            // 原4D3D94检查reader正文，不依据现代bool或control是否非空。
            if (!reader)
            {
                failure.error = ScrScriptSwitchError::ResourceLoadFailed;
                failureDetail = ResourceFailureDetail(resourceName, ResourceLoadFailedCp932);
                throw 1;
            }
            ScrParseDiagnostic diagnostic;
            try
            {
                // 4D3DB9调原reader+0C；callee按值增持同一Ref，不复制文件正文。
                auto* const input = reader->BytesAt4172C0();
                if (!input) throw std::logic_error("SCR reader returned null byte reference");
                NativeParserAtA0().LoadAt4D5BA0(scriptId, input->RawSlot(), ProcessScriptHeaderSource(), &diagnostic);
            }
            catch (char*)
            {
                failure.error = ScrScriptSwitchError::ContainerParseFailed;
                failure.containerError = diagnostic.error;
                failure.containerErrorOffset = diagnostic.offset;
                throw;
            }
            catch (const ScrContainerException& error)
            {
                // 现代范围拒绝不冒充原marker-reset；保留已执行的发布前缀。
                failure.error = ScrScriptSwitchError::ContainerParseFailed;
                failure.containerError = error.Error();
                failure.containerErrorOffset = error.Offset();
                throw;
            }
            if (!NativeParserAtA0().bytesAt04.RawSlot().baseAt00)
            {
                // 4D3F67只检查原base，不要求control存在。
                failure.error = ScrScriptSwitchError::ContainerParseFailed;
                failureDetail = ResourceFailureDetail(resourceName, NotScriptFileCp932);
                throw 1;
            }
            // 4D3E2A：解析成功且正文有效后才写当前脚本ID；同ID调用不写。
            generationSlot_->Current().SaveParams2At30().scriptFileIdAtSaveParams04 = scriptId;
        }
        // 4D3E36..5E：同ID也重新定位版本头后的首指令；写入仍在原try范围内。
        generationSlot_->Current().programCounter = NativeParserAtA0().CodeOffset();
        return ScrScriptSwitchResult{true, ScrScriptSwitchError::None,
            std::nullopt, std::nullopt, 0, {}};
    }
    catch (...) {}
    // PE5F186C覆盖state2..5，5F185C只有catch-all；不以what()或const char*覆盖syntax默认值。
    // Prefix连接位于4D3EB6的原try之外，连接失败不能再次被同一catch吞掉。
    failure.message = PrefixMessage(failureDetail);
    return failure;
}
}
