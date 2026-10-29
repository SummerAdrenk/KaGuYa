#include <Runtime/DrawManagerResourceRuntime.h>
#include <Runtime/DrawManagerSpriteResourceNative.h>

#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/SpriteRuntime.h>
#include <Runtime/SpriteResourceNativeLoad.h>
#include <Runtime/AnimationFormatNativePictures.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原 PE 5B340C/5B3430 的 CP932 错误文本，不是日志所用的 UTF-8。
char readFailure[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
char loadFailure[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x83\x8D\x81\x5B\x83\x68\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";

class DrawErrorText final
{
public:
    explicit DrawErrorText(IRuntimeStringManagerSource& strings)
    {
        InitializeEmptyRuntimeCString(storage_.value, strings);
        cachedHeader_ = reinterpret_cast<ATL::CStringData*>(
            const_cast<char*>(storage_.value.GetString())) - 1;
    }
    ~DrawErrorText() noexcept
    {
        if (armed_) DestroyRuntimeCStringAt40CD40(&storage_.value);
    }
    RuntimeCString& Value() noexcept { return storage_.value; }
    void ReleaseSuccess()
    {
        // 48014A/48159D 先撤销 state，再释放入口缓存的 nil header；失败不重试。
        armed_ = false;
        ReleaseRuntimeCStringHeader(cachedHeader_);
    }
    void ReleaseDisplayedError()
    {
        // 4801CB/48161E 正常 Free 仍在 state0 内；首次抛出后展开会再读槽释放。
        DestroyRuntimeCStringAt40CD40(&storage_.value);
        armed_ = false;
    }
private:
    union Storage
    {
        Storage() noexcept {}
        ~Storage() noexcept {}
        RuntimeCString value;
    } storage_;
    ATL::CStringData* cachedHeader_ = nullptr;
    bool armed_ = true;
};

template<class Load, class Errors>
bool LoadWithOriginalErrorDisplay(
    const char* name,
    Errors& errors,
    const char* suffix,
    int suffixLimit,
    IArchiveResourceSource& source,
    const FileTransactionContext* transaction,
    IStartFileApi& files,
    const IEnvironmentKeyTableSource& key,
    ArchiveCodecServices codecs,
    IRuntimeStringManagerSource& strings,
    Load&& load)
{
    // 4800CB/48151B 的 GetNil 在装载 try 之前；失败不得变为 syntax error。
    DrawErrorText error(strings);
    bool success = false;
    try
    {
        ArchiveReaderMemoryApi memory;
        ArchiveReaderFactoryRuntime factory(source, transaction, files, memory, codecs);
        // 484339 清理异常进入外层错误分类，不能再次折叠成读失败。
        if (!factory.LoadBorrowedAt483D90(name, key))
            throw readFailure;

        bool loaded = false;
        try
        {
            // 只传外层借用槽；目标在Ready/cache/parse/name各原点自己重新Get。
            loaded = load(factory.Reader());
        }
        catch (const StaticSurfaceResourceException&)
        {
            // 41FCB0 格式识别失败以 false 到达 4800A0，不归类为一般装载异常。
            loaded = false;
        }
        if (!loaded)
            throw loadFailure;
        success = true;
    }
    catch (const char* text)
    {
        FormatRuntimeCStringAt417130(error.Value(), "%s %s", name, text);
    }
    catch (...)
    {
        SetRuntimeCStringAt437680(error.Value(), "syntax error", 12);
    }

    if (success)
    {
        error.ReleaseSuccess();
        return true;
    }

    // 48019C..4801C2 已在装载 try 外；错误显示抛出必须继续向调用者传播。
    AppendRuntimeCStringAt413550(error.Value(), suffix, suffixLimit);
    // 原生域借CString槽，旧const-char域在显式接口转换时取chars；不按失败选择另一条显示链。
    errors.Show(error.Value());
    error.ReleaseDisplayedError();
    return false;
}

template<class Errors>
bool LoadNativeSprite(SpriteNativeBodyA0h* target, const char* name, std::uint8_t restart,
    IArchiveResourceSource& source, const FileTransactionContext* transaction, IStartFileApi& files,
    const IEnvironmentKeyTableSource& key, Errors& errors, AnimationFormatNativePictureServices& services)
{
    return LoadWithOriginalErrorDisplay(name, errors, " [ CDrawManager::LoadSprite ]", 0x1D,
        source, transaction, files, key, services.codecs, services.strings,
        [target, restart, &services](const ArchiveReaderReference& reader)
        {
            AnimationFormatNativePictures pictures(services);
            return LoadSpriteArchiveNativeAt480650(*target, reader, restart, 0, pictures,
                services.surfaces, services.strings, &services.inputMode);
        });
}
}

bool LoadDrawManagerStaticInto(
    RuntimeGraphicsSurface& target,
    const std::string& nameCp932,
    const ResourceCatalog& catalog,
    const FileTransactionContext* transaction,
    IStartFileApi& files,
    const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs)
{
    FixedArchiveResourceSource source(catalog);
    return LoadDrawManagerStaticBorrowedAt4800A0(target, nameCp932.c_str(),
        source, transaction, files, key, errors, codecs);
}

bool LoadDrawManagerStaticBorrowedAt4800A0(RuntimeGraphicsSurface& target, const char* nameCp932,
    IArchiveResourceSource& source, const FileTransactionContext* transaction,
    IStartFileApi& files, const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs,
    IRuntimeStringManagerSource& strings)
{
    // 5E9D3C 容许 const char*；工厂及 reader 保留到目标消费结束。
    return LoadWithOriginalErrorDisplay(
        nameCp932, errors, " [ CDrawManager::Load ]", 0x17,
        source, transaction, files, key, codecs, strings,
        [&target](const ArchiveReaderReference& reader)
        {
            return target.LoadArchiveReaderAt419D70(reader, 0u);
        });
}

bool LoadDrawManagerSprite(
    SpriteRuntime& target,
    const std::string& nameCp932,
    const ResourceCatalog& catalog,
    const FileTransactionContext* transaction,
    IStartFileApi& files,
    const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs)
{
    FixedArchiveResourceSource source(catalog);
    return LoadDrawManagerSpriteBorrowedAt4814F0(target, nameCp932.c_str(), 1,
        source, transaction, files, key, errors, codecs);
}

bool LoadDrawManagerSpriteBorrowedAt4814F0(SpriteRuntime& target, const char* nameCp932,
    std::uint8_t restart, IArchiveResourceSource& source, const FileTransactionContext* transaction,
    IStartFileApi& files, const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs,
    IRuntimeStringManagerSource& strings)
{
    // 5E9E58 与静态入口同为 .PAD + HT_IsConst，必须同时接受 char* / const char*。
    return LoadWithOriginalErrorDisplay(
        nameCp932, errors, " [ CDrawManager::LoadSprite ]", 0x1D,
        source, transaction, files, key, codecs, strings,
        [&target, restart, &codecs](const ArchiveReaderReference& reader)
        {
            return target.LoadArchiveReaderAt480650(reader, restart, 0, codecs);
        });
}
bool LoadDrawManagerSpriteNativeAt4814F0(SpriteNativeBodyA0h* target, const char* nameCp932,
    std::uint8_t restart, IArchiveResourceSource& source, const FileTransactionContext* transaction,
    IStartFileApi& files, const IEnvironmentKeyTableSource& key, IGraphicsResourceErrorDisplay& errors,
    AnimationFormatNativePictureServices& services)
{
    return LoadNativeSprite(target, nameCp932, restart, source, transaction, files, key, errors, services);
}
bool LoadDrawManagerSpriteNativeAt4814F0(SpriteNativeBodyA0h* target, const char* name,
    std::uint8_t restart, IArchiveResourceSource& source, const FileTransactionContext* transaction,
    IStartFileApi& files, const IEnvironmentKeyTableSource& key, IGraphicsResourceErrorDisplayNative& errors,
    AnimationFormatNativePictureServices& services)
{
    return LoadNativeSprite(target, name, restart, source, transaction, files, key, errors, services);
}
DrawManagerSpriteNativeResourceRuntime::DrawManagerSpriteNativeResourceRuntime(RootRuntimeOwner& root,
    IStartFileApi& files, const IEnvironmentKeyTableSource& key, IGraphicsResourceErrorDisplay& errors,
    AnimationFormatNativePictureServices& pictures, const FileTransactionContext* transaction) noexcept
    : root_(root), archives_(root), files_(files), key_(key), errors_(errors), pictures_(pictures), transaction_(transaction) {}
void* DrawManagerSpriteNativeResourceRuntime::CaptureDrawReceiverAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.RequirePublished().PostWindow().DrawManagerAt18();
}
bool DrawManagerSpriteNativeResourceRuntime::LoadAt4814F0(void*, SpriteNativeBodyA0h* target,
    const char* name, std::uint8_t restart)
{ return LoadDrawManagerSpriteNativeAt4814F0(target, name, restart, archives_, transaction_, files_, key_, errors_, pictures_); }
}
