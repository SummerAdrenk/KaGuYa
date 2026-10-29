#pragma once
#include <Runtime/SettingDialogNative.h>
#include <Runtime/GeneralConfirmDialogNative.h>
#include <Runtime/NameRegistrationDialogNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 538BB0局部CSerialize真实18h。仅建存储，不提前写+11..13原未写padding。
struct SettingDialogNativeConfirmSerialize18h final
{
    const void* vptrAt00;
    std::byte* beginAt04;
    std::byte* endAt08;
    std::byte* capacityAt0C;
    std::uint8_t writingAt10;
    std::uint8_t unwrittenAt11[3];
    std::uint32_t cursorAt14;
};
static_assert(sizeof(SettingDialogNativeConfirmSerialize18h)==0x18);
static_assert(offsetof(SettingDialogNativeConfirmSerialize18h,writingAt10)==0x10);
static_assert(offsetof(SettingDialogNativeConfirmSerialize18h,cursorAt14)==0x14);

class ISettingDialogNativeConfirmSource
{
public:
    virtual ~ISettingDialogNativeConfirmSource() = default;
    // site仅审计标签，不是原业务实参/可执行VA。getter只借当前真实singleton，禁止成功空替身。
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t site) = 0;
    virtual ParamsRuntimeRoot* CaptureParamsAt418480(std::uint32_t site) = 0;
    virtual void EnsureRootAt413BE0(std::uint32_t site) = 0;
    virtual void* RootAt6198C4(std::uint32_t site) = 0;
    virtual YtlReferenceSlotRuntime& RootSystemSlotAt3C(void* fixedRoot) = 0;
    virtual BOOL PostMessageA(HWND,UINT,WPARAM,LPARAM) = 0;

    // 原生归档/ISerialize边界由NativeSerialize18提供，不是SerializeBuffer适配器。
    // getter须无副作用地借新native表，绝不能返回原PE表或现代对象的表。
    virtual const void* SerializeMethodsAt5B22F4() = 0;
    virtual const void* ISerializeMethods() = 0;
    // 仅原归档slot00这一叶；必须使用传入18h buffer/mode/cursor和固定归档receiver。
    // 未提供真正原生实现须明确失败，不得吞异常/填默认声值或委托整段538BB0。
    virtual void SerializeArchiveAtVslot00(void* nativeArchive,SettingDialogNativeConfirmSerialize18h&,
        std::uint32_t site) = 0;
};
// 只允许既有原生构造接口，返回该storage的真实正文或抛出原异常，不返现代对象/空成功替身。
// factory、发布、Ref资格和Modal流程全部在本集，不交给Source。
class ISettingDialogNativeConfirmConstructors
{
public:
    virtual ~ISettingDialogNativeConfirmConstructors() = default;
    virtual GuiDialogNativeBody5Ch* GeneralAt4E3330(void* raw84h,std::int32_t mode) = 0;
    virtual NameRegistrationDialogNativeBody94h* NameAt4E4340(void* raw94h) = 0;
};
class SettingDialogNativeConfirmConstructors final : public ISettingDialogNativeConfirmConstructors
{
public:
    GuiDialogNativeBody5Ch* GeneralAt4E3330(void*,std::int32_t) override;
    NameRegistrationDialogNativeBody94h* NameAt4E4340(void*) override;
};
struct SettingDialogNativeConfirmServices final
{
    ISettingDialogNativeConfirmSource& source;
    ISettingDialogNativeConfirmConstructors& constructors;
    GeneralConfirmDialogNativeServices& general;
    NameRegistrationDialogNativeServices& names;
    IModalDialogRunnerServices& modal;
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
};
// 长寿作用域只借服务，不在AC/84/94/control添字段。必须覆盖Modal、EH及最后逃逸拥有Ref。
class SettingDialogNativeConfirmScope final
{
public:
    explicit SettingDialogNativeConfirmScope(SettingDialogNativeConfirmServices&) noexcept;
    ~SettingDialogNativeConfirmScope() noexcept;
    SettingDialogNativeConfirmScope(const SettingDialogNativeConfirmScope&)=delete;
    SettingDialogNativeConfirmScope& operator=(const SettingDialogNativeConfirmScope&)=delete;
private:
    SettingDialogNativeConfirmServices* previous_;
    GeneralConfirmDialogNativeScope general_;
    NameRegistrationDialogNativeScope names_;
};
SettingDialogNativeConfirmServices& RequireSettingDialogNativeConfirmServices();
// 51E4B0原不消费ECX，无业务栈参数；其余两项原ECX=已有AC，正常均返回0。
std::int32_t PollSettingDialogModeNativeAt51E4B0();
std::int32_t PollSettingDialogDefaultsNativeAt51B730(SettingDialogNativeBodyACh&);
std::int32_t PollSettingDialogNameNativeAt521AD0(SettingDialogNativeBodyACh&);
void ResetSettingDialogDefaultsNativeAt538BB0(SettingDialogNativeBodyACh&);
// 仅factory新fresh槽，非Replace已有Ref。control失败不补删已完成正文。
YtlReferenceSlotRuntime* CreateSettingDialogGeneralConfirmNativeReference(YtlReferenceSlotRuntime&,std::int32_t);
YtlReferenceSlotRuntime* CreateSettingDialogNameConfirmNativeReference(YtlReferenceSlotRuntime&);

struct SettingDialogNativeConfirmEhEvidence final
{
    std::uint32_t functionVA,funcInfoVA;
    std::span<const std::uint32_t,9> funcInfo;
    std::span<const std::uint32_t> unwind,tryBlockMap,handlerTypes;
};
std::span<const SettingDialogNativeConfirmEhEvidence> SettingDialogNativeConfirmEhSample18() noexcept;
enum class SettingDialogNativeConfirmWriteKind{Byte,Word,Dword,And,Or};
struct SettingDialogNativeConfirmDefaultWrite final
{
    std::uint32_t captureSite,archiveIndex,offset,value;
    SettingDialogNativeConfirmWriteKind kind;
};
// 相同captureSite的连续项使用同一固定archive，其他项必须独立捕获。
std::span<const SettingDialogNativeConfirmDefaultWrite> SettingDialogNativeConfirmDefaultPrefixSample18() noexcept;
std::span<const SettingDialogNativeConfirmDefaultWrite> SettingDialogNativeConfirmDefaultTailSample18() noexcept;
}
