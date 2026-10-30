#pragma once

#include <Runtime/GameBuildProfile.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <cstddef>
#include <span>

namespace KaGuYa::Start::Runtime::ero_zemi_ecchi_ni_yaru_ki_ni_abc
{
const GuiStaticDescriptorNative24h& SharedBackground() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 6> SharedButtons() noexcept;
const char* BgmName() noexcept;
const char* ClickCue() noexcept;
const char* HoverCue() noexcept;
const char* OverlayName() noexcept;
const char* BlackName() noexcept;
TitleNamePromptGate NameGate() noexcept;
std::size_t GivenNameMaxBytes() noexcept;
std::size_t FamilyNameMaxBytes() noexcept;
}
