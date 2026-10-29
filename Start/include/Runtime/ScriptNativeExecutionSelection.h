#pragma once

namespace KaGuYa::Start::Runtime
{
class IRestoredScriptNativeExecution;
struct ScriptNativeBody;

struct ScriptNativeExecutionBinding final
{
    IRestoredScriptNativeExecution* execution;
    ScriptNativeBody* body;
};

// 只保存调用期借用身份，不保存PC/flags/Params，不增持原Ref。
// 固定执行器无需路由；惰性生产路由使用线程内嵌套链，退出/异常均恢复外层选择。
class ScriptNativeExecutionSelectionScope final
{
public:
    ScriptNativeExecutionSelectionScope(IRestoredScriptNativeExecution& execution,ScriptNativeBody& body)noexcept
        :binding_{&execution,&body}{}
    ScriptNativeExecutionSelectionScope(const IRestoredScriptNativeExecution& owner,IRestoredScriptNativeExecution& execution,ScriptNativeBody& body)noexcept
        :owner_(&owner),binding_{&execution,&body},previous_(current_)
    {current_=this;}
    ~ScriptNativeExecutionSelectionScope()noexcept{if(owner_)current_=previous_;}
    ScriptNativeExecutionSelectionScope(const ScriptNativeExecutionSelectionScope&)=delete;
    ScriptNativeExecutionSelectionScope& operator=(const ScriptNativeExecutionSelectionScope&)=delete;
    [[nodiscard]] static const ScriptNativeExecutionBinding* CurrentFor(const IRestoredScriptNativeExecution& owner)noexcept
    {
        for(auto* scope=current_;scope;scope=scope->previous_)
            if(scope->owner_==&owner)return &scope->binding_;
        return nullptr;
    }
private:
    const IRestoredScriptNativeExecution* owner_=nullptr;
    ScriptNativeExecutionBinding binding_;
    ScriptNativeExecutionSelectionScope* previous_=nullptr;
    inline static thread_local ScriptNativeExecutionSelectionScope* current_=nullptr;
};
}
