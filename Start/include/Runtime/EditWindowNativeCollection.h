#pragma once
#include <Runtime/EditWindowNativeLifetime.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
using EditWindowNativeReferences=EnvironmentVectorStorage<YtlReferenceSlotRuntime>;
static_assert(sizeof(EditWindowNativeReferences)==12);
// 只操作原root+54/58/5C三指针，不把RootRuntimePayload强转成21Ch。
YtlReferenceSlotRuntime* GrowEditWindowNativeReferencesAt452E60(EditWindowNativeReferences&,
    YtlReferenceSlotRuntime* position,const YtlReferenceSlotRuntime* source);
void AppendEditWindowNativeReferenceAt46FC43(EditWindowNativeReferences&,const YtlReferenceSlotRuntime&);
YtlReferenceSlotRuntime* FindEditWindowNativeAt479DF0(EditWindowNativeReferences&,YtlReferenceSlotRuntime*,const void*);
void RemoveEditWindowNativeAt486B60(EditWindowNativeReferences&,const void*);
void DestroyEditWindowNativeReferencesAt40DE10(EditWindowNativeReferences&) noexcept;
}
