#pragma once
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/ModalDialogRunnerRuntime.h>
#include <cstddef>

namespace KaGuYa::Start::Runtime
{
struct SystemInformationDisplayNativeBody14h;
struct SystemDialogNativeServices;
struct SystemInformationDisplayNativeVtable1 final
{
    SystemInformationDisplayNativeBody14h* (__thiscall* deleteAt00)(SystemInformationDisplayNativeBody14h*,std::uint32_t);
};
// 4DC4C0/5C0A98：一个删除槽，04为原88h SystemDialog Ref，10为独立DWORD。
struct SystemInformationDisplayNativeBody14h final
{
    const SystemInformationDisplayNativeVtable1* vptrAt00;
    union { YtlReferenceSlotRuntime dialogAt04; };
    std::uint32_t suppressedAt10;
    SystemInformationDisplayNativeBody14h() noexcept {}
    ~SystemInformationDisplayNativeBody14h() noexcept {}
};
static_assert(sizeof(SystemInformationDisplayNativeBody14h)==0x14);
static_assert(sizeof(SystemInformationDisplayNativeVtable1)==4);
static_assert(offsetof(SystemInformationDisplayNativeBody14h,dialogAt04)==4);
static_assert(offsetof(SystemInformationDisplayNativeBody14h,suppressedAt10)==0x10);
class ISystemInformationDisplayNativeSource
{
public:
    virtual ~ISystemInformationDisplayNativeSource()=default;
    // 未完成的88h子正文是明确依赖，不允许返回现代SystemMenuDialog/shared_ptr。
    virtual GuiDialogNativeBody5Ch* ConstructSystemDialogAt4F41A0(void* storage88h)=0;
    virtual std::uint32_t ReadRootModalGateAfter413BE0()=0;
    virtual std::int32_t TriggerSystemDialogAt5104A0(GuiDialogNativeBody5Ch*)=0;
    // 仅借稳定外部服务；getter不Capture，不替代任何模态业务步骤。
    virtual IModalDialogRunnerServices& ModalServices()=0;
};
struct SystemInformationDisplayNativeServices final
{
    ISystemInformationDisplayNativeSource& source;
    IEnvironmentRootMemory& memory;
    // 仅声明同源调用期元数据，不进入14h正文或Ref；原88h服务须覆盖最终逃逸Ref。
    SystemDialogNativeServices* dialog=nullptr;
};
const SystemInformationDisplayNativeVtable1& SystemInformationDisplayNativeMethods() noexcept;
SystemInformationDisplayNativeBody14h* ConstructSystemInformationDisplayNativeAt4DC4C0(
    SystemInformationDisplayNativeBody14h&,SystemInformationDisplayNativeServices&);
void DestroySystemInformationDisplayNativeAt4F74E0(SystemInformationDisplayNativeBody14h&) noexcept;
SystemInformationDisplayNativeBody14h* DeleteSystemInformationDisplayNativeAt4FC080(
    SystemInformationDisplayNativeBody14h*,std::uint32_t) noexcept;
std::int32_t QuerySystemInformationDisplayNativeAt5105C0(SystemInformationDisplayNativeBody14h&,
    SystemInformationDisplayNativeServices&);
std::int32_t RunSystemInformationDisplayNativeAt5075B0(SystemInformationDisplayNativeBody14h&,
    SystemInformationDisplayNativeServices&);
// 5071D0唯一显式参数是可重解的Ref地址；不消费ECX，不复制或增持目标。
std::int32_t RunNativeGuiModalReferenceAt5071D0(const YtlReferenceSlotRuntime*,IModalDialogRunnerServices&);
}
