#include <Runtime/ScriptVoiceSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SoundSlotManagerRuntime.h>
#include <Runtime/SoundManagerNativePlayback.h>
#include <Runtime/VoiceBatchNative.h>

namespace KaGuYa::Start::Runtime
{
ScriptVoiceManagerReceiver::ScriptVoiceManagerReceiver(IScrVmVoiceRuntimeBoundary* object) noexcept
    : object_(object), playback_(nullptr), open_(nullptr)
{
    static const Operations operations{
        [](void* p, SoundManagerNativePlayback*) { static_cast<IScrVmVoiceRuntimeBoundary*>(p)->StopAndClearAllVoiceSlots(); },
        [](void* p, SoundManagerNativePlayback*, ISoundManagerNativeOpen*, const RuntimeCStringArrayReference& names) {
            return PlayVoiceBatchAt42C4F0(*static_cast<IScrVmVoiceRuntimeBoundary*>(p), names);
        },
        [](void* p, SoundManagerNativePlayback*, ISoundManagerNativeOpen*, const YtlReferenceSlotRuntime* names) {
            return PlayVoiceBatchAt42C4F0(*static_cast<IScrVmVoiceRuntimeBoundary*>(p),
                *reinterpret_cast<const RuntimeCStringArrayReference*>(names));
        }};
    operations_ = &operations;
}
ScriptVoiceManagerReceiver ScriptVoiceManagerReceiver::Native(SoundManagerNativeBody50h* body,
    SoundManagerNativePlayback& playback, ISoundManagerNativeOpen& open) noexcept
{
    static const Operations operations{
        [](void* p, SoundManagerNativePlayback* service) {
            (void)service->StopAndClearAt4384D0(*static_cast<SoundManagerNativeBody50h*>(p));
        },
        [](void* p, SoundManagerNativePlayback* service, ISoundManagerNativeOpen* opener,
            const RuntimeCStringArrayReference& names) {
            return service->PlayBatchAt42C4F0(*static_cast<SoundManagerNativeBody50h*>(p), names.RawSlot(), *opener);
        },
        [](void* p, SoundManagerNativePlayback* service, ISoundManagerNativeOpen* opener,
            const YtlReferenceSlotRuntime* names) {
            return service->PlayBatchAt42C4F0(*static_cast<SoundManagerNativeBody50h*>(p), *names, *opener);
        }};
    return {body, &playback, &open, operations};
}
void ScriptVoiceManagerReceiver::StopAndClearAllVoiceSlots() const
{ operations_->stop(object_, playback_); }
bool ScriptVoiceManagerReceiver::PlayBatchAt42C4F0(const RuntimeCStringArrayReference& names) const
{ return operations_->batch(object_, playback_, open_, names); }
bool ScriptVoiceManagerReceiver::PlayBatchRawAt42C4F0(const YtlReferenceSlotRuntime* names) const
{ return operations_->rawBatch(object_, playback_, open_, names); }

ScriptVoiceManagerReceiver RootScriptVoiceSource::CaptureVoiceManagerAfterEnsureAt413BE0()
{
    owner_.EnsureCreatedAt413BE0();
    return owner_.RequirePublished().PostWindow().VoiceReceiverAt24();
}
}
