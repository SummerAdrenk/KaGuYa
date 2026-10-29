#include <Runtime/DirectMovieReferenceNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
class MovieControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit MovieControl(DirectMovieNativeBody38h* body)noexcept:EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00()override
    {
        auto* body=static_cast<DirectMovieNativeBody38h*>(Read(payloadAt14));
        if(body)Read(body->vptrAt00)->deleteAt28(body,1);
    }
    void DeleteControlAtVslot04(unsigned flags)override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(MovieControl)==0x18);
}
void DirectMovieView::Resize()const
{
    if(!native_){static_cast<DirectMovieRuntime*>(body_)->Resize();return;}
    DirectMovieNativeScope scope(services_);auto* body=static_cast<DirectMovieNativeBody38h*>(body_);
    Read(body->vptrAt00)->resizeAt24(body);
}
void ReplaceDirectMovieNativeReferenceAt4A0C81(YtlReferenceSlotRuntime& slot,DirectMovieNativeBody38h* created,IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    PublishDirectMovieNativeReferenceAt4A0CC5(slot,created,memory);
}
void PublishDirectMovieNativeReferenceAt4A0CC5(YtlReferenceSlotRuntime& slot,DirectMovieNativeBody38h* created,IEnvironmentRootMemory& memory)
{
    if(!created)return;
    if(auto* previous=Read(slot.controlAt08))previous->DeleteControlAtVslot04(1);
    slot.biasAt04=0;slot.baseAt00=reinterpret_cast<std::uintptr_t>(created);
    auto* raw=memory.AllocateBody(0x18);auto* control=raw?::new(raw)MovieControl(created):nullptr;
    slot.controlAt08=control;
    // 4A0CF0：先发布，随后直接写control+4；null申请保留原非法访问，不能伪成功。
    *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(control)+4)=1;
    Read(slot.controlAt08)->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->strideAt0C=0x38;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->countAt10=1;
}
void ReleaseDirectMovieNativeReference(YtlReferenceSlotRuntime& slot,DirectMovieNativeServices* services)
{
    if(!Read(slot.controlAt08))return;
    // 无绑定时也先按原Ref规则减计数/执行虚槽；不把未owning的Ref提前判失败。
    // 清空本链TLS，不能借外层其它Root的残留服务掩盖真实缺口。
    DirectMovieNativeScope scope(services);ReleaseYtlReference(slot);
}
DirectMovieView RootDirectMovieReference::Borrow()const noexcept
{
    if(native_)return DirectMovieView(ResolveEnvironmentObjectReference<DirectMovieNativeBody38h>(slot_),services_);
    return DirectMovieView(ResolveEnvironmentObjectReference<DirectMovieRuntime>(slot_));
}
bool RootDirectMovieReference::HasBase()const noexcept{return Read(slot_.baseAt00)!=0;}
DirectMovieRuntime* RootDirectMovieReference::ControlledBody()const
{
    if(native_&&Read(slot_.baseAt00))throw std::logic_error("Controlled movie accessor cannot consume a native Root movie");
    return native_?nullptr:ResolveEnvironmentObjectReference<DirectMovieRuntime>(slot_);
}
YtlReferenceSlotRuntime& RootDirectMovieReference::NativeSlot()
{if(!native_)throw std::logic_error("Native movie accessor cannot consume a controlled Root movie");return slot_;}
const YtlReferenceSlotRuntime& RootDirectMovieReference::NativeSlot()const
{if(!native_)throw std::logic_error("Native movie accessor cannot consume a controlled Root movie");return slot_;}
void RootDirectMovieReference::ReplaceNative(DirectMovieNativeBody38h* created,DirectMovieNativeServices& services,IEnvironmentRootMemory& memory)
{
    // 旧槽用其既有服务释放；新base发布前切换正文外服务，分配回调应见新绑定。
    // 没有完成标志回滚，也不能把control分配抛出后的孤儿base清掉。
    Release();
    if(!created)return;
    native_=true;services_=&services;
    DirectMovieNativeScope scope(services);PublishDirectMovieNativeReferenceAt4A0CC5(slot_,created,memory);
}
void RootDirectMovieReference::ReplaceControlled(std::unique_ptr<DirectMovieRuntime> created)
{
    Release();native_=false;services_=nullptr;
    // 历史受控域显式使用现代delete；正式native38不经过本入口。
    ReplaceEnvironmentObjectReference(slot_,created.release(),sizeof(DirectMovieRuntime));
}
void RootDirectMovieReference::Release()
{
    if(native_)ReleaseDirectMovieNativeReference(slot_,services_);else ReleaseYtlReference(slot_);
}
}
