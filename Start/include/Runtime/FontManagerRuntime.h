#pragma once

#include <Runtime/FontManagerNative.h>
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class IFontResourcePlatformApi
{
public:
    virtual ~IFontResourcePlatformApi() = default;
    virtual BOOL RemoveFontResource(const char* name, DWORD flags, void* reserved) = 0;
};

class Win32FontResourcePlatformApi final : public IFontResourcePlatformApi
{
public:
    BOOL RemoveFontResource(const char* name, DWORD flags, void* reserved) override;
};

// 489C90:48AC96..48AD46：两张空 Ref 表；未发现样本18的填表/字体注册入口。
// 显式现代栈owner外壳，唯一业务状态为1Ch native正文；根原Ref接入仍待迁移。
class FontManagerRuntime final
{
public:
    explicit FontManagerRuntime(IFontResourcePlatformApi& platform) noexcept;
    ~FontManagerRuntime() noexcept;
    FontManagerRuntime(const FontManagerRuntime&) = delete;
    FontManagerRuntime& operator=(const FontManagerRuntime&) = delete;

    // 现代显式转移一个已有引用；分配成功才接管，不声称是原业务填表函数。
    void AdoptResourceReference(FontResourceReferenceSlotRuntime reference);
    void AdoptSecondaryReference(YtlReferenceSlotRuntime reference);
    [[nodiscard]] std::span<FontResourceReferenceSlotRuntime> ResourceReferences() noexcept
        { return body_.ResourceReferences(); }
    [[nodiscard]] std::span<YtlReferenceSlotRuntime> SecondaryReferences() noexcept
        { return body_.SecondaryReferences(); }
    [[nodiscard]] FontManagerNativeBody1Ch& NativeBody() noexcept { return body_; }

private:
    IFontResourcePlatformApi& platform_;
    FontManagerNativeBody1Ch body_;
    // 仅约束现代Adopt口：原462140之后此成员生命周期已经结束。
    bool secondaryStorageDestroyed_ = false;
};
}
