#pragma once

#include <Runtime/SaveFileCatalogItemNative.h>
#include <Runtime/SaveFileFactoryNativeIo.h>

#include <array>
#include <span>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentArchiveRuntime;

// 仅系统叶；不依赖Quick业务、RollingSaveRuntime或General确认装配。
class IAutoSaveOperationsNativeSource : public ISaveFileFactoryNativeIoSource
{
public:
    virtual IEnvironmentArchiveRuntime* RegistryAt417990(
        EnvironmentRootStorage* capturedEnvironment, std::uint32_t index) = 0;
};

struct AutoSaveOperationsNativeServices final
{
    IAutoSaveOperationsNativeSource& source;
    ISaveFileCatalogItemNativeSource& catalog;
    IEnvironmentRootMemory& memory;
};

// 同线程LIFO，覆盖入口、系统叶回调和最后一次字符串释放。
class AutoSaveOperationsNativeScope final
{
public:
    explicit AutoSaveOperationsNativeScope(AutoSaveOperationsNativeServices&) noexcept;
    ~AutoSaveOperationsNativeScope() noexcept;
    AutoSaveOperationsNativeScope(const AutoSaveOperationsNativeScope&) = delete;
    AutoSaveOperationsNativeScope& operator=(const AutoSaveOperationsNativeScope&) = delete;
private:
    AutoSaveOperationsNativeServices* previous_;
};
AutoSaveOperationsNativeServices& RequireAutoSaveOperationsNativeServices();

using AutoSaveOperationsNativeFactoryBody10h = SaveFileDeleteFactoryNativeRollingBody10h;
// 45B800的save构造 + 46B2AD..46B2D2的asave构造；只写+0C低byte，不写+0D..0F。
// 构造失败已执行对应展开，调用者不得再次销毁失败正文。
void ConstructAutoSaveOperationsNativeFactory(
    AutoSaveOperationsNativeFactoryBody10h&, std::uint8_t preview);
// 46B280不消费ECX，ret4；参数仅消费低byte，返回void，不伪造EAX成功值。
void __stdcall AutoSaveNativeAt46B280(std::uint32_t preview);
// 4883F0原thiscall正文的语义核；接真实10h正文，返回完整DWORD 0/1。
// factory拥有权仍属调用者；不追加范围检查、异常catch或EH回滚。
std::int32_t RollingAutoSaveNativeAt4883F0(AutoSaveOperationsNativeFactoryBody10h&);

struct AutoSaveOperationsNativeFunctionInfo final
{
    std::uint32_t entry, functionInfo, unwindMap, unwindCount, ehFlags;
};
std::span<const AutoSaveOperationsNativeFunctionInfo, 6> AutoSaveOperationsNativeFunctionInfos() noexcept;
std::span<const std::array<std::uint32_t, 2>, 2> AutoSaveOperationsNativeEntryUnwindRows() noexcept;
std::span<const std::array<std::uint32_t, 2>, 2> AutoSaveOperationsNativeRollingUnwindRows() noexcept;
std::span<const std::array<std::uint32_t, 2>, 1> AutoSaveOperationsNativeBaseConstructorUnwindRows() noexcept;
std::span<const std::uint32_t, 1> AutoSaveOperationsNativeFactoryOriginalSlots() noexcept;
const char* AutoSaveOperationsNativeTitleFormat() noexcept; // 5B2AC8原CP932字节。
}
