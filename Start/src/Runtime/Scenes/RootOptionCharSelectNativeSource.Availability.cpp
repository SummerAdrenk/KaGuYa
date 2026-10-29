#include "RootOptionCharSelectNativeSource.Internal.h"
#include <Runtime/SoundParameterNative.h>

namespace KaGuYa::Start::Runtime
{
std::uint8_t* BorrowRootOptionCharDisplayAt4787E0(EnvironmentSoundParamsArchive& receiver,
    std::int32_t index) noexcept
{
    return BorrowSoundCharacterDisplayAt4787E0(receiver,index);
}
}
