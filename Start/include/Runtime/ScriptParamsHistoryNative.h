#pragma once

#include <Runtime/ScriptCallStacksNative.h>
#include <Runtime/SerializeBuffer.h>

namespace KaGuYa::Start::Runtime
{
// 457300：第二个原参数未读取；唯一存储仍是调用者的12字节三指针。
void ResizeScriptParamsHistoryAt457300(ScriptDwordVectorStorage& history, std::uint32_t count,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 5B22F4+20 -> 4661B0。这里只适配现有SerializeBuffer，不代表原CSerialize正文/虚调用已迁移。
// scalar沿用适配器的整DWORD截断拒绝；原465C80/465D80是逐字节先推进游标再读取。
SerializeBuffer& SerializeScriptParamsHistoryAt4661B0(SerializeBuffer& archive,
    ScriptDwordVectorStorage& history,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 4C81F4先捕获CParams+60，再追加到+3C；调用者传入已捕获的DWORD，不延迟读取正文。
void AppendScriptParamsHistoryAt4C81F0(ScriptDwordVectorStorage& history,
    std::uint32_t capturedFormat,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
