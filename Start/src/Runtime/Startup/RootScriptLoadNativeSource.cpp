#include <Runtime/RootScriptLoadNativeSource.h>
#include <Runtime/ParamsResourceNameRuntime.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
RootScriptLoadNativeSource::RootScriptLoadNativeSource(RootScriptLoadNativeDependencies dependencies) noexcept
    :dependencies_(dependencies){}
const char* RootScriptLoadNativeSource::FindNameAt4AD1B0(std::int32_t id)
{
    ParamsResourceNameRuntime names(dependencies_.params);
    return names.CaptureParamsForNames().CapturePatternNames().FindNameAt4AD1B0(id);
}
void RootScriptLoadNativeSource::LoadAt4D44C0(ScriptReaderFactoryNativeBody14h& target,const char* name)
{
    // 同源483D90/4D44C0工厂沿原目录/reader/codec读取；接管已有Ref，不另增持。
    ArchiveReaderFactoryRuntime factory(dependencies_.archives,dependencies_.transaction,
        dependencies_.files,dependencies_.readers,dependencies_.codecs);
    const auto transfer=[&]() noexcept
    {
        const auto slot=factory.DetachReaderSlot();
        target.readerAt04.baseAt00=slot.baseAt00;target.readerAt04.biasAt04=slot.biasAt04;
        target.readerAt04.controlAt08=slot.controlAt08;
    };
    try{static_cast<void>(factory.LoadBorrowedAt4D44C0(name,dependencies_.key,&target.fieldAt10));}
    catch(...){transfer();throw;}
    transfer();
}
ScriptParamsNativeBody* RootScriptLoadNativeSource::CaptureParamsAfter413BE0()
{dependencies_.root.EnsureCreatedAt413BE0();return &dependencies_.root.Published()->ParamsAt47BDB0();}
EnvironmentRootStorage* RootScriptLoadNativeSource::CaptureEnvironmentAt4180C0()
{return dependencies_.environment.CaptureAt4180C0();}
int RootScriptLoadNativeSource::MessageBoxAt4D3EE6(HWND window,const char* text,const char* title,UINT type)
{return dependencies_.messageBox(window,text,title,type);}
RestoredScriptNativeExecutionRuntime::RestoredScriptNativeExecutionRuntime(ScriptLoadNativeServices& loading,
    IScriptExecutionRootSource& root,IScriptNativeEntryContext& context,IEnvironmentRootMemory& memory) noexcept
    :loading_(loading),root_(root),context_(context),memory_(memory){}
ScriptNativeExecutionSelectionScope RestoredScriptNativeExecutionRuntime::BindSelectedReceiver(
    RootRuntimePayload&,ScriptNativeBody& body)
{
    // 此底层执行器已由外层绑定context；保留原Load允许指定parser、Dispatch核对receiver的合同。
    return ScriptNativeExecutionSelectionScope(*this,body);
}
std::uint8_t RestoredScriptNativeExecutionRuntime::LoadAt4D3C90(ScriptNativeBody& body,std::int32_t id)
{return LoadScriptNativeAt4D3C90(body,id,loading_);}
std::int32_t RestoredScriptNativeExecutionRuntime::DispatchAt4D3FF0(ScriptNativeBody& body)
{
    class Dispatch final:public IScriptNativeCommandDispatch
    {
    public:
        explicit Dispatch(ScriptNativeBody& body) noexcept:body_(body){}
        std::int32_t DispatchAt4D4200(std::uint32_t index) override
        {auto fn=*reinterpret_cast<ScriptNativeEntry const volatile*>(&body_.entriesAt2C[index]);return fn(&body_);}
    private:ScriptNativeBody& body_;
    } commands(body);
    ScriptNativeEntryScope scope(context_);
    return ExecuteScriptLoopAt4D3FF0(body.parserAtA0,root_,commands,memory_);
}
}
