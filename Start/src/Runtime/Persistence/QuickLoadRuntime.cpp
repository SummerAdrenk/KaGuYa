#include <Runtime/QuickLoadRuntime.h>

#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/SaveLoadFileFactoryRuntime.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
std::string_view QuickLoadFailureTextCp932(QuickLoadFailure failure)
{
    switch (failure)
    {
    case QuickLoadFailure::NoSavedData:
        // 原PE 5B36EC，无格式参数：快速存档的数据不存在。
        return "\x83\x4e\x83\x43\x83\x62\x83\x4e\x83\x5a\x81\x5b\x83\x75"
            "\x82\xb3\x82\xea\x82\xbd\x83\x66\x81\x5b\x83\x5e\x82\xaa"
            "\x91\xb6\x8d\xdd\x82\xb5\x82\xdc\x82\xb9\x82\xf1\x81\x49";
    case QuickLoadFailure::LoadFailed:
        // 原PE 5B3718，无格式参数：快速存档数据读取失败。
        return "\x83\x4e\x83\x43\x83\x62\x83\x4e\x83\x5a\x81\x5b\x83\x75"
            "\x82\xb3\x82\xea\x82\xbd\x83\x66\x81\x5b\x83\x5e\x82\xcc"
            "\x83\x8d\x81\x5b\x83\x68\x82\xc9\x8e\xb8\x94\x73\x82\xb5"
            "\x82\xdc\x82\xb5\x82\xbd\x81\x49";
    }
    throw std::invalid_argument("unknown quick load failure");
}

Win32QuickLoadFailureDisplay::Win32QuickLoadFailureDisplay(
    const HWND& window, const std::string& captionCp932) noexcept
    : window_(window), caption_(captionCp932) {}

void Win32QuickLoadFailureDisplay::ShowFailureAt485F39(QuickLoadFailure failure)
{
    const auto message = QuickLoadFailureTextCp932(failure);
    // 485F1B/485F2C仅push10h；不追加确认按钮标志，不消费MessageBox返回值。
    static_cast<void>(::MessageBoxA(window_, message.data(), caption_.c_str(), MB_ICONERROR));
}

QuickLoadRuntime::QuickLoadRuntime(SaveFileCatalog& catalog,
    const SaveDataEnvironmentArchive& save, SaveFileContainerRuntime& container,
    ISaveLoadPreviewServices& preview, ISaveLoadDeleteFileApi& deletion,
    const std::string& directoryCp932, IQuickLoadConfirmationServices& confirmation,
    IQuickLoadFailureDisplay& failures) noexcept
    : catalog_(catalog), save_(save), container_(container), preview_(preview),
      deletion_(deletion), directory_(directoryCp932), confirmation_(confirmation), failures_(failures) {}

bool QuickLoadRuntime::QuickLoadAt485CE0()
{
    if (save_.bytesAt77[3] != 0)
    {
        bool accepted;
        {
            auto dialog = confirmation_.CreateQuickLoadConfirmationAt4E3330();
            if (!dialog) throw std::logic_error("quick load confirmation returned no object");
            dialog->CaptureDisplayAt48CA30();
            accepted = dialog->RunModalAt5071D0() == 6;
        }
        // 485E20..485E4C先释放局部Ref，再判断是否继续；确认异常不改为false。
        if (!accepted) return false;
    }

    SaveLoadFileFactoryRuntime factory(container_, preview_, deletion_, directory_,
        "qsave", 8, catalog_.quickLatestItemAt04);
    if (catalog_.quickLatestItemAt04 < 0)
    {
        failures_.ShowFailureAt485F39(QuickLoadFailure::NoSavedData);
        return false;
    }
    // 485EFE原DWORD加一，不绕回8，也不按目录项available决定是否读取。
    const auto slot = std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(catalog_.quickLatestItemAt04) + 1u);
    if (factory.LoadSlotAt480830(slot) != 0)
    {
        failures_.ShowFailureAt485F39(QuickLoadFailure::LoadFailed);
        return false;
    }
    return true;
}
}
