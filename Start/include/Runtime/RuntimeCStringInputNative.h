#pragma once

#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
// 420510的显式来源边界；false表示miss且不改目标，异常保留已发生的副作用。
// 默认入口复用现有ATL资源叶，不声称已验证原61A698 Module38注册图。
class IRuntimeCStringInputResourceSource
{
public:
    virtual ~IRuntimeCStringInputResourceSource() = default;
    [[nodiscard]] virtual bool LoadAt420510(RuntimeCString& destination, std::uint32_t id) = 0;
};

// 40B530：fresh必须是未构造的4-byte槽，成功返回fresh才向caller交付清理资格。
// GetNil发布前不清输出；之后失败清当前槽，展开Free再抛则终止，不回滚/补nil。
RuntimeCString* ConstructRuntimeCStringInputNativeAt40B530(RuntimeCString* fresh,
    const char* input, IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 同名注入重载必须显式提供两个source；只调用resources，不尝试默认fallback。
RuntimeCString* ConstructRuntimeCStringInputNativeAt40B530(RuntimeCString* fresh,
    const char* input, IRuntimeStringManagerSource& strings,
    IRuntimeCStringInputResourceSource& resources);
}
