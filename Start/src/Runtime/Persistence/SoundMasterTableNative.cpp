#include <Runtime/SoundMasterTableNative.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>

#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
SoundMasterTableNative::~SoundMasterTableNative() noexcept
{
    DestroyEnvironmentArchiveReferencesAt497080(storage_);
}

void SoundMasterTableNative::clear() noexcept
{
    ClearEnvironmentArchiveReferencesAt498312(storage_);
}

const SoundTypeNative* SoundMasterTableNative::FindAt47B0C0(const char* name) const
{
    const auto resolve = [](const YtlReferenceSlotRuntime* slot) noexcept
    {
        const auto& raw = *static_cast<const volatile YtlReferenceSlotRuntime*>(slot);
        const auto base = raw.baseAt00;
        if (!base) return std::uintptr_t{0};
        auto* const control = raw.controlAt08;
        const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
            reinterpret_cast<std::uintptr_t>(control) + 0x0Cu) : 0u;
        const auto bias = raw.biasAt04;
        return base + bias * stride;
    };
    const volatile auto& storage = storage_;
    for (auto* slot = storage.begin; slot != storage.end; ++slot)
    {
        const auto body = resolve(slot);
        // 47B0F3先拒绝null查询；47B0F8才裸读正文+0C，空正文保留原读故障。
        if (!name) throw ATL::CAtlException(E_FAIL);
        const auto key = *reinterpret_cast<const char* const volatile*>(body + 0x0Cu);
        if (_mbscmp(reinterpret_cast<const unsigned char*>(key),
                reinterpret_cast<const unsigned char*>(name)) == 0)
            return reinterpret_cast<const SoundTypeNative*>(resolve(slot));
    }
    return nullptr;
}

void SoundMasterTableNative::AppendReferenceAt47025E(const YtlReferenceSlotRuntime& source,
    IEnvironmentRootMemory& memory)
{
    // 4567A0与454310：相同12-byte拷贝增持、1.5倍扩容、32-byte对齐及原序释放。
    AppendEnvironmentArchiveReferenceAt49852D(storage_, source, memory);
}

void SoundMasterTableNative::SerializeAt48B750(SerializeBuffer& serializer,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    std::uint32_t count = serializer.IsWriting() ? size() : 0u;
    serializer.SerializeSlot58Dword(count);
    if (serializer.IsWriting())
    {
        // 每次重读end，不能先快照正文或缓存end；虚调用允许改变存储状态。
        for (auto* slot = storage_.begin; slot != storage_.end; ++slot)
        {
            auto* body = ResolveEnvironmentObjectReference<SoundTypeNative>(*slot);
            if (!body) _invalid_parameter_noinfo_noreturn();
            body->Serialize(serializer);
        }
        return;
    }
    for (std::int32_t index = 0; index < std::bit_cast<std::int32_t>(count); ++index)
    {
        auto* incoming = CreateSoundTypeAt46FEBD(strings, memory);
        YtlReferenceSlotRuntime local;
        AdoptSoundNativeReference(local, incoming, memory);
        auto* cached = local.controlAt08;
        SoundNativeTemporary unwind(local);
        if (!incoming) _invalid_parameter_noinfo_noreturn();
        incoming->Serialize(serializer);
        for (auto* slot = storage_.begin; slot != storage_.end; ++slot)
        {
            auto* target = ResolveEnvironmentObjectReference<SoundTypeNative>(*slot);
            if (!target) _invalid_parameter_noinfo_noreturn();
            if (!target->keyAt0C.GetString()) throw ATL::CAtlException(E_FAIL);
            if (_mbscmp(reinterpret_cast<const unsigned char*>(incoming->keyAt0C.GetString()),
                    reinterpret_cast<const unsigned char*>(target->keyAt0C.GetString())) != 0) continue;
            // 48B908/48B92E分别重解同一目标Ref，只回填标量，不替换名称/正文/控制块。
            ResolveEnvironmentObjectReference<SoundTypeNative>(*slot)->valueAt04 = incoming->valueAt04;
            ResolveEnvironmentObjectReference<SoundTypeNative>(*slot)->valueAt08 = incoming->valueAt08;
            break;
        }
        unwind.Finish(cached);
    }
}
}
