#pragma once
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/TitleDialogNative.h>
#include <cstddef>
#include <optional>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct TitleSceneNativeBody1Ch;
struct SceneManagerNativeBody48h;
struct SceneManagerNativeServices;
struct TitleSceneNativeVtable4 final
{
    std::uint8_t (__thiscall* runAt00)(TitleSceneNativeBody1Ch*,std::int32_t);
    TitleSceneNativeBody1Ch* (__thiscall* deleteAt04)(TitleSceneNativeBody1Ch*,std::uint32_t);
    void (__thiscall* updateAt08)(TitleSceneNativeBody1Ch*);
    void (__thiscall* drawAt0C)(TitleSceneNativeBody1Ch*);
};
// 4A2850/5B4F8C。1Ch只持一个D8h Dialog Ref；14/18不在构造中写入。
struct TitleSceneNativeBody1Ch final
{
    const TitleSceneNativeVtable4* vptrAt00;
    std::int32_t phaseAt04;
    union { YtlReferenceSlotRuntime dialogAt08; };
    std::uint32_t unwrittenAt14;
    std::uint32_t unwrittenAt18;
    TitleSceneNativeBody1Ch() noexcept {}
    ~TitleSceneNativeBody1Ch() noexcept {}
};
static_assert(sizeof(void*)==4);
static_assert(sizeof(TitleSceneNativeBody1Ch)==0x1C);
static_assert(sizeof(TitleSceneNativeVtable4)==0x10);
static_assert(std::is_standard_layout_v<TitleSceneNativeBody1Ch>);
static_assert(offsetof(TitleSceneNativeBody1Ch,phaseAt04)==4);
static_assert(offsetof(TitleSceneNativeBody1Ch,dialogAt08)==8);
static_assert(offsetof(TitleSceneNativeBody1Ch,unwrittenAt14)==0x14);
static_assert(offsetof(TitleSceneNativeBody1Ch,unwrittenAt18)==0x18);

struct TitleSceneNativeManagerReceiver final
{
    SceneManagerNativeBody48h* body;
    SceneManagerNativeServices* services;
};
class ITitleSceneNativeDialogFactory
{
public:
    virtual ~ITitleSceneNativeDialogFactory()=default;
    // 外层已分配唯一D8h；callee负责自己的成员EH，不再申请现代Dialog owner。
    virtual void* ConstructAt4F4530(void* storage)=0;
    // 正文外服务声明；raw叶夹具无需D8业务服务，真实生产factory必须返回同一套。
    virtual TitleDialogNativeServices* NativeDialogServices() const noexcept{return nullptr;}
};
class TitleDialogSceneNativeConstruction final:public ITitleSceneNativeDialogFactory
{
public:
    explicit TitleDialogSceneNativeConstruction(TitleDialogNativeServices& services) noexcept:services_(services){}
    void* ConstructAt4F4530(void* storage) override
    {return ConstructTitleDialogNativeAt4F4530(storage,services_);}
    TitleDialogNativeServices* NativeDialogServices() const noexcept override{return &services_;}
private:
    TitleDialogNativeServices& services_;
};
class ITitleSceneNativeSource
{
public:
    virtual ~ITitleSceneNativeSource()=default;
    virtual TitleSceneNativeManagerReceiver CaptureScenesAfter413BE0()=0;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0()=0;
    virtual void ClearInformationDwordAfter413BE0()=0;
    virtual bool EvaluateLoadPredicateAfter413BE0()=0;
    virtual void WriteRootByteAfter413BE0(std::uint32_t offset,std::uint8_t value)=0;
    virtual void ClearReplayAt4A2DD6()=0;
    virtual std::uint8_t LoadStartupScriptAfter413BE0(const char* name,std::int32_t argument)=0;
};
struct TitleSceneNativeServices final
{
    IEnvironmentRootMemory& memory;
    ITitleSceneNativeDialogFactory& dialogs;
    ITitleSceneNativeSource& source;
};
class TitleSceneNativeScope final
{
public:
    explicit TitleSceneNativeScope(TitleSceneNativeServices&) noexcept;
    ~TitleSceneNativeScope() noexcept;
    TitleSceneNativeScope(const TitleSceneNativeScope&)=delete;
    TitleSceneNativeScope& operator=(const TitleSceneNativeScope&)=delete;
private:
    TitleSceneNativeServices* previous_;
    std::optional<TitleDialogNativeScope> dialog_;
};
const TitleSceneNativeVtable4& TitleSceneNativeMethods() noexcept;
TitleSceneNativeBody1Ch* ConstructTitleSceneNativeAt4A2850(TitleSceneNativeBody1Ch&,TitleSceneNativeServices&);
void DestroyTitleSceneNativeAt4A71E0(TitleSceneNativeBody1Ch&) noexcept;
TitleSceneNativeBody1Ch* DeleteTitleSceneNativeAt4A71E0(TitleSceneNativeBody1Ch*,std::uint32_t) noexcept;
void UpdateTitleSceneNativeAt4BADA0(TitleSceneNativeBody1Ch&,TitleSceneNativeServices&);
}
