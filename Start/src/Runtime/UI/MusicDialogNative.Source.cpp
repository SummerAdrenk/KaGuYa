#include "MusicDialogNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace { thread_local MusicDialogNativeServices* current = nullptr; }
MusicDialogNativeScope::MusicDialogNativeScope(MusicDialogNativeServices& s) noexcept
    : previous_(current), surfaces_(s.surfaces), graphics_(s.graphics), input_(s.input), screen_(s.screen)
{ current = &s; }
MusicDialogNativeScope::~MusicDialogNativeScope() noexcept { current = previous_; }
MusicDialogNativeServices& RequireMusicDialogNativeServices()
{
    if (!current) throw std::logic_error("MusicDialog native services are not bound");
    return *current;
}
GuiDialogNativeBody5Ch* MusicDialogNativeConstruction::ConstructMusicDialogAt4E3900(void* storage)
{
    MusicDialogNativeScope scope(services_);
    return ConstructMusicDialogNativeAt4E3900(storage);
}
}
