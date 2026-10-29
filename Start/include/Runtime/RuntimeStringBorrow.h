#pragma once

#include <concepts>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
// 现代消费者借用实际字符串对象，逐次取数据，不缓存写时分离之前的字符指针。
// 不拥有副本，也不冒充四字节CString布局；此头不向声音/窗口接口引入ATL/COM类型。
class RuntimeStringBorrow final
{
public:
    template<class CString>
        requires requires(const CString& value) {
            { value.GetString() } -> std::convertible_to<const char*>;
            { value.GetLength() } -> std::convertible_to<int>;
        }
    RuntimeStringBorrow(const CString& value) noexcept : object_(&value), read_([](const void* p) {
        const auto& text = *static_cast<const CString*>(p);
        return std::string_view(text.GetString(), static_cast<std::size_t>(text.GetLength())); }),
        type_(&Type<CString>) {}
    RuntimeStringBorrow(const std::string& value) noexcept : object_(&value), read_([](const void* p) {
        return std::string_view(*static_cast<const std::string*>(p)); }), type_(&Type<std::string>) {}
    RuntimeStringBorrow(const char* value) noexcept : object_(value), read_([](const void* p) {
        return std::string_view(static_cast<const char*>(p)); }), type_(nullptr) {}
    [[nodiscard]] std::string_view View() const noexcept { return read_(object_); }
    [[nodiscard]] std::string Snapshot() const { return std::string(View()); }
    [[nodiscard]] const void* Object() const noexcept { return object_; }
    // 显式类型标记仅属于现代借用器；不根据字符内容或函数地址猜测 CString。
    // 用于保留原路径 CString 的 manager，不给原生对象/Ref/control 添加字段。
    template<class String>
    [[nodiscard]] const String* GetIf() const noexcept
    { return type_ == &Type<String> ? static_cast<const String*>(object_) : nullptr; }
private:
    template<class String> inline static char Type;
    const void* object_;
    std::string_view (*read_)(const void*);
    const void* type_;
};
}
