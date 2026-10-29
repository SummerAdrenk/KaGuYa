#include <Runtime/CategorizedSoundEnvironmentFile.h>
#include <Runtime/CategorizedSoundEnvironmentAdapter.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SaveDataNative18.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include "NativeSerialize18.Internal.h"
#include <limits>
#include <stdexcept>
#include <string>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Adapter=CategorizedSoundEnvironmentAdapter;
using Result=CategorizedSoundEnvironmentWire::Result;

EnvironmentSoundParamsArchive& Sound(void* sound)
{
    if(!sound)throw std::logic_error("Categorized env selected a null sound archive");
    return *static_cast<EnvironmentSoundParamsArchive*>(sound);
}
void Check(const Result& result)
{
    if(!result)
        throw std::runtime_error("Categorized sound env read failed: error="+
            std::to_string(static_cast<unsigned>(result.error))+", field="+
            std::to_string(static_cast<unsigned>(result.field))+", offset="+
            std::to_string(result.offset)+", entry="+std::to_string(result.entry));
}
std::uint32_t Cursor(std::size_t cursor)
{
    if(cursor>std::numeric_limits<std::uint32_t>::max())
        throw std::length_error("Categorized env cursor exceeds native DWORD range");
    return static_cast<std::uint32_t>(cursor);
}
void BufferSound(void* sound,SerializeBuffer& serializer,void* context)
{
    auto& adapter=*static_cast<Adapter*>(context);
    if(serializer.IsWriting())
    {
        const auto bytes=adapter.Write(Sound(sound));
        auto& output=serializer.MutableBytes();
        if(bytes.size()>std::numeric_limits<std::uint32_t>::max()-output.size())
            throw std::length_error("Categorized env output exceeds native DWORD range");
        output.insert(output.end(),bytes.begin(),bytes.end());
    }
    else
    {
        const auto result=adapter.Read(Sound(sound),serializer.Bytes(),serializer.Cursor());
        // 旧reader逐字段提交；adapter在失败时也已回填前缀，先公布游标再传播诊断。
        serializer.MutableCursor()=Cursor(result.cursor);
        Check(result);
    }
}
void NativeSound(void* sound,void* serializer,void* context)
{
    using namespace NativeSerialize18Detail;
    auto& adapter=*static_cast<Adapter*>(context);
    if(Mode(serializer))
    {
        const auto bytes=adapter.Write(Sound(sound));
        // 借原native vector追加，保留其分配/释放域；不把std::vector强转为18h serializer。
        for(const auto byte:bytes)AppendByte(Vector{Buffer(serializer)},&byte);
    }
    else
    {
        const Vector buffer{Buffer(serializer)};
        const auto* begin=buffer.Begin();const auto* end=buffer.End();
        const auto first=reinterpret_cast<std::uintptr_t>(begin),last=reinterpret_cast<std::uintptr_t>(end);
        if(last<first||(!begin&&end))throw std::logic_error("Categorized env native buffer has invalid bounds");
        auto* cursor=NativeSerialize18Detail::Cursor(serializer);
        const auto result=adapter.Read(Sound(sound),std::span<const std::byte>{begin,last-first},*cursor);
        *cursor=Cursor(result.cursor);
        Check(result);
    }
}
}
void SerializeCategorizedEnvironmentFile(SaveDataEnvironmentArchive& save,SerializeBuffer& serializer,
    Adapter& adapter)
{
    save.SerializeWithSoundContext(serializer,BufferSound,&adapter);
}
void SerializeCategorizedEnvironmentFileNative(SaveDataEnvironmentArchive& save,NativeSerialize18Body& serializer,
    Adapter& adapter)
{
    SerializeSaveDataNative18WithSound(&save,&serializer,NativeSound,&adapter);
}
}
