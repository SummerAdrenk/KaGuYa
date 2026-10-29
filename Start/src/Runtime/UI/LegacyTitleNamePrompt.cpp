#include <Runtime/LegacyTitleNamePrompt.h>

namespace KaGuYa::Start::Runtime
{
bool LegacyTitleNamePrompt::EditableNameBit(std::uint32_t bit) { return source_.EditableNameBit(bit); }
bool LegacyTitleNamePrompt::SkipNamePrompt() { return source_.SkipNamePrompt(); }
std::int32_t LegacyTitleNamePrompt::RunModal()
{
    // 49E2DC..49E34D：构造后才使能IME；普通1/2/-1都写 gate=1、IME=false。
    // 49DFD0 EH 仅析构姓名对象；异常不补 gate/IME finally，也不转成取消。
    LegacyTitleNameDialog dialog(source_, api_, strings_);
    static_cast<void>(api_.EnableIme(source_.CaptureRoot().MainWindow(), TRUE));
    source_.WriteInputGate(false);
    const auto result = dialog.RunModal();
    source_.WriteInputGate(true);
    static_cast<void>(api_.EnableIme(source_.CaptureRoot().MainWindow(), FALSE));
    return static_cast<std::int32_t>(result);
}
}
