#include <Runtime/TextWindowNativeColors.h>
#include <Runtime/ParamsGameSystemRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>

#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;

// PE 5B2CB0/5B2CC0/5B2CD0/5B2CDC：保留原CP932字节及四组顺序。
constexpr char NormalBodyName[] = "\x83\x65\x83\x4C\x83\x58\x83\x67\x95\xB6\x8E\x9A\x90\x46";
constexpr char ReadBodyName[] = "\x8A\xF9\x93\xC7\x83\x65\x83\x4C\x83\x58\x83\x67\x90\x46";
constexpr char NameName[] = "\x96\xBC\x91\x4F\x95\xB6\x8E\x9A\x90\x46";
constexpr char NameShadowName[] = "\x96\xBC\x91\x4F\x8F\xFC\x82\xE8\x90\x46";

class LocalTag final
{
public:
    LocalTag() noexcept {}
    ~LocalTag() noexcept { if (armed) ReleaseYtlReference(slot); }
    void ReleaseNormalCurrent() noexcept
    {
        // 45DD56/45DE32/45DF0E/45DFEA：先撤EH资格；正常尾异常须终止，不能由颜色catch吞下。
        armed = false;
        auto* control = slot.controlAt08;
        if (!control || --control->referenceCountAt04 != 0) return;
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        // 45DD6E/45DE4A/45DF26/45E002：payload后重读，允许换control或清空；不再次减持。
        control = slot.controlAt08;
        if (control) control->DeleteControlAtVslot04(1);
    }
    union { Slot slot; };
    bool armed = false;
};

void TryColor(Slot& destination, const char* name, ITextWindowNativeColorSource& source,
    IEnvironmentRootMemory& memory)
{
    try
    {
        auto* const game = source.CaptureGameSystemAt418480();
        auto* const reader = source.CaptureReaderAt47BCF0();
        LocalTag tag;
        // 45DD26等：reader捕获后才取固定game当前+7C；不快照、不增持game/parent。
        source.ReadChildAt477E90(reader, tag.slot, game->settingTags[1].Native(), name);
        tag.armed = true;
        const auto color = source.ReadRgbAt4797C0(reader, tag.slot, "Value");
        tag.ReleaseNormalCurrent();
        RebuildMessageWindowColorReferenceAt469EC0(destination, memory);
        // 45DD8B/45DE67/45DF43/45E01F：完成469EC0后才按当前base/bias/stride提交DWORD。
        ResolveMessageWindowNativeColor(destination)->valueAt00 = color;
    }
    catch (...) {}
}
}

void InitializeTextWindowNativeColorsAt45DCF5(TextWindowLayerNativeBodyE8h& body,
    ITextWindowNativeColorSource& source, IEnvironmentRootMemory& memory)
{
    TryColor(body.normalBodyColorAt9C, NormalBodyName, source, memory);
    TryColor(body.readBodyColorAtA8, ReadBodyName, source, memory);
    TryColor(body.nameColorAtB4, NameName, source, memory);
    TryColor(body.nameShadowColorAtC0, NameShadowName, source, memory);
}

void TryInitializeWindowNativeColor(Slot& destination, const char* name,
    ITextWindowNativeColorSource& source, IEnvironmentRootMemory& memory)
{
    // 45CCFE/45CDDB/45CEB8与45DCF5等逐组同序；各自正常tag释放状态都指向terminate。
    TryColor(destination, name, source, memory);
}

TextWindowNativeColorInitialization::TextWindowNativeColorInitialization(ITextWindowNativeColorSource& source) noexcept
    : source_(source) {}
void TextWindowNativeColorInitialization::InitializeAt45DCF5(TextWindowLayerNativeBodyE8h& body,
    IEnvironmentRootMemory& memory)
{ InitializeTextWindowNativeColorsAt45DCF5(body, source_, memory); }

TextWindowNativeColorSource::TextWindowNativeColorSource(ParamsRuntimeRootOwner& params,
    ISettingTagErrorSource& readers, IMessageWindowNativeConfigExpressions& expressions) noexcept
    : params_(params), readers_(readers), expressions_(expressions) {}
ParamsGameSystem* TextWindowNativeColorSource::CaptureGameSystemAt418480()
{ return params_.CaptureAt418480()->gameSystemAt04.get(); }
RuntimeCString* TextWindowNativeColorSource::CaptureReaderAt47BCF0()
{ return readers_.CaptureAt47BCF0(); }
void TextWindowNativeColorSource::ReadChildAt477E90(RuntimeCString* reader, Slot& out,
    const Slot& parent, const char* key)
{
    // 477F26：直接在调用者临时槽接收raw返回值，不构造ParamsSettingTagReference现代owner。
    ::new (static_cast<void*>(&out)) Slot(CaptureSettingChildNativeAt477E90(reader, parent, key));
}
std::uint32_t TextWindowNativeColorSource::ReadRgbAt4797C0(RuntimeCString* reader,
    const Slot& tag, const char* key)
{ return expressions_.ReadRgbAt4797C0(reader, tag, key); }
}
