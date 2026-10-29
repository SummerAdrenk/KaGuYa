#pragma once

#include <Runtime/ParamsText.h>
#include <Runtime/RuntimeCStringVector.h>

#include <cstddef>
#include <initializer_list>

namespace KaGuYa::Start::Runtime
{
// GS+F8：唯一的三指针 CString vector，无 cookie、Ref/control 或现代值镜像。
class ParamsSceneNameTable final
{
public:
    using value_type = RuntimeCString;
    using iterator = RuntimeCString*;
    using const_iterator = const RuntimeCString*;

    ParamsSceneNameTable() noexcept = default;
    ~ParamsSceneNameTable() noexcept;

    // 现代 fixture/profile 深快照；生产复制追加使用 Append，不移动源 CString。
    ParamsSceneNameTable(std::initializer_list<ParamsText> values);
    ParamsSceneNameTable(const ParamsSceneNameTable& other);
    ParamsSceneNameTable& operator=(const ParamsSceneNameTable& other);
    ParamsSceneNameTable(ParamsSceneNameTable&& other) noexcept;
    ParamsSceneNameTable& operator=(ParamsSceneNameTable&& other) noexcept;
    ParamsSceneNameTable& operator=(std::initializer_list<ParamsText> values);
    [[nodiscard]] bool operator==(const ParamsSceneNameTable& other) const;

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    [[nodiscard]] iterator begin() noexcept { return storage.begin; }
    [[nodiscard]] iterator end() noexcept { return storage.end; }
    [[nodiscard]] const_iterator begin() const noexcept { return storage.begin; }
    [[nodiscard]] const_iterator end() const noexcept { return storage.end; }
    [[nodiscard]] const_iterator cbegin() const noexcept { return begin(); }
    [[nodiscard]] const_iterator cend() const noexcept { return end(); }

    // [] 按 DWORD 索引回绕解址；at 保留现代显式越界保护，不冒充原异常路径。
    [[nodiscard]] RuntimeCString& operator[](std::size_t index) noexcept;
    [[nodiscard]] const RuntimeCString& operator[](std::size_t index) const noexcept;
    [[nodiscard]] RuntimeCString& at(std::size_t index);
    [[nodiscard]] const RuntimeCString& at(std::size_t index) const;

    // 41D74C..41D76A：缓存旧范围正序释放，成功后重读 begin 写 end；保留 capacity。
    // state22 -> terminate；不能用 Resize(0) 的缓存 begin 或 Destroy 的退容量代替。
    void clear() noexcept;
    void Append(const RuntimeCString& source,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    // 只导入夹具 cp932 字节，不保存 ParamsText 或重建 utf16。
    void push_back(const ParamsText& fixture);

    // 不内嵌分配器；Append 的注入分配须兼容默认存储释放。
    EnvironmentVectorStorage<RuntimeCString> storage{nullptr, nullptr, nullptr};
};
static_assert(sizeof(ParamsSceneNameTable) == 12);
static_assert(offsetof(ParamsSceneNameTable, storage) == 0);
}
