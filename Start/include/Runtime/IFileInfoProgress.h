#pragma once

#include <Windows.h>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 5B2A28：按MSVC x86将四个业务槽放在前、scalar deleting destructor放在+10。
// 独立Environment回归已直接调用生成虚表的五个槽位，核对顺序与参数传递。
// C++析构声明不暴露原468910(flags)的EAX=this返回契约；不据此宣称完整删除ABI等价。
class IFileInfoProgress
{
public:
    [[nodiscard]] virtual bool PollCancelAt47F0B0() = 0;
    virtual LRESULT StepAt46E820() = 0;
    virtual LRESULT AddRangeAt46A5A0(std::uint32_t delta) = 0;
    [[nodiscard]] virtual HWND WindowAt47ADF0() const noexcept = 0;
    virtual ~IFileInfoProgress() = default;
};
}
