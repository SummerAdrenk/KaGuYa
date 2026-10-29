#include <Runtime/EditWindowNativeMaps.h>
#include <Runtime/YtlReferenceArrayRuntime.h>
#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RectControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit RectControl(RECT* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {if(payloadAt14)::operator delete(payloadAt14,std::size_t{0x10});}
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(RectControl)==0x18);
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
std::int32_t Sub(std::int32_t a,std::int32_t b) noexcept{return std::bit_cast<std::int32_t>(std::uint32_t(a)-std::uint32_t(b));}
}
void ReplaceEditWindowNativeMarginsAt469F70(YtlReferenceSlotRuntime& slot)
{
    ReleaseYtlReference(slot);
    void* raw=::operator new(0x10);
    auto* rectangle=raw?::new(raw)RECT{}:nullptr;
    // 469FDC重入后control只调用删除槽，不再减持或重读本次EDI rectangle。
    if(slot.controlAt08)slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04=0;slot.baseAt00=reinterpret_cast<std::uintptr_t>(rectangle);
    void* storage=::operator new(0x18);
    auto* control=storage?::new(storage)RectControl(rectangle):nullptr;
    slot.controlAt08=control;
    control->referenceCountAt04=1;slot.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C=0x10;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10=1;
}
void ApplyEditWindowNativeLayoutAt4880F0(EditWindowNativeBody98h& body,const RECT* target,
    const RECT* margins,IEditWindowNativeMapApi& api)
{
    RECT current{};
    if(!api.GetWindowRect(Read(body.outerAt00.windowAt04),&current)||api.EqualRect(&current,target))return;
    if(!margins)ReleaseYtlReference(body.layoutAt6C);
    else
    {
        ReplaceEditWindowNativeMarginsAt469F70(body.layoutAt6C);
        // 488181..488218每个DWORD都重新解析当前Ref，赋值次序是left/right/top/bottom。
        auto* selected=ResolveEnvironmentObjectReference<RECT>(body.layoutAt6C);const auto left=Read(margins->left);selected->left=left;
        selected=ResolveEnvironmentObjectReference<RECT>(body.layoutAt6C);const auto right=Read(margins->right);selected->right=right;
        selected=ResolveEnvironmentObjectReference<RECT>(body.layoutAt6C);const auto top=Read(margins->top);selected->top=top;
        selected=ResolveEnvironmentObjectReference<RECT>(body.layoutAt6C);const auto bottom=Read(margins->bottom);selected->bottom=bottom;
    }
    const auto y=Read(target->top);const auto height=Sub(Read(target->bottom),y);
    const auto x=Read(target->left);const auto width=Sub(Read(target->right),x);
    static_cast<void>(api.MoveWindow(Read(body.outerAt00.windowAt04),x,y,width,height,1));
}
}
