#pragma once

#include <cstddef>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 原445750读取payload+4；输出大小由调用者在临时Ref分配前单独取得。
// 不重读晚取payload的长度DWORD，也不分配或复制输出正文。
// 有界检查/未初始化窗口拒绝是现代安全边界；尚非原1010h共享CLzss owner。
void DecodeArchiveLzssPayload(std::span<const std::byte> payload,
    std::span<std::byte> destination);
}
