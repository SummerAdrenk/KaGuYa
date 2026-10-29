#pragma once
#include <Runtime/ToolDialogNative.h>
#include <Runtime/WaitMarkNative.h>
#include <Runtime/SelectDialogNative.h>
#include <Runtime/OpacityDialogNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
struct StartupResourceProfile final
{
    const ToolDialogNativeProfile& tool;
    const ToolDialogNativeRelation& toolRelation;
    const WaitMarkNativeProfile& wait;
    const SelectDialogNativeProfile& selection;
    const OpacityDialogNativeProfile& opacity;
};
inline const StartupResourceProfile& RequireConfiguredStartupResources(const StartupResourceProfile* value)
{
    if (!value) throw std::logic_error("Configured startup resource data is missing");
    return *value;
}
}
