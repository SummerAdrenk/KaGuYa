#include <Runtime/SpriteAnimationNativeExecution.h>

#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add(std::int32_t value, std::uint32_t amount) noexcept
{ return std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(value) + amount); }
std::int32_t Count(const YtlReferenceSlotRuntime& slot) noexcept
{
    const auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08);
    return control ? std::bit_cast<std::int32_t>(control->countAt10) : 0;
}
template<class T> T* Indexed(const YtlReferenceSlotRuntime& slot, std::int32_t index) noexcept
{
    // 410460/444730：只判base，不判index正负或范围；32位加乘按x86回绕。
    const auto base = slot.baseAt00;
    if (!base) return nullptr;
    const auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08);
    const auto stride = control ? control->strideAt0C : 0u;
    return reinterpret_cast<T*>(base + (slot.biasAt04 + std::bit_cast<std::uint32_t>(index)) * stride);
}
void Fill(YtlReferenceSlotRuntime& slot, std::int32_t value)
{
    for (std::int32_t index = 0; index < Count(slot); index = Add(index, 1))
        *Indexed<std::int32_t>(slot, index) = value;
}
}

std::int32_t AdvanceSpriteAnimationNativeAt443740(SpriteAnimationNativeBody54h& body)
{
    if (!(body.flagsAt00 & 1)) return 1;
    if (body.flagsAt00 & 2) return 2;
    if (body.waitAt04 != 0)
    {
        body.waitAt04 = Add(body.waitAt04, 0xFFFFFFFFu);
        return 2;
    }
    for (;;)
    {
        const auto commandCount = Count(body.commandsAt3C);
        const auto index = body.indexAt08;
        if (index >= commandCount) return -1;
        auto* const command = Indexed<SpriteCommandNativeBody10h>(body.commandsAt3C, index);
        // 4437C4先提交游标，然后才读opcode；错误也保留已取指的前缀。
        body.indexAt08 = Add(index, 1);
        bool waited = false;
        switch (command->opcodeAt00)
        {
        case 0:
            break;
        case 1:
        {
            auto* const arguments = command->argumentsAt04.begin;
            auto* const target = Indexed<std::int32_t>(body.currentAt18, arguments[0]);
            *target = arguments[1];
            break;
        }
        case 2:
            body.waitAt04 = command->argumentsAt04.begin[0];
            waited = true;
            break;
        case 3:
        {
            if (!body.branchesAt48.baseAt00) return -1;
            const auto branchCount = Count(body.branchesAt48);
            const auto branchIndex = command->argumentsAt04.begin[0];
            if (branchIndex >= branchCount) return -1;
            auto* const branch = Indexed<SpriteBranchNativeBody8h>(body.branchesAt48, branchIndex);
            auto* const counter = Indexed<std::int32_t>(body.countersAt30, branchIndex);
            *counter = branch->secondAt04;
            break;
        }
        case 4:
        {
            if (!body.branchesAt48.baseAt00) return -1;
            const auto branchCount = Count(body.branchesAt48);
            const auto branchIndex = command->argumentsAt04.begin[0];
            if (branchIndex >= branchCount) return -1;
            auto* const branch = Indexed<SpriteBranchNativeBody8h>(body.branchesAt48, branchIndex);
            if (branch->secondAt04 > 0)
            {
                if (!*Indexed<std::int32_t>(body.countersAt30, branchIndex)) break;
                auto* const counter = Indexed<std::int32_t>(body.countersAt30, branchIndex);
                *counter = Add(*counter, 0xFFFFFFFFu);
            }
            // <=0不是结束；它绕过counter访问，无条件采用原跳转目标。
            body.indexAt08 = branch->firstAt00;
            break;
        }
        case 5:
            body.resultAt0C = command->argumentsAt04.begin[0];
            break;
        default:
            return -1;
        }
        if (body.indexAt08 >= Count(body.commandsAt3C))
        {
            const auto repeat = body.repeatAt10;
            body.indexAt08 = 0;
            if (!repeat) { body.flagsAt00 = 0; return 0; }
        }
        if (waited) return 0;
    }
}

void CopySpriteFinalMappingsNativeAt4C23A0(SpriteAnimationNativeBody54h& body)
{
    auto* const source = Indexed<std::int32_t>(body.finalAt24, 0);
    auto* const target = Indexed<std::int32_t>(body.currentAt18, 0);
    const auto bytes = std::bit_cast<std::uint32_t>(body.mappingCountAt14) * 4u;
    std::memmove(target, source, bytes);
}

bool InitializeSpriteAnimationNativeAt47E980(SpriteAnimationNativeBody54h& body, std::int32_t playFromStart)
{
    ReplaceSpriteIntArrayAt4127B0(body.currentAt18, body.mappingCountAt14);
    Fill(body.currentAt18, -1);
    // 47E9DA在current的全部释放/分配回调之后重新取mappingCount。
    ReplaceSpriteIntArrayAt4127B0(body.finalAt24, body.mappingCountAt14);
    Fill(body.finalAt24, -1);
    for (std::int32_t index = 0; index < Count(body.commandsAt3C); index = Add(index, 1))
    {
        auto* const command = Indexed<SpriteCommandNativeBody10h>(body.commandsAt3C, index);
        if (command->opcodeAt00 == 1)
        {
            auto* const arguments = command->argumentsAt04.begin;
            auto* const destination = Indexed<std::int32_t>(body.finalAt24, arguments[0]);
            *destination = arguments[1];
        }
    }
    const auto branches = Count(body.branchesAt48);
    if (branches > 0)
    {
        ReplaceSpriteIntArrayAt4127B0(body.countersAt30, branches);
        Fill(body.countersAt30, 0);
    }
    // branch count<=0不清旧counter；repeat/result同样保留，不用现代Reset代替。
    body.flagsAt00 = 1;
    body.waitAt04 = 0;
    body.indexAt08 = 0;
    if (AdvanceSpriteAnimationNativeAt443740(body) < 0)
    {
        body.indexAt08 = 0;
        body.waitAt04 = 0;
        body.flagsAt00 = 0;
        return false;
    }
    body.flagsAt00 = 1;
    if (!playFromStart)
    {
        body.flagsAt00 = 3;
        CopySpriteFinalMappingsNativeAt4C23A0(body);
        // 47EBEE..47EBFB先读当前Count；读取故障时47EBFC尚未清wait。
        const auto last = Add(Count(body.commandsAt3C), 0xFFFFFFFFu);
        body.waitAt04 = 0;
        body.indexAt08 = last;
    }
    return true;
}

std::int32_t AdvanceSpriteNativeAt46B1D0(SpriteNativeBodyA0h& body)
{
    if (body.logicalNameAt0C.GetLength() == 0 || body.modeAt04 != 2) return 1;
    return AdvanceSpriteAnimationNativeAt443740(body.animationAt34);
}

void AdvanceSpriteAnimationToFinalNativeAt4C1D20(SpriteAnimationNativeBody54h& body)
{
    const auto flags = body.flagsAt00;
    body.flagsAt00 = 1;
    const auto last = Add(Count(body.commandsAt3C), 0xFFFFFFFFu);
    while (body.indexAt08 < last)
    {
        body.waitAt04 = 0;
        if (AdvanceSpriteAnimationNativeAt443740(body) < 0) break;
    }
    if (body.flagsAt00) body.flagsAt00 = flags;
}

void AdvanceSpriteAnimationSegmentsNativeAt4D5790(SpriteAnimationNativeBody54h& body, std::int32_t segmentCount)
{
    const auto flags = body.flagsAt00;
    const auto repeat = body.repeatAt10;
    body.flagsAt00 = 1;
    body.waitAt04 = 0;
    body.indexAt08 = 0;
    body.repeatAt10 = 0;
    const auto last = Add(Count(body.commandsAt3C), 0xFFFFFFFFu);
    std::int32_t segments = 0;
    // 原4D57D2首先比较count-1；0/1条命令均不执行，不能直接套现代while(flags)。
    while (body.indexAt08 < last)
    {
        body.waitAt04 = 0;
        const auto status = AdvanceSpriteAnimationNativeAt443740(body);
        if (status < 0 || status == 1) break;
        if (status == 0 && segmentCount >= 0)
        {
            segments = Add(segments, 1);
            if (segments >= segmentCount) break;
        }
    }
    body.repeatAt10 = repeat;
    if (body.flagsAt00) body.flagsAt00 = flags;
}
}
