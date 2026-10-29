#include <Runtime/ReadScriptInputNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScriptArithmeticCommandNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
KeyboardManagerNativeBody438h* Keyboard(InputRootOwner& input)
{return ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input.CaptureAt418210()->keyboardAt18);}
std::uint8_t Byte(IEnvironmentArchiveRuntime* body,std::uint32_t offset)
{return *reinterpret_cast<const volatile std::uint8_t*>(reinterpret_cast<std::uintptr_t>(body)+offset);}
IEnvironmentArchiveRuntime* Archive(EnvironmentRootOwner& env,std::uint32_t index)
{return ResolveEnvironmentArchiveAt417990(*env.CaptureAt4180C0(),index);}
bool FixedKey(const KeyboardManagerNativeBody438h& body,unsigned action,bool edge)
{
    const auto& descriptor=body.actionsAt14[action];
    auto at=reinterpret_cast<std::uintptr_t>(Read(descriptor.begin));
    const auto end=reinterpret_cast<std::uintptr_t>(Read(descriptor.end));
    if(at==end)return false;
    const auto buffer=Read(body.bufferAt10);
    do
    {
        if(*reinterpret_cast<const volatile std::uint8_t*>(at+buffer+1u)!=0&&
            (!edge||*reinterpret_cast<const volatile std::uint8_t*>(at-buffer+2u)==0))return true;
        at+=3u;
    }while(at!=end);
    return false;
}
}
bool ReadScriptSuppressInputAt4C3430(EnvironmentRootOwner& env,InputRootOwner& input)
{
    if(FixedKey(*Keyboard(input),6,false))return true;
    if(Byte(Archive(env,2),0x14)&2u)return true;
    return Byte(Archive(env,1),0x6F)==0;
}
bool ReadScriptAccelerateInputAt4AA970(InputRootOwner& input)
{
    if(FixedKey(*Keyboard(input),5,false))return true;
    auto* mouse=ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(input.CaptureAt418210()->mouseAt00);
    return Read(mouse->previousAt30[0])==0&&Read(mouse->pressedAt10[0])!=0&&Read(mouse->suppressionAt28)==0;
}
bool ReadScriptLoadKeyAt47F650(EnvironmentRootOwner& env,InputRootOwner& input)
{
    // F8描述符=(F8-14)/12=19；未命中时不捕获Env。
    if(!FixedKey(*Keyboard(input),19,true))return false;
    return Byte(Archive(env,2),9)!=0;
}
bool ReadScriptSaveKeyAt47F830(EnvironmentRootOwner& env,InputRootOwner& input)
{
    // 47F857的+EC描述符为action18；命中edge之后才取Env2+08。
    if(!FixedKey(*Keyboard(input),18,true))return false;
    return Byte(Archive(env,2),8)!=0;
}
bool ReadScriptQuickSaveKeyAt47F780(EnvironmentRootOwner& env,InputRootOwner& input,RootRuntimeOwner& root,
    IScrVmCommandErrorSink& errors,ScriptNamedValueNativeTables names,IRuntimeStringManagerSource& strings)
{
    if(!FixedKey(*Keyboard(input),20,true))return false;
    if(Byte(Archive(env,2),8)==0)return false;
    // 47F7F9先固定Root，随后Env0/命名查询可回调；Params仍取该Root。
    root.EnsureCreatedAt413BE0();auto* selected=root.ReadPublishedAt6198C4();
    static_cast<void>(Archive(env,0));
    const auto index=ResolveScriptNamedValueNativeAt498020("replay",names,strings);
    return ReadScriptValueAt4C73A0(selected->ParamsAt47BDB0(),index,errors)==0;
}
bool ReadScriptQuickLoadKeyAt47F6D0(EnvironmentRootOwner& env,InputRootOwner& input,RootRuntimeOwner& root,
    IScrVmCommandErrorSink& errors,ScriptNamedValueNativeTables names,IRuntimeStringManagerSource& strings)
{
    // 110描述符=(110-14)/12=21，不是19，也不重用4C3430的held判定。
    if(!FixedKey(*Keyboard(input),21,true))return false;
    if(Byte(Archive(env,2),9)==0)return false;
    root.EnsureCreatedAt413BE0();auto* selected=root.ReadPublishedAt6198C4();
    static_cast<void>(Archive(env,0));
    const auto index=ResolveScriptNamedValueNativeAt498020("replay",names,strings);
    return ReadScriptValueAt4C73A0(selected->ParamsAt47BDB0(),index,errors)==0;
}
bool ReadScriptLikabilityKeyAt47F530(EnvironmentRootOwner& env,InputRootOwner& input,RootRuntimeOwner& root,
    std::uint32_t checkFlag,IScrVmCommandErrorSink& errors,ScriptNamedValueNativeTables names,IRuntimeStringManagerSource& strings)
{
    if(!FixedKey(*Keyboard(input),22,true))return false;
    const auto mode=*reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(Archive(env,2))+4u);
    if(mode==2)return false;
    root.EnsureCreatedAt413BE0();auto* selected=root.ReadPublishedAt6198C4();
    static_cast<void>(Archive(env,0));
    const auto replay=ResolveScriptNamedValueNativeAt498020("replay",names,strings);
    if(ReadScriptValueAt4C73A0(selected->ParamsAt47BDB0(),replay,errors)!=0)return false;
    // 47F5E7再次Ensure/取根；两次命名值查询不共享旧的Root或CParams。
    root.EnsureCreatedAt413BE0();selected=root.ReadPublishedAt6198C4();
    static_cast<void>(Archive(env,0));
    constexpr char flagName[]="\x8D\x44\x8A\xB4\x93\x78\x81\x40\x95\x5C\x8E\xA6\x83\x74\x83\x89\x83\x4F";
    const auto flag=ResolveScriptNamedValueNativeAt498020(flagName,names,strings);
    if(ReadScriptValueAt4C73A0(selected->ParamsAt47BDB0(),flag,errors)==0)return false;
    if(!checkFlag)return true;
    return (*reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(Archive(env,2))+0x20u)&1u)!=0;
}
}
