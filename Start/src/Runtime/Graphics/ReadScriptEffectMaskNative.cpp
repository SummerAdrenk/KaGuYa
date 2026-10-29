#include <Runtime/ReadScriptEffectMaskNative.h>
#include <Runtime/GraphicsSurfaceReference.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Surface = GraphicsSurfaceNativeBody;
template<class T> T Read(const T& value) noexcept { return static_cast<const volatile T&>(value); }

// 原PE 5B340C/5B3430/5B3510，保持CP932、原感叹号及可变char*异常类型。
char readFailure[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x93\xC7\x82\xDD"
    "\x8D\x9E\x82\xDD\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
char loadFailure[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x83\x8D\x81\x5B"
    "\x83\x68\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
char createFailure[] = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x83\x74\x83\x40\x83\x43\x83\x8B"
    "\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";

Surface* Resolve(const Slot& slot) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    auto* const control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<Surface*>(base + Read(slot.biasAt04) * stride);
}
struct ErrorText final
{
    union { RuntimeCString value; };
    bool active = false;
    ErrorText() noexcept {}
    ~ErrorText() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&value); }
};
struct TemporarySurface final
{
    Slot& slot;
    bool active = true;
    ~TemporarySurface() noexcept { if (active) ReleaseYtlReference(slot); }
    void ReleaseNormal() noexcept
    {
        // 480C84设state0Eh；5E9F8C -> 541E55，失败不得进入外层错误消息或重试。
        active = false;
        ReleaseYtlReference(slot);
    }
};

void Produce(Slot& output, const Slot& source, ReadScriptEffectMaskNativeDependencies& s)
{
    // 480C4A..53固定宽高；后续Surface回调可改正文，但平铺步长不再重读。
    const auto* const image = Resolve(source);
    const auto width = static_cast<std::uint32_t>(Read(image->widthAt08));
    const auto height = static_cast<std::uint32_t>(Read(image->heightAt0C));
    if (width == Read(s.program.CaptureAt4180C0()->widthAt18) &&
        height == Read(s.program.CaptureAt4180C0()->heightAt1C))
    {
        // 480C79/465870：旧释放完成才从临时槽复制并增持；不用现代move/swap。
        CopyAssignGraphicsSurfaceReferenceAt465870(output, source);
        return;
    }
    if (!Read(output.baseAt00))
        AdoptGraphicsSurfaceAt407A80(output, NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings));

    // 480D28先高、480D30再宽，两次Capture；输出解址在第二次取根之后。
    const auto targetHeight = std::bit_cast<std::int32_t>(Read(s.program.CaptureAt4180C0()->heightAt1C));
    const auto targetWidth = std::bit_cast<std::int32_t>(Read(s.program.CaptureAt4180C0()->widthAt18));
    if (!Resolve(output)->CreateAt415010(s.surfaces, targetWidth, targetHeight, 1, 0)) throw createFailure;

    // 480D70/480E7E内联4180C0：每次条件均Enter/Leave后重读发布根，JNB为unsigned。
    // 不缩放、不修正源原点、不预先判零宽高；DWORD ADD回绕是原行为。
    for (std::uint32_t y = 0; y < Read(s.program.CaptureAt4180C0()->heightAt1C); y += height)
    {
        for (std::uint32_t x = 0; x < Read(s.program.CaptureAt4180C0()->widthAt18); x += width)
        {
            const auto sourceHandle = Read(Resolve(source)->handleAt04);
            const auto destinationHandle = Read(Resolve(output)->handleAt04);
            // 480FA9先source、480FD1再destination；4810FF丢弃返回，不按AL补失败分支。
            s.copy.SurfaceCopyBlitRaw(destinationHandle, sourceHandle, std::bit_cast<std::int32_t>(x),
                std::bit_cast<std::int32_t>(y), nullptr, nullptr, nullptr, 0);
        }
    }
}
}

ReadScriptEffectMaskNative::ReadScriptEffectMaskNative(ReadScriptEffectMaskNativeDependencies dependencies) noexcept
    : dependencies_(dependencies) {}

bool ReadScriptEffectMaskNative::LoadMaskAt480B30(DrawManagerNativeBody2Ch* receiver,
    Slot& output, const char* name)
{
    static_cast<void>(receiver); // 原480B30在首个call之前没有保存或读取ECX。
    auto& s = dependencies_;
    GraphicsSurfaceNativeScope graphics(s.surfaces);
    ErrorText error;
    InitializeEmptyRuntimeCString(error.value, s.strings); // 480B5B..75：在try及state0之前。
    error.active = true;
    auto* const successHeader = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(error.value.GetString()) - 16);
    bool completed = false;
    // 原PE 5E9EC4：try=[1,0Eh]、catchHigh=0Fh；5E9ED8为const char*兼容handler，
    // 第二handler为catch-all。5E9F1C中state3清factory，state5清临时Surface。
    try
    {
        ArchiveReaderFactoryRuntime factory(s.archives, s.transaction, s.files, s.readers, s.codecs);
        static_cast<void>(factory.LoadBorrowedAt483D90(name, s.key));
        if (!factory.Reader().Slot().baseAt00) throw readFailure; // 480BB4只查base，不消费Load结果。
        Slot source;
        auto* const body = NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings);
        ConstructGraphicsSurfaceReferenceAt451500(source, body, 1);
        // 480BF4之后才取得source展开资格；control分配抛出不回收已构造的孤立24h。
        TemporarySurface cleanup{source};
        if (!LoadGraphicsSurfaceArchiveReaderAt419D70(*Resolve(source), factory.Reader(), s.surfaces, 0))
            throw loadFailure;
        Produce(output, source, s);
        cleanup.ReleaseNormal();
        // 4638A0的reader析构在此try退出时执行；正常/展开均noexcept，之后才释放error。
        completed = true;
    }
    catch (const char* reason) { FormatRuntimeCStringAt417130(error.value, "%s %s", name, reason); }
    catch (...) { SetRuntimeCStringAt437680(error.value, "syntax error", 12); }

    if (completed)
    {
        // 480CC2撤state0，480CC9使用入口缓存header；Free失败外传，不二次释放。
        error.active = false;
        ReleaseRuntimeCStringHeader(successHeader);
        return true;
    }
    AppendRuntimeCStringAt413550(error.value, " [ CDrawManager::LoadEffect ]", 0x1D);
    auto* const program = s.program.CaptureAt4180C0(); // 48114D在装载try之外，且晚于后缀追加。
    const char* const caption = program->titleAt10.GetString();
    const char* const text = error.value.GetString();
    const auto window = reinterpret_cast<HWND>(static_cast<std::uintptr_t>(Read(program->windowAt2C)));
    static_cast<void>(s.messageBox(window, text, caption, 0x10));
    // 481163仍在state0；正常Free若抛出，展开再次读取error槽，二次失败必须terminate。
    DestroyRuntimeCStringAt40CD40(&error.value);
    error.active = false;
    return false;
}
}
