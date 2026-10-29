#include <Runtime/ParamsSerializeInput.h>
#include <Runtime/NativeSerialize18.h>

#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class Function> Function CurrentSlot(void* input, std::size_t offset) noexcept
{
    const void* table;
    Function entry;
    std::memcpy(&table, input, sizeof(table));
    std::memcpy(&entry, static_cast<const std::byte*>(table) + offset, sizeof(entry));
    return entry;
}
}

std::uint8_t ParamsNativeSerialize18Input::ReadModeAt04()
{
    // 434981保留返回的整个AL，包括非规范非零mode，不归一为bool或读字段镜像。
    return CurrentSlot<decltype(NativeISerializeVtable29::modeAt04)>(input_, 4)(input_);
}

ParamsInputBytesBorrow ParamsNativeSerialize18Input::CaptureInputAt14()
{
    // 434993等getter无参数，不访问scalar cursor，也不提前解引用返回载体。
    auto* carrier = CurrentSlot<decltype(NativeISerializeVtable29::bufferAt14)>(input_, 0x14)(input_);
    return ParamsInputBytesBorrow::NativeVector12(carrier);
}
}
