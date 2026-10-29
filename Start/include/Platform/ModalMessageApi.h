#pragma once

#include <Windows.h>

namespace KaGuYa::Start::Platform
{
// 507398..5073E9的四个消息叶；不包含外层MessageLoop的过滤、退出或重投WM_QUIT策略。
class IModalMessageApi
{
public:
    virtual ~IModalMessageApi() = default;
    [[nodiscard]] virtual bool PeekMessageNoRemove(MSG& message) = 0;
    // 保留GetMessageA的-1/0/>0，不转换成bool；地址后缀避开Windows的A/W宏。
    [[nodiscard]] virtual int GetMessageAt5073B0(MSG& message) = 0;
    virtual void TranslateMessageAt5073C9(const MSG& message) = 0;
    virtual void DispatchMessageAt5073D3(const MSG& message) = 0;
};

class Win32ModalMessageApi final : public IModalMessageApi
{
public:
    [[nodiscard]] bool PeekMessageNoRemove(MSG& message) override;
    [[nodiscard]] int GetMessageAt5073B0(MSG& message) override;
    void TranslateMessageAt5073C9(const MSG& message) override;
    void DispatchMessageAt5073D3(const MSG& message) override;
};
}
