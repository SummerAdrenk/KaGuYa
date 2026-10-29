#include "SettingDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace SettingDialogNativeDetail
{
void* CaptureRegistry(ISettingDialogNativeSource& source, std::uint32_t index, bool beginFirst)
{
    auto* system = source.SystemAt4180C0();
    std::uintptr_t begin, end;
    if (beginFirst)
    { begin = reinterpret_cast<std::uintptr_t>(Read(system->archivesAt30.begin)); end = reinterpret_cast<std::uintptr_t>(Read(system->archivesAt30.end)); }
    else
    { end = reinterpret_cast<std::uintptr_t>(Read(system->archivesAt30.end)); begin = reinterpret_cast<std::uintptr_t>(Read(system->archivesAt30.begin)); }
    const auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(end - begin) / 12);
    if (index >= count) return nullptr;
    return Resolve<void>(*reinterpret_cast<const Slot*>(begin + 12u * index));
}
}
void SnapshotSettingDialogNativeAt4FF430(SettingDialogNativeBodyACh& b)
{
    using namespace SettingDialogNativeDetail;
    auto& s = RequireSettingDialogNativeServices();
    auto* registry = CaptureRegistry(s.source, 2);
    auto& names = Field<EnvironmentVectorStorage<RuntimeCString>>(registry, 0x30);
    const auto end = reinterpret_cast<std::uintptr_t>(Read(names.end));
    const auto begin = reinterpret_cast<std::uintptr_t>(Read(names.begin));
    const auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(end - begin) / 4);
    ResizeRuntimeCStringVectorAt456E80(b.snapshotNamesAt84, count, s.strings, s.memory);
    for (std::uint32_t index = 0;; ++index)
    {
        // 原循环先一次System锁/查count，再另一次查source；不能固定最初count和names指针。
        registry = CaptureRegistry(s.source, 2, true);
        auto& current = Field<EnvironmentVectorStorage<RuntimeCString>>(registry, 0x30);
        const auto currentEnd = reinterpret_cast<std::uintptr_t>(Read(current.end));
        const auto currentBegin = reinterpret_cast<std::uintptr_t>(Read(current.begin));
        const auto currentCount = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(currentEnd - currentBegin) / 4);
        if (index >= currentCount) break;
        registry = CaptureRegistry(s.source, 2);
        auto* source = Read(Field<RuntimeCString*>(registry, 0x30)) + index;
        auto* destination = Read(b.snapshotNamesAt84.begin) + index;
        AssignRuntimeCStringAt410220(*destination, *source);
    }
    b.savedAt90 = Read(Field<std::uint32_t>(CaptureRegistry(s.source, 1), 0x44));
    b.savedAt94 = Read(Field<std::uint32_t>(CaptureRegistry(s.source, 1), 0x4C));
    b.savedBytesAtA0[0] = Read(Field<std::uint8_t>(CaptureRegistry(s.source, 1), 0x60));
    b.savedBytesAtA0[1] = Read(Field<std::uint8_t>(CaptureRegistry(s.source, 1), 0x61));
    b.savedBytesAtA0[2] = static_cast<std::uint8_t>((Read(Field<std::uint32_t>(CaptureRegistry(s.source, 1), 0x38)) >> 4) & 1u);
    b.savedBytesAtA0[3] = Read(Field<std::uint8_t>(CaptureRegistry(s.source, 1), 0x6C));
    AssignRuntimeCStringAt410220(b.firstNameAt7C, Field<RuntimeCString>(CaptureRegistry(s.source, 1), 0x58));
    AssignRuntimeCStringAt410220(b.secondNameAt80, Field<RuntimeCString>(CaptureRegistry(s.source, 1), 0x5C));
    // 98/9C、74/78、A4/A8不是本函数的提前初始化区。
}
}
