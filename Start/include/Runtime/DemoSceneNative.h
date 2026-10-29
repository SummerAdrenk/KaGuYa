#pragma once
#include <Runtime/DemoSequenceNativeRuntime.h>
#include <Runtime/LogoSceneRuntime.h>
#include <Runtime/EndingSceneRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct DemoSceneNativeBody18h;
// 原PE：Logo5B4F40、Ending5B4F60只有五槽，表后是RTTI而不是第六个函数。
struct DemoSceneNativeVtable final
{
    std::uint8_t (__thiscall* runAt00)(DemoSceneNativeBody18h*,std::int32_t);
    DemoSceneNativeBody18h* (__thiscall* deleteAt04)(DemoSceneNativeBody18h*,std::uint32_t);
    void (__thiscall* updateAt08)(DemoSceneNativeBody18h*);
    void (__thiscall* drawAt0C)(DemoSceneNativeBody18h*);
    void (__thiscall* finishAt10)(DemoSceneNativeBody18h*);
};
struct DemoSceneNativeBody18h final
{
    const DemoSceneNativeVtable* vptrAt00;
    std::uint32_t phaseAt04;
    union {YtlReferenceSlotRuntime sequenceAt08;};
    std::uint8_t completionAt14;
    std::uint8_t inputEnabledAt15;
    std::uint8_t unwrittenAt16[2];
    DemoSceneNativeBody18h() noexcept{}
    ~DemoSceneNativeBody18h() noexcept{}
};
static_assert(sizeof(DemoSceneNativeVtable)==0x14);
static_assert(sizeof(DemoSceneNativeBody18h)==0x18);
static_assert(offsetof(DemoSceneNativeBody18h,sequenceAt08)==8);
static_assert(offsetof(DemoSceneNativeBody18h,completionAt14)==0x14);
static_assert(offsetof(DemoSceneNativeBody18h,unwrittenAt16)==0x16);
struct DemoSceneNativeServices final
{
    // 4B5FC0共有输入来源；正式Root两接口由同一对象实现，不按vptr猜类型。
    ILogoSceneRuntimeServices& logo;
    IEndingSceneRuntimeServices& ending;
    IDemoParamsSource& params;
    DemoSequenceNativeServices& sequences;
};
class DemoSceneNativeScope final
{
public:
    explicit DemoSceneNativeScope(DemoSceneNativeServices&) noexcept;
    ~DemoSceneNativeScope() noexcept;
    DemoSceneNativeScope(const DemoSceneNativeScope&)=delete;
    DemoSceneNativeScope& operator=(const DemoSceneNativeScope&)=delete;
private:
    DemoSceneNativeServices* previous_;
    DemoSequenceNativeScope sequences_;
};
const DemoSceneNativeVtable& DemoSceneBaseMethods() noexcept;
const DemoSceneNativeVtable& LogoSceneNativeMethods() noexcept;
const DemoSceneNativeVtable& EndingSceneNativeMethods() noexcept;
DemoSceneNativeBody18h* ConstructDemoSceneNativeAt49B5C0(DemoSceneNativeBody18h&,DemoSceneNativeServices&);
DemoSceneNativeBody18h* ConstructLogoNativeAt4AAFEF(DemoSceneNativeBody18h&,DemoSceneNativeServices&);
DemoSceneNativeBody18h* ConstructEndingNativeAt49DA10(DemoSceneNativeBody18h&,DemoSceneNativeServices&);
DemoSceneNativeBody18h* NewLogoNativeAt4AAFF1(DemoSceneNativeServices&);
DemoSceneNativeBody18h* NewEndingNativeAt4AB782(DemoSceneNativeServices&);
void DestroyDemoSceneNativeAt4A3C20(DemoSceneNativeBody18h&,DemoSceneNativeServices&) noexcept;
DemoSceneNativeBody18h* DeleteLogoNativeAt4A6B10(DemoSceneNativeBody18h*,std::uint32_t,DemoSceneNativeServices&) noexcept;
DemoSceneNativeBody18h* DeleteEndingNativeAt4A6890(DemoSceneNativeBody18h*,std::uint32_t,DemoSceneNativeServices&) noexcept;
std::uint8_t RunDemoSceneNativeAt4C3630(DemoSceneNativeBody18h&,std::int32_t,DemoSceneNativeServices&);
void UpdateDemoSceneNativeAt4B5FC0(DemoSceneNativeBody18h&,DemoSceneNativeServices&);
}
