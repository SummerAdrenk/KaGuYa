#include "DirectMovieNative.Internal.h"
#include <dshow.h>

namespace KaGuYa::Start::Runtime
{
using namespace DirectMovieNativeDetail;
std::int32_t MessageDirectMovieNativeAt4BF3C0(Body& b,HWND,UINT message,WPARAM,LPARAM,LRESULT* result,unsigned handled)
{
    if(handled)return 0;
    if(message==0x8001)
    {
        const auto state=Read(b.stateAt2C);
        if(state==3||state==1)
        {
            long code;LONG_PTR p1,p2;
            while(Com<HRESULT,long*,LONG_PTR*,LONG_PTR*,long>(Read(b.eventAt1C),0x20,&code,&p1,&p2,0)>=0)
            {
                Com<HRESULT,long,LONG_PTR,LONG_PTR>(Read(b.eventAt1C),0x30,code,p1,p2);
                if(code==1)
                {
                    if(Read(b.loopAt28))
                    {
                        LONGLONG position=0;
                        Com<HRESULT,LONGLONG*,DWORD,LONGLONG*,DWORD>(Read(b.seekingAt0C),0x38,&position,1,nullptr,0);
                    }
                    else b.stateAt2C=0;
                }
                // 即使完成事件将 state 清零，原循环仍继续取剩余事件。
            }
        }
    }
    else if(message==WM_DESTROY)
    {
        if(auto* window=Read(b.windowAt20))
        {
            if(Read(b.stateAt2C)==1)
            {
                Read(b.vptrAt00)->pauseAt14(&b);window=Read(b.windowAt20);b.stateAt2C=0;
            }
            Com<HRESULT,long>(window,0x4C,0);
            Com<HRESULT,OAHWND>(Read(b.windowAt20),0x74,0);
        }
    }
    else return 0;
    *result=0;return 1;
}
}
