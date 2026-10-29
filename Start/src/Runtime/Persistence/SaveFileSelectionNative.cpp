#include <Runtime/SaveFileSelectionNative.h>
#include <Runtime/SaveFileStateNativeArchive.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
SaveFileStateNativeArchive* Capture(ISaveFileDeleteFactoryNativeSource& source)
{
    auto* env = source.CaptureEnvironmentAt4180C0();
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(env->archivesAt30.end) -
        reinterpret_cast<std::uintptr_t>(env->archivesAt30.begin));
    const auto count = std::uint32_t(std::bit_cast<std::int32_t>(bytes) / 12);
    return count > 6 ? ResolveEnvironmentObjectReference<SaveFileStateNativeArchive>(env->archivesAt30.begin[6]) : nullptr;
}
const SaveFileNativeTime* Time(const YtlReferenceSlotRuntime& pageRef, std::int32_t index) noexcept
{
    auto* page = ResolveEnvironmentObjectReference<SaveFileNativePage>(pageRef);
    const auto address = reinterpret_cast<std::uintptr_t>(page->itemsAt04.begin) + std::uint32_t(index) * 12u;
    auto* item = ResolveEnvironmentObjectReference<SaveFileNativeItem>(*reinterpret_cast<const YtlReferenceSlotRuntime*>(address));
    return ResolveEnvironmentObjectReference<SaveFileNativeTime>(item->timeAt00);
}
bool Later(const SaveFileNativeTime& candidate, const SaveFileNativeTime& previous) noexcept
{
    // 48D701有符号比较高DWORD；相同才于48D70F无符号比较低DWORD，等时保留先页。
    const auto high = std::bit_cast<std::int32_t>(candidate.highAt04);
    const auto oldHigh = std::bit_cast<std::int32_t>(previous.highAt04);
    return high > oldHigh || (high == oldHigh && candidate.lowAt00 > previous.lowAt00);
}
}
SaveFileStateNativeArchive* RefreshSaveFileSelectionNativeAt48D290(ISaveFileDeleteFactoryNativeSource& source)
{
    Capture(source)->regularLatestItemAt1C = -1;
    Capture(source)->regularLatestPageAt18 = -1;
    auto* at = Capture(source)->regularPagesAt20.begin;
    SaveFileNativeTime latest{0, 0};
    for (;; ++at)
    {
        // 原每轮都重新锁/捕获Env并读当前end，不缓存初始catalog或数组尺寸。
        auto* catalog = Capture(source);
        if (at == catalog->regularPagesAt20.end) return catalog;
        auto page = *at;
        if (page.controlAt08) ++page.controlAt08->referenceCountAt04;
        const auto index = FindLatestSaveFileItemAt48D090(page); // 消费按值Ref。
        if (index < 0) continue;
        if (Capture(source)->regularLatestPageAt18 >= 0 && !Later(*Time(*at, index), latest)) continue;
        const auto begin = Capture(source)->regularPagesAt20.begin;
        const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(at) - reinterpret_cast<std::uintptr_t>(begin));
        const auto pageIndex = std::bit_cast<std::int32_t>(bytes) / 12;
        Capture(source)->regularLatestPageAt18 = pageIndex;
        Capture(source)->regularLatestItemAt1C = index;
        latest = *Time(*at, index);
    }
}
}
