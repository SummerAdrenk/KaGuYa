#include <Runtime/RootLegacyScriptHistorySource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentPathRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/MessageOwnerNative.h>
#include <Runtime/FileManagerNative.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <Runtime/RuntimeCStringResult.h>
#include <cstring>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
void CloneReturn(RuntimeCString* output, const RuntimeCString& source)
{
    auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(source.GetString())) - 1;
    const char* chars = reinterpret_cast<const char*>(CloneRuntimeCStringDataAt414540(header) + 1);
    // 414540已取得引用；向未构造返回槽写入原生指针表示，不经CString赋值增减引用。
    std::memcpy(static_cast<void*>(output), &chars, sizeof(chars));
}
bool EqualName(const RuntimeCString& saved, const RuntimeCString& original)
{
    // 41EDA0 -> 4216F0：区分大小写的进程MB比较；不是_mbsicmp或Unicode折叠。
    const auto* right = reinterpret_cast<const unsigned char*>(original.GetString());
    if (!right) throw ATL::CAtlException(E_FAIL);
    return _mbscmp(reinterpret_cast<const unsigned char*>(saved.GetString()), right) == 0;
}
}

void RootLegacyScriptHistorySource::ConstructDirectoryAt4C7340(RuntimeCString* output)
{
    auto* env = dependencies_.environment.CaptureAt4180C0();
    auto* paths = ResolveEnvironmentObjectReference<EnvironmentPathRuntime>(env->pathsAt48);
    // 原41F090先拼接本地串并Clone到返回槽；4C7340再Clone一次。正常释放次序不合并。
    RuntimeCStringResult local([&](RuntimeCString* result) {
        RuntimeCStringConcatenation concatenation(paths->temporaryAt14, nullptr, dependencies_.strings);
        CloneReturn(result, concatenation.Value());
    });
    CloneReturn(output, local.Value());
}
std::uint8_t RootLegacyScriptHistorySource::NamesMatchDefaultsAt41B450()
{
    // 4CD44E固定一次Save；41B450分别重取两个默认名字，第一项不等便短路。
    auto* env = dependencies_.environment.CaptureAt4180C0();
    auto* saved = static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*env, 1));
    const auto& given = dependencies_.params.CaptureAt418480()->GameSystem()->defaultGivenName.Native();
    if (!EqualName(saved->stringAt58, given)) return 0;
    const auto& family = dependencies_.params.CaptureAt418480()->GameSystem()->defaultFamilyName.Native();
    return EqualName(saved->stringAt5C, family);
}
std::int32_t RootLegacyScriptHistorySource::ReadFormatAt41B4C0(std::int32_t index,
    MessageFormatSelectionRawTarget output, std::uint32_t variant, bool markRead)
{
    // 4CD48E在姓名查询后另取当前FileManager/Message，不增持另一代根。
    dependencies_.root.EnsureCreatedAt413BE0();
    auto* files = ResolveFileManagerAt479FF0(dependencies_.root.Published()->FileManagerSlotAt00());
    auto* message = ResolveMessageReference(files->messageAt10);
    auto reader = message->SourceWithContext({dependencies_.messageFiles});
    auto flags = message->ReadFlagsWithSources(dependencies_.messageFiles, dependencies_.messagePaths);
    MessageFormatSelectionRuntime selection(reader, flags);
    return selection.ReadFormatRawAt417C40(index, output, variant, markRead);
}
void RootLegacyScriptHistorySource::WarnAt416B00(const char* text, UINT type)
{
    auto* env = dependencies_.environment.CaptureAt4180C0();
    const auto* caption = env->titleAt10.GetString();
    const auto window = reinterpret_cast<HWND>(env->windowAt2C);
    static_cast<void>(dependencies_.messageBox(window, text, caption, type));
}
std::uint8_t RootLegacyScriptHistorySource::RebuildSelected(ScriptParamsNativeBody& params)
{
    InstallBakuLegacyScriptHistory(params, dependencies_.historyFiles, dependencies_.temporary, dependencies_.strings);
    return RebuildBakuLegacyScriptHistoryAt4CD230(params, *this);
}
void RootLegacyScriptHistorySource::ResetSelected(ScriptParamsNativeBody& params)
{
    InstallBakuLegacyScriptHistory(params, dependencies_.historyFiles, dependencies_.temporary, dependencies_.strings);
    ResetBakuLegacyScriptHistoryAt4CD090(params, *this);
}
}
