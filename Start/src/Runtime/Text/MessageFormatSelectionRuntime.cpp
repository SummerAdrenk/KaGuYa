#include <Runtime/MessageFormatSelectionRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
void AssignMessageReferenceAt410360(YtlReferenceSlotRuntime& target, const YtlReferenceSlotRuntime& source)
{
    if (&target == &source) return;
    ReleaseYtlReference(target);
    // 4103A0..4103B4：释放结束才逐槽读源并发布，增持在三槽发布之后。
    volatile auto& destination = target;
    const volatile auto& input = source;
    destination.baseAt00 = input.baseAt00;
    destination.biasAt04 = input.biasAt04;
    auto* const control = input.controlAt08;
    destination.controlAt08 = control;
    if (control) ++control->referenceCountAt04;
}

MessageFormatSelectionRuntime::MessageFormatSelectionRuntime(
    IMessageFormatSource& source, IMessageFormatReadState& readState) noexcept
    : source_(source), readState_(readState) {}

std::int32_t MessageFormatSelectionRuntime::ReadFormatAt417C40(std::int32_t index,
    MessageFormatSelectionTarget output, std::uint32_t variant, bool markRead)
{
    return ReadFormatRawAt417C40(index,
        {output.name.RawSlot(), output.voices.RawSlot(), output.body.RawSlot()}, variant, markRead);
}

std::int32_t MessageFormatSelectionRuntime::ReadFormatRawAt417C40(std::int32_t index,
    MessageFormatSelectionRawTarget output, std::uint32_t variant, bool markRead)
{
    try
    {
        const auto format = source_.GetFormat(index);
        if (!format) throw 3;
        AssignMessageReferenceAt410360(output.name, format->name.RawSlot());
        if (format->messages.empty())
        {
            ReleaseYtlReference(output.voices);
            ReleaseYtlReference(output.body);
        }
        else
        {
            // 417DCC..417DD1 是 unsigned 比较，负值的位模式同样改选第零项。
            const auto selected = variant < format->messages.size() ? variant : 0u;
            if (!format->messages.at(selected))
                throw std::logic_error("CMessage format contains a null message reference");
            AssignMessageReferenceAt410360(output.voices, format->messages[selected]->voices.RawSlot());
            // 417E1F..417E6B 在提交声音后重新解引用 variant，不能预存正文快照。
            if (!format->messages.at(selected))
                throw std::logic_error("CMessage format message reference disappeared after voice assignment");
            AssignMessageReferenceAt410360(output.body, format->messages[selected]->text.RawSlot());
        }
        // 417E73 之后才检查 index/已读表。保留旧 byte，写入失败也不撤销上面的引用。
        const auto previous = readState_.ReadFormatFlag(index);
        if (!previous) return -1;
        if (markRead)
        {
            readState_.SetFormatRead(index);
            if (*previous == 0) readState_.PersistUnreadFlags();
        }
        return *previous;
    }
    catch (...)
    {
        // EH 5E0E28 -> 417F17/417F1D；包括持久化抛出的异常，统一返回 -1。
        return -1;
    }
}

void ClearMessageFormatAt4C5850(MessageFormatSelectionTarget output) noexcept
{
    output.name.reset();
    output.body.reset();
    output.voices.reset();
}
void ClearMessageFormatRawAt4C5850(MessageFormatSelectionRawTarget output)
{
    ReleaseYtlReference(output.name);
    ReleaseYtlReference(output.body);
    ReleaseYtlReference(output.voices);
}
}
