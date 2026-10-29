#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// 原生 x86 DWORD 位容器；初始化由调用者负责，不内嵌分配器或自动析构。
struct RuntimeBitVectorStorage final
{
    std::uint32_t* beginAt00;
    std::uint32_t* endAt04;
    std::uint32_t* capacityAt08;
    std::uint32_t countAt0C;
};
static_assert(sizeof(RuntimeBitVectorStorage) == 16);
static_assert(alignof(RuntimeBitVectorStorage) == 4);
static_assert(offsetof(RuntimeBitVectorStorage, beginAt00) == 0);
static_assert(offsetof(RuntimeBitVectorStorage, endAt04) == 4);
static_assert(offsetof(RuntimeBitVectorStorage, capacityAt08) == 8);
static_assert(offsetof(RuntimeBitVectorStorage, countAt0C) == 12);
static_assert(std::is_trivially_copyable_v<RuntimeBitVectorStorage>);
static_assert(std::is_standard_layout_v<RuntimeBitVectorStorage>);

// 494BD0：增加时仅填新增位；等长不写；缩短保留容量并清末字无效高位。
// 分配和旧块释放异常直接外传，不能替调用者补回滚或吞掉异常。
void ResizeRuntimeBitVectorAt494BD0(RuntimeBitVectorStorage& storage,
    std::uint32_t count, bool value,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 462850：释放成功后只清三指针，保留 count；begin 为空则所有字段不变。
void DestroyRuntimeBitVectorAt462850(RuntimeBitVectorStorage& storage,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
}
