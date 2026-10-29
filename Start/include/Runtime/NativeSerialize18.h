#pragma once
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/RuntimeBitVector.h>
#include <Runtime/FileStreamRuntime.h>
#include <span>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct NativeSerialize18Vtable31;
using NativeSerialize18ByteVector=EnvironmentVectorStorage<std::byte>;
// 18h真实正文；不内嵌服务、现代vector或SerializeBuffer。未写padding不做DMI。
struct NativeSerialize18Body final
{
    const NativeSerialize18Vtable31* vptrAt00;
    NativeSerialize18ByteVector bytesAt04;
    std::uint8_t writingAt10;
    std::uint8_t unwrittenAt11[3];
    std::uint32_t cursorAt14;
};
static_assert(sizeof(void*)==4&&sizeof(NativeSerialize18Body)==0x18);
static_assert(std::is_standard_layout_v<NativeSerialize18Body>&&std::is_trivially_destructible_v<NativeSerialize18Body>);
static_assert(offsetof(NativeSerialize18Body,bytesAt04)==4&&offsetof(NativeSerialize18Body,writingAt10)==0x10&&offsetof(NativeSerialize18Body,cursorAt14)==0x14);

// 接收者是原18h存储地址。void*允许冻结Confirm的相同原布局使用本表，不复制/假转现代对象。
using NativeSerialize18Argument=void*(__thiscall*)(void*,void*);
struct NativeISerializeVtable29 final
{
    std::byte*(__thiscall* resetAt00)(void*);
    std::uint8_t(__thiscall* modeAt04)(void*);
    std::uint8_t*(__thiscall* modeAddressAt08)(void*);
    std::uint8_t(__thiscall* setModeAt0C)(void*,std::uint8_t);
    std::uint8_t(__thiscall* writeModeOnlyAt10)(void*,std::uint8_t);
    NativeSerialize18ByteVector*(__thiscall* bufferAt14)(void*);
    NativeSerialize18Argument stateAt18,intVectorAt1C,longVectorAt20,wordVectorAt24,stringVectorAt28;
    NativeSerialize18Argument byteVectorAt2C,bitVectorAt30,unsignedVectorAt34;
    NativeSerialize18Argument intAt38,longAt3C,wordAt40,dwordAt44,shortAt48,stringAt4C,byteAt50,boolAt54,unsignedAt58;
    NativeSerialize18Argument archiveAt5C,copyAt60;
    std::int32_t(__thiscall* saveFileAt64)(void*,const char*);
    std::int32_t(__thiscall* loadFileAt68)(void*,const char*);
    std::uint32_t*(__thiscall* cursorAddressAt6C)(void*);
    void*(__thiscall* deletingDestructorAt70)(void*,std::uint32_t);
};
struct NativeSerialize18Vtable31 final
{
    NativeISerializeVtable29 common;
    std::int32_t(__thiscall* readStreamAt74)(void*,IFileStreamRuntime*,std::uint32_t);
    std::int32_t(__thiscall* importByteReferenceAt78)(void*,const YtlReferenceSlotRuntime* borrowed);
};
static_assert(sizeof(NativeISerializeVtable29)==0x74&&sizeof(NativeSerialize18Vtable31)==0x7C);
static_assert(offsetof(NativeISerializeVtable29,archiveAt5C)==0x5C&&offsetof(NativeISerializeVtable29,deletingDestructorAt70)==0x70);
static_assert(offsetof(NativeSerialize18Vtable31,readStreamAt74)==0x74&&offsetof(NativeSerialize18Vtable31,importByteReferenceAt78)==0x78);

IStartFileApi& DefaultNativeSerialize18Files() noexcept;
struct NativeSerialize18Services final
{
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IStartFileApi& files=DefaultNativeSerialize18Files();
    // 默认原CAtlFile事务槽为null；非null只属于调用者明确选择的文件扩展域。
    const FileTransactionContext* transaction=nullptr;
};
class NativeSerialize18Scope final
{
public:
    explicit NativeSerialize18Scope(NativeSerialize18Services&) noexcept;
    ~NativeSerialize18Scope() noexcept;
    NativeSerialize18Scope(const NativeSerialize18Scope&)=delete;
    NativeSerialize18Scope& operator=(const NativeSerialize18Scope&)=delete;
private:NativeSerialize18Services* previous_;
};
NativeSerialize18Services& RequireNativeSerialize18Services();
const NativeSerialize18Vtable31& NativeSerialize18Methods() noexcept;
const NativeISerializeVtable29& NativeISerializeMethods() noexcept;
NativeSerialize18Body* ConstructNativeSerialize18(void* storage18h) noexcept;
NativeSerialize18Body* ConstructNativeSerialize18InlineDword(void* storage18h) noexcept;
void DestroyNativeSerialize18At463E60(void* native18h);
// +5C只适用于真正接受native18的归档slot00，不允许传shared的Serialize(SerializeBuffer&)表。
void* SerializeNativeArchiveAt466380(void* native18h,void* nativeArchive);
void ResizeNativeSerialize18ByteVectorAt4570D0(void* vector12h,std::uint32_t count,const void* unusedCtrl);
void DestroyNativeSerialize18PrimitiveVector(void* vector12h,std::uint32_t width);

class EnvironmentSoundParamsArchive;
class SoundTypeNative;class SoundCharacterNative;class SoundTypeGroupNative;class SoundCharacterGroupNative;
void SerializeSoundTypeNative18(SoundTypeNative&,void* native18h);
void SerializeSoundCharacterNative18(SoundCharacterNative&,void* native18h);
void SerializeSoundTypeGroupNative18(SoundTypeGroupNative&,void* native18h);
void SerializeSoundCharacterGroupNative18(SoundCharacterGroupNative&,void* native18h);
// 具名原生叶：只操作既有44h正文/Ref图，绝不调用旧modern Serialize虚槽、换旧vptr或做codec往返。
void SerializeSoundParamsNative18At48B750(EnvironmentSoundParamsArchive&,void* native18h);
enum class NativeSerialize18ArchiveKind{SoundParams44,SoundType10,SoundCharacter18,SoundTypeGroup14,SoundCharacterGroup14};
// 宿主必须从真实持有者/原factory确定kind；不以VA、名字或现代vptr做猜测分派。
void SerializeKnownArchiveNative18(NativeSerialize18ArchiveKind,void* nativeArchive,void* native18h);
struct SettingDialogNativeConfirmSerialize18h;
void SerializeSoundParamsForSettingConfirmNative18(void* nativeSound44h,SettingDialogNativeConfirmSerialize18h&,
    std::uint32_t auditSite);

std::span<const std::uint32_t,29> NativeISerializeOriginalTableSample18() noexcept;
std::span<const std::uint32_t,31> NativeSerialize18OriginalTableSample18() noexcept;
struct NativeSerialize18EhEvidence final
{
    std::uint32_t functionVA,funcInfoVA;
    std::span<const std::uint32_t,9> funcInfo;
    std::span<const std::uint32_t> unwind,tryBlocks,handlers;
};
std::span<const NativeSerialize18EhEvidence> NativeSerialize18EhSample18() noexcept;
}
