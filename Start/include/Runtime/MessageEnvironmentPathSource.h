#pragma once

#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
// 现代生产装配口：每次操作重新取得同根Environment+48，不缓存payload地址。
// 原生CMessage的34h正文/无参虚析构不得嵌入此service；完整全局根锁另行迁移。
class IMessageEnvironmentPathSource
{
public:
    virtual ~IMessageEnvironmentPathSource() = default;
    [[nodiscard]] virtual const RuntimeCString& CaptureMessagePrefixAt45A5D0() const = 0;
    [[nodiscard]] virtual const RuntimeCString& CaptureUnreadPrefixAt43D3D0() const = 0;
};
}
