#pragma once

#include <Runtime/GeneralConfirmDialogNative.h>
#include <Runtime/SaveFileCatalogItemNative.h>
#include <Runtime/SaveFileFactoryNativeIo.h>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentArchiveRuntime;

// 只提供真实叶；不得转调旧QuickLoadRuntime/RollingSaveRuntime或整个quick/rolling业务。
class IQuickOperationsNativeSource : public ISaveFileFactoryNativeIoSource
{
public:
    virtual IEnvironmentArchiveRuntime* RegistryAt417990(
        EnvironmentRootStorage* capturedEnvironment, std::uint32_t index) = 0;
    // 已Ensure并捕获的Root正文；此叶不得再次Ensure或改取当前全局Root。
    virtual YtlReferenceSlotRuntime& TransitionFrameAt1F0(void* capturedRoot) = 0;
    // 解析capturedRoot+3C原Ref，保持该Program身份直至5071D0返回。
    // 返回的services仅执行Modal的细粒度叶，不代办确认、consume或整个5071D0。
    virtual IModalDialogRunnerServices& ModalForProgramAt3C(void* capturedRoot) = 0;
    virtual int MessageBoxA(HWND, const char* text, const char* caption, UINT type) = 0;
};

struct QuickOperationsNativeServices final
{
    IQuickOperationsNativeSource& source;
    // 直接借48E5F0：ReadContainer接收它自己的固定局部70h，不得取Root Params替代。
    ISaveFileCatalogItemNativeSource& catalog;
    GeneralConfirmDialogNativeServices& confirmation;
    IEnvironmentRootMemory& memory;
};

// 同线程LIFO；覆盖全部入口、确认回调及最后Ref释放。无Root/GUI头反向依赖。
class QuickOperationsNativeScope final
{
public:
    explicit QuickOperationsNativeScope(QuickOperationsNativeServices&) noexcept;
    ~QuickOperationsNativeScope() noexcept;
    QuickOperationsNativeScope(const QuickOperationsNativeScope&) = delete;
    QuickOperationsNativeScope& operator=(const QuickOperationsNativeScope&) = delete;
private:
    QuickOperationsNativeServices* previous_;
    GeneralConfirmDialogNativeScope confirmation_;
};
QuickOperationsNativeServices& RequireQuickOperationsNativeServices();

using QuickOperationsNativeFactoryBody10h = SaveFileDeleteFactoryNativeRollingBody10h;
// 原45B800基构造后，先发quick表/+0C低byte/+08=8，再第二Set；+0D..0F不写。
void ConstructQuickOperationsNativeFactory(QuickOperationsNativeFactoryBody10h&, std::uint8_t preview);
// 入口不消费ECX。485CE0只定义AL；485FC0 ret4为void，不伪造成功返回。
bool QuickLoadNativeAt485CE0();
void __stdcall QuickSaveNativeAt485FC0(std::uint32_t preview);
// 原thiscall接收真实10h正文；完整业务在本模块，返回DWORD 0/1，异常原样外传。
std::int32_t RollingQuickSaveNativeAt4888C0(QuickOperationsNativeFactoryBody10h&);

struct QuickOperationsNativeFunctionInfo final
{
    std::uint32_t entry;
    std::uint32_t functionInfo;
    std::uint32_t unwindMap;
    std::uint32_t unwindCount;
};
std::span<const QuickOperationsNativeFunctionInfo, 4> QuickOperationsNativeFunctionInfos() noexcept;
std::span<const std::array<std::uint32_t, 2>, 5> QuickOperationsNativeLoadUnwindRows() noexcept;
std::span<const std::array<std::uint32_t, 2>, 5> QuickOperationsNativeSaveUnwindRows() noexcept;
std::span<const std::array<std::uint32_t, 2>, 2> QuickOperationsNativeRollingUnwindRows() noexcept;
std::span<const std::uint32_t, 1> QuickOperationsNativeFactoryOriginalSlots() noexcept;
const char* QuickOperationsNativeRollingTitleFormat() noexcept; // 原5B2AA4原字节
const char* QuickOperationsNativeNoSaveMessage() noexcept;      // 原5B36EC原字节
const char* QuickOperationsNativeLoadErrorMessage() noexcept;   // 原5B3718原字节
}
