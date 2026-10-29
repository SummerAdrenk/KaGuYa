#pragma once

#include <Runtime/RuntimeCString.h>
#include <cstdint>
#include <ctime>

namespace KaGuYa::Start::Runtime
{
// source仅借用，不通过接口删除；原PE localtime64目标为5697BB，540659是cookie检查。
class ISaveLoadTimestampNativeSource
{
public:
    // output入参未初始化，不得读取；合法成功调用须完整写出tm，返回0。
    // 非0返回时helper不读output；time可指向可写快照，后续调用必须看见其修改。
    virtual int LocalTime64At5697BB(std::tm* output, const std::int64_t* time) = 0;
};

class CrtSaveLoadTimestampNativeSource final : public ISaveLoadTimestampNativeSource
{
public:
    int LocalTime64At5697BB(std::tm* output, const std::int64_t* time) override;
};

// 仅格式化已经构造的CString，不初始化、不接管其清理资格；异常原样外传。
// minute/hour/day/month/year独立转换，同一个可写8-byte本地快照贯穿五次调用。
void FormatSaveLoadTimestampNativeAt525480And520AB0(RuntimeCString& alreadyConstructed,
    std::int64_t time, ISaveLoadTimestampNativeSource& source);

// 原样传递time指针，不另做快照；失败返回0，成功按32-bit回绕加1900。
std::int32_t SaveLoadTimestampYearNativeAt47E120(const std::int64_t* time,
    ISaveLoadTimestampNativeSource& source);
}
