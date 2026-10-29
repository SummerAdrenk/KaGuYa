#include <Runtime/SaveLoadDialogNativeCommands.h>
#include <Runtime/SystemInformationDisplayNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/GeneralConfirmDialogNative.h>
#include <bit>
#include <new>
#include <optional>

namespace KaGuYa::Start::Runtime
{
namespace
{
class Control final:public EnvironmentObjectReferenceControl
{
public:
    explicit Control(GuiDialogNativeBody5Ch* body) noexcept:EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {if(auto* body=static_cast<GuiDialogNativeBody5Ch*>(payloadAt14))body->vptrAt00->deletingDestructorAt04(body,1);}
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(Control)==0x18);
YtlReferenceSlotRuntime Construct(GuiDialogNativeBody5Ch* body)
{
    YtlReferenceSlotRuntime out{};if(!body)return out;
    out.baseAt00=reinterpret_cast<std::uintptr_t>(body);auto* raw=::operator new(0x18);
    auto* control=raw?::new(raw)Control(body):nullptr;out.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    out.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C=0x84;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10=1;return out;
}
}
std::int32_t ReturnToTitleFromSaveLoadNativeAt51E320(ISaveLoadDialogNativeReturnTitleSource& source)
{
    std::optional<GeneralConfirmDialogNativeScope> nativeScope;
    if(auto* services=source.NativeGeneralConfirmationServices())nativeScope.emplace(*services);
    auto* env=source.CaptureEnvironmentAt4180C0();
    const auto end=reinterpret_cast<std::uintptr_t>(env->archivesAt30.end),begin=reinterpret_cast<std::uintptr_t>(env->archivesAt30.begin);
    const auto count=std::uint32_t(std::bit_cast<std::int32_t>(std::uint32_t(end-begin))/12);
    auto* registry=count>1?ResolveEnvironmentObjectReference<void>(env->archivesAt30.begin[1]):nullptr;
    if(*reinterpret_cast<const std::uint8_t*>(reinterpret_cast<std::uintptr_t>(registry)+0x7D))
    {
        auto* raw=::operator new(0x84);GuiDialogNativeBody5Ch* body=nullptr;
        {
            struct RawUnwind{void* raw;bool armed=true;~RawUnwind() noexcept{if(armed)::operator delete(raw,std::size_t{0x84});}} cleanup{raw};
            if(raw)body=source.ConstructGeneralConfirmationAt4E3330(raw,1);
            cleanup.armed=false;
        }
        auto target=Construct(body);
        struct Temporary
        {
            YtlReferenceSlotRuntime& slot;bool armed=true;
            ~Temporary() noexcept{if(armed)ReleaseYtlReference(slot);}
            void Finish() noexcept
            {
                armed=false;auto* captured=slot.controlAt08;
                if(captured&&--captured->referenceCountAt04==0)
                {if(captured->owningAt08)captured->DestroyPayloadAtVslot00();captured->DeleteControlAtVslot04(1);}
            }
        } lifetime{target};
        // 51E3D6..51E40E仍选Root并解Info；5071D0不消费此ECX，但原读取不能省略。
        (void)source.CaptureInformationAfter413BE0();
        const auto result=RunNativeGuiModalReferenceAt5071D0(&target,source.ModalServices());
        const bool accepted=result==6;lifetime.Finish();
        if(!accepted)return 0;
    }
    env=source.CaptureEnvironmentAt4180C0();
    (void)source.PostMessageA(reinterpret_cast<HWND>(env->windowAt2C),WM_COMMAND,0x8015,0);
    env=source.CaptureEnvironmentAt4180C0();
    auto* unsave=ResolveEnvironmentArchiveAt417990(*env,2);
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(unsave)+4u)=0;
    return 1;
}
}
