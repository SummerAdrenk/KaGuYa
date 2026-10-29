#pragma once

#include <Runtime/TextWindowLayerNative.h>
#include <Runtime/MessageWindowNativeConfig.h>

namespace KaGuYa::Start::Runtime
{
// CTextWin四项与CSelectWin三项的同序颜色叶；只共享已核对的捕获/Ref/EH协议。
void TryInitializeWindowNativeColor(YtlReferenceSlotRuntime& destination, const char* name,
    ITextWindowNativeColorSource& source, IEnvironmentRootMemory& memory);
// 45DCF5..45E05E：四个成员Ref已由E8h构造清零；此口不重复清槽或接管外层构造。
// 每组独立catch，正常tag尾不清字段且重读control；EH按当前槽清理。
void InitializeTextWindowNativeColorsAt45DCF5(TextWindowLayerNativeBodyE8h& body,
    ITextWindowNativeColorSource& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// E8h父层只借此初始化边界；四Ref的初始清零、Surface及外层catch仍由父层负责。
class ITextWindowNativeColorInitialization
{
public:
    virtual ~ITextWindowNativeColorInitialization() = default;
    virtual void InitializeAt45DCF5(TextWindowLayerNativeBodyE8h& body, IEnvironmentRootMemory& memory) = 0;
};
class TextWindowNativeColorInitialization final : public ITextWindowNativeColorInitialization
{
public:
    explicit TextWindowNativeColorInitialization(ITextWindowNativeColorSource& source) noexcept;
    void InitializeAt45DCF5(TextWindowLayerNativeBodyE8h& body, IEnvironmentRootMemory& memory) override;
private:
    ITextWindowNativeColorSource& source_;
};

// 418480/47BCF0/477E90的稳定借用代理；只借服务，不缓存正文、reader或Ref。
// 4797C0表达式内部原生化范围由显式expressions决定，不伪装现代解析器为原生叶。
class TextWindowNativeColorSource final : public ITextWindowNativeColorSource
{
public:
    TextWindowNativeColorSource(ParamsRuntimeRootOwner& params,
        ISettingTagErrorSource& readers, IMessageWindowNativeConfigExpressions& expressions) noexcept;
    ParamsGameSystem* CaptureGameSystemAt418480() override;
    RuntimeCString* CaptureReaderAt47BCF0() override;
    void ReadChildAt477E90(RuntimeCString* reader, YtlReferenceSlotRuntime& out,
        const YtlReferenceSlotRuntime& parent, const char* key) override;
    std::uint32_t ReadRgbAt4797C0(RuntimeCString* reader,
        const YtlReferenceSlotRuntime& tag, const char* key) override;
private:
    ParamsRuntimeRootOwner& params_;
    ISettingTagErrorSource& readers_;
    IMessageWindowNativeConfigExpressions& expressions_;
};
}
