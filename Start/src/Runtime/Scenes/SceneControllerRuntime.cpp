#include <Runtime/SceneControllerRuntime.h>
#include <Runtime/SceneControllerNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 未全迁的17路ISceneTask只在显式bridge持有，不复制原28h的业务状态。
struct TaskBridge final
{
    const TaskNativeVtable* vptr;
    std::shared_ptr<ISceneTask> task;
};
std::uint8_t __fastcall BridgeRun(TaskNativeBody4h* body,void*,std::int32_t argument)
{reinterpret_cast<TaskBridge*>(body)->task->Run(argument);return 1;}
TaskNativeBody4h* __fastcall BridgeDelete(TaskNativeBody4h* body,void*,std::uint32_t flags) noexcept;
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const TaskNativeVtable taskMethods{
    reinterpret_cast<decltype(TaskNativeVtable::runAt00)>(&BridgeRun),
    reinterpret_cast<decltype(TaskNativeVtable::deleteAt04)>(&BridgeDelete)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
TaskNativeBody4h* __fastcall BridgeDelete(TaskNativeBody4h* body,void*,std::uint32_t flags) noexcept
{
    auto* bridge=reinterpret_cast<TaskBridge*>(body);bridge->~TaskBridge();
    if(flags&1u)::operator delete(bridge,std::size_t{sizeof(TaskBridge)});return body;
}
}
struct SceneControllerRuntime::NativeStorage final
{
    ISceneFactory& factory;
    SceneControllerNativeServices services{DefaultEnvironmentRootMemory()};
    SceneControllerNativeBody28h* body=nullptr;
    static thread_local NativeStorage* current;
    class Scope final
    {
    public:
        explicit Scope(NativeStorage& source) noexcept:previous_(current),controller_(source.services){current=&source;}
        ~Scope() noexcept{current=previous_;}
    private:
        NativeStorage* previous_;
        SceneControllerNativeScope controller_;
    };
    static TaskNativeBody4h* __fastcall Create(SceneFactoryNativeBody4h*,void*,const char* name)
    {
        auto task=current->factory.Create(name);if(!task)return nullptr;
        auto* raw=::operator new(sizeof(TaskBridge));
        return reinterpret_cast<TaskNativeBody4h*>(::new(raw) TaskBridge{&taskMethods,std::move(task)});
    }
    explicit NativeStorage(ISceneFactory& source):factory(source)
    {
        Scope scope(*this);body=NewSceneControllerNativeAt45BA52(services);
        struct Unwind final
        {
            NativeStorage& storage;bool armed=true;
            ~Unwind() noexcept{if(armed&&storage.body)storage.body->vptrAt00->deleteAt04(reinterpret_cast<TaskNativeBody4h*>(storage.body),1);}
        } unwind{*this};
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        static const SceneFactoryNativeVtable methods{
            reinterpret_cast<decltype(SceneFactoryNativeVtable::createAt00)>(&Create)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        auto* factoryBody=static_cast<SceneFactoryNativeBody4h*>(services.memory.AllocateBody(4));
        if(factoryBody)factoryBody->vptrAt00=&methods;
        AdoptSceneFactoryNativeAt45BB78(*body,factoryBody,services);unwind.armed=false;
    }
    ~NativeStorage() noexcept
    {Scope scope(*this);if(body)body->vptrAt00->deleteAt04(reinterpret_cast<TaskNativeBody4h*>(body),1);}
};
thread_local SceneControllerRuntime::NativeStorage* SceneControllerRuntime::NativeStorage::current=nullptr;
SceneControllerRuntime::NativeCallScope::NativeCallScope(SceneControllerRuntime& runtime) noexcept
    :previous_(NativeStorage::current),controller_(runtime.native_->services){NativeStorage::current=runtime.native_.get();}
SceneControllerRuntime::NativeCallScope::~NativeCallScope() noexcept{NativeStorage::current=static_cast<NativeStorage*>(previous_);}
SceneControllerRuntime::SceneControllerRuntime(ISceneFactory& factory):native_(std::make_unique<NativeStorage>(factory)){}
SceneControllerRuntime::~SceneControllerRuntime() noexcept=default;
void SceneControllerRuntime::Queue(const char* name)
{NativeStorage::Scope scope(*native_);QueueSceneControllerNativeAt47F966(*native_->body,name,native_->services);}
void SceneControllerRuntime::PushActiveScene(const char* name)
{NativeStorage::Scope scope(*native_);auto* factory=ResolveEnvironmentObjectReference<SceneFactoryNativeBody4h>(native_->body->factoryAt1C);
PushSceneTaskNativeAt40B050(native_->body->activeAt10,factory->vptrAt00->createAt00(factory,name),native_->services);}
void SceneControllerRuntime::PopThenActivatePendingAt4C1760()
{NativeStorage::Scope scope(*native_);ActivatePendingSceneNativeAt4C1782(*native_->body,native_->services);}
bool SceneControllerRuntime::Run(std::int32_t argument)
{NativeStorage::Scope scope(*native_);return RunSceneControllerNativeAt48E100(*native_->body,argument,native_->services)!=0;}
void SceneControllerRuntime::Clear() noexcept
{NativeStorage::Scope scope(*native_);ClearSceneControllerNativeAt4C14A5(*native_->body);}
std::size_t SceneControllerRuntime::PendingCount() const noexcept{return PendingSceneCountNative(*native_->body);}
std::size_t SceneControllerRuntime::ActiveCount() const noexcept{return static_cast<std::uint32_t>(ActiveSceneCountNative(*native_->body));}
const SceneControllerNativeBody28h& SceneControllerRuntime::NativeBody() const noexcept{return *native_->body;}
TaskNativeBody4h* SceneControllerRuntime::BorrowNativeTask() noexcept{return reinterpret_cast<TaskNativeBody4h*>(native_->body);}
}
