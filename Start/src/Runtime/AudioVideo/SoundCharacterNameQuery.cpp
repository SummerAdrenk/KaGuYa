#include <Runtime/SoundCharacterGroupConfiguration.h>

namespace KaGuYa::Start::Runtime
{
void ConstructSoundCharacterNameAt478B60(RuntimeCString* out,
    const YtlReferenceSlotRuntime& group, const char* resourceName, IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(*out, strings);
    struct ReturnCleanup final
    {
        RuntimeCString* value;
        ~ReturnCleanup() noexcept { if (value) DestroyRuntimeCStringAt40CD40(value); }
    } result{out};
    // 478BDA仍执行空字面串的437680；state1到state0保留隐藏返回槽的销毁资格。
    SetRuntimeCStringAt437680(*out, "", 0);
    YtlReferenceSlotRuntime selected;
    FindSoundCharacterAt489630(selected, ResolveEnvironmentObjectReference<SoundCharacterGroupNative>(group), resourceName);
    auto* cached = selected.controlAt08;
    SoundNativeTemporary cleanup(selected); // FuncInfo5E8064: state2 -> Ref -> 返回CString。
    if (selected.baseAt00)
        AssignRuntimeCStringAt410220(*out, ResolveEnvironmentObjectReference<SoundCharacterNative>(selected)->keyAt08);
    // 原state3的正常减持若抛出直接terminate，不提前解除返回槽的state0资格。
    cleanup.Finish(cached);
    result.value = nullptr;
}
}
