#include <Runtime/ParamsDemoRuntimeDecoder.h>
#include <Runtime/ParamsRuntimeReadCursor.h>

#include <Windows.h>
#include <atlexcept.h>

#include <bit>
#include <cstring>
#include <string>

namespace KaGuYa::Start::Runtime
{
namespace
{
// Start18 原 PE #bk/Start.exe：VA 005AE094，文件偏移 001ACC94；__TI2PAD 抛 char*。
char DemoCommandError[] =
    "\x96\xA2\x91\xCE\x89\x9E\x82\xCC\x83\x66\x83\x82\x83\x66\x81\x5B"
    "\x83\x5E\x82\xCC\x83\x52\x83\x7D\x83\x93\x83\x68\x82\xC5\x82\xB7\x81\x49";

[[nodiscard]] ParamsText ReadShortTextAt420650(ParamsRuntimeReadCursor& cursor)
{
    const auto byteCount = cursor.U8();
    const auto bytes = cursor.Bytes(byteCount);
    const auto wideCount = static_cast<std::size_t>(byteCount >> 1u);
    ParamsText text;
    // 42069E..42070A：每次新建空串；n=0/1 均为空，奇数末字节消费但不转换。
    if (wideCount == 0)
    {
        return text;
    }

    std::wstring wide(wideCount, L'\0');
    std::memcpy(wide.data(), bytes.data(), wideCount * sizeof(wchar_t));
    text.utf16.resize(wideCount);
    std::memcpy(text.utf16.data(), bytes.data(), wideCount * sizeof(char16_t));
    const int count = static_cast<int>(wideCount);
    // 420725 / 420760：显式 WCHAR 数量，保留嵌入 NUL；不能复用 TextBytes 的 -1 路径。
    const int required = WideCharToMultiByte(CP_THREAD_ACP, 0, wide.data(), count,
        nullptr, 0, nullptr, nullptr);
    if (required < 0)
    {
        throw ATL::CAtlException(E_INVALIDARG);
    }
    // 420756..42076E：容量为完整 required，不减1；返回0及第二次返回值均不作拒绝条件。
    // 原未写输出无确定值，置零沿用重写侧安全约束，不宣称未初始化内存等价。
    text.cp932.resize(static_cast<std::size_t>(required), '\0');
    static_cast<void>(WideCharToMultiByte(CP_THREAD_ACP, 0, wide.data(), count,
        text.cp932.data(), required, nullptr, nullptr));
    return text;
}
}

DecodedParamsDemoCommand DecodeRuntimeParamsDemoCommand(const ParamsDemoCommand& command)
{
    ParamsRuntimeReadCursor payload(command.payload);
    switch (command.type)
    {
    case 0:
        return DemoEndCommand{};
    case 1:
        return DemoNextCommand{};
    case 2:
        // 41A1A8..41A1D0：mode 不在解码阶段校验；ticks 保留 DWORD 有符号位型。
        return DemoWaitCommand{payload.U8(), std::bit_cast<std::int32_t>(payload.U32())};
    case 3:
        return DemoSoundCommand{payload.U8(), payload.U8(), ReadShortTextAt420650(payload)};
    case 4:
        return DemoLoadCommand{payload.U8(), ReadShortTextAt420650(payload)};
    case 5:
        return DemoTransitionCommand{ReadShortTextAt420650(payload),
            std::bit_cast<std::int32_t>(payload.U32()), ReadShortTextAt420650(payload)};
    case 6:
    {
        // 41A881->41A886：先读visible，再读slot，安全截断也按该读取顺序报告。
        payload.Seek(1);
        const bool visible = payload.U8() != 0;
        payload.Seek(0);
        const auto slot = payload.U8();
        // 41A8AB..41A8C4：NOT byte -> MOVZX -> DEC；0x80 仍为 +128，不是 int8 符号扩展。
        const std::int32_t decodedSlot = slot <= 0x80u ? slot
            : static_cast<std::int32_t>(static_cast<std::uint8_t>(~slot)) - 1;
        return DemoVisibilityCommand{decodedSlot, visible};
    }
    case 7:
        return DemoUpdateCommand{};
    case 8:
        // 41AAC8..41AAE7：slot 零扩展，后续为 duration/startX/startY/endX/endY。
        return DemoMoveCommand{payload.U8(),
            std::bit_cast<std::int32_t>(payload.U32()),
            std::bit_cast<std::int32_t>(payload.U32()),
            std::bit_cast<std::int32_t>(payload.U32()),
            std::bit_cast<std::int32_t>(payload.U32()),
            std::bit_cast<std::int32_t>(payload.U32())};
    case 9:
        return DemoPositionCommand{payload.U8(),
            std::bit_cast<std::int32_t>(payload.U32()),
            std::bit_cast<std::int32_t>(payload.U32())};
    default:
        throw DemoCommandError;
    }
}
}
