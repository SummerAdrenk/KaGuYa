#include <Runtime/SoundManagerNativeOpen.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t HandleAt(SoundManagerNativeBody50h& body, std::uint32_t byteOffset)
{
    const auto base = reinterpret_cast<std::uintptr_t>(static_cast<volatile SoundManagerNativeBody50h&>(body).slotsBeginAt04);
    return *reinterpret_cast<const volatile std::int32_t*>(base + byteOffset);
}
}
SoundManagerNativeOpen::SoundManagerNativeOpen(IArchiveResourceSource& archives,
    SoundModuleNativeApi& sound, IRuntimeWideStringNativeApi& wide, IRuntimeStringManagerSource& strings) noexcept
    : archives_(archives), sound_(sound), wide_(wide), strings_(strings) {}

bool SoundManagerNativeOpen::OpenAt42BC90(SoundManagerNativeBody50h& body, std::uint32_t slot,
    const char* name, std::int32_t value, std::int32_t loop, std::uint8_t noAutoPlay)
{
    const auto query = archives_.CaptureCatalogAt413BE0().ResolveForReadBorrowed(name, strings_);
    if (!query || query->Flags() != 0) return false; // 原VFS拒绝早于任何物理槽访问。
    const auto byteOffset = slot * 8U;
    const auto length = query->Length();
    // 原42BD32只取offset低DWORD，不对finfo的高DWORD加现代溢出拒绝。
    const auto offset = static_cast<std::uint32_t>(query->Offset());
    const auto handle = HandleAt(body, byteOffset);
    const char* const path = query->Path();
    const auto* const capturedModule = sound_.CaptureAt42BD45();
    RuntimeWideStringNative logical(name, wide_);
    RuntimeWideStringNative physical(path, wide_);
    const bool success = sound_.OpenCapturedAt42BD8F(*capturedModule, handle, logical.Characters(),
        path ? physical.Characters() : nullptr, std::bit_cast<std::int32_t>(offset), std::bit_cast<std::int32_t>(length));
    physical.ReleasePhysicalNormal();
    logical.ReleaseNormal(); // 42BE75先撤state0，logical正常Free失败不重试。
    if (!success) return false;
    sound_.SetVolume(HandleAt(body, byteOffset), 100, value);
    sound_.SetLoopAt42BEBF(HandleAt(body, byteOffset), loop);
    if (!noAutoPlay)
    {
        sound_.PlayHandleAt438090(HandleAt(body, byteOffset));
        auto* const target = reinterpret_cast<RuntimeCString*>(reinterpret_cast<std::uintptr_t>(
            static_cast<volatile SoundManagerNativeBody50h&>(body).slotsBeginAt04) + byteOffset + 4U);
        const auto bytes = name ? static_cast<std::uint32_t>(std::strlen(name)) : 0U;
        SetRuntimeCStringAt437680(*target, name, std::bit_cast<std::int32_t>(bytes));
    }
    return true;
}
}
