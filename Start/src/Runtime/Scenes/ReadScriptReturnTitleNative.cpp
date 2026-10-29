#include <Runtime/ReadScriptReturnTitleNative.h>
#include <Runtime/ReadScriptSceneNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SystemInformationDisplayNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct RawUnwind final
{
    void* body;IEnvironmentRootMemory& memory;bool armed=true;
    ~RawUnwind()noexcept{if(armed)memory.DeleteBody(body,0x84);}
};
struct RefUnwind final
{
    YtlReferenceSlotRuntime& ref;bool armed=true;
    ~RefUnwind()noexcept{if(armed)ReleaseYtlReference(ref);}
    void Finish()noexcept{armed=false;ReleaseYtlReference(ref);}
};
void* Root(IReadScriptReturnTitleNativeSource& source,std::uint32_t ensure,std::uint32_t read)
{source.EnsureRootAt413BE0(ensure);return source.RootAt6198C4(read);}
}
bool RunReadScriptReturnTitleNative(ReadScriptSceneNativeBodyBCh& body,
    ReadScriptReturnTitleNativeServices& services,ReadScriptReturnTitleCaller caller)
{
    const bool text=caller==ReadScriptReturnTitleCaller::Text;
    auto& source=services.source;
    auto* env=source.CaptureEnvironmentAt4180C0(text?0x4B414E:0x4B2506);
    auto* saved=ResolveEnvironmentArchiveAt417990(*env,1);
    if(*(reinterpret_cast<const std::uint8_t*>(saved)+0x7D))
    {
        GeneralConfirmDialogNativeScope scope(services.confirmation);
        auto* raw=services.memory.AllocateBody(0x84);
        GuiDialogNativeBody5Ch* dialog=nullptr;
        {
            RawUnwind allocation{raw,services.memory}; // state2，不保护已完成正文的control分配。
            if(raw)dialog=ConstructGeneralConfirmDialogNativeAt4E3330(raw,1);
            allocation.armed=false;
        }
        YtlReferenceSlotRuntime ref;
        ConstructGeneralConfirmDialogNativeReferenceAt4515E0(ref,dialog,1);
        RefUnwind lifetime{ref}; // state3在4515E0正常完成后才成立。
        std::uint8_t oldWait=0;
        if(text)
        {
            auto* root=Root(source,0x4B41AA,0x4B41AF);
            oldWait=source.ReadWaitMarkAt418C10(root,0x4B41BE);
            root=Root(source,0x4B41CE,0x4B41D3);
            source.WriteWaitMarkAt418C10(root,0,0x4B41E2);
        }
        auto* root=Root(source,text?0x4B41F0:0x4B2562,text?0x4B41F5:0x4B2567);
        source.CaptureToolAt4C01F0(root,text?0x4B420B:0x4B257D);
        root=Root(source,text?0x4B4215:0x4B2587,text?0x4B421A:0x4B258C);
        source.SetToolCaptureModeAt4C2300(root,0,text?0x4B4232:0x4B25A4);
        root=Root(source,text?0x4B423C:0x4B25AE,text?0x4B4241:0x4B25B3);
        const auto layers=source.LayersAt4175A0(root,text?0x4B424F:0x4B25C1);
        // 参数Ref直接由4A8340交48CA30；caller没有另一个临时截图EH资格。
        union Argument{YtlReferenceSlotRuntime ref;Argument()noexcept{}~Argument()noexcept{}} argument;
        CompositeGraphicsFrameAt4A8340(argument.ref,services.surfaces,services.sizes,layers,0);
        auto* target=ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(ref);
        AssignGeneralConfirmDialogNativeCaptureAt48CA30(target,argument.ref);
        root=Root(source,text?0x4B426F:0x4B25E1,text?0x4B4274:0x4B25E6);
        source.SetToolCaptureModeAt4C2300(root,1,text?0x4B428C:0x4B25FE);
        root=Root(source,text?0x4B4296:0x4B2608,text?0x4B429B:0x4B260D);
        const bool accepted=RunNativeGuiModalReferenceAt5071D0(&ref,
            source.ModalAt4AD230(root,text?0x4B42A5:0x4B2617))==6;
        if(text)
        {
            root=Root(source,0x4B42BE,0x4B42C5);
            source.WriteWaitMarkAt418C10(root,oldWait,0x4B42D2);
        }
        root=Root(source,text?0x4B42E2:0x4B2630,text?0x4B42E7:0x4B2635);
        source.RestoreToolAt4BF4E0(root,text?0x4B42FD:0x4B264B);
        lifetime.Finish(); // 先撤state3再进入40D8A0，不重试抛出的正常尾。
        if(!accepted){body.substateAt0C=body.savedSubstateAt50;return false;}
    }
    env=source.CaptureEnvironmentAt4180C0(text?0x4B432F:0x4B267D);
    static_cast<void>(source.PostMessage(reinterpret_cast<HWND>(std::uintptr_t(env->windowAt2C)),
        WM_COMMAND,0x8015,0,text?0x4B4343:0x4B2691));
    env=source.CaptureEnvironmentAt4180C0(text?0x4B3F2E:0x4B227D);
    auto* unsaved=ResolveEnvironmentArchiveAt417990(*env,2);
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::byte*>(unsaved)+4)=0;
    return false;
}
}
