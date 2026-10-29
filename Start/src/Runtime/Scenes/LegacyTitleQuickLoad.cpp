#include <Runtime/LegacyTitleQuickLoad.h>
#include <Runtime/RuntimeCStringResult.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SaveFileStateNativeArchive.h>
#include <Logging/Logger.h>

#include <bit>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char confirmation[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68"
    "\x82\xB5\x82\xDC\x82\xB7\x82\xA9\x81\x48";

template<class T> T& Require(T* selected, const char* diagnostic)
{
    if (!selected)
    {
        // 原空解引用危险域明确拒绝；不把缺绑定解释为关闭确认或没有存档。
        Logging::Write(Logging::Level::Error, diagnostic);
        throw std::logic_error(diagnostic);
    }
    return *selected;
}

bool BeginConfirmation(LegacyTitleQuickLoadModalState& state, ILegacyTitleQuickLoadSource& source)
{
    try
    {
        // 4DC1E9..4DC1FF：先提交三个字段，result 保持旧值；随后才查真实消息框。
        state.busy = 1;
        state.kind = 0;
        state.mode = 4;
        auto* message = source.FindMessage(state);
        if (!message) throw 1;
        message->Begin();
        return true;
    }
    catch (...)
    {
        // 55AFF4 catch-all -> 4DC25C：原有失败协议，不是新增兜底。
        state.Reset();
        return false;
    }
}

struct ReaderLifetime final
{
    SaveFileDeleteFactoryNativeBody0Ch& body;
    bool armed = true;
    ~ReaderLifetime() noexcept { if (armed) DestroySaveFileDeleteFactoryNativeAt463A90(body); }
    void Finish()
    {
        // 44FB76/44FBBA：正常析构先撤资格，首次 Free 异常外传，不重复减持。
        armed = false;
        DestroySaveFileDeleteFactoryNativeAt463A90(body);
    }
};

void ConstructReader(SaveFileDeleteFactoryNativeRollingBody10h& reader,
    IRuntimeStringManagerSource& strings)
{
    // 4609E0/4608F0/460850：共享已核 CString/工厂析构原语；旧槽数在第二 Set 前写 6。
    reader.base.vptrAt00 = &BaseSaveFileDeleteFactoryNativeVtable();
    InitializeEmptyRuntimeCString(reader.base.prefixAt04, strings);
    ReaderLifetime unwind{reader.base};
    reader.base.vptrAt00 = &RegularSaveFileDeleteFactoryNativeVtable();
    reader.base.slotCountAt08 = 0;
    SetRuntimeCStringAt437680(reader.base.prefixAt04, "save", 4);
    reader.base.vptrAt00 = &QuickSaveFileDeleteFactoryNativeVtable();
    reader.previewAt0C = 1;
    reader.base.slotCountAt08 = 6;
    SetRuntimeCStringAt437680(reader.base.prefixAt04, "qsave", 5);
    unwind.armed = false;
}

void ConstructDataName(RuntimeCString& output, const SaveFileDeleteFactoryNativeBody0Ch& reader,
    std::int32_t slot, IRuntimeStringManagerSource& strings)
{
    // 460E40：先构造/Format 本地串，再 Clone 到隐藏返回槽，最后析构本地串。
    // 现代 47AF10 直接格式化 output，不能拿来替代这里的完整返回槽/EH时序。
    RuntimeCStringResult local([&](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, strings); });
    FormatRuntimeCStringAt417130(local.Value(), "%s%02d.dat", reader.prefixAt04.GetString(), slot);
    auto* header = reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(local.Value().GetString()) - sizeof(ATL::CStringData));
    auto* cloned = CloneRuntimeCStringDataAt414540(header); // Baku 41EC60 -> 428140
    const auto chars = reinterpret_cast<std::uintptr_t>(cloned) + sizeof(ATL::CStringData);
    static_assert(sizeof(output) == sizeof(chars));
    std::memcpy(static_cast<void*>(&output), &chars, sizeof(chars));
    // 返回槽发布后本地 Free 抛出时，调用者尚未取得该返回槽清理资格。
}
}

void LegacyTitleQuickLoadModalState::Reset() noexcept
{
    // 4199C0 按顺序写四个 DWORD，原偏移只作证据，不映射原内存。
    busy = 0;
    kind = -1;
    mode = 0;
    result = -1;
}

void TickLegacyTitleQuickLoadModal(LegacyTitleQuickLoadModalState& state,
    ILegacyTitleQuickLoadSource& source)
{
    // 4DC2A0：固定本次消息框身份，Poll -> Result -> busy=0 -> End -> 可选 Reset。
    auto* message = source.FindMessage(state);
    if (!message || message->Poll()) return;
    state.result = message->Result();
    state.busy = 0;
    if (message->End()) state.Reset();
}

bool LegacyTitleQuickLoad::ConfirmationEnabled()
{
    auto* env = shared_.CaptureEnvironmentAt4180C0();
    auto* save = static_cast<SaveDataEnvironmentArchive*>(shared_.RegistryAt417990(env, 1));
    // 44FA25/44FAA5 原 SaveData+86 是 quick-load 确认；统一模型为 bytesAt77[3]。
    // 407950/40FAC0 和 4E9B60 的确认设置交叉引用，与现代 485CE0+7A 对照。
    return Require(save, "LegacyTitleQuickLoad: missing SaveData (Baku 44FA25)").bytesAt77[3] != 0;
}

std::int32_t LegacyTitleQuickLoad::LatestQuickItem()
{
    auto* env = shared_.CaptureEnvironmentAt4180C0();
    auto* files = static_cast<SaveFileStateNativeArchive*>(shared_.RegistryAt417990(env, 6));
    return Require(files, "LegacyTitleQuickLoad: missing catalog (Baku 4610F5)").quickLatestItemAt04;
}

void* LegacyTitleQuickLoad::CaptureParams()
{
    shared_.EnsureRootAt413BE0();
    return shared_.ParamsAt47BDB0(shared_.RootAt6198C4());
}

int LegacyTitleQuickLoad::MessageBox(const char* text, UINT type)
{
    // 416B00 的 caption=null 分支：每次重取 Environment 的标题和 HWND，不捕获入口快照。
    auto& env = Require(shared_.CaptureEnvironmentAt4180C0(),
        "LegacyTitleQuickLoad: missing environment (Baku 416B00)");
    const char* caption = env.titleAt10.GetString();
    HWND window = reinterpret_cast<HWND>(env.windowAt2C);
    return shared_.MessageBoxA(window, text, caption, type);
}

std::int32_t LegacyTitleQuickLoad::Prepare()
{
    if (!ConfirmationEnabled()) return 1;
    if (!legacy_.CaptureModal())
        return MessageBox(confirmation, 0x44) == 6 ? 1 : 0;
    // 44FA47..44FA5E：再次取 root/GUI；忽略 Begin 的普通失败值，仍返回 2。
    auto& modal = Require(legacy_.CaptureModal(),
        "LegacyTitleQuickLoad: GUI disappeared before Begin (Baku 44FA59)");
    static_cast<void>(BeginConfirmation(modal, legacy_));
    return 2;
}

std::int32_t LegacyTitleQuickLoad::Poll()
{
    if (!ConfirmationEnabled()) return 1;
    if (!legacy_.CaptureModal()) return 1; // 44FB08：不再弹 Win32 确认。
    auto& match = Require(legacy_.CaptureModal(),
        "LegacyTitleQuickLoad: GUI disappeared before match (Baku 44FAD9)");
    // 419A40 仅比较 kind/mode；busy 门控属于 root 调度，不能在此追加等待条件。
    if (match.kind != 0 || match.mode != 4) return 0;
    auto& result = Require(legacy_.CaptureModal(),
        "LegacyTitleQuickLoad: GUI disappeared before consume (Baku 44FAF4)");
    const auto selected = result.result;
    result.Reset(); // 419990：先取结果再清状态，不再调用消息框 Result。
    return selected == 6 ? 1 : 2;
}

YtlReferenceSlotRuntime* LegacyTitleQuickLoad::MessageBackgroundSlot()
{
    auto* modal = legacy_.CaptureModal();
    if (!modal) return nullptr;
    auto* message = legacy_.FindMessage(*modal);
    return message ? &message->BackgroundSlot() : nullptr;
}

bool LegacyTitleQuickLoad::Load()
{
    SaveFileDeleteFactoryNativeRollingBody10h reader;
    ConstructReader(reader, shared_.Strings()); // 44FB4D 成功后才建立 reader EH 资格。
    ReaderLifetime lifetime{reader.base};
    std::int32_t error = 1;
    if (LatestQuickItem() >= 0)
    {
        // 4610E0 两次独立 slot6；第二次不补负值检查，不钳制到 6 个槽。
        const auto slot = std::bit_cast<std::int32_t>(std::uint32_t(LatestQuickItem()) + 1u);
        std::int32_t readResult;
        try
        {
            bool readFailed;
            {
                RuntimeCStringResult name([&](RuntimeCString* out)
                    { ConstructDataName(*out, reader.base, slot, shared_.Strings()); });
                auto* params = CaptureParams();
                readFailed = static_cast<std::uint8_t>(legacy_.ReadLegacyContainer(
                    params, name.Value().GetString(), 1)) == 0;
            } // 4611BA..4611C1：先释放文件名，再按锁存 AL 判断。
            if (readFailed) throw 1;
            // 4611E3 再取 root/CParams，不能在容器回调后继续缓存旧 params。
            if (static_cast<std::uint8_t>(legacy_.RebuildLegacyHistory(CaptureParams())) == 0) throw 2;
            readResult = 0;
        }
        catch (int code) { readResult = code; } // EH 553D1C -> 461222
        catch (...) { readResult = -1; }       // EH 553D1C -> 46123B
        // 4610E0：任意非零读取结果合并为 2；catch(int 0) 则保留原成功返回。
        error = readResult == 0 ? 0 : 2;
    }
    if (error != 0)
    {
        // Baku 529EB8/529EE4 与现代 5B36EC/5B3718 原 CP932 字节相同，复用真实提示常量。
        static_cast<void>(MessageBox(error == 1 ? QuickOperationsNativeNoSaveMessage() :
            QuickOperationsNativeLoadErrorMessage(), 0x10));
    }
    lifetime.Finish(); // 提示在 reader 释放之前；提示/释放异常均不转成 false。
    return error == 0;
}

const char* LegacyTitleQuickLoadConfirmationText() noexcept { return confirmation; }
}
