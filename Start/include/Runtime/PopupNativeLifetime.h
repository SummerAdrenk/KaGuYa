#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
// 4D7860是构造口而非替换口：清输出、不释放旧值，调用者必须提供非拥有输出。
YtlReferenceSlotRuntime* ConstructGuiFactoryReferenceAt4D7860(
    YtlReferenceSlotRuntime& output, void* body, std::uint8_t owning);
void ReleaseGuiFactoryReferenceAt4F6540(YtlReferenceSlotRuntime& slot) noexcept;
// 4EAB14尾路径固定control，和4F6540的slot重读不同。
void FinishGuiFactoryAt4EAB14(YtlReferenceSlotRuntime& slot) noexcept;

// 构造函数局部守卫，不是1字节原生factory正文，也不进入Popup的A4h存储。
class GuiFactoryConstructionLifetime final
{
public:
    GuiFactoryConstructionLifetime();
    ~GuiFactoryConstructionLifetime() noexcept;
    GuiFactoryConstructionLifetime(const GuiFactoryConstructionLifetime&) = delete;
    GuiFactoryConstructionLifetime& operator=(const GuiFactoryConstructionLifetime&) = delete;
    void Finish() noexcept;
    YtlReferenceSlotRuntime& Reference() noexcept { return slot_; }
private:
    YtlReferenceSlotRuntime slot_;
    bool finished_ = false;
};

using GuiPopupOpaqueRecord = std::array<std::byte, 12>;
using GuiPopupOpaqueStorage = EnvironmentVectorStorage<GuiPopupOpaqueRecord>;
void DestroyGuiPopupOpaqueStorageAt4F6A90(GuiPopupOpaqueStorage& storage) noexcept;

// 对应+8C/+98的独立三指针正文。原析构只释放capacity存储，不解释元素为Ref。
struct GuiPopupOpaqueRecords final
{
    GuiPopupOpaqueStorage storage{};
    ~GuiPopupOpaqueRecords() noexcept { DestroyGuiPopupOpaqueStorageAt4F6A90(storage); }
    GuiPopupOpaqueRecords() = default;
    GuiPopupOpaqueRecords(const GuiPopupOpaqueRecords&) = delete;
    GuiPopupOpaqueRecords& operator=(const GuiPopupOpaqueRecords&) = delete;
    void Clear() noexcept { storage.end = storage.begin; }
};
static_assert(sizeof(GuiPopupOpaqueRecords) == 12);
}
