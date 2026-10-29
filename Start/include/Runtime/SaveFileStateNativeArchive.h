#pragma once

#include <Runtime/SaveFileNativePages.h>
#include <Runtime/SystemDataEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime
{
// 47E441..47E49B：44h正文只有原两虚槽和三条拥有链，不驻留scan诊断或现代catalog。
// 尚未扫描时+04..1C保持未写；只可在对应原写入后读取标量。
class SaveFileStateNativeArchive final : public IEnvironmentArchiveRuntime
{
public:
    static constexpr std::size_t OriginalObjectSize = 0x44;
    SaveFileStateNativeArchive() noexcept;
    ~SaveFileStateNativeArchive() noexcept;
    SaveFileStateNativeArchive(const SaveFileStateNativeArchive&) = delete;
    SaveFileStateNativeArchive& operator=(const SaveFileStateNativeArchive&) = delete;
    void Serialize(SerializeBuffer&) noexcept;
    void SerializeNative18(void* serializer18) noexcept override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    [[nodiscard]] SaveFileReferenceView<SaveFileNativePage> RegularPages() noexcept { return SaveFileReferenceView<SaveFileNativePage>(regularPagesAt20); }
    [[nodiscard]] SaveFileReferenceView<const SaveFileNativePage> RegularPages() const noexcept { return SaveFileReferenceView<const SaveFileNativePage>(regularPagesAt20); }
    [[nodiscard]] SaveFileNativePage& QuickPage() noexcept { return *ResolveEnvironmentObjectReference<SaveFileNativePage>(quickPageAt2C); }
    [[nodiscard]] const SaveFileNativePage& QuickPage() const noexcept { return *ResolveEnvironmentObjectReference<SaveFileNativePage>(quickPageAt2C); }
    [[nodiscard]] SaveFileNativePage& AutoPage() noexcept { return *ResolveEnvironmentObjectReference<SaveFileNativePage>(autoPageAt38); }
    [[nodiscard]] const SaveFileNativePage& AutoPage() const noexcept { return *ResolveEnvironmentObjectReference<SaveFileNativePage>(autoPageAt38); }

    std::int32_t quickLatestItemAt04;
    std::int32_t autoLatestItemAt08;
    std::int32_t regularSelectedPageAt0C;
    std::int32_t regularDisplayedPageAt10;
    std::uint32_t regularPageStepAt14;
    std::int32_t regularLatestPageAt18;
    std::int32_t regularLatestItemAt1C;
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> regularPagesAt20;
    YtlReferenceSlotRuntime quickPageAt2C;
    YtlReferenceSlotRuntime autoPageAt38;
};
static_assert(sizeof(SaveFileStateNativeArchive) == 0x44);
static_assert(offsetof(SaveFileStateNativeArchive, quickLatestItemAt04) == 4);
static_assert(offsetof(SaveFileStateNativeArchive, regularPagesAt20) == 0x20);
static_assert(offsetof(SaveFileStateNativeArchive, quickPageAt2C) == 0x2C);
static_assert(offsetof(SaveFileStateNativeArchive, autoPageAt38) == 0x38);

void RegisterSaveFileStateArchiveAt47E441(EnvironmentRootOwner& owner,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
