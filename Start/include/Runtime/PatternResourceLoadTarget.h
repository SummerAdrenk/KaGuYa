#pragma once

#include <Runtime/PatternResourceLoadRuntime.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 原捕获点的借用 Sprite 地址；不是索引、Ref 或拥有 token，不延长对象寿命。
using PatternSpriteReceiver = std::uintptr_t;

class IPatternResourceLoadCommit
{
public:
    virtual ~IPatternResourceLoadCommit() = default;
    // 同步调用栈对象。normal Direct 自行晚解当前槽，不能统一沿用装载 receiver。
    virtual void Commit() = 0;
};
}
