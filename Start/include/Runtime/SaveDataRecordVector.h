#pragma once
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct SaveDataRecordAt10 final
{
    std::uint32_t valueAt00 = 0;
    std::uint32_t valueAt04 = 0;
    std::uint32_t valueAt08 = 0;
    std::uint32_t valueAt0C = 0;
    bool operator==(const SaveDataRecordAt10&) const = default;
};
static_assert(sizeof(SaveDataRecordAt10) == 0x10);
using SaveDataRecordStorage = EnvironmentVectorStorage<SaveDataRecordAt10>;
void ResizeSaveDataRecordsAt457A60(SaveDataRecordStorage& storage, std::uint32_t count,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void DestroySaveDataRecordsAt461590(SaveDataRecordStorage& storage,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// SaveData+10只有三指针，没有现代vector、额外allocator或重复记录表。
class SaveDataRecordVector final
{
public:
    SaveDataRecordVector() noexcept = default;
    ~SaveDataRecordVector() noexcept;
    SaveDataRecordVector(const SaveDataRecordVector&) = delete;
    SaveDataRecordVector& operator=(const SaveDataRecordVector&) = delete;
    void resize(std::uint32_t count) { ResizeSaveDataRecordsAt457A60(storage_, count); }
    void clear() noexcept { storage_.end = storage_.begin; }
    [[nodiscard]] std::uint32_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage_.begin == storage_.end; }
    [[nodiscard]] SaveDataRecordAt10& operator[](std::uint32_t index) noexcept { return storage_.begin[index]; }
    [[nodiscard]] const SaveDataRecordAt10& operator[](std::uint32_t index) const noexcept { return storage_.begin[index]; }
    [[nodiscard]] const SaveDataRecordStorage& Native() const noexcept { return storage_; }
    [[nodiscard]] bool operator==(const SaveDataRecordVector& other) const noexcept;
private:
    SaveDataRecordStorage storage_{};
};
static_assert(sizeof(SaveDataRecordVector) == 12);
}
