#include <Runtime/EnvironmentRegistryRuntime.h>
#include <Runtime/EnvironmentErrorRuntime.h>
#include <Runtime/SaveDataConstructionSource.h>
#include <Runtime/RuntimeCStringVector.h>
#include <Runtime/CategorizedSoundEnvironmentAdapter.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/GameBuildProfile.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentRegistryRuntime::EnvironmentRegistryRuntime(EnvironmentRootOwner& owner,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory) noexcept
    : owner_(owner), strings_(strings), memory_(memory) {}

void EnvironmentRegistryRuntime::RegisterAt47E2B0(ParamsRuntimeRootOwner& params,
    ISoundParamsSettingsSource& sounds, IEnvironmentPathApi& paths, ISettingTagErrorSource& errors)
{
    SaveDataRootConstructionSource saved(owner_, params);
    owner_.ResetCategorizedSoundEnvironment();
    RegisterSaveDataArchiveAt47E2D8(owner_, saved, sounds, errors, strings_, memory_);
    if(CompiledGameProfile().soundSettings==GameBuildProfile::SoundSettings::Categorized)
    {
        auto& save=SaveData();
        auto adapter=std::make_unique<CategorizedSoundEnvironmentAdapter>(
            sounds.ReadGameSystem(SoundParamsSettingsReadPoint::MasterVolume),save.BorrowSoundParamsAt1C(),errors);
        owner_.BindCategorizedSoundEnvironment(save,std::move(adapter));
    }
    UnSaveDataParamsSource unsaved(params);
    RegisterUnSaveDataArchiveAt47E312(owner_, unsaved, strings_, memory_);
    SecretCgParamsRootSource cg(params);
    RegisterSecretCgArchiveAt47E349(owner_, cg, strings_, memory_);
    SecretScParamsSource scene(params);
    RegisterSecretScArchiveAt47E380(owner_, scene, memory_);
    RegisterClickInfoArchiveAt47E3B7(owner_, memory_);
    RegisterSaveFileStateArchiveAt47E441(owner_, memory_);
    RegisterGlobalParamsArchiveAt47E4A8(owner_, memory_);
    ResolveSystemDataInstallFilesAt47E520(*this, paths, *this);
}

EnvironmentRootStorage& EnvironmentRegistryRuntime::CaptureRoot() const
{
    auto* root = owner_.CaptureAt4180C0();
    if (!root) throw std::logic_error("4180C0: missing Environment root (safe boundary)");
    return *root;
}

IEnvironmentArchiveRuntime* EnvironmentRegistryRuntime::CaptureArchiveAt4180C0(std::uint32_t index) const
{ return ResolveEnvironmentArchiveAt417990(CaptureRoot(), index); }
std::size_t EnvironmentRegistryRuntime::Size() const
{
    const auto& slots = CaptureRoot().archivesAt30;
    return slots.begin ? static_cast<std::size_t>(slots.end - slots.begin) : 0;
}

EnvironmentPersistenceCoordinator EnvironmentRegistryRuntime::Persistence(
    const EnvironmentPathRuntime& paths, const IEnvironmentKeyByteSource& keys,
    const FileTransactionContext* transaction, IStartFileApi& files) const
{
    const auto& sys = SystemData();
    auto& save=SaveData();
    auto* categorized=CompiledGameProfile().soundSettings==GameBuildProfile::SoundSettings::Categorized
        ? &owner_.RequireCategorizedSoundEnvironment(save):nullptr;
    return {save, SecretCg(), SecretSc(), GlobalParams(), keys,
        {std::string(RuntimeStringView(paths.saveAt10)) + sys.stringAt28,
         std::string(RuntimeStringView(paths.saveAt10)) + sys.stringAt2C,
         std::string(RuntimeStringView(paths.saveAt10)) + sys.stringAt30}, transaction, files,categorized};
}
SystemDataEnvironmentArchive& EnvironmentRegistryRuntime::SystemData() const
{ return Require<SystemDataEnvironmentArchive>(0); }
SaveDataEnvironmentArchive& EnvironmentRegistryRuntime::SaveData() const
{ return Require<SaveDataEnvironmentArchive>(1); }
UnSaveDataRuntime& EnvironmentRegistryRuntime::UnSaveData() const { return Require<UnSaveDataRuntime>(2); }
SecretCgArchive& EnvironmentRegistryRuntime::SecretCg() const { return Require<SecretCgArchive>(3); }
SecretScArchive& EnvironmentRegistryRuntime::SecretSc() const { return Require<SecretScArchive>(4); }
ClickInfoArchive& EnvironmentRegistryRuntime::ClickInfo() const { return Require<ClickInfoArchive>(5); }
SaveFileStateNativeArchive& EnvironmentRegistryRuntime::SaveFileState() const
{ return Require<SaveFileStateNativeArchive>(6); }
RuntimeStringBorrow EnvironmentRegistryRuntime::CaptureSaveDirectoryAt4180C0()
{ return CaptureInstallPathsAt4180C0().saveAt10; }
GlobalParamsArchive& EnvironmentRegistryRuntime::GlobalParams() const { return Require<GlobalParamsArchive>(7); }
std::uint8_t EnvironmentRegistryRuntime::ReadRegistrySlot1ByteAt6D() const { return SaveData().bytesAt6D[0]; }
const SystemDataInstalledFiles& EnvironmentRegistryRuntime::CaptureSystemDataInstallFilesAt4180C0() const
{ return SystemData().installFilesAt08; }
EnvironmentPathRuntime& EnvironmentRegistryRuntime::CaptureInstallPathsAt4180C0() const
{
    auto* paths = ResolveEnvironmentObjectReference<EnvironmentPathRuntime>(CaptureRoot().pathsAt48);
    if (!paths) throw std::logic_error("47E8F1: Environment path has no payload (safe boundary)");
    return *paths;
}

void EnvironmentRegistryRuntime::AppendFormat(const char* format, ...)
{
    // 46A0FD/46A129先Capture，4769B0内部才GetNil/FormatV；参数已由调用者固定。
    auto& root = CaptureRoot();
    va_list args;
    va_start(args, format);
    try { AppendEnvironmentErrorVAt4769B0(root, format, args, strings_, memory_); }
    catch (...) { va_end(args); throw; }
    va_end(args);
}
void EnvironmentRegistryRuntime::AppendMissingInstallFile(const char* nameCp932)
{
    AppendFormat("\x81\x77 %s \x81\x78\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xAA\x8C\xA9\x82\xC2\x82\xA9\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49", nameCp932);
}
void EnvironmentRegistryRuntime::AppendInsertInstallDisc(int driveByte)
{
    AppendFormat("\x83\x68\x83\x89\x83\x43\x83\x75 %c \x82\xC9\x83\x66\x83\x42\x83\x58\x83\x4E\x82\xF0\x93\xFC\x82\xEA\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81\x42", driveByte);
}
void EnvironmentRegistryRuntime::AppendPathError(std::string textCp932)
{
    auto& root = CaptureRoot();
    const RuntimeCStringResult text([&](RuntimeCString* out) {
        InitializeEmptyRuntimeCString(*out, strings_);
        try { SetRuntimeCStringAt437680(*out, textCp932.data(), static_cast<int>(textCp932.size())); }
        catch (...) { DestroyRuntimeCStringAt40CD40(out); throw; }
    });
    AppendRuntimeCStringAt41D5E9(root.errorsAt3C, text.Value(), memory_);
}
RuntimeCStringResult EnvironmentRegistryRuntime::ErrorTextAt479EA0() const
{
    auto& root = CaptureRoot();
    return RuntimeCStringResult([&](RuntimeCString* out) { BuildEnvironmentErrorTextAt479EA0(root, *out, strings_); });
}
}
