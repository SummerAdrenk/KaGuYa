#include <Runtime/TaskControllerNativeRuntime.h>

namespace KaGuYa::Start::Runtime
{
TaskControllerNativeRuntime::TaskControllerNativeRuntime(IEnvironmentRootMemory& memory)
    :services_{memory},body_(NewTaskControllerNativeAt45B960(services_)){}
TaskControllerNativeRuntime::~TaskControllerNativeRuntime() noexcept{DeleteTaskControllerNative(body_,services_);}
void TaskControllerNativeRuntime::Register(TaskNativeBody4h* task,std::int32_t priority)
{RegisterTaskNativeAt46A330(*body_,task,priority,services_);}
void TaskControllerNativeRuntime::Run(std::int32_t argument)
{RunTaskControllerNativeAt46BC90(*body_,argument,services_);}
}
