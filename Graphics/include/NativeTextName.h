#pragma once

#include "NativeAnsiString.h"

namespace KaGuYa::Graphics
{
// 原1000F5C0的D0/D4各为一个字符指针；本类型自己开始生命周期，不借用未构造的CString。
class NativeTextName final : public NativeAnsiString
{
public:
    NativeTextName() : NativeAnsiString(Initialization::EmptyAssignment) {}
    ~NativeTextName() noexcept(false) = default;
    NativeTextName(const NativeTextName&) = delete;
    NativeTextName& operator=(const NativeTextName&) = delete;

};
static_assert(sizeof(NativeTextName) == 4);
}
