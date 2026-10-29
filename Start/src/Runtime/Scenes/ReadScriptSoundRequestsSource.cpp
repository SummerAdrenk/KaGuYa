#include <Runtime/ReadScriptSoundRequestsSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/DrawLayersView.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/TextWindowLayerRuntime.h>
#include <Runtime/TextWindowLayerNative.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
UnSaveDataRuntime* InlineUnsave(EnvironmentRootOwner& source, bool endFirst)
{
    const auto& slots = static_cast<const volatile EnvironmentVectorStorage<YtlReferenceSlotRuntime>&>(
        source.CaptureAt4180C0()->archivesAt30);
    std::uintptr_t begin, end;
    if (endFirst) { end = reinterpret_cast<std::uintptr_t>(slots.end); begin = reinterpret_cast<std::uintptr_t>(slots.begin); }
    else { begin = reinterpret_cast<std::uintptr_t>(slots.begin); end = reinterpret_cast<std::uintptr_t>(slots.end); }
    const auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(end - begin)) / 12);
    if (count <= 2) return nullptr;
    const auto& ref = *reinterpret_cast<const volatile YtlReferenceSlotRuntime*>(begin + 24U);
    const auto base = ref.baseAt00;
    if (!base) return nullptr;
    auto* const control = ref.controlAt08;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
    const auto bias = ref.biasAt04;
    return reinterpret_cast<UnSaveDataRuntime*>(base + bias * stride);
}
template<class T> T* CaptureArchive(EnvironmentRootOwner& source, std::uint32_t index)
{ return static_cast<T*>(ResolveEnvironmentArchiveAt417990(*source.CaptureAt4180C0(), index)); }
RuntimeCString* Mapping(UnSaveDataRuntime* body, std::uint32_t port)
{
    const auto begin = reinterpret_cast<std::uintptr_t>(static_cast<volatile UnSaveDataRuntime&>(*body).mappingsAt30_.begin);
    return reinterpret_cast<RuntimeCString*>(begin + port * 4U);
}
}
ReadScriptUnreadReceiver ReadScriptUnreadReceiver::Native(TextWindowLayerNativeBodyE8h* body) noexcept
{ return {body, [](void* p) { return static_cast<const volatile TextWindowLayerNativeBodyE8h*>(p)->unreadAt9A; }}; }
ReadScriptUnreadReceiver ReadScriptUnreadReceiver::Modern(FrameLayerItem* item) noexcept
{
    return {item, [](void* p) -> std::uint8_t {
        // 过渡producer保留旧安全差异；类型检查只发生在4AA5B0真正读取正文时。
        auto* window = dynamic_cast<TextWindowLayerRuntime*>(static_cast<FrameLayerItem*>(p));
        if (!window) throw std::logic_error("ReadScript modern Draw producer requires TextWin slot3");
        return window->TextState().unreadAt9A.value();
    }};
}
EnvironmentReadScriptSoundRequestsSource::EnvironmentReadScriptSoundRequestsSource(EnvironmentRootOwner& environment,
    ParamsRuntimeRootOwner& params, InputRootOwner& input, RootRuntimeOwner& root) noexcept
    : environment_(environment), params_(params), input_(input), root_(root) {}
std::int32_t EnvironmentReadScriptSoundRequestsSource::MappingCountInlineAt4A9202()
{
    const auto& mappings = static_cast<const volatile UnSaveDataRuntime&>(*InlineUnsave(environment_, false)).mappingsAt30_;
    const auto end = reinterpret_cast<std::uintptr_t>(mappings.end);
    const auto begin = reinterpret_cast<std::uintptr_t>(mappings.begin);
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(end - begin)) >> 2;
}
bool EnvironmentReadScriptSoundRequestsSource::MappingEmptyInlineAt4A9371(std::uint32_t port)
{ return Mapping(InlineUnsave(environment_, false), port)->GetLength() == 0; }
RuntimeCString* EnvironmentReadScriptSoundRequestsSource::CaptureMappingSlotInlineAt4A9505(std::uint32_t port)
{ return Mapping(InlineUnsave(environment_, true), port); }
ReadScriptSoundRequestsReceiver EnvironmentReadScriptSoundRequestsSource::CaptureManagerAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    auto& post = root_.RequirePublished().PostWindow();
    return {post.SoundWaitReceiverAt24(), post.SoundLeafReceiverAt24()};
}
ReadScriptUnreadReceiver EnvironmentReadScriptSoundRequestsSource::CaptureTextAfter413BE0At4A95A2()
{
    root_.EnsureCreatedAt413BE0();
    // 4A95A7..4A9615固定槽3 receiver；+9A仍到4AA5B0才读取。
    return ReadScriptUnreadReceiver::View(root_.RequirePublished().PostWindow().DrawLayersAt18().TextAtSlotThree());
}
bool EnvironmentReadScriptSoundRequestsSource::ReadKeyboardSixHeldAt4AA573()
{
    auto* const root = input_.CaptureAt418210();
    auto* const keyboard = ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(root->keyboardAt18);
    return KaGuYa::Start::Runtime::ReadKeyboardSixHeldAt4AA573(*keyboard);
}
std::uint32_t EnvironmentReadScriptSoundRequestsSource::ReadUnsaveFlagsAt14Via417990()
{ return static_cast<const volatile UnSaveDataRuntime&>(*CaptureArchive<UnSaveDataRuntime>(environment_, 2)).flagsAt14_; }
std::uint8_t EnvironmentReadScriptSoundRequestsSource::ReadSaveFlagsAt38Via417990()
{ return CaptureArchive<SaveDataEnvironmentArchive>(environment_, 1)->bytesAt34[4]; }
const ParamsGameSystem* EnvironmentReadScriptSoundRequestsSource::CaptureGameSystemAt418480()
{
    auto* const root = params_.CaptureAt418480();
    return ResolveEnvironmentObjectReference<ParamsGameSystem>(root->gameSystemAt04.Native());
}
const char* EnvironmentReadScriptSoundRequestsSource::CaptureMappingNameVia417990(std::uint32_t port)
{ return Mapping(CaptureArchive<UnSaveDataRuntime>(environment_, 2), port)->GetString(); }
SoundMappingParameterReceiver EnvironmentReadScriptSoundRequestsSource::CaptureParametersVia417990()
{
    auto* const save = CaptureArchive<SaveDataEnvironmentArchive>(environment_, 1);
    return SoundMappingParameterReceiver::RawNative(ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(save->soundParamsAt1C));
}
}
