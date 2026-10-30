#include <Runtime/SceneManagerRuntime.h>

#include <stdexcept>
#include <cstring>
#include <limits>

namespace KaGuYa::Start::Runtime
{
SceneManagerRuntime::SceneManagerRuntime(
    ISceneConstructionServices& sceneServices,
    ISceneManagerPublicDialogFactory& publicDialogFactory,
    IRuntimeStringManagerSource& strings)
    : sceneFactory_(sceneServices),
      dispatch_(sceneFactory_),
      pendingCommandAt1C_([&](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, strings); }),
      publicDialogs_(publicDialogFactory)
{
    InitializeEmptyRuntimeCString(demoNameAt3C_.value, strings);
}

SceneManagerRuntime::~SceneManagerRuntime() noexcept
{
    // 463B16..463B36 / FuncInfo flags=5：Free 抛出即终止，不继续释放其它成员。
    DestroyRuntimeCStringAt40CD40(&demoNameAt3C_.value);
}

SceneManagerDispatchRuntime& SceneManagerRuntime::Dispatch() noexcept
{
    return dispatch_;
}

const SceneManagerDispatchRuntime& SceneManagerRuntime::Dispatch() const noexcept
{
    return dispatch_;
}

SceneManagerPublicDialogRuntime& SceneManagerRuntime::PublicDialogs() noexcept
{
    return publicDialogs_;
}

const SceneManagerPublicDialogRuntime& SceneManagerRuntime::PublicDialogs() const noexcept
{
    return publicDialogs_;
}

std::string_view SceneManagerRuntime::ReadPendingCommandAt1C() const noexcept
{
    return RuntimeStringView(pendingCommandAt1C_.Value());
}

const RuntimeCString& SceneManagerRuntime::PendingCommandSlotAt1C() const noexcept
{
    return pendingCommandAt1C_.Value();
}

RuntimeCStringResult SceneManagerRuntime::ClonePendingCommandAt414540() const
{
    return RuntimeCStringResult([&](RuntimeCString* out) {
        auto* header = reinterpret_cast<ATL::CStringData*>(
            const_cast<char*>(pendingCommandAt1C_.Value().GetString())) - 1;
        auto* copy = CloneRuntimeCStringDataAt414540(header);
        const auto* chars = reinterpret_cast<const char*>(copy + 1);
        // Clone引用交给隐藏返回槽，不经CString赋值或额外析构。
        std::memcpy(static_cast<void*>(out), &chars, sizeof(chars));
    });
}

void SceneManagerRuntime::WritePendingCommandAt1C(const std::string_view commandCp932)
{
    if (commandCp932.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("SceneManager command exceeds signed CString length");
    SetRuntimeCStringAt437680(pendingCommandAt1C_.Value(), commandCp932.empty() ? "" : commandCp932.data(),
        static_cast<int>(commandCp932.size()));
}

void SceneManagerRuntime::PopThenActivatePendingAt4C1760()
{
    dispatch_.Controller().PopThenActivatePendingAt4C1760();
}

const RuntimeCString& SceneManagerRuntime::DemoNameSlotAt3C() const noexcept
{
    return demoNameAt3C_.value;
}

void SceneManagerRuntime::SetDemoNameAt4D6820(const char* const value)
{
    const auto length = value ? static_cast<int>(std::strlen(value)) : 0;
    SetRuntimeCStringAt437680(demoNameAt3C_.value, value, length);
}

std::int32_t SceneManagerRuntime::ReadSceneMarkerAt38() const
{
    if (!sceneMarkerAt38_)
        throw std::logic_error("CSceneManager +38 has not been written before ReadScript restore");
    return *sceneMarkerAt38_;
}

void SceneManagerRuntime::WriteSceneMarkerAt38(const std::int32_t value) noexcept
{
    sceneMarkerAt38_ = value;
}

std::shared_ptr<ICgDisplayButtonGroupSelector>
SceneManagerRuntime::AcquireSelectorAt478A30()
{
    const auto& owner = publicDialogs_.OptionCharSelectDialogOwnerAt2C();
    if (!owner)
        throw std::runtime_error("CSceneManager option-char dialog is not installed");

    auto selector = std::dynamic_pointer_cast<ICgDisplayButtonGroupSelector>(owner);
    if (!selector)
    {
        throw std::runtime_error(
            "CSceneManager option-char dialog does not implement the CG selector ABI");
    }
    return selector;
}

std::vector<std::shared_ptr<CgUiButtonRuntime>>
SceneManagerRuntime::AcquireModeButtonsAt47B7B0()
{
    const auto& owner = publicDialogs_.ModeSelectButtonOwnerAt20();
    if (!owner)
        throw std::runtime_error("CSceneManager mode-select buttons are not installed");
    return owner->AcquireButtonsAt47B7B0();
}
}
