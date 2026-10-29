#include <Runtime/CountDownWindowResources.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/RuntimeCString.h>

#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
CountDownWindowResourceProfile BuildMamatomoCountDownWindowResourceProfile()
{
    // 全部 72 个名称均逐项与 PE 比较；这里只消除已经证实的重复字节。
    constexpr const char* prefix = "\x8E\xCB\x90\xB8\x83\x4A\x83\x45\x83\x93\x83\x67\x83\x5F\x83\x45\x83\x93_";
    constexpr std::array<const char*, 4> styles{{
        "\x83\x7A\x83\x8F\x83\x43\x83\x67", "\x83\x4F\x83\x8C\x81\x5B",
        "\x83\x7A\x83\x8F\x83\x43\x83\x67\x94\xBC\x93\xA7\x96\xBE",
        "\x83\x4F\x83\x8C\x81\x5B\x94\xBC\x93\xA7\x96\xBE"}};
    constexpr std::array<const char*, 3> sizes{{"\x8F\xAC", "\x92\x86", "\x91\xE5"}};
    CountDownWindowResourceProfile result{};
    for (std::size_t style = 0; style < styles.size(); ++style)
        for (std::size_t size = 0; size < sizes.size(); ++size)
        {
            const auto variant = style * 3 + size;
            result.suffixNamesCp932[variant] = std::string(prefix) + styles[style] +
                "\x8E\xCB\x90\xB8\x82\xDC\x82\xC5" + sizes[size] + ".vrm";
            for (std::size_t digit = 0; digit < 5; ++digit)
                result.numberNamesCp932[variant * 5 + digit] = std::string(prefix) +
                    static_cast<char>('1' + digit) + styles[style] + sizes[size] + ".vrm";
        }
    result.suffixPositions = {{{1222, 627}, {1210, 610}, {1198, 593},
        {36, 627}, {24, 610}, {12, 593}}};
    result.numberPositions = {{{1222, 640}, {1210, 627}, {1198, 614},
        {36, 640}, {24, 627}, {12, 614}}};
    return result;
}

void Win32CountDownWindowMessageApi::MessageBoxAt481615(const HWND window,
    const char* const text, const char* const caption, const UINT type)
{
    static_cast<void>(::MessageBoxA(window, text, caption, type));
}

CountDownWindowResources::CountDownWindowResources(const ResourceCatalog& catalog,
    const FileTransactionContext* const transaction, IStartFileApi& files,
    const IEnvironmentKeyTableSource& key, ILoadSpriteErrorEnvironmentSource& environment,
    ICountDownWindowMessageApi& messages, ArchiveCodecServices codecs,
    IRuntimeStringManagerSource& strings) noexcept
    : catalog_(catalog), transaction_(transaction), files_(files), key_(key),
      environment_(environment), messages_(messages), codecs_(codecs), strings_(strings)
{
}

bool CountDownWindowResources::LoadSpriteAt4814F0(SpriteRuntime& sprite,
    const std::string& nameCp932, const bool playAnimationFromStart)
{
    // 5B340C/5B3430：读取失败与 Sprite 装载失败是两个不同的 char* 异常。
    constexpr const char* readFailure =
        "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
    constexpr const char* loadFailure =
        "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x83\x8D\x81\x5B\x83\x68\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
    // 48151B..481535 位于内部 try 之前：manager/空串失败不能改写为 syntax error。
    RuntimeCString error(strings_.CaptureAt417360());
    try
    {
        ArchiveReaderMemoryApi memory;
        ArchiveReaderFactoryRuntime factory(catalog_, transaction_, files_, memory, codecs_);
        if (!factory.Load(nameCp932, key_)) throw readFailure;
        bool cached = false;
        // 48069C/4807DF读取CFileReader+10，比较/保存完整逻辑引用而非fileinfo.title。
        if (!sprite.LoadArchiveReaderAt480650(factory.Reader(),
            static_cast<std::uint8_t>(playAnimationFromStart), 0, codecs_, &cached))
            throw loadFailure;
        // 480778/480780、47EF7C/47EF7F：decoder 的资源坐标落到原 +2C/+30。
        // 当前共享模型把装载坐标与命令定位分开；缓存命中必须保留原定位。
        if (!cached) sprite.SetOpaqueValuesAt2CAnd30(
            static_cast<std::uint32_t>(sprite.X()), static_cast<std::uint32_t>(sprite.Y()));
        return true;
    }
    catch (const char* message)
    {
        FormatRuntimeCStringAt417130(error, "%s %s", nameCp932.c_str(), message);
    }
    catch (...)
    {
        error.SetString("syntax error", 12); // 481639 -> 437680
    }
    // MessageBox 位于内部 catch 之外；它抛错时由构造器外层 catch 处理。
    error.Append(" [ CDrawManager::LoadSprite ]", 29); // 4815EF -> 413550
    // 481605..481615：拼接成功后只取一次环境，按caption/text/HWND顺序借值。
    const auto environment = environment_.CaptureAt4180C0();
    const char* const caption = environment.CaptionAt10();
    const char* const text = error.GetString();
    const HWND window = environment.windowAt2C.Read();
    messages_.MessageBoxAt481615(window, text, caption, MB_ICONHAND);
    return false;
}
}
