#include <Runtime/FileInfoProgressRuntime.h>
#include <Logging/Logger.h>

#include <CommCtrl.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
class Win32Confirmation final : public IFileInfoProgressConfirmation
{
public:
    int Confirm(HWND owner, const char* text, const char* title, UINT flags) override
    {
        return ::MessageBoxA(owner, text, title, flags);
    }
};

IFileInfoProgressConfirmation& DefaultConfirmation()
{
    static Win32Confirmation confirmation;
    return confirmation;
}

// 原PE 5B2A3C的36个CP932字节：游戏退出确认文本，不受编译机代码页影响。
constexpr char CancelText[] =
    "\x83\x51\x81\x5B\x83\x80\x82\xF0\x8F\x49\x97\xB9"
    "\x82\xB5\x82\xDC\x82\xB7\x81\x42\x82\xE6\x82\xEB"
    "\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48";
static_assert(sizeof(CancelText) == 37);
}

FileInfoProgressRuntime::FileInfoProgressRuntime(HINSTANCE instance, RuntimeStringBorrow title)
    : FileInfoProgressRuntime(instance, title, DefaultConfirmation()) {}

FileInfoProgressRuntime::FileInfoProgressRuntime(HINSTANCE instance, RuntimeStringBorrow title,
    IFileInfoProgressConfirmation& confirmation)
    : dialog_(instance), title_(title), confirmation_(confirmation)
{
    // 458AE3..458B2F：创建后两次重取控件；不按返回值新增失败分支。
    ::SendMessageA(::GetDlgItem(dialog_.Window(), 1000), PBM_SETRANGE, 0, 0x10000);
    ::SendMessageA(::GetDlgItem(dialog_.Window(), 1000), PBM_SETSTEP, 1, 0);
    // 日志不改变原窗口API留下的LastError，也不在逐文件步进中刷日志。
    const DWORD error = GetLastError();
    KaGuYa::Logging::WriteFormat(dialog_.Window() ? KaGuYa::Logging::Level::Info :
        KaGuYa::Logging::Level::Warning, "FileInfoProgress 4588F0: hwnd=%p lastError=%lu",
        static_cast<void*>(dialog_.Window()), static_cast<unsigned long>(error));
    SetLastError(error);
}

bool FileInfoProgressRuntime::PollCancelAt47F0B0()
{
    dialog_.PumpAt485C70();
    // 47F102..47F10B在确认/字符串准备前清除+21；拒绝退出不保留旧请求。
    if (!dialog_.ConsumeCancelRequest()) return false;
    return confirmation_.Confirm(dialog_.Window(), CancelText, title_.View().data(), 0x21u) == IDOK;
}

LRESULT FileInfoProgressRuntime::StepAt46E820()
{
    dialog_.PumpAt485C70();
    return ::SendMessageA(::GetDlgItem(dialog_.Window(), 1000), PBM_STEPIT, 0, 0);
}

LRESULT FileInfoProgressRuntime::AddRangeAt46A5A0(std::uint32_t delta)
{
    // 原46A5D8读取PBRANGE后将low/新high各截成WORD，不能换成PBM_SETRANGE32。
    // 控件未写入的异常输入不读取未初始化栈字节，使用确定性零初值。
    PBRANGE range{};
    ::SendMessageA(::GetDlgItem(dialog_.Window(), 1000), PBM_GETRANGE, 1,
        reinterpret_cast<LPARAM>(&range));
    const auto high = static_cast<std::uint16_t>(static_cast<std::uint32_t>(range.iHigh) + delta);
    const auto packed = static_cast<std::uint32_t>(static_cast<std::uint16_t>(range.iLow)) |
        (static_cast<std::uint32_t>(high) << 16u);
    return ::SendMessageA(::GetDlgItem(dialog_.Window(), 1000), PBM_SETRANGE, 0,
        std::bit_cast<std::int32_t>(packed));
}

HWND FileInfoProgressRuntime::WindowAt47ADF0() const noexcept
{
    return dialog_.Window();
}
}
