#include <Runtime/SaveLoadTimestampNative.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
static_assert(sizeof(int) == 4 && sizeof(std::int64_t) == 8);

std::int32_t WrapAdd(std::int32_t value, std::uint32_t increment) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(value) + increment);
}
}

int CrtSaveLoadTimestampNativeSource::LocalTime64At5697BB(std::tm* output,
    const std::int64_t* time)
{
    // 不预验指针/时间范围，不另改errno、处理器或时区；保留CRT自身副作用。
    return _localtime64_s(output, time);
}

std::int32_t SaveLoadTimestampYearNativeAt47E120(const std::int64_t* time,
    ISaveLoadTimestampNativeSource& source)
{
    std::tm output;
    if (source.LocalTime64At5697BB(&output, time) != 0) return 0;
    return WrapAdd(output.tm_year, 1900u);
}

void FormatSaveLoadTimestampNativeAt525480And520AB0(RuntimeCString& alreadyConstructed,
    std::int64_t time, ISaveLoadTimestampNativeSource& source)
{
    // 52571F/525743及520E57/520E60：先形成8-byte快照；不是const对象。
    std::int64_t snapshot = time;
    // 原局部来自未初始化栈，不用{}或预填业务默认值掩盖source的非法成功输出。
    std::tm minuteOutput;
    const std::int32_t minute = source.LocalTime64At5697BB(&minuteOutput, &snapshot) == 0
        ? minuteOutput.tm_min : -1;
    std::tm hourOutput;
    const std::int32_t hour = source.LocalTime64At5697BB(&hourOutput, &snapshot) == 0
        ? hourOutput.tm_hour : -1;
    std::tm dayOutput;
    const std::int32_t day = source.LocalTime64At5697BB(&dayOutput, &snapshot) == 0
        ? dayOutput.tm_mday : 0;
    std::tm monthOutput;
    const std::int32_t month = source.LocalTime64At5697BB(&monthOutput, &snapshot) == 0
        ? WrapAdd(monthOutput.tm_mon, 1u) : 0;
    const std::int32_t year = SaveLoadTimestampYearNativeAt47E120(&snapshot, source);

    // 52583B/520F74的cdq/idiv保留有符号余数；%02d是最小宽度，不裁剪字段。
    FormatRuntimeCStringAt417130(alreadyConstructed, "%02d/%02d/%02d\r\n   %02d:%02d",
        static_cast<int>(year % 100), static_cast<int>(month), static_cast<int>(day),
        static_cast<int>(hour), static_cast<int>(minute));
}
}
