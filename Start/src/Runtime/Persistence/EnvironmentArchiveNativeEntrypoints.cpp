#include <Runtime/AuxiliaryArchiveNative18.h>
#include <Runtime/EnvironmentArchiveNative18.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/NativeSerialize18.h>
#include <Runtime/PatternFilterNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SaveDataNative18.h>
#include <Runtime/SaveFileStateNativeArchive.h>
#include <Runtime/ScriptCallStacksNative.h>
#include <Runtime/SecretCgNativeArchive.h>
#include <Runtime/SoundParamsBaseNative18.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/SoundValueNative18.h>
#include <Runtime/UnSaveDataRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 原nullsub均ret4，不读参数或字段；不是未实现分支的成功占位。
void SystemDataEnvironmentArchive::SerializeNative18(void*) noexcept {} // 498220
void UnSaveDataRuntime::SerializeNative18(void*) noexcept {} // 48BA10
void SaveFileStateNativeArchive::SerializeNative18(void*) noexcept {} // 48B6A0
void SaveDataEnvironmentArchive::SerializeNative18(void* p) { SerializeSaveDataNative18At48B150(this, p); }
void ParamsRuntimeRoot::SerializeNative18(void* p) { SerializeNativeAt434940(p); }
void SecretCgNativeArchive::SerializeNative18(void* p) { SerializeSecretFlagsNative18(this, p); }
void SecretScArchive::SerializeNative18(void* p) { SerializeSecretFlagsNative18(this, p); }
void GlobalParamsArchive::SerializeNative18(void* p) { SerializeGlobalParamsNative18(this, p); }
void ClickInfoArchive::SerializeNative18(void* p) { SerializeClickInfoNative18At48AFB0(this, p); }
void PatternFilterNativeBody20h::SerializeNative18(void* p) { SerializePatternFilterNative18At48AFF0(this, p); }
void ScriptStackFrameNative::SerializeNative18(void* p) { SerializeScriptStackFrameNative18At4D5760(this, p); }
void SoundBasicValueNative::SerializeNative18(void* p) { SerializeSoundBasicValueNative18(this, p); }
void SoundChildValueNative::SerializeNative18(void* p) { SerializeSoundChildValueNative18(this, p); }
void SoundEffectValueNative::SerializeNative18(void* p) { SerializeSoundEffectValueNative18(this, p); }
void SoundKeyValueNative::SerializeNative18(void* p) { SerializeSoundKeyValueNative18(this, p); }
void SoundParamsBaseNative::SerializeNative18(void* p) { SerializeSoundParamsBaseNative18At48BB40(this, p); }
void EnvironmentSoundParamsArchive::SerializeNative18(void* p) { SerializeSoundParamsNative18At48B750(*this, p); }
void SoundTypeNative::SerializeNative18(void* p) { SerializeSoundTypeNative18(*this, p); }
void SoundCharacterNative::SerializeNative18(void* p) { SerializeSoundCharacterNative18(*this, p); }
void SoundTypeGroupNative::SerializeNative18(void* p) { SerializeSoundTypeGroupNative18(*this, p); }
void SoundCharacterGroupNative::SerializeNative18(void* p) { SerializeSoundCharacterGroupNative18(*this, p); }
}
