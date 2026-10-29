#pragma once

#include <Runtime/RuntimeCString.h>
#include <Runtime/NativeArchiveRuntime.h>
#include <Runtime/SystemDataEnvironmentArchive.h>

#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// 46CE76 / vtable 5B267C：真实 raw 两槽 vptr + 两个 CString，无 C++ 基类/owner。
class ScrSaveParamsRuntimeState final
{
public:
    explicit ScrSaveParamsRuntimeState(
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~ScrSaveParamsRuntimeState() noexcept;
    ScrSaveParamsRuntimeState(const ScrSaveParamsRuntimeState&) = delete;
    ScrSaveParamsRuntimeState& operator=(const ScrSaveParamsRuntimeState&) = delete;

    // 明确旧宿主入口，不进入 native vptr。
    void Serialize(SerializeBuffer& archive);
    void* DeleteAtVslot04(std::uint32_t flags) noexcept;

    const NativeArchiveVtable2* vptrAt00;
    // union 禁止新版 ATL 自动构造/析构插入额外 manager 调用或改变异常边界。
    union { RuntimeCString saveDescriptionAt04; };
    union { RuntimeCString saveTitleAt08; };
};
static_assert(sizeof(ScrSaveParamsRuntimeState) == 0x0C);
static_assert(alignof(ScrSaveParamsRuntimeState) == 4);
static_assert(!std::is_polymorphic_v<ScrSaveParamsRuntimeState>);
static_assert(std::is_standard_layout_v<ScrSaveParamsRuntimeState>);
static_assert(offsetof(ScrSaveParamsRuntimeState, vptrAt00) == 0);
static_assert(offsetof(ScrSaveParamsRuntimeState, saveDescriptionAt04) == 4);
static_assert(offsetof(ScrSaveParamsRuntimeState, saveTitleAt08) == 8);
}
