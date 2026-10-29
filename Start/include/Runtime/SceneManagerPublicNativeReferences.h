#pragma once

#include <Runtime/ModeSelectButtonNative.h>
#include <Runtime/OptionCharSelectDialogNative.h>

namespace KaGuYa::Start::Runtime
{
// 借用原数组槽及同源服务；holder view同时借服务绑定，reset换源后不沿用旧来源。
// 复制view不增持，不缓存正文/count/stride；raw view的槽/来源配对由caller维护。
// 调用方按原循环比较有符号count；这里不追加index检查或空对象兜底。
class SceneManagerModeButtonNativeView final
{
public:
    explicit SceneManagerModeButtonNativeView(const YtlReferenceSlotRuntime* array,
        std::int32_t index, ModeSelectButtonNativeServices& services) noexcept;
    [[nodiscard]] CgUiButtonNativeBody6Ch* Get() const noexcept;
    [[nodiscard]] std::int32_t ProcessAt4A98E0(std::int32_t forced) const;
    [[nodiscard]] std::uint8_t DrawIntoAt4B5290(GraphicsSurfaceNativeBody* destination,
        const RECT* clip) const;
private:
    friend class SceneManagerModeButtonsNativeReference;
    SceneManagerModeButtonNativeView(const YtlReferenceSlotRuntime* array,
        std::int32_t index, ModeSelectButtonNativeServices* const* binding) noexcept;
    [[nodiscard]] ModeSelectButtonNativeServices& Services() const noexcept;
    const YtlReferenceSlotRuntime* array_;
    std::int32_t index_;
    ModeSelectButtonNativeServices* services_;
    ModeSelectButtonNativeServices* const* binding_;
};

// 外观不是原返回ABI；唯一拥有槽仍为Ref12，服务指针不拥有根或业务状态。
// borrowed必须是真实可读槽；47B7B0的地址4不改成空引用。
// services及其来源须比holder/view存活更久，跨线程时由接收线程使用同源服务。
class SceneManagerModeButtonsNativeReference final
{
public:
    SceneManagerModeButtonsNativeReference() noexcept = default;
    explicit SceneManagerModeButtonsNativeReference(const YtlReferenceSlotRuntime* borrowed,
        ModeSelectButtonNativeServices& services) noexcept;
    SceneManagerModeButtonsNativeReference(const SceneManagerModeButtonsNativeReference&) noexcept;
    SceneManagerModeButtonsNativeReference(SceneManagerModeButtonsNativeReference&&) noexcept;
    SceneManagerModeButtonsNativeReference& operator=(const SceneManagerModeButtonsNativeReference&);
    SceneManagerModeButtonsNativeReference& operator=(SceneManagerModeButtonsNativeReference&&);
    ~SceneManagerModeButtonsNativeReference() noexcept;

    // 普通reset异常可外传；析构/EH退役异常按noexcept终止，不重试释放。
    void Reset();
    void Reset(const YtlReferenceSlotRuntime* borrowed, ModeSelectButtonNativeServices& services);
    [[nodiscard]] const YtlReferenceSlotRuntime& RawReference() const noexcept { return reference_; }
    [[nodiscard]] explicit operator bool() const noexcept { return reference_.baseAt00 != 0; }
    [[nodiscard]] std::int32_t Count() const noexcept;
    [[nodiscard]] SceneManagerModeButtonNativeView BorrowAt(std::int32_t index) const & noexcept;
    SceneManagerModeButtonNativeView BorrowAt(std::int32_t) const && = delete;
private:
    YtlReferenceSlotRuntime reference_;
    ModeSelectButtonNativeServices* services_ = nullptr;
};

// 单一8Ch正文借用；每次从当时Ref和vptr分派，不绑定静态实现或复制选择状态。
class SceneManagerOptionCharNativeView final
{
public:
    explicit SceneManagerOptionCharNativeView(const YtlReferenceSlotRuntime* borrowed,
        OptionCharSelectDialogNativeDependencies& services) noexcept;
    [[nodiscard]] OptionCharSelectDialogNativeBody8Ch* Get() const noexcept;
    [[nodiscard]] std::int32_t ProcessAtVslot48() const;
    [[nodiscard]] std::int32_t SelectionAtVslot50() const;
    void ResetAtVslot90() const;
    void AttachDirtyRectsAtVslot94(const YtlReferenceSlotRuntime* borrowedDirty) const;
    void DrawAtVslot60(const RECT* clip) const;
    // 消费调用方已转入的一次参数引用；callee正常/EH均退役，不在caller二次释放。
    void CompositeOwnedAtVslot5C(YtlReferenceSlotRuntime ownedTarget, const RECT* clip) const;
    // 便捷借用口只为按值参数增持一次，不增持selector；参数由callee消费。
    void CompositeBorrowedAtVslot5C(const YtlReferenceSlotRuntime& target, const RECT* clip) const;
    // CG原4A819D..4A81AD：451500建立owning=0的原Surface参数，再交+5C消费。
    void CompositeSurfaceAtVslot5C(GraphicsSurfaceNativeBody* target, const RECT* clip) const;
private:
    friend class SceneManagerOptionCharNativeReference;
    SceneManagerOptionCharNativeView(const YtlReferenceSlotRuntime* borrowed,
        OptionCharSelectDialogNativeDependencies* const* binding) noexcept;
    [[nodiscard]] OptionCharSelectDialogNativeDependencies& Services() const noexcept;
    const YtlReferenceSlotRuntime* reference_;
    OptionCharSelectDialogNativeDependencies* services_;
    OptionCharSelectDialogNativeDependencies* const* binding_;
};

class SceneManagerOptionCharNativeReference final
{
public:
    SceneManagerOptionCharNativeReference() noexcept = default;
    explicit SceneManagerOptionCharNativeReference(const YtlReferenceSlotRuntime* borrowed,
        OptionCharSelectDialogNativeDependencies& services) noexcept;
    SceneManagerOptionCharNativeReference(const SceneManagerOptionCharNativeReference&) noexcept;
    SceneManagerOptionCharNativeReference(SceneManagerOptionCharNativeReference&&) noexcept;
    SceneManagerOptionCharNativeReference& operator=(const SceneManagerOptionCharNativeReference&);
    SceneManagerOptionCharNativeReference& operator=(SceneManagerOptionCharNativeReference&&);
    ~SceneManagerOptionCharNativeReference() noexcept;

    void Reset();
    void Reset(const YtlReferenceSlotRuntime* borrowed, OptionCharSelectDialogNativeDependencies& services);
    [[nodiscard]] const YtlReferenceSlotRuntime& RawReference() const noexcept { return reference_; }
    [[nodiscard]] explicit operator bool() const noexcept { return reference_.baseAt00 != 0; }
    [[nodiscard]] SceneManagerOptionCharNativeView Borrow() const & noexcept;
    SceneManagerOptionCharNativeView Borrow() const && = delete;
private:
    YtlReferenceSlotRuntime reference_;
    OptionCharSelectDialogNativeDependencies* services_ = nullptr;
};

// 已绑定外观各16字节：一份Ref12加一个非拥有来源，不是原对象字段或额外control。
static_assert(sizeof(SceneManagerModeButtonsNativeReference) == 0x10);
static_assert(sizeof(SceneManagerOptionCharNativeReference) == 0x10);
}
