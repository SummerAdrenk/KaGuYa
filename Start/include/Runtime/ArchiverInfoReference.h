#pragma once

#include <Runtime/YtlReferenceArrayRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ArchiverInfoRuntime;

// 4590DA/460030：正文与24-byte控制块分别分配，Ref只保存原三个槽。
class ArchiverInfoReference final
{
public:
    ArchiverInfoReference() noexcept = default;
    ArchiverInfoReference(const ArchiverInfoReference& source) noexcept;
    ArchiverInfoReference(ArchiverInfoReference&& source) noexcept;
    ArchiverInfoReference& operator=(const ArchiverInfoReference& source);
    ArchiverInfoReference& operator=(ArchiverInfoReference&& source);
    ~ArchiverInfoReference() noexcept;

    [[nodiscard]] ArchiverInfoRuntime* get() const noexcept;
    [[nodiscard]] ArchiverInfoRuntime* operator->() const noexcept { return get(); }
    [[nodiscard]] explicit operator bool() const noexcept { return get() != nullptr; }
    void Reset();

    // 正文已经构造完成；控制块分配失败时不得回收正文及其progress引用。
    [[nodiscard]] static ArchiverInfoReference AdoptAt4590DA(ArchiverInfoRuntime* created);

    YtlReferenceSlotRuntime reference;
};
static_assert(sizeof(ArchiverInfoReference) == 12);
}
