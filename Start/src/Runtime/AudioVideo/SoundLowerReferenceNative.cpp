#include <Runtime/SoundLowerReferenceNative.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>

namespace KaGuYa::Start::Runtime
{
void AppendSoundLowerAt45D435(SoundManagerNativeBody50h& body, const YtlReferenceSlotRuntime& source,
    IEnvironmentRootMemory& memory)
{
    auto& storage = body.lowerAt30;
    auto* const end = storage.end;
    if (end != storage.capacity)
    {
        // 45D441..45D454内联段写literal0；慢路径455900复制输入bias，不能混同。
        end->baseAt00 = source.baseAt00;
        end->biasAt04 = 0;
        end->controlAt08 = source.controlAt08;
        if (end->controlAt08) ++end->controlAt08->referenceCountAt04;
        storage.end = reinterpret_cast<YtlReferenceSlotRuntime*>(reinterpret_cast<std::uintptr_t>(storage.end) + 12U);
        return;
    }
    // 455900与454310：12-byte/signed distance/15555555/growth/alignment/复制范围
    // 及state=-1后旧存储释放的分支已逐项对照；不创建第二份三指针存储。
    AppendEnvironmentArchiveReferenceAt49852D(storage, source, memory);
}
void ClearSoundLowerAt45D368(SoundManagerNativeBody50h& body) noexcept
{
    // 4529E0逐项释放，payload后重读当前control；最后end用回调后的begin。
    ClearEnvironmentArchiveReferencesAt498312(body.lowerAt30);
}
void DestroySoundLowerAt462240(SoundManagerNativeBody50h& body, IEnvironmentRootMemory& memory) noexcept
{
    DestroyEnvironmentArchiveReferencesAt497080(body.lowerAt30, memory);
}
}
