#pragma once

#include <Windows.h>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 原18号PE的CP932资源字节；返回本模块静态字符串，不读取原PE。
std::span<const char* const> SaveLoadPageDigitsNativeSample18() noexcept;
std::span<const char* const> SaveLoadSlotDigitsNativeSample18() noexcept;
std::span<const char* const> SaveLoadDateDigitsNativeSample18() noexcept;
std::span<const char* const> SaveLoadNoDataNativeSample18() noexcept;
std::span<const char* const> SaveLoadNoPreviewNativeSample18() noexcept;
std::span<const char* const> SaveLoadNewMarkerNativeSample18() noexcept;
std::span<const POINT, 6> SaveLoadPageDigitPositionsNativeSample18() noexcept;

// 以下VA全部是审计整数，不是宿主函数指针，不能驱动生产构造或展开。
std::span<const std::uint32_t, 37> SaveLoadDialogOriginalVtableNativeSample18() noexcept;

struct SaveLoadResourceTableNativeAudit final
{
    std::uint32_t tableVa;
    std::uint32_t constructorReadVa;
    std::uint32_t loopCompareVa;
    std::uint32_t endVa; // 排他边界；原表没有额外的null指针终止项。
    std::uint32_t count;
};
std::span<const SaveLoadResourceTableNativeAudit, 6> SaveLoadResourceTablesNativeSample18Audit() noexcept;

struct SaveLoadCtorUnwindNativeAudit final
{
    std::int32_t previousState;
    std::uint32_t actionVa; // span下标即原EH state。
};
static_assert(sizeof(SaveLoadCtorUnwindNativeAudit) == 8);
std::span<const std::uint32_t, 9> SaveLoadDialogCtorFuncInfoNativeSample18Audit() noexcept;
std::span<const SaveLoadCtorUnwindNativeAudit, 41> SaveLoadDialogCtorUnwindNativeSample18Audit() noexcept;

struct SaveLoadCtorCallerNativeAudit final
{
    std::uint32_t callerEntryVa;
    std::uint32_t allocationPushVa;
    std::uint32_t callerRootByteReadVa;
    std::uint32_t secondArgumentPushVa;
    std::uint32_t firstArgumentPushVa;
    std::uint32_t constructorCallVa;
    std::uint32_t constructorVa;
    std::uint32_t argumentCompareVa;
    std::uint32_t returnVa;
};
// caller有两个push、callee有ret 8，不代表callee读取第二参数。
const SaveLoadCtorCallerNativeAudit& SaveLoadDialogCtorCallerNativeSample18Audit() noexcept;
}
