#pragma once

#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
// 47A762、47A85D/47A932、47A9D8：正常释放可抛，不能由现代 ATL 的 throw() 提前终止。
class RuntimeCStringTemporary final
{
public:
    RuntimeCStringTemporary(const char* source, int length, ATL::IAtlStringMgr* manager)
        : storage_(manager)
    {
        // legacy四处调用40B530：先取得共享nil，40B5A8再调用437680赋值。
        // 不能使用ATL的ptr/length构造，否则空串也Allocate(0)，丢失原nil语义。
        // 此入口只接已校验的负载字符串，不冒充40B530的低位资源ID分支。
        try { storage_.value.SetString(source, length); }
        catch (...)
        {
            // 590F30展开清理已构造字符串；再次抛出不能替换正在展开的首次异常。
            [](RuntimeCString* value) noexcept { DestroyRuntimeCStringAt40CD40(value); }(&storage_.value);
            throw;
        }
    }
    ~RuntimeCStringTemporary() noexcept(false) { DestroyRuntimeCStringAt40CD40(&storage_.value); }
    RuntimeCStringTemporary(const RuntimeCStringTemporary&) = delete;
    RuntimeCStringTemporary& operator=(const RuntimeCStringTemporary&) = delete;
    RuntimeCStringTemporary(RuntimeCStringTemporary&&) = delete;
    RuntimeCStringTemporary& operator=(RuntimeCStringTemporary&&) = delete;

    [[nodiscard]] const RuntimeCString& Value() const noexcept { return storage_.value; }
    [[nodiscard]] const char* GetString() const noexcept { return storage_.value.GetString(); }
    [[nodiscard]] bool IsEmpty() const noexcept { return storage_.value.IsEmpty(); }

private:
    // 命名存储的空析构禁止成员自动析构；原始释放抛出也不再次减持。
    // 不用直接在外层初始化的匿名union，否则外层构造抛出仍会自动析构活跃成员。
    // 不增加额外状态或分配。
    // 正常释放的首次异常交给调用者；展开清理中再次抛出由 C++ 终止。
    union Storage
    {
        explicit Storage(ATL::IAtlStringMgr* manager) : value(manager) {}
        ~Storage() noexcept {}
        RuntimeCString value;
    } storage_;
};
static_assert(sizeof(RuntimeCStringTemporary) == 4);
static_assert(alignof(RuntimeCStringTemporary) == alignof(RuntimeCString));
}
