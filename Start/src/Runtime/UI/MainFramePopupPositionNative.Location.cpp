#include <Runtime/MainFramePopupPositionNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const void* p,DWORD offset=0)noexcept
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<DWORD>(p)+offset);}
void* Resolve(const void* ref)noexcept
{
    const auto base=Read<DWORD>(ref);if(!base)return nullptr;
    const auto* control=Read<const void*>(ref,8);const auto stride=control?Read<DWORD>(control,0x0C):0u;
    return reinterpret_cast<void*>(base+Read<DWORD>(ref,4)*stride);
}
DWORD Divide(DWORD value,DWORD denominator)
{
    DWORD result;
    __asm
    {
        mov eax,value
        xor edx,edx
        div denominator
        mov result,eax
    }
    return result;
}
LONG Add(LONG a,DWORD b)noexcept{return std::bit_cast<LONG>(static_cast<DWORD>(a)+b);}
LONG Sub(LONG a,DWORD b)noexcept{return std::bit_cast<LONG>(static_cast<DWORD>(a)-b);}
DWORD Half(DWORD value)noexcept{return static_cast<DWORD>(std::bit_cast<std::int32_t>(value)/2);}
void Commit(EnvironmentRootStorage* environment,POINT& point,DWORD* x,DWORD* y,IMainFramePopupPositionNativeApi& api)noexcept
{
    // 原最终state9没有撤销，ClientToScreen或提交后的C++异常必须终止；不重试解锁。
    static_cast<void>(api.ClientToScreen(Read<HWND>(environment,0x2C),&point));
    *x=static_cast<DWORD>(point.x);*y=static_cast<DWORD>(point.y);
}
}
void LocateMainFramePopupNativeAt46B640(void* entry,DWORD* x,DWORD* y,IMainFramePopupPositionNativeSource& source,
    IMainFramePopupPositionNativeApi& api)
{
    auto& state=source.PositionAt208(entry);
    if(Read<std::int32_t>(&state)<0||!Read<DWORD>(&state,4)||!Read<DWORD>(&state,8))return;
    auto* game=source.GameSystemAt418480();const auto end=Read<DWORD>(game,0xA4),begin=Read<DWORD>(game,0xA0);
    if(Read<std::int32_t>(&state)>=std::bit_cast<std::int32_t>(end-begin)/12)return;
    game=source.GameSystemAt418480();const auto indexY=Read<DWORD>(&state);
    const auto* yEntry=reinterpret_cast<const void*>(Read<DWORD>(game,0xA0)+indexY*12u);
    game=source.GameSystemAt418480();const auto indexX=Read<DWORD>(&state);
    const auto* xEntry=reinterpret_cast<const void*>(Read<DWORD>(game,0xA0)+indexX*12u);
    source.EnsureRootAt413BE0();auto* mode=Resolve(&source.ModeAt0C(source.RootAt6198C4()));
    const auto logicalX=Read<DWORD>(xEntry,4),logicalY=Read<DWORD>(yEntry,8);
    POINT point{std::bit_cast<LONG>(logicalX),std::bit_cast<LONG>(logicalY)};
    if(Read<BYTE>(mode,4))
    {
        auto* environment=source.EnvironmentAt4180C0();const auto divisorX=Read<DWORD>(environment,0x18);
        const auto width=Read<DWORD>(mode,0x14);point.x=std::bit_cast<LONG>(Divide(width*logicalX,divisorX));
        environment=source.EnvironmentAt4180C0();const auto divisorY=Read<DWORD>(environment,0x1C);
        const auto height=Read<DWORD>(mode,0x18),product=height*logicalY;
        const auto offsetY=Read<DWORD>(mode,0x10),offsetX=Read<DWORD>(mode,0x0C);
        point.x=Add(point.x,offsetX);point.y=std::bit_cast<LONG>(Divide(product,divisorY)+offsetY);
    }
    game=source.GameSystemAt418480();const auto index=Read<DWORD>(&state);
    const auto* item=reinterpret_cast<const void*>(Read<DWORD>(game,0xA0)+index*12u);
    switch(Read<DWORD>(item))
    {
    case 1:point.x=Sub(point.x,Read<DWORD>(&state,4));break;
    case 2:point.y=Sub(point.y,Read<DWORD>(&state,8));break;
    case 3:point.x=Sub(point.x,Read<DWORD>(&state,4));point.y=Sub(point.y,Read<DWORD>(&state,8));break;
    case 4:point.x=Sub(point.x,Half(Read<DWORD>(&state,4)));break;
    case 5:point.x=Sub(point.x,Half(Read<DWORD>(&state,4)));point.y=Sub(point.y,Read<DWORD>(&state,8));break;
    case 6:point.y=Sub(point.y,Half(Read<DWORD>(&state,8)));break;
    case 7:point.x=Sub(point.x,Read<DWORD>(&state,4));point.y=Sub(point.y,Half(Read<DWORD>(&state,8)));break;
    default:break;
    }
    // Capture自身的构造失败仍可外传；正常Leave后的提交才进入上面的state9终止域。
    auto* environment=source.EnvironmentAt4180C0();Commit(environment,point,x,y,api);
}
}
