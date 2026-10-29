#include <Runtime/OptionCharSelectDialogNative.h>

#include <array>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原PE 5B8544..5B893C的CP932字节；不转UTF-8，不引用modernObjectGraph表。
#define PREFIX "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F"
#define RECORD_NAMES(id, stem) \
    constexpr char id##On[] = PREFIX stem "\x83\x49\x83\x93\x2E\x61\x6C\x70"; \
    constexpr char id##Off[] = PREFIX stem "\x83\x49\x83\x74\x2E\x61\x6C\x70"; \
    constexpr char id##Target[] = PREFIX stem "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x61\x6C\x70"; \
    constexpr char id##Click[] = PREFIX stem "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x61\x6C\x70"
RECORD_NAMES(r0, "\x31\x81\x69\x90\x53\x8D\xD8\x81\x6A");
RECORD_NAMES(r1, "\x32\x81\x69\x90\xE1\x88\xA4\x81\x6A");
RECORD_NAMES(r2, "\x33\x81\x69\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x6A");
RECORD_NAMES(r3, "\x34\x81\x69\x89\xC4\x91\xE3\x8E\x71\x81\x6A");
RECORD_NAMES(r4, "\x82\xBB\x82\xCC\x91\xBC");
RECORD_NAMES(r5, "\x34\x81\x69\x89\xC4\x91\xE3\x8E\x71\x81\x40\x96\xA2\x8A\x4A\x95\xFA\x81\x6A");
#undef RECORD_NAMES
#undef PREFIX

#define DESCRIPTOR(event, id) {{event, {id##On, id##Off, nullptr, id##Target, id##Target, id##Click, nullptr}, nullptr}, 0, -1}
const std::array<GuiButtonDescriptorNative2Ch, 6> records{{
    DESCRIPTOR(0, r0), DESCRIPTOR(1, r1), DESCRIPTOR(2, r2),
    DESCRIPTOR(3, r3), DESCRIPTOR(4, r4), DESCRIPTOR(3, r5)}};
#undef DESCRIPTOR
// 6083F0五组POINT；不是六组，上一页wrap取表末记录event3，不取4。
constexpr std::array<POINT, 5> positions{{{297, 623}, {394, 623}, {491, 623}, {588, 623}, {685, 623}}};
}
std::span<const GuiButtonDescriptorNative2Ch, 6> OptionCharSelectDialogNativeRecordsSample18() noexcept
{ return records; }
std::span<const POINT, 5> OptionCharSelectDialogNativePositionsSample18() noexcept
{ return positions; }
}
