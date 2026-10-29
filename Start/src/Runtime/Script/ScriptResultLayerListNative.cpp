#include <Runtime/ScriptResultLayerListNative.h>
#include <Runtime/ScriptResultLayerNative.h>
#include <Runtime/DrawLayerReferenceArray.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;

struct CurrentReferenceUnwind final
{
    Slot& slot;
    bool armed = true;
    ~CurrentReferenceUnwind() noexcept
    {
        if (armed) ReleaseScriptResultLayerReferenceAt4C93A0(slot);
    }
};

struct ListUnwind final
{
    ScriptResultLayerReferenceList& list;
    bool armed = true;
    ~ListUnwind() noexcept
    {
        // 5F1970 state0：在展开期间清理，不能移到catch内改变回调可见的异常状态。
        if (armed) DestroyScriptResultLayersNativeAt4C9C70(list);
    }
};

void ReleaseAppendTemporary(Slot& temporary, YtlReferenceControlRuntime* control) noexcept
{
    // 4CB134/state4：正常尾始终用ESI；payload回调不能换掉本次待删control。
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    temporary.controlAt08 = nullptr;
    temporary.baseAt00 = 0;
}

void ReleaseReaderTemporary(Slot& reader) noexcept
{
    // 4CB16B/state5：不同于上面的缓存尾，这里payload之后重读当前reader.control。
    ReleaseScriptResultLayerReferenceAt4D71D0(reader);
}

void ReleaseTerminalReader(Slot& reader) noexcept
{
    // 4CB1D9/state2：最终base为0仍退control，但原尾不清槽，也不再次析构该Ref。
    auto* const control = reader.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (reader.controlAt08) reader.controlAt08->DeleteControlAtVslot04(1);
    }
}
}

void DestroyScriptResultLayersNativeAt4C9C70(ScriptResultLayerReferenceList& list) noexcept
{
    // 4C9C70/461750及4C8600/452440的Ref12算术、读序和PE EH已逐项证明同义。
    DestroyDrawLayerReferencesAt461750(list);
}

void CollectScriptResultLayersNativeAt4CB08D(const ScrContainer& container,
    std::int32_t layerGroupIndex, ScriptResultLayerReferenceList& out,
    IEnvironmentRootMemory& memory)
{
    out.begin = nullptr;
    out.end = nullptr;
    out.capacity = nullptr;
    ListUnwind listUnwind{out};
    {
        Slot reader;
        std::uint32_t index = 0;
        for (;;)
        {
            // reader调用自身负责失败展开；成功返回前，外层不能重复取得该槽清理资格。
            ReadScriptResultLayerNativeAt4D3670(container, reader, layerGroupIndex,
                std::bit_cast<std::int32_t>(index));
            if (!reader.baseAt00)
            {
                ReleaseTerminalReader(reader);
                break;
            }

            CurrentReferenceUnwind readerUnwind{reader};
            auto* cachedControl = reader.controlAt08;
            Slot temporary{reader.baseAt00, reader.biasAt04, cachedControl};
            CurrentReferenceUnwind temporaryUnwind{temporary};
            if (cachedControl) ++cachedControl->referenceCountAt04;

            auto* const position = out.end;
            if (position == out.capacity)
            {
                // 4C8A70与454580共核；source实际槽必须活过分配回调，不预复制内容。
                GrowDrawLayerReferencesAt454580(out, position, temporary, memory);
                cachedControl = temporary.controlAt08;
            }
            else
            {
                position->baseAt00 = temporary.baseAt00;
                position->biasAt04 = temporary.biasAt04;
                position->controlAt08 = cachedControl;
                if (cachedControl) ++cachedControl->referenceCountAt04;
                out.end = position + 1;
            }

            temporaryUnwind.armed = false;
            ReleaseAppendTemporary(temporary, cachedControl);
            readerUnwind.armed = false;
            ReleaseReaderTemporary(reader);
            ++index;
        }
    }
    listUnwind.armed = false;
}
}
