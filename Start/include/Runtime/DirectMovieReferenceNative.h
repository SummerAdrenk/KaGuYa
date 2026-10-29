#pragma once
#include <Runtime/DirectMovieNative.h>
#include <Runtime/EnvironmentObjectReference.h>

namespace KaGuYa::Start::Runtime
{
// 显式选择域，不检查正文虚表猜测类型，也不在失败后换回现代路径。
class DirectMovieView final
{
public:
    explicit DirectMovieView(DirectMovieRuntime* controlled)noexcept:body_(controlled),native_(false){}
    DirectMovieView(DirectMovieNativeBody38h* native,DirectMovieNativeServices* services)noexcept:body_(native),services_(services),native_(true){}
    bool IsNative()const noexcept{return native_;}
    explicit operator bool()const noexcept{return body_!=nullptr;}
    const void* Address()const noexcept{return body_;}
    DirectMovieNativeBody38h* NativeBody()const noexcept{return native_?static_cast<DirectMovieNativeBody38h*>(body_):nullptr;}
    DirectMovieRuntime* ControlledBody()const noexcept{return native_?nullptr:static_cast<DirectMovieRuntime*>(body_);}
    void Resize()const;
private:
    void* body_;
    DirectMovieNativeServices* services_=nullptr;
    bool native_;
};

void ReplaceDirectMovieNativeReferenceAt4A0C81(YtlReferenceSlotRuntime&,DirectMovieNativeBody38h*,IEnvironmentRootMemory&);
void PublishDirectMovieNativeReferenceAt4A0CC5(YtlReferenceSlotRuntime&,DirectMovieNativeBody38h*,IEnvironmentRootMemory&);
void ReleaseDirectMovieNativeReference(YtlReferenceSlotRuntime&,DirectMovieNativeServices*);

// Root 的唯一 Ref12 与正文外服务。正常析构由4642F3/PostWindow固定顺序显式调用，
// 本类析构不补释放；没有第二个unique_ptr或重复的published指针。
class RootDirectMovieReference final
{
public:
    RootDirectMovieReference()noexcept=default;
    RootDirectMovieReference(const RootDirectMovieReference&)=delete;
    RootDirectMovieReference& operator=(const RootDirectMovieReference&)=delete;
    DirectMovieView Borrow()const noexcept;
    bool HasBase()const noexcept;
    DirectMovieRuntime* ControlledBody()const;
    YtlReferenceSlotRuntime& NativeSlot();
    const YtlReferenceSlotRuntime& NativeSlot()const;
    void ReplaceNative(DirectMovieNativeBody38h*,DirectMovieNativeServices&,IEnvironmentRootMemory&);
    void ReplaceControlled(std::unique_ptr<DirectMovieRuntime>);
    void Release();
    // 仅在Root及逃逸引用已退役后解除借用，不补释放或改变引用/业务域。
    void ClearNativeServices()noexcept{services_=nullptr;}
private:
    YtlReferenceSlotRuntime slot_{};
    DirectMovieNativeServices* services_=nullptr;
    bool native_=true;
};
}
