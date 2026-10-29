#include <Runtime/ProgramFixedActionRuntime.h>
#include <Runtime/GameSceneRoutes.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ProgramFixedActionRuntime::ProgramFixedActionRuntime(
    IProgramActionFactory& factory,
    IProgramActionQueue& queue) noexcept
    : factory_(&factory), queue_(&queue)
{
}

void ProgramFixedActionRuntime::Execute(const ScrVmFixedAction action)
{
    // 0x4A9030 对 factory 返回值不做空检查，直接原样加入队列。
    queue_->Push(factory_->Create(GameFixedActionSceneName(action)));
}

std::string_view ProgramFixedActionRuntime::ActionName(
    const ScrVmFixedAction action)
{
    switch (action)
    {
    case ScrVmFixedAction::Code8:
        return "\x83\x47\x83\x93\x83\x66\x83\x43\x83\x93\x83\x4F";
    case ScrVmFixedAction::Code9:
        return "\x83\x7D\x83\x62\x83\x76\x8F\x88\x97\x9D";
    case ScrVmFixedAction::Code10:
        return "\x93\xFA\x95\x74\x95\xCF\x8D\x58";
    case ScrVmFixedAction::Code11:
        return "\x83\x80\x81\x5B\x83\x72\x81\x5B\x8D\xC4\x90\xB6";
    case ScrVmFixedAction::Code12:
        return "\x83\x65\x83\x4C\x83\x58\x83\x67\x95\x5C\x8E\xA6\x8F\x88\x97\x9D";
    case ScrVmFixedAction::Code13:
        return "\x83\x76\x83\x8D\x83\x4F\x83\x89\x83\x80\x91\x49\x91\xF0\x8F\x88\x97\x9D";
    case ScrVmFixedAction::Code15:
        return "\x83\x41\x83\x43\x83\x65\x83\x80\x8E\xE6\x93\xBE\x8F\x88\x97\x9D";
    case ScrVmFixedAction::Code16:
        return "\x97\x8E\x8F\x91\x82\xAB\x83\x79\x81\x5B\x83\x57\x8F\x88\x97\x9D";
    }
    throw std::out_of_range("unsupported fixed VM action code");
}
}
