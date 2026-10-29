#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 48F980只访问控制块这两个虚槽及+04/+08；不要求知道所持正文的业务类型。
// 刻意不设虚析构：原slot04接收delete flag，与正文释放slot00分开。
class YtlReferenceControlRuntime
{
public:
    YtlReferenceControlRuntime() noexcept : referenceCountAt04(1), owningAt08(1) {}
    virtual void DestroyPayloadAtVslot00() = 0;
    virtual void DeleteControlAtVslot04(std::uint32_t flags) = 0;
    std::uint32_t referenceCountAt04;
    std::uint8_t owningAt08;
    // 原构造只写+08一个byte；+09..+0B不是业务字段，分配/placement构造不得清零它们。
    std::uint8_t unwrittenAt09[3];
protected:
    // 412870 先发布 control，再初始化引用字段；仅供显式分阶段构造使用。
    struct UninitializedFieldsTag final {};
    explicit YtlReferenceControlRuntime(UninitializedFieldsTag) noexcept {}
    ~YtlReferenceControlRuntime() = default;
};

struct YtlReferenceSlotRuntime final
{
    std::uintptr_t baseAt00 = 0;
    std::uint32_t biasAt04 = 0;
    YtlReferenceControlRuntime* controlAt08 = nullptr;
};
static_assert(sizeof(YtlReferenceSlotRuntime) == 12, "YTL reference slots require x86");

// 451767..451795：释放异常外传，回调结束后才清槽；无control时不清base/bias。
void ReleaseYtlReference(YtlReferenceSlotRuntime& reference);

// 按原范围正序释放。元素销毁异常终止，但46D170此前的Graphics异常仍可外传。
void ReleaseYtlReferencesAt48F980(std::span<YtlReferenceSlotRuntime> references) noexcept;

class YtlReferenceArrayRuntime final
{
public:
    ~YtlReferenceArrayRuntime();
    YtlReferenceArrayRuntime() = default;
    YtlReferenceArrayRuntime(const YtlReferenceArrayRuntime&) = delete;
    YtlReferenceArrayRuntime& operator=(const YtlReferenceArrayRuntime&) = delete;

    // 现代显式所有权转移口，不声称是已逆出的TextWin业务填充入口。
    // 成功才接收调用方持有的一个引用计数；分配失败仍由调用方负责。
    void AdoptReference(YtlReferenceSlotRuntime reference);
    void ClearPreservingStorage() noexcept;
    void DestroyStorage() noexcept;
    [[nodiscard]] std::span<YtlReferenceSlotRuntime> References() noexcept { return references_; }
    [[nodiscard]] std::span<const YtlReferenceSlotRuntime> References() const noexcept { return references_; }
    [[nodiscard]] std::size_t Capacity() const noexcept { return references_.capacity(); }
private:
    std::vector<YtlReferenceSlotRuntime> references_;
};
}
