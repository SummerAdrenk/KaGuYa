#include <Runtime/IdleLoopNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
thread_local IWindowClassNativeApi* currentApi=nullptr;
struct Scope final
{
    IWindowClassNativeApi* previous;
    explicit Scope(IWindowClassNativeApi& api)noexcept:previous(currentApi){currentApi=&api;}
    ~Scope()noexcept{currentApi=previous;}
};
IWindowClassNativeApi& Api()noexcept{return currentApi?*currentApi:DefaultWindowClassNativeApi();}
__declspec(noreturn)void Bounds(IWindowClassNativeApi& api)
{
    api.RaiseException(EXCEPTION_ARRAY_BOUNDS_EXCEEDED,EXCEPTION_NONCONTINUABLE,0,nullptr);
    __assume(0); // 原43A2A0非继续异常叶没有合法返回路径。
}
void* At(const IdleLoopNativeArray0Ch& values,DWORD index)noexcept
{
    const auto address=reinterpret_cast<DWORD>(Read(values.valuesAt00))+index*4u;
    return Read(*reinterpret_cast<void* const*>(address));
}
BOOL __fastcall Filter(IdleLoopNativeBody38h* self,void*,MSG* message)
{return FilterIdleLoopNativeAt42D1D0(*self,message,Api());}
BOOL __fastcall Idle(IdleLoopNativeBody38h* self,void*,LONG unused)
{return DispatchIdleLoopNativeAt424E60(*self,unused,Api());}
}
const IdleLoopNativeMethods8h& IdleLoopNativeMethods()noexcept
{
    // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 void* 仅占 EDX、不读取其输入。
    // MSG*/LONG 仍各占 4 字节栈实参并由被调方清栈；BOOL 返回及可抛异常约定一致。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
    static const IdleLoopNativeMethods8h methods{
        reinterpret_cast<IdleLoopNativeMethods8h::Filter>(&Filter),reinterpret_cast<IdleLoopNativeMethods8h::Idle>(&Idle)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    return methods;
}
void InitializeIdleLoopNativeAt4342D1(IdleLoopNativeBody38h& body)noexcept
{
    body.idlesAt10.countAt04=0;body.filtersAt04={nullptr,0,0};
    body.idlesAt10.valuesAt00=nullptr;body.idlesAt10.capacityAt08=0;body.vptrAt00=&IdleLoopNativeMethods();
}
void RootIdleLoopNativeSource::EnsureRootAt413BE0(){root_.EnsureCreatedAt413BE0();}
void* RootIdleLoopNativeSource::RootAt6198C4(){return root_.ReadPublishedAt6198C4();}
DWORD RootIdleLoopNativeSource::ExitFlagAt1BC(void* root)
{
    // 原43E9AF直接读已选root，不追加现代RequirePreWindowInitialization或第二次取根。
    return Read(static_cast<RootRuntimePayload*>(root)->Frame().Lifecycle().valueAt1BC);
}
BOOL FilterIdleLoopNativeAt42D1D0(IdleLoopNativeBody38h& body,MSG* message,IWindowClassNativeApi& api)
{
    auto index=Read(body.filtersAt04.countAt04)-1u;
    while(std::bit_cast<LONG>(index)>=0)
    {
        if(std::bit_cast<LONG>(index)>=std::bit_cast<LONG>(Read(body.filtersAt04.countAt04)))Bounds(api);
        if(auto* filter=At(body.filtersAt04,index))
        {
            using Method=BOOL(__thiscall*)(void*,MSG*);
            auto* table=Read(*static_cast<Method* const*>(filter));
            if(Read(table[0])(filter,message))return TRUE;
        }
        --index;
    }
    return FALSE;
}
BOOL DispatchIdleLoopNativeAt424E60(IdleLoopNativeBody38h& body,LONG,IWindowClassNativeApi& api)
{
    DWORD index=0;
    while(std::bit_cast<LONG>(index)<std::bit_cast<LONG>(Read(body.idlesAt10.countAt04)))
    {
        if(std::bit_cast<LONG>(index)<0||std::bit_cast<LONG>(index)>=std::bit_cast<LONG>(Read(body.idlesAt10.countAt04)))Bounds(api);
        if(auto* idle=At(body.idlesAt10,index))
        {
            using Method=BOOL(__thiscall*)(void*);
            auto* table=Read(*static_cast<Method* const*>(idle));static_cast<void>(Read(table[0])(idle));
        }
        ++index;
    }
    return FALSE;
}
int RunIdleLoopNativeAt43E980(IdleLoopNativeBody38h& body,HWND,IIdleLoopNativeRootSource& root,
    IModalDialogMessageApi& messages,IWindowClassNativeApi& api)
{
    Scope metadata(api);auto* const message=&body.messageAt1C;
    for(;;)
    {
        if(!messages.Peek(*message))
        {
            root.EnsureRootAt413BE0();auto* selected=root.RootAt6198C4();
            if(root.ExitFlagAt1BC(selected))break;
            auto* table=Read(body.vptrAt00);static_cast<void>(Read(table->idleAt04)(&body,0));
            continue;
        }
        const auto result=messages.Get(*message);
        if(result==-1)continue;if(!result)break;
        auto* table=Read(body.vptrAt00);
        if(!Read(table->filterAt00)(&body,message)){messages.Translate(*message);messages.Dispatch(*message);}
    }
    return std::bit_cast<int>(Read(body.messageAt1C.wParam));
}
void ReleaseIdleLoopArraysNativeAt4346DA(IdleLoopNativeBody38h& body,IWindowClassNativeApi& api)
{
    if(auto* values=Read(body.idlesAt10.valuesAt00)){api.Free(values);body.idlesAt10.valuesAt00=nullptr;}
    auto* filters=Read(body.filtersAt04.valuesAt00);
    body.idlesAt10.countAt04=0;body.idlesAt10.capacityAt08=0;
    if(filters)api.Free(filters);
}
void ReleaseIdleLoopArraysNativeAt40E940(IdleLoopNativeBody38h& body,IWindowClassNativeApi& api)
{
    if(auto* values=Read(body.idlesAt10.valuesAt00)){api.Free(values);body.idlesAt10.valuesAt00=nullptr;}
    body.idlesAt10.countAt04=0;body.idlesAt10.capacityAt08=0;
    if(auto* values=Read(body.filtersAt04.valuesAt00)){api.Free(values);body.filtersAt04.valuesAt00=nullptr;}
    body.filtersAt04.capacityAt08=0;body.filtersAt04.countAt04=0;
}
}
