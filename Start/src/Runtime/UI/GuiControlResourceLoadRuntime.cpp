#include <Runtime/GuiControlResourceLoadRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原5B340C/5B3430的CP932字节；两种失败消息不可合并。
char readFailed[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x93\xC7\x82\xDD"
    "\x8D\x9E\x82\xDD\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
char loadFailed[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x83\x8D\x81\x5B"
    "\x83\x68\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
struct ErrorText final
{
    union { RuntimeCString value; };
    bool active = false;
    ErrorText() noexcept {}
    ~ErrorText() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&value); }
};
}

bool LoadGuiControlSurfaceReferenceAt480000(YtlReferenceSlotRuntime& target,
    void* capturedReceiver, const char* resourceName, IGuiControlResourceLoadServices& loader,
    IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings)
{
    if (!target.baseAt00)
        AdoptGraphicsSurfaceAt407A80(target, NewGraphicsSurfaceAt40C2E0(surfaces, strings));
    auto* const body = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(target);
    return loader.LoadIntoAt4800A0(capturedReceiver, *body, resourceName);
}

int GuiControlResourceMessageApi::MessageBoxAt4801C2(HWND window, const char* text,
    const char* caption, UINT type)
{
    return ::MessageBoxA(window, text, caption, type);
}

GuiControlResourceLoadRuntime::GuiControlResourceLoadRuntime(RootRuntimeOwner& root,
    EnvironmentStartupRuntime& environment, IStartFileApi& files, const IEnvironmentKeyTableSource& key,
    ArchiveCodecServices codecs, IGraphicsSurfaceApi& surfaces, IGuiControlResourceMessageApi& messages,
    IRuntimeStringManagerSource& strings, const FileTransactionContext* transaction) noexcept
    : root_(root), environment_(environment), archiveSource_(root), files_(files), key_(key), codecs_(codecs),
      surfaces_(surfaces), messages_(messages), strings_(strings), transaction_(transaction) {}

void* GuiControlResourceLoadRuntime::CaptureLoadReceiverAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.RequirePublished().PostWindow().DrawManagerAt18();
}

bool GuiControlResourceLoadRuntime::LoadIntoAt4800A0(void* capturedReceiver,
    GraphicsSurfaceNativeBody& destination, const char* resourceName)
{
    // 4800A0不读取ECX；outer捕获和内部archive取根仍是两个独立调用点。
    static_cast<void>(capturedReceiver);
    return LoadGuiControlResourceIntoAt4800A0(destination, resourceName, archiveSource_, transaction_,
        files_, key_, surfaces_, *this, codecs_, strings_);
}

void GuiControlResourceLoadRuntime::ShowAt4801B2(const RuntimeCString& text)
{
    auto& environment = environment_.CaptureAt4180C0();
    const char* const caption = environment.titleAt10.GetString();
    const char* const message = text.GetString();
    const auto window = reinterpret_cast<HWND>(static_cast<std::uintptr_t>(environment.windowAt2C));
    static_cast<void>(messages_.MessageBoxAt4801C2(window, message, caption, 0x10));
}

bool LoadGuiControlResourceIntoAt4800A0(GraphicsSurfaceNativeBody& destination,
    const char* resourceName, IArchiveResourceSource& source, const FileTransactionContext* transaction,
    IStartFileApi& files, const IEnvironmentKeyTableSource& key, IGraphicsSurfaceApi& surfaces,
    IGuiControlResourceErrorDisplay& errors, ArchiveCodecServices codecs, IRuntimeStringManagerSource& strings)
{
    ErrorText error;
    InitializeEmptyRuntimeCString(error.value, strings);
    error.active = true;
    auto* const successHeader = reinterpret_cast<ATL::CStringData*>(
        const_cast<char*>(error.value.GetString()) - 16);
    bool loaded = false;
    try
    {
        ArchiveReaderMemoryApi readerMemory;
        ArchiveReaderFactoryRuntime factory(source, transaction, files, readerMemory, codecs);
        static_cast<void>(factory.LoadBorrowedAt483D90(resourceName, key));
        // 48011E只测base：control失败的部分reader不能因Load返回false而跳过419D70。
        if (!factory.Reader().Slot().baseAt00) throw readFailed;
        if (!LoadGraphicsSurfaceArchiveReaderAt419D70(destination, factory.Reader(), surfaces)) throw loadFailed;
        loaded = true;
        // 4638A0先结束独立factory，正常/展开的reader释放均为noexcept边界。
    }
    catch (const char* reason)
    {
        FormatRuntimeCStringAt417130(error.value, "%s %s", resourceName, reason);
    }
    catch (...)
    {
        SetRuntimeCStringAt437680(error.value, "syntax error", 12);
    }
    if (loaded)
    {
        // 48014A撤error保护，正常尾使用入口缓存ESI；Free抛出外传且不重试。
        error.active = false;
        ReleaseRuntimeCStringHeader(successHeader);
        return true;
    }
    AppendRuntimeCStringAt413550(error.value, " [ CDrawManager::Load ]", 0x17);
    errors.ShowAt4801B2(error.value);
    // 4801CB仍为state0：本次Free抛出时展开还会重读槽清理一次，不能提前撤保护。
    DestroyRuntimeCStringAt40CD40(&error.value);
    error.active = false;
    return false;
}
}
