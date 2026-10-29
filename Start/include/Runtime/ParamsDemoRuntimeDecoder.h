#pragma once

#include <Runtime/ParamsDemoCommandRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 419EB0 / 420650：消费实际字段 payload；不解析记录 length，不要求 EOF。
// 未知 type 抛原 char*；截断由共享 Cursor 安全拒绝，不模拟原版越界访问。
// 短文本在调用线程按 ACP 转换；与加载阶段代码页不同仍是转换时机差异。
[[nodiscard]] DecodedParamsDemoCommand DecodeRuntimeParamsDemoCommand(
    const ParamsDemoCommand& command);
}
