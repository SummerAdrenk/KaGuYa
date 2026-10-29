#pragma once

#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
// 原隐藏返回槽：producer成功返回后，调用方才取得40CD40清理资格。
// producer已写返回槽但在释放自身临时串时抛出，不能替它回收返回槽。
class RuntimeCStringResult final
{
public:
    template<class Producer>
    explicit RuntimeCStringResult(Producer&& producer) { producer(&storage_.value); }
    ~RuntimeCStringResult() noexcept(false) { DestroyRuntimeCStringAt40CD40(&storage_.value); }
    RuntimeCStringResult(const RuntimeCStringResult&) = delete;
    RuntimeCStringResult& operator=(const RuntimeCStringResult&) = delete;
    RuntimeCStringResult(RuntimeCStringResult&&) = delete;
    RuntimeCStringResult& operator=(RuntimeCStringResult&&) = delete;

    [[nodiscard]] RuntimeCString& Value() noexcept { return storage_.value; }
    [[nodiscard]] const RuntimeCString& Value() const noexcept { return storage_.value; }

private:
    union Storage
    {
        Storage() noexcept {}
        ~Storage() noexcept {}
        RuntimeCString value;
    } storage_;
};
static_assert(sizeof(RuntimeCStringResult) == 4);
}
