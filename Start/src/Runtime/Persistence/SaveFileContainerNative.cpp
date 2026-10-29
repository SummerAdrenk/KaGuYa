#include <Runtime/SaveFileContainerNativeBinding.h>
#include <Runtime/ScriptParamsNativeBody.h>
#include <Runtime/ScrVmProgramHandler.h>
#include <Runtime/RuntimeCStringInputNative.h>
#include <Runtime/RuntimeCStringResult.h>
#include "SaveFileContainerNative.Blocks.h"
#include "NativeSerialize18.Internal.h"
#include <bit>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveFileContainerDetail;
using U = std::uint32_t;
U Word(const void* p, U offset = 0) noexcept
{ return *reinterpret_cast<const volatile U*>(static_cast<const std::byte*>(p) + offset); }
void* Archive(ScriptParamsNativeBody& receiver, U offset, bool cachedBase) noexcept
{
    const U base = Word(&receiver, offset);
    if (!base) return nullptr;
    const auto* control = reinterpret_cast<const void*>(Word(&receiver, offset + 8));
    const U stride = control ? Word(control, 0x0C) : 0;
    const U bias = Word(&receiver, offset + 4);
    return reinterpret_cast<void*>((cachedBase ? base : Word(&receiver, offset)) + bias * stride);
}
const NativeSerialize18Vtable31& Table(NativeSerialize18Body& body) noexcept
{ return **reinterpret_cast<const NativeSerialize18Vtable31* const volatile*>(&body); }
void FreeHistory(ScriptDwordVectorStorage& history)
{
    const auto begin = Word(&history);
    if (begin)
    {
        const U bytes = (Word(&history, 8) - begin) & ~3u;
        NativeSerialize18Detail::Free(reinterpret_cast<std::byte*>(begin), bytes);
    }
    // 原正常内联尾不清三指针；不能额外调用现代容器析构或归零。
}
union HistoryStorage final
{
    ScriptDwordVectorStorage value;
    HistoryStorage() noexcept { ::new (&value) ScriptDwordVectorStorage; }
    ~HistoryStorage() noexcept {}
};
struct HistoryLife final
{
    ScriptDwordVectorStorage& body;
    bool armed = true;
    // state8展开59F328->4614A0在Free成功后清三指针；只有正常内联尾不清。
    ~HistoryLife() noexcept { if (armed) DestroyNativeSerialize18PrimitiveVector(&body, 4); }
    void Normal() { armed = false; FreeHistory(body); } // 4C7FED先退state7再Free。
};
void ValidateProfile(const SaveFileFormatProfile& profile)
{
    // 旧显式宿主域保留构造前置条件，不把任意长度std::string当作原生magic槽。
    if (!profile.nativeMagicSlot && profile.magicBytes.size() != 32)
        throw std::invalid_argument("save container owned profile requires 32 bytes");
}
}
std::uint32_t WriteSaveFileContainerNativeAt4C7B10(ScriptParamsNativeBody& receiver,
    const char* input, SaveFileContainerNativeServices services)
{
    ValidateProfile(services.profile);
    try
    {
        NativeSerialize18Scope serialization(services.serialization);
        auto& strings = services.environment.Strings();
        RuntimeCStringResult path([&](RuntimeCString* out){ ConstructRuntimeCStringInputNativeAt40B530(out, input, strings); });
        ResolveContainerPath(path.Value(), "", input, &services.environment, strings);
        ContainerFile file(services.files, path.Value().GetString(), true, true);
        file.RequireOpen();
        const U seed = std::bit_cast<U>(services.random.NextRand());
        WorkBuffer work(true);
        WriteContainerHeader(file, work, services.profile, seed);
        SaveSerializerStorage serializer;
        U firstSize, remainingSize;
        {
            SaveSerializerLife life(serializer);
            SerializeNativeArchiveAt466380(&serializer.body, Archive(receiver, 8, true));
            WriteNativeSaveBlock(file, work, serializer.body, firstSize, seed, true);
            life.NormalWithUnwind();
        }
        YtlReferenceControlRuntime* secondControl;
        {
            SaveSerializerLife life(serializer);
            SerializeNativeArchiveAt466380(&serializer.body, Archive(receiver, 0x30, false));
            secondControl = WriteNativeSaveBlock(file, work, serializer.body, remainingSize, seed, false);
            life.NormalWithUnwind();
        }
        {
            SaveSerializerLife life(serializer);
            if (services.environment.CaptureHistoryFlagAt4C7F9B())
                Table(serializer.body).common.longVectorAt20(&serializer.body, &receiver.historyAt3C);
            else if (receiver.messageFormatIndexAt60 != -1)
            {
                HistoryStorage single;
                HistoryLife history{single.value};
                single.value.GrowAndInsertAt453A70(single.value.storage.end,
                    reinterpret_cast<const U&>(receiver.messageFormatIndexAt60), services.serialization.memory);
                Table(serializer.body).common.longVectorAt20(&serializer.body, &single.value);
                history.Normal();
            }
            WriteNativeHistoryBlock(file, work, serializer.body, remainingSize, seed, secondControl);
            life.NormalWithUnwind();
        }
        return 1;
    }
    catch (...) { return 0; } // 原5F11C4，保留文件前缀和已发布状态，不回滚。
}
std::uint32_t ReadSaveFileContainerNativeAt4C7420(ScriptParamsNativeBody& receiver,
    const char* input, U mode, SaveFileContainerNativeServices services)
{
    ValidateProfile(services.profile);
    try
    {
        NativeSerialize18Scope serialization(services.serialization);
        auto& strings = services.environment.Strings();
        RuntimeCStringResult path([&](RuntimeCString* out){ ConstructRuntimeCStringInputNativeAt40B530(out, input, strings); });
        ResolveContainerPath(path.Value(), "", input, &services.environment, strings);
        ContainerFile file(services.files, path.Value().GetString(), false, true);
        file.RequireOpen();
        const auto time = ReadContainerLastWriteTime(services.files, path.Value().GetString());
        *reinterpret_cast<volatile U*>(&receiver.timestampLowAt00) = static_cast<U>(time);
        *reinterpret_cast<volatile U*>(&receiver.timestampHighAt04) = static_cast<U>(time >> 32);
        WorkBuffer work(true);
        U firstSize;
        const U seed = ReadContainerHeader(file, work, services.profile, firstSize);
        file.Read(&firstSize, 4);
        ReadNativeSaveBlock(file, work, firstSize, seed);
        SaveSerializerStorage serializer;
        {
            SaveSerializerLife life(serializer);
            ImportNativeSaveBlock(serializer.body, work);
            Table(serializer.body).common.archiveAt5C(&serializer.body, Archive(receiver, 8, false));
            life.NormalAfterStateDrop();
        }
        if (static_cast<std::uint8_t>(mode) == 0) return 1;
        file.Read(&mode, 4);
        ReadNativeSaveBlock(file, work, mode, seed);
        {
            SaveSerializerLife life(serializer);
            ImportNativeSaveBlock(serializer.body, work);
            Table(serializer.body).common.archiveAt5C(&serializer.body, Archive(receiver, 0x30, false));
            life.NormalAfterStateDrop();
        }
        file.Read(&mode, 4);
        if (!mode) receiver.historyAt3C.storage.end = receiver.historyAt3C.storage.begin;
        else
        {
            ReadNativeSaveBlock(file, work, mode, seed);
            SaveSerializerLife life(serializer);
            ImportNativeSaveBlock(serializer.body, work);
            Table(serializer.body).common.longVectorAt20(&serializer.body, &receiver.historyAt3C);
            life.NormalWithUnwind(); // 第三次读4C78A9没有先退state。
        }
        return 1;
    }
    catch (...) { return 0; } // 原5F1134，前块、时间及IO前缀保留。
}
}
