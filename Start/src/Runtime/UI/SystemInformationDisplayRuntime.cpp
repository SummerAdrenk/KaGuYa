#include <Runtime/SystemInformationDisplayRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SystemInformationDisplayRuntime::SystemInformationDisplayRuntime(GuiModalDialogDependencies dependencies,
    ISystemMenuDialogServices& services)
    : rootSource_(dependencies.rootSource), runner_(dependencies.modal),
      child_(std::make_shared<SystemMenuDialogRuntime>(dependencies, services)), suppressedAt10_(0)
{
    // 4DC5CA：只有 child 完整构造成功才发布 +10=0。
}
SystemInformationDisplayRuntime::~SystemInformationDisplayRuntime() noexcept = default;

bool SystemInformationDisplayRuntime::CanOpenAt5105C0()
{
    if (rootSource_.ModalGateAt413BE0() != 0 || suppressedAt10_ != 0) return false;
    if (!child_) throw std::logic_error("5105C0 dereferenced an empty SystemDialog slot");
    return child_->CanRemainOpenAt5104A0();
}
std::int32_t SystemInformationDisplayRuntime::RunAt5075B0()
{
    // 5075B0 不重复 CanOpen、不修改 +10，异常也不补调 End。
    return runner_.RunAt5071D0(*this);
}
IModalDialogRuntimeTarget* SystemInformationDisplayRuntime::ResolveAt4A5330()
{
    return child_.get();
}
}
