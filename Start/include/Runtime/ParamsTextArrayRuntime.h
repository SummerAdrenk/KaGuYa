#pragma once

#include <Runtime/ParamsText.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <span>

namespace KaGuYa::Start::Runtime
{
enum class ParamsTextArrayAllocation { Elements, Control };

class IParamsTextArrayMemory
{
public:
    virtual ~IParamsTextArrayMemory() = default;
    // 使用现代ParamsText实际尺寸；不假称原4-byte CString布局或分配器ABI已经相同。
    virtual void* Allocate(ParamsTextArrayAllocation kind, std::size_t bytes) = 0;
    virtual void Deallocate(ParamsTextArrayAllocation kind, void* address, std::size_t bytes) noexcept = 0;
    virtual void Construct(ParamsText* element) = 0;
    virtual void Destroy(ParamsText* element) = 0;
};

IParamsTextArrayMemory& DefaultParamsTextArrayMemory();

class ParamsTextArrayControl : public YtlReferenceControlRuntime
{
public:
    std::uint32_t strideAt0C = sizeof(ParamsText);
    std::uint32_t countAt10 = 0;
    ParamsText* baseAt14 = nullptr;
protected:
    ~ParamsTextArrayControl() = default;
};
static_assert(sizeof(ParamsTextArrayControl) == 0x18);

struct ParamsTextArraySlot final
{
    std::uintptr_t baseAt00 = 0;
    std::uint32_t biasAt04 = 0;
    ParamsTextArrayControl* controlAt08 = nullptr;
};
static_assert(sizeof(ParamsTextArraySlot) == 12);

// 数组正文独立保活；不复制ParamsText，也不保活整个Pattern。
class ParamsTextArrayReference final
{
public:
    ParamsTextArrayReference() noexcept = default;
    ParamsTextArrayReference(const ParamsTextArrayReference& other) noexcept;
    ParamsTextArrayReference(ParamsTextArrayReference&& other) noexcept;
    ParamsTextArrayReference& operator=(const ParamsTextArrayReference& other);
    ParamsTextArrayReference& operator=(ParamsTextArrayReference&& other);
    ~ParamsTextArrayReference() noexcept;

    [[nodiscard]] ParamsText* Get() const noexcept;
    [[nodiscard]] const ParamsTextArraySlot& Slot() const noexcept { return slot_; }
    [[nodiscard]] static ParamsTextArrayReference Adopt(ParamsTextArraySlot& slot) noexcept;
    // 412870先释放旧Ref，signed<=0不分配；正文发布后才分配control。
    void RebuildAt412870(std::int32_t count, IParamsTextArrayMemory& memory);
    // 41288E/45FE3C在正文销毁后重读槽control；无control不清base/bias。
    void Reset();
    // 46EF95先撤销本局部的EH清理资格；正常路径使用保存control，抛错不再由析构重试。
    void ResetSavedControlAt46EFA1();
private:
    ParamsTextArraySlot slot_;
    bool releaseOnDestruction_ = true;
};

// profile值拷贝仍为深快照；运行时消费者必须显式Share引用，只有一份正文存储。
class ParamsTextArray final
{
public:
    ParamsTextArray() noexcept = default;
    ParamsTextArray(std::initializer_list<ParamsText> values);
    ParamsTextArray(const ParamsTextArray& other);
    ParamsTextArray(ParamsTextArray&& other) noexcept = default;
    ParamsTextArray& operator=(const ParamsTextArray& other);
    ParamsTextArray& operator=(ParamsTextArray&& other) = default;
    ParamsTextArray& operator=(std::initializer_list<ParamsText> values);
    bool operator==(const ParamsTextArray& other) const;

    void AssignSnapshot(std::span<const ParamsText> values);
    void RebuildAt412870(std::int32_t count, IParamsTextArrayMemory& memory = DefaultParamsTextArrayMemory());
    [[nodiscard]] ParamsTextArrayReference Share() const noexcept { return reference_; }
    [[nodiscard]] const ParamsTextArraySlot& Slot() const noexcept { return reference_.Slot(); }
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return size() == 0; }
    [[nodiscard]] ParamsText* begin() noexcept { return reference_.Get(); }
    [[nodiscard]] const ParamsText* begin() const noexcept { return reference_.Get(); }
    [[nodiscard]] ParamsText* end() noexcept;
    [[nodiscard]] const ParamsText* end() const noexcept;
    [[nodiscard]] ParamsText& operator[](std::size_t index);
    [[nodiscard]] const ParamsText& operator[](std::size_t index) const;
private:
    ParamsTextArrayReference reference_;
};
}
