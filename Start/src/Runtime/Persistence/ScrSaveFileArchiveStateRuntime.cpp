#include <Runtime/ScrSaveFileArchiveStateRuntime.h>
#include <Runtime/ScriptParamsNativeBody.h>
#include <Runtime/ScrSaveParams2Native.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
ScrSaveFileArchiveStateRuntime::ScrSaveFileArchiveStateRuntime(
    ScrVmParamsGenerationBinding generation)
    : generation_(std::move(generation))
{
    if (!generation_) throw std::invalid_argument("save archive generation slot is null");
}

ScrSaveFileArchiveStateRuntime::ScrSaveFileArchiveStateRuntime(ScriptParamsNativeBody& receiver) noexcept
    : receiver_(&receiver) {}

ScriptParamsNativeBody& ScrSaveFileArchiveStateRuntime::BorrowParams() const
{
    return receiver_ ? *receiver_ : generation_->Current();
}

ScriptParamsNativeBody& ScrSaveFileArchiveStateRuntime::CaptureParamsAtEntry()
{
    return BorrowParams();
}

void ScrSaveFileArchiveStateRuntime::SetLastWriteTimeAt00(ScriptParamsNativeBody& receiver,
    std::uint64_t value)
{
    receiver.SetSaveTimestamp(value);
}

void ScrSaveFileArchiveStateRuntime::SerializeSaveParamsAt48B720(ScriptParamsNativeBody& receiver,
    SerializeBuffer& archive)
{
    // 4C7D53：按本次接收者的当前Ref解析；本次Serialize内部不重新解子槽。
    receiver.SaveParamsAt08().Serialize(archive);
}

void ScrSaveFileArchiveStateRuntime::SerializeSaveParams2At48B6B0(ScriptParamsNativeBody& receiver,
    SerializeBuffer& archive)
{
    // 4C7E7E：第二块消费点才取+30，保留首块回调替换子正文的影响。
    receiver.SaveParams2At30().Serialize(archive);
}

ScriptDwordVectorStorage& ScrSaveFileArchiveStateRuntime::FormatHistoryAt3C(ScriptParamsNativeBody& receiver)
{
    return receiver.historyAt3C;
}

std::int32_t& ScrSaveFileArchiveStateRuntime::CurrentFormatAt60(ScriptParamsNativeBody& receiver)
{
    return receiver.messageFormatIndexAt60;
}

void ScrSaveFileArchiveStateRuntime::SetLastWriteTimeAt00(std::uint64_t value)
{
    SetLastWriteTimeAt00(CaptureParamsAtEntry(), value);
}

void ScrSaveFileArchiveStateRuntime::SerializeSaveParamsAt48B720(SerializeBuffer& archive)
{
    SerializeSaveParamsAt48B720(CaptureParamsAtEntry(), archive);
}

void ScrSaveFileArchiveStateRuntime::SerializeSaveParams2At48B6B0(SerializeBuffer& archive)
{
    SerializeSaveParams2At48B6B0(CaptureParamsAtEntry(), archive);
}

ScriptDwordVectorStorage& ScrSaveFileArchiveStateRuntime::FormatHistoryAt3C()
{
    return FormatHistoryAt3C(CaptureParamsAtEntry());
}

std::int32_t ScrSaveFileArchiveStateRuntime::CurrentFormatAt60() const
{
    return BorrowParams().messageFormatIndexAt60;
}
}
