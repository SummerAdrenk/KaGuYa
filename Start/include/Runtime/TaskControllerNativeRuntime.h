#pragma once
#include <Runtime/TaskControllerNative.h>

namespace KaGuYa::Start::Runtime
{
// 唯一18h拥有外观，不持调度状态镜像；Item仅借用task，任务必须在执行期存活。
class TaskControllerNativeRuntime final
{
public:
    explicit TaskControllerNativeRuntime(IEnvironmentRootMemory& =DefaultEnvironmentRootMemory());
    ~TaskControllerNativeRuntime() noexcept;
    TaskControllerNativeRuntime(const TaskControllerNativeRuntime&)=delete;
    TaskControllerNativeRuntime& operator=(const TaskControllerNativeRuntime&)=delete;
    void Register(TaskNativeBody4h*,std::int32_t);
    void Run(std::int32_t);
    TaskControllerNativeBody18h* Body() const noexcept{return body_;}
private:
    TaskControllerNativeServices services_;
    TaskControllerNativeBody18h* body_;
};
}
