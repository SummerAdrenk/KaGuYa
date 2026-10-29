#pragma once

#include <Runtime/DrawManagerNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/MessageWindowNativePane.h>
#include <Runtime/ScriptParamsNativeBody.h>

#include <span>

namespace KaGuYa::Start::Runtime
{
class IScrVmCommandErrorSink;
class IEnvironmentArchiveRuntime;

struct MessageWindowLayerNativeBody50h final
{
    LayerItemNativeBody14h layer;
    RECT boundsAt14;
    union { DrawLayerReferenceArray stylesAt24; };
    std::int32_t selectedAt30;
    std::uint8_t rememberedNameAt34;
    std::uint8_t unwrittenAt35[3];
    union { YtlReferenceSlotRuntime type1At38; };
    union { YtlReferenceSlotRuntime type2At44; };
    MessageWindowLayerNativeBody50h() noexcept {}
};
static_assert(sizeof(MessageWindowLayerNativeBody50h) == 0x50);
static_assert(offsetof(MessageWindowLayerNativeBody50h, boundsAt14) == 0x14);
static_assert(offsetof(MessageWindowLayerNativeBody50h, stylesAt24) == 0x24);
static_assert(offsetof(MessageWindowLayerNativeBody50h, selectedAt30) == 0x30);
static_assert(offsetof(MessageWindowLayerNativeBody50h, rememberedNameAt34) == 0x34);
static_assert(offsetof(MessageWindowLayerNativeBody50h, type1At38) == 0x38);
static_assert(offsetof(MessageWindowLayerNativeBody50h, type2At44) == 0x44);
static_assert(std::is_standard_layout_v<MessageWindowLayerNativeBody50h>);
static_assert(std::is_trivially_destructible_v<MessageWindowLayerNativeBody50h>);

// 固定root，不固定它的Params/Draw成员；4870F2跨498020仍使用此receiver。
struct MessageWindowNativeRootReceiver final
{
    void* object;
    ScriptParamsNativeBody* (*paramsAt47BDB0)(void*);
    DrawManagerNativeBody2Ch* (*drawManagerAt4175A0)(void*);
};
class IMessageWindowNativeRootSource
{
public:
    virtual ~IMessageWindowNativeRootSource() = default;
    virtual MessageWindowNativeRootReceiver CaptureAfterEnsureAt413BE0() = 0;
};
class IMessageWindowNativeStateSource : public IMessageWindowNativeRootSource,
    public IMessageWindowNativeNameSource
{
public:
    virtual std::uint8_t MessageWindowAllowedAt419290(DrawManagerNativeBody2Ch*) = 0;
    virtual const IEnvironmentArchiveRuntime* CaptureSlot0At4180C0() = 0;
    virtual std::int32_t ResolveAt498020(const IEnvironmentArchiveRuntime*, const char*) = 0;
    // 实际manager槽3、TextWin+70.base；不使用rememberedName或现代HasName快照。
    virtual std::uintptr_t CaptureTextWindowNameBaseAt70() = 0;
};
struct MessageWindowLayerNativeServices final
{
    MessageWindowNativePaneServices& panes;
    MessageWindowNativeConfigServices& config;
    IMessageWindowNativeStateSource& state;
    IScrVmCommandErrorSink& errors;
};
class MessageWindowLayerNativeScope final
{
public:
    explicit MessageWindowLayerNativeScope(MessageWindowLayerNativeServices&) noexcept;
    ~MessageWindowLayerNativeScope() noexcept;
    MessageWindowLayerNativeScope(const MessageWindowLayerNativeScope&) = delete;
    MessageWindowLayerNativeScope& operator=(const MessageWindowLayerNativeScope&) = delete;
private:
    MessageWindowLayerNativeServices* previous_;
    MessageWindowNativePaneScope panes_;
};

const LayerItemNativeFrameVtable& MessageWindowLayerNativeMethods() noexcept;
std::span<const MessageWindowStyleNativeDescriptor> MessageWindowNativeStylesSample18At601408() noexcept;
MessageWindowLayerNativeBody50h* ConstructMessageWindowLayerNativeAt459AB0(
    MessageWindowLayerNativeBody50h&, LayerListNativeBody18h*, MessageWindowLayerNativeServices&,
    std::span<const MessageWindowStyleNativeDescriptor> = MessageWindowNativeStylesSample18At601408());
MessageWindowLayerNativeBody50h* DeleteMessageWindowLayerNativeAt468D00(
    MessageWindowLayerNativeBody50h&, std::uint32_t flags) noexcept;
void DrawMessageWindowLayerNativeAt482A10(MessageWindowLayerNativeBody50h&,
    const RECT*, MessageWindowLayerNativeServices&);
void CompositeMessageWindowLayerNativeAt4819C0(MessageWindowLayerNativeBody50h&,
    GraphicsSurfaceNativeBody*, const RECT*, MessageWindowLayerNativeServices&);
std::uint8_t RequestMessageWindowLayerNativeAt487030(MessageWindowLayerNativeBody50h&,
    std::uint8_t requested, std::int32_t unused, MessageWindowLayerNativeServices&);
void ResetMessageWindowLayerNativeAt47ED90(MessageWindowLayerNativeBody50h&) noexcept;
void RefreshMessageWindowStyleNativeAt5326F0(MessageWindowStyleNativeBody34h&);
void RefreshMessageWindowLayerNativeAt5325F0(MessageWindowLayerNativeBody50h&, std::int32_t index);
// TextWin414C3C..414CFB：非法selector改0，但空vector不能改用外层type1。
MessageWindowNativeConfig14h* ResolveMessageWindowTextConfigNativeAt414C3C(
    MessageWindowLayerNativeBody50h&) noexcept;
MessageWindowLayerNativeBody50h* CreateAndAppendMessageWindowLayerNative(
    IDrawManagerNativeRootSource&, MessageWindowLayerNativeServices&);
}
