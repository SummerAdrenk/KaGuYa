#include <Runtime/InputNativeObjects.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value) noexcept{return static_cast<const volatile T&>(value);}
template<class Body>Body* Resolve(const YtlReferenceSlotRuntime& slot) noexcept
{
    const auto base=Read(slot.baseAt00);
    if(!base)return nullptr;
    const auto* control=Read(slot.controlAt08);
    const auto stride=control?Read(static_cast<const EnvironmentObjectReferenceControl*>(control)->strideAt0C):0u;
    return reinterpret_cast<Body*>(base+Read(slot.biasAt04)*stride);
}
void UpdateKeyboard(KeyboardManagerNativeBody438h& keyboard)
{
    if(!Read(keyboard.rawAt04.baseAt00))return;
    keyboard.bufferAt10=Read(keyboard.bufferAt10)==0?1:0;
    for(std::int32_t index=0;index<Read(keyboard.actionCountAt434);++index)
    {
        auto& action=keyboard.actionsAt14[index];
        auto* cursor=Read(action.begin);
        while(cursor!=Read(action.end))
        {
            // 450DA0每项重解raw；450DB4在virtual查询回调之后才读当前buffer。
            auto* raw=Resolve<KeyboardRawNativeBody104h>(keyboard.rawAt04);
            const auto key=Read(cursor->key);
            using Query=std::uint8_t(__thiscall*)(void*,std::uint32_t);
            Query query;std::memcpy(&query,reinterpret_cast<const std::byte*>(raw->vptrAt00)+4,4);
            const auto value=query(raw,key);
            const auto address=reinterpret_cast<std::uintptr_t>(cursor)+Read(keyboard.bufferAt10)+1u;
            *reinterpret_cast<std::uint8_t*>(address)=value;
            ++cursor;
        }
    }
}
}
void UpdateInputNativeAt450AA0(InputRootNativeBody24h& input,IInputFrameNativeSource& source,
    const Platform::IMousePositionApi& position,InputNativeServices& services)
{
    InputNativeScope scope(services);
    if(Resolve<MouseManagerNativeBody3Ch>(input.mouseAt00))
    {
        // 原首轮mouse只判资格，Source回调后450BDF再次解析同一input的当前mouse Ref。
        const auto window=source.ReadWindowAt450ADB();
        const auto width=source.ReadWidthAt450AE6();
        const auto height=source.ReadHeightAt450AF1();
        const auto mode=source.ReadModeAt450B01();
        std::int32_t x=0,y=0,viewportWidth,viewportHeight;
        if(mode)
        {
            const auto* offsets=source.ReadViewportAt450B3F();x=offsets[0];y=offsets[1];
            const auto* dimensions=source.ReadViewportAt450B7B();viewportWidth=dimensions[2];viewportHeight=dimensions[3];
        }
        else
        {viewportWidth=source.ReadWidthAt450AE6();viewportHeight=source.ReadHeightAt450AF1();}
        auto* mouse=Resolve<MouseManagerNativeBody3Ch>(input.mouseAt00);
        const auto suppression=Read(mouse->suppressionAt28);
        if(suppression)mouse->suppressionAt28=suppression-1u;
        const auto secondary=Read(mouse->secondaryAt2C);
        if(secondary)mouse->secondaryAt2C=secondary-1u;
        if(Read(mouse->rawAt04.baseAt00))
        {
            mouse->previousAt30=mouse->pressedAt10;
            const auto previousX=Read(mouse->logicalAt18.x),previousY=Read(mouse->logicalAt18.y);
            auto* raw=Resolve<MouseRawNativeBody10h>(mouse->rawAt04);
            POINT point;position.ReadScreenPosition(point);
            std::array<std::uint8_t,3> pressed,latched;
            std::memcpy(pressed.data(),raw->pressedAt04,3);std::memcpy(latched.data(),raw->latchedAt07,3);
            const auto wheel=static_cast<std::int32_t>(Read(raw->wheelAt0A));
            // 450C82的DWORD同时消费右/中锁存与wheel；左锁存单独清零。
            std::memset(raw->latchedAt07+1,0,4);raw->latchedAt07[0]=0;
            if(Read(mouse->unwrittenAt38[0])){pressed[0]=0;latched[0]=0;}
            mouse->pressedAt10=pressed;mouse->latchedAt13=latched;mouse->wheelAt34=wheel;
            mouse->logicalAt18=point;position.ConvertToClient(window,mouse->logicalAt18);
            mouse->clientAt20=mouse->logicalAt18;
            if(mode)
            {
                mouse->logicalAt18.x=std::bit_cast<LONG>(std::uint32_t(Read(mouse->logicalAt18.x))-std::uint32_t(x));
                const auto productX=std::bit_cast<std::int32_t>(std::uint32_t(Read(mouse->logicalAt18.x))*std::uint32_t(width));
                mouse->logicalAt18.y=std::bit_cast<LONG>(std::uint32_t(Read(mouse->logicalAt18.y))-std::uint32_t(y));
                mouse->logicalAt18.x=productX/viewportWidth;
                const auto productY=std::bit_cast<std::int32_t>(std::uint32_t(Read(mouse->logicalAt18.y))*std::uint32_t(height));
                mouse->logicalAt18.y=productY/viewportHeight;
            }
            if(position.IsWindowMinimized(window)){mouse->logicalAt18.y=-1;mouse->logicalAt18.x=-1;}
            mouse->movedAt33=Read(mouse->logicalAt18.x)!=previousX||Read(mouse->logicalAt18.y)!=previousY;
            mouse->unwrittenAt38[0]=0;
        }
    }
    if(Resolve<KeyboardManagerNativeBody438h>(input.keyboardAt18))
        UpdateKeyboard(*Resolve<KeyboardManagerNativeBody438h>(input.keyboardAt18));
}
}
