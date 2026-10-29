#include <Runtime/ReadScriptSaveCaptureNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/DrawManagerNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
void* Resolve(const YtlReferenceSlotRuntime& slot,std::uintptr_t base)noexcept
{
    if(!base)return nullptr;
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    return reinterpret_cast<void*>(base+Read(slot.biasAt04)*stride);
}
struct LocalRef final
{
    YtlReferenceSlotRuntime& ref;bool armed=true;
    ~LocalRef()noexcept{if(armed)ReleaseYtlReference(ref);}
    void Finish()noexcept{armed=false;ReleaseYtlReference(ref);} // state1是原541E55终止域。
};
}
void PrepareGuiChildrenForSaveNativeAt5325A0(GuiDialogNativeBody5Ch* dialog)
{
    if(!Read(dialog->vptrAt00)->readEnabledAt28(dialog))return;
    auto* position=Read(dialog->childrenAt0C.begin);
    while(position!=Read(dialog->childrenAt0C.end))
    {
        const auto base=Read(position->baseAt00);
        auto* child=static_cast<GuiControlNativeBody50h*>(Resolve(*position,base));
        Read(child->vptrAt00)->resetAt08(child);++position;
    }
}
void CaptureBeforeSaveNativeAt4B01A0(IReadScriptSaveCaptureNativeSource& source)
{
    source.EnsureRootAt413BE0(0x4B01CB);auto* root=source.RootAt6198C4(0x4B01D0);
    auto* draw=source.DrawAt4B01D6(root);auto* begin=Read(draw->ownedLayersAt14.begin);
    const auto end=Read(draw->ownedLayersAt14.end);
    const auto count=std::uint32_t(std::bit_cast<std::int32_t>(std::uint32_t(
        reinterpret_cast<std::uintptr_t>(end)-reinterpret_cast<std::uintptr_t>(begin)))/12);
    // 原4B0212是jnb：count==2仍按begin[2]解址，不加现代安全钳制。
    ToolWindowNativeBody40h* tool=nullptr;
    if(count>=2)tool=static_cast<ToolWindowNativeBody40h*>(Resolve(begin[2],Read(begin[2].baseAt00)));
    union Selected{YtlReferenceSlotRuntime ref;Selected()noexcept{}~Selected()noexcept{}} selected;
    auto* returned=source.SelectToolAt417B50(tool,selected.ref);
    const auto base=Read(returned->baseAt00);
    LocalRef lifetime{selected.ref}; // 417B50正常返回后才有state0资格。
    PrepareGuiChildrenForSaveNativeAt5325A0(static_cast<GuiDialogNativeBody5Ch*>(Resolve(*returned,base)));
    lifetime.Finish();
    source.EnsureRootAt413BE0(0x4B02B4);root=source.RootAt6198C4(0x4B02B9);
    source.CapturePreviewAt48DBE0(root);
}
}
