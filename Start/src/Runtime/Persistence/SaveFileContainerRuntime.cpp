#include <Runtime/SaveFileContainerRuntime.h>
#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/FileStatusRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SaveFileMetadataLoader.h>
#include <Runtime/ScrVmProgramHandler.h>
#include <Runtime/SerializeBuffer.h>
#include <Runtime/ScriptParamsHistoryNative.h>
#include <Runtime/RuntimeCStringInputNative.h>
#include <Runtime/RuntimeCStringResult.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include "SaveFileContainerRuntime.Internal.h"

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstring>
#include <limits>
#include <mbstring.h>
#include <stdexcept>
#include <vector>

namespace KaGuYa::Start::Runtime
{
namespace
{
using SaveFileContainerDetail::ContainerFile;
using SaveFileContainerDetail::WorkBuffer;
using SaveFileContainerDetail::WriteContainerHeader;
using SaveFileContainerDetail::ReadContainerHeader;
using SaveFileContainerDetail::ResolveContainerPath;
using SaveFileContainerDetail::ReadContainerLastWriteTime;
void WriteBlock(ContainerFile& file, const SerializeBuffer& archive,
    WorkBuffer& work, std::uint32_t seed, bool omitEmptyBody)
{
    if (!work.IsNative() && archive.Bytes().size() > static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
        throw std::length_error("save archive exceeds original signed allocation range");
    const auto size = static_cast<std::uint32_t>(archive.Bytes().size());
    work.Allocate(size);
    std::memmove(work.Data(), archive.Bytes().data(), size);
    work.XorAt4C7AA0(seed);
    file.Write(&size, sizeof(size));
    // 首两块无条件调用 WriteFile；只有第三块原 4C80CC 根据空 Ref 跳过正文写入。
    if (!omitEmptyBody || size != 0) file.Write(work.Data(), size);
}

void ReadBlock(ContainerFile& file, WorkBuffer& work, std::uint32_t size,
    std::uint32_t seed, SerializeBuffer& archive)
{
    work.Allocate(size);
    const auto count = work.Size();
    auto* data = work.Data();
    file.Read(data, count);
    work.XorAt4C7AA0(seed);
    // 480490 是复制正文，不是压缩解码；无额外版本头或全块消费检查。
    if (work.Size()) archive.MutableBytes().assign(work.Data(), work.Data() + work.Size());
    else archive.MutableBytes().clear();
    archive.SetWriting(false);
}
}

SaveFileContainerRuntime::SaveFileContainerRuntime(IStartFileApi& files, ISaveFileArchiveState& state,
    const SaveDataEnvironmentArchive& save, RuntimeStringBorrow directory,
    IScrVmProgramRandSource& random, const SaveFileFormatProfile& profile)
    : files_(files), state_(state), save_(&save), directory_(directory), random_(random), profile_(profile)
{
    // 样本 profile 的装配错误在对象构造时拒绝，不通过写入空魔数伪装成功。
    if (!profile_.nativeMagicSlot && profile_.magicBytes.size() != 32) throw std::invalid_argument("save container requires a 32-byte profile");
}
SaveFileContainerRuntime::SaveFileContainerRuntime(IStartFileApi& files, ISaveFileArchiveState& state,
    ISaveFileContainerEnvironmentSource& environment, IScrVmProgramRandSource& random,
    const SaveFileFormatProfile& profile)
    : files_(files), state_(state), save_(nullptr), directory_(""), random_(random), profile_(profile),
      environment_(&environment)
{
    if (!profile_.nativeMagicSlot && profile_.magicBytes.size() != 32) throw std::invalid_argument("save container requires a 32-byte profile");
}

bool SaveFileContainerRuntime::WriteAt4C7B10(const std::string& input)
{
    return WriteAt4C7B10(input.c_str());
}
bool SaveFileContainerRuntime::WriteAt4C7B10(const char* input)
{
    // 488D47先调用47BDB0取得this，488D4F才进入原容器的catch范围。
    SaveFileArchiveOperation operation(state_);
    try
    {
        auto& strings = environment_ ? environment_->Strings() : DefaultRuntimeStringManagerSource();
        RuntimeCStringResult path([&](RuntimeCString* out){ConstructRuntimeCStringInputNativeAt40B530(out,input,strings);});
        ResolveContainerPath(path.Value(), directory_, input, environment_, strings);
        const bool native = profile_.nativeMagicSlot != nullptr;
        ContainerFile file(files_, path.Value().GetString(), true, native);
        file.RequireOpen();
        const auto seed = std::bit_cast<std::uint32_t>(random_.NextRand());
        WorkBuffer work(native); // 原state4在rand之后；fresh Ref先清三个DWORD再读Src。
        WriteContainerHeader(file, work, profile_, seed);
        {
            SerializeBuffer archive;
            operation.SerializeSaveParamsAt48B720(archive);
            WriteBlock(file, archive, work, seed, false);
        }
        {
            SerializeBuffer archive;
            operation.SerializeSaveParams2At48B6B0(archive);
            WriteBlock(file, archive, work, seed, false);
        }
        {
            SerializeBuffer archive;
            // 原 4C7F9B 才查询 Save+75，不能在写头或序列化首两块前取快照。
            if ((environment_ ? environment_->CaptureHistoryFlagAt4C7F9B() : save_->bytesAt74[1]) != 0)
                SerializeScriptParamsHistoryAt4661B0(archive, operation.FormatHistoryAt3C());
            else
            {
                auto& current = operation.CurrentFormatAt60();
                if (current != -1)
                {
                    ScriptDwordVectorStorage single;
                    // 4C7FCE传原+60地址；453A70分配后晚取，不能先复制为局部DWORD。
                    single.GrowAndInsertAt453A70(single.storage.end,
                        reinterpret_cast<std::uint32_t&>(current));
                    SerializeScriptParamsHistoryAt4661B0(archive, single);
                }
            }
            WriteBlock(file, archive, work, seed, true);
        }
        return true;
    }
    catch (...)
    {
        // 5F11C4：try 0..8，handler(type=0, adjectives=40h) 4C8126 -> AL=0。
        return false;
    }
}

bool SaveFileContainerRuntime::ReadAt4C7420(const std::string& input, std::uint32_t mode)
{
    return ReadAt4C7420(input.c_str(), mode);
}
bool SaveFileContainerRuntime::ReadAt4C7420(const char* input, std::uint32_t mode)
{
    // 480884的47BDB0属于调用者；470A28/48E70B等则直接传已构造的栈正文。
    SaveFileArchiveOperation operation(state_);
    try
    {
        auto& strings = environment_ ? environment_->Strings() : DefaultRuntimeStringManagerSource();
        RuntimeCStringResult path([&](RuntimeCString* out){ConstructRuntimeCStringInputNativeAt40B530(out,input,strings);});
        ResolveContainerPath(path.Value(), directory_, input, environment_, strings);
        const bool native = profile_.nativeMagicSlot != nullptr;
        ContainerFile file(files_, path.Value().GetString(), false, native);
        file.RequireOpen();
        operation.SetLastWriteTimeAt00(ReadContainerLastWriteTime(files_, path.Value().GetString()));
        WorkBuffer work(native);
        std::uint32_t firstSize;
        const auto seed = ReadContainerHeader(file, work, profile_, firstSize); // Lcompare作为首长度DWORD初值。
        file.Read(&firstSize, sizeof(firstSize));
        {
            SerializeBuffer archive;
            ReadBlock(file, work, firstSize, seed, archive);
            operation.SerializeSaveParamsAt48B720(archive);
        }
        if (static_cast<std::uint8_t>(mode) == 0) return true; // 4C7735：首块已经提交。
        // 4C7741/4C77F6复用完整arg_4；FALSE/LastError0未覆盖的字节保留原值。
        file.Read(&mode, sizeof(mode));
        {
            SerializeBuffer archive;
            ReadBlock(file, work, mode, seed, archive);
            operation.SerializeSaveParams2At48B6B0(archive);
        }
        file.Read(&mode, sizeof(mode));
        if (mode == 0)
        {
            operation.FormatHistoryAt3C().clear(); // 4C7811..4C7814，不读取正文、不释放容量。
        }
        else
        {
            SerializeBuffer archive;
            ReadBlock(file, work, mode, seed, archive);
            // 4661B0先resize，再逐DWORD提交；截断不能预检整块而跳过原提交前缀。
            SerializeScriptParamsHistoryAt4661B0(archive, operation.FormatHistoryAt3C());
        }
        return true;
    }
    catch (...)
    {
        // 5F1134：try 0..7，handler(type=0, adjectives=40h) 4C78DF -> AL=0。
        return false;
    }
}
}
