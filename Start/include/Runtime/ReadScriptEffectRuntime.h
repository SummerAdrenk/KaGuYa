#pragma once

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class IReadScriptEffectRuntime
{
public:
    virtual ~IReadScriptEffectRuntime() = default;
    [[nodiscard]] virtual bool TickAtVslot00() = 0;
    virtual void DrawAtVslot04() = 0;
    // 4B0730: mov al,[ecx+4]; ret。仅查询原 ready byte，不执行初始化。
    [[nodiscard]] virtual std::uint8_t ReadyAtVslot08() const = 0;

    // 显式、可抛出的完成业务，析构绝不自动调用。
    // 正常完成由 owner 在受控释放路径调用，再销毁已接管的 Effect。
    // 各 caller 的资源准备/解绑顺序仍按原实现，不因本接口前移 Finish。
    // 替换时先构造新对象，再 Finish 旧对象。
    // 外层失败时，未安装的新候选只销毁，不自动补绘制。
    // 每个对象仅尝试一次 Finish；无论返回或抛出，之后仅允许销毁。
    virtual void Finish() = 0;
};
}
