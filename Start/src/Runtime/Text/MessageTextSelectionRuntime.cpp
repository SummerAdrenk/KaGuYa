#include <Runtime/MessageTextSelectionRuntime.h>
#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime
{
MessageChoiceSelectionRuntime::MessageChoiceSelectionRuntime(IMessageChoiceSource& source,
    const ByteArrayReferenceRuntime& readFlagsAt1C) noexcept
    : source_(source), readFlags_(readFlagsAt1C) {}

std::int32_t MessageChoiceSelectionRuntime::ReadChoiceAt418700(
    std::int32_t index, MessageTextSelection& output, std::int32_t)
{
    try
    {
        // 41875F 先查 reader；空引用抛3发生在释放旧输出之前。
        const auto choice = source_.GetChoice(index);
        if (!choice) throw 3;
        output.storageAt00.reset();
        output.voicesAt0C.reset();
        output.textAt18 = choice;
        // 4187FB/4AD6A0为有符号jge，不把带高位的control count当成超大有效表。
        if (index < 0 || index >= std::bit_cast<std::int32_t>(readFlags_.Size())) return -1;
        return *readFlags_.DataAt(static_cast<std::uint32_t>(index));
    }
    catch (...)
    {
        // EH 5E0EC4 -> 5E0E80（flags40/type0）->41885F/418865，失败不回滚输出。
        return -1;
    }
}

MessageTextSelectionRuntime::MessageTextSelectionRuntime(IMessageTextSource& source,
    const ByteArrayReferenceRuntime& readFlagsAt28) noexcept
    : source_(&source), readFlags_(&readFlagsAt28)
{
}

std::int32_t MessageChoiceSelectionRuntime::ReadChoiceRawAt418700(
    std::int32_t index, MessageFormatSelectionRawTarget output, std::int32_t)
{
    try
    {
        // 原hidden-result只有Ref12；显式管理正常不清槽/展开清槽两种退役。
        union Storage { Storage() {} ~Storage() {} MessageTextCommandReference value; } storage;
        ::new (static_cast<void*>(&storage.value)) MessageTextCommandReference(source_.GetChoice(index));
        auto& choice = storage.value.RawSlot();
        struct Lifetime final
        {
            YtlReferenceSlotRuntime& slot;
            bool active = true;
            ~Lifetime() noexcept { if (active) ReleaseYtlReference(slot); }
            void Finish() noexcept
            {
                active = false;
                auto* control = slot.controlAt08;
                if (control && --control->referenceCountAt04 == 0)
                {
                    if (control->owningAt08) control->DestroyPayloadAtVslot00();
                    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
                }
            }
        } lifetime{choice};
        if (!choice.baseAt00) throw 3;
        ReleaseYtlReference(output.name);
        ReleaseYtlReference(output.voices);
        AssignMessageReferenceAt410360(output.body, choice); // 4103C0与410360为同序Ref赋值。
        std::int32_t result = -1;
        if (index >= 0)
        {
            const volatile auto& flags = readFlags_.RawSlot();
            const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(flags.controlAt08);
            const auto count = control ? std::bit_cast<std::int32_t>(std::uint32_t(control->countAt10)) : 0;
            if (index < count)
            {
                const auto base = flags.baseAt00;
                const auto stride = base && control ? control->strideAt0C : 0u;
                const auto address = base ? base + (flags.biasAt04 + std::uint32_t(index)) * stride : 0u;
                result = *reinterpret_cast<const std::uint8_t*>(address);
            }
        }
        lifetime.Finish(); // PE5E0EC4 state2 -> terminate；不让正常释放异常落入业务catch。
        return result;
    }
    catch (...) { return -1; }
}

namespace
{
template<class Clear>
std::int32_t ReadText(IMessageTextSource& source, const ByteArrayReferenceRuntime& flags,
    std::int32_t index, Clear clearFirst, RuntimeCStringArrayReference& voices, MessageTextCommandReference& text)
{
    try
    {
        // 4AD5EF 的 reader 先运行；取不到条目时不得动调用者已有的三个引用。
        const auto message = source.GetMessage(index);
        if (!message)
            throw 3;
        clearFirst();
        voices = message->voices;
        text = message->text;
        // 4AD689..4AD6C2 在提交两个引用之后查询 byte，-1 不表示输出没有更新。
        if (index < 0 || index >= std::bit_cast<std::int32_t>(flags.Size()))
            return -1;
        return *flags.DataAt(static_cast<std::uint32_t>(index));
    }
    catch (...)
    {
        // 原 EH 5EDF94 -> 5EDF60：flags40/type0，4AD704 返回 -1，且不回滚输出。
        return -1;
    }
}
}
std::int32_t MessageTextSelectionRuntime::ReadTextAt4AD590(std::int32_t index, MessageTextSelection& output)
{
    return ReadText(*source_, *readFlags_, index, [&] { output.storageAt00.reset(); }, output.voicesAt0C, output.textAt18);
}
std::int32_t MessageTextSelectionRuntime::ReadTextAt4AD590(std::int32_t index, MessageFormatSelectionTarget output)
{
    return ReadText(*source_, *readFlags_, index, [&] { output.name.reset(); }, output.voices, output.body);
}

std::int32_t MessageTextSelectionRuntime::ReadTextRawAt4AD590(std::int32_t index,
    MessageFormatSelectionRawTarget output)
{
    try
    {
        union Storage{Storage(){}~Storage(){}MessageVoiceAndTextRef value;} storage;
        ::new(&storage.value) MessageVoiceAndTextRef(source_->GetMessage(index));
        auto& slot=storage.value.RawSlot();
        struct Lifetime final
        {
            YtlReferenceSlotRuntime& slot;bool active=true;
            ~Lifetime() noexcept{if(active)ReleaseYtlReference(slot);}
            void Finish() noexcept
            {
                active=false;auto* control=slot.controlAt08;
                if(control&&--control->referenceCountAt04==0)
                {if(control->owningAt08)control->DestroyPayloadAtVslot00();
                    if(slot.controlAt08)slot.controlAt08->DeleteControlAtVslot04(1);}
            }
        } lifetime{slot};
        if(!slot.baseAt00)throw 3;
        // 4AD604先清name；每次赋值前重新解临时Message，跨旧输出释放不能冻结正文。
        ReleaseYtlReference(output.name);
        AssignMessageReferenceAt410360(output.voices,storage.value.Get()->voices.RawSlot());
        AssignMessageReferenceAt410360(output.body,storage.value.Get()->text.RawSlot());
        std::int32_t result=-1;
        if(index>=0)
        {
            const volatile auto& flags=readFlags_->RawSlot();
            const auto* control=static_cast<const volatile EnvironmentObjectReferenceControl*>(flags.controlAt08);
            const auto count=control?std::bit_cast<std::int32_t>(std::uint32_t(control->countAt10)):0;
            if(index<count)
            {
                const auto base=flags.baseAt00;
                const auto stride=base&&control?control->strideAt0C:0u;
                const auto address=base?base+(flags.biasAt04+std::uint32_t(index))*stride:0u;
                result=*reinterpret_cast<const std::uint8_t*>(address);
            }
        }
        lifetime.Finish();return result;
    }
    catch(...){return -1;}
}

TextDisplayMessageTextRuntime::TextDisplayMessageTextRuntime(IGraphicsTextApi& graphics,
    MessageTextSelectionRuntime& messages) noexcept
    : graphics_(&graphics), messages_(&messages)
{
}

bool TextDisplayMessageTextRuntime::CreateFontFromContextSample18(
    const std::int32_t contextIndex)
{
    return graphics_->CreateTextFont(TextDisplayFontSample18(contextIndex));
}

TextDisplayTextInitializationResult TextDisplayMessageTextRuntime::InitializeFromMessageIndex(
    const std::int32_t textStringIndex)
{
    MessageTextSelection selection;
    if (messages_->ReadTextAt4AD590(textStringIndex, selection) < 0 || !selection.textAt18)
        return TextDisplayTextInitializationResult::ParseFailed;
    const auto text = selection.textAt18->GetTextAt47D550();
    return graphics_->InitializeText(text.GetString(), 0, 0)
        ? TextDisplayTextInitializationResult::Success
        : TextDisplayTextInitializationResult::GraphicsInitializationFailed;
}
}
