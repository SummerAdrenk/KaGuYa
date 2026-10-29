#include <Runtime/RootTitleConstructionState.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
RootTitleConstructionState::RootTitleConstructionState(EnvironmentStartupRuntime& environment,
    RootRuntimeOwner& root, ScriptNamedValueNativeTables tables, decltype(&::MessageBoxA) messageBox,
    IRuntimeStringManagerSource& strings) noexcept
    : environment_(environment), root_(root), tables_(tables), messageBox_(messageBox), strings_(strings) {}

void RootTitleConstructionState::WriteSlot2DwordAt04(std::uint32_t value)
    { environment_.Archives().UnSaveData().ModeAt04() = value; }
void RootTitleConstructionState::WriteSlot2ByteAt24(std::uint8_t value)
    { environment_.Archives().UnSaveData().BytesAt24()[0] = value; }
void RootTitleConstructionState::WriteSlot2ByteAt25(std::uint8_t value)
    { environment_.Archives().UnSaveData().BytesAt24()[1] = value; }
void RootTitleConstructionState::WriteSlot2ByteAt26(std::uint8_t value)
    { environment_.Archives().UnSaveData().BytesAt24()[2] = value; }
void RootTitleConstructionState::WriteSlot2ByteAt27(std::uint8_t value)
    { environment_.Archives().UnSaveData().BytesAt24()[3] = value; }
void RootTitleConstructionState::WriteSlot2ByteAt08(std::uint8_t value)
    { environment_.Archives().UnSaveData().BytesAt08()[0] = value; }
void RootTitleConstructionState::WriteSlot2ByteAt09(std::uint8_t value)
    { environment_.Archives().UnSaveData().BytesAt08()[1] = value; }
void RootTitleConstructionState::WriteSlot2ByteAt0C(std::uint8_t value)
{
    // 4A2C0A的byte写不能破坏同DWORD中其它三字节。
    auto& field = environment_.Archives().UnSaveData().DwordAt0C();
    *reinterpret_cast<unsigned char*>(&field) = value;
}
void RootTitleConstructionState::WriteSlot2DwordAt18(std::uint32_t value)
    { environment_.Archives().UnSaveData().FlagsAt18() = value; }
void RootTitleConstructionState::WriteSlot2DwordAt14(std::uint32_t value)
    { environment_.Archives().UnSaveData().FlagsAt14() = value; }
void RootTitleConstructionState::WriteSlot2DwordAt1C(std::uint32_t value)
    { environment_.Archives().UnSaveData().FlagsAt1C() = value; }
void RootTitleConstructionState::WriteSlot2ByteAt29(std::uint8_t value)
    { environment_.Archives().UnSaveData().BytesAt24()[5] = value; }
void RootTitleConstructionState::WriteSlot2ByteAt28(std::uint8_t value)
    { environment_.Archives().UnSaveData().BytesAt24()[4] = value; }

void RootTitleConstructionState::ClearReplayAt4A2DD6()
{
    // 4A2DD6/4A2E22/4A2E36：先解slot0，再捕获root，然后查replay。
    // 498020不消费slot0正文；空slot0也不能跳过原查询或凭空增加非空门控。
    static_cast<void>(environment_.Archives().CaptureArchiveAt4180C0(0));
    root_.EnsureCreatedAt413BE0();
    auto& captured = root_.RequirePublished();
    const auto index = ResolveScriptNamedValueNativeAt498020("replay",tables_,strings_);
    auto& params = captured.ParamsAt47BDB0();
    if (index == -1)
    {
        const auto& error = environment_.CaptureAt4180C0();
        static_cast<void>(messageBox_(reinterpret_cast<HWND>(error.windowAt2C),
            "SetValue over num", error.titleAt10.GetString(), MB_ICONERROR));
        throw 1;
    }
    auto& values = params.SaveParams2At30().values;
    if (index < 0 || static_cast<std::size_t>(index) >= values.size())
        throw ScrVmValueException(ScrVmValueError::IndexOutOfRange, index,
            "title replay index is outside the 512-slot table (modern safety boundary)");
    values[static_cast<std::size_t>(index)] = 0;
}

void RootTitleConstructionState::WriteByteAt218(std::uint8_t value)
{
    root_.EnsureCreatedAt413BE0();
    root_.RequirePublished().WriteByteAt218(value);
}
void RootTitleConstructionState::WriteByteAt219(std::uint8_t value)
{
    root_.EnsureCreatedAt413BE0();
    root_.RequirePublished().WriteByteAt219(value);
}
}
