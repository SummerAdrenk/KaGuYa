#include "SaveLoadPageSecondNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* EnvironmentSaveLoadPageSecondNativeSource::CaptureEnvironmentAt4180C0()
{ return owner_.CaptureAt4180C0(); }
SaveFileStateNativeArchive* EnvironmentSaveLoadPageSecondNativeSource::SaveAt417990(
    EnvironmentRootStorage* environment, std::uint32_t slot)
{
    using namespace SaveLoadPageSecondNativeDetail;
    auto* begin = Read(environment->archivesAt30.begin);
    const auto bytes = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Read(environment->archivesAt30.end)) - reinterpret_cast<std::uintptr_t>(begin)));
    if (slot >= static_cast<std::uint32_t>(bytes / 12)) return nullptr;
    return Resolve<SaveFileStateNativeArchive>(begin[slot]); // 原417990没有动态类型检查或catalog转换。
}
}
