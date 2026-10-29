#pragma once

#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
struct EnvironmentRootStorage;

class ISaveLoadLabelNativeSource
{
public:
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    virtual IRuntimeStringManagerSource& Strings() = 0;
    // 必填原子边界：fresh未构造；成功才交付清理资格，失败由40B530自身清理。
    // 普通输入不能用ATL隐式构造替代；完整资源ID路径由装配方明确选择。
    virtual void ConstructInputAt40B530(RuntimeCString* fresh, const char* input) = 0;
};

// fresh必须是未构造的4-byte返回槽；成功由调用者raw销毁，失败由本入口展开。
RuntimeCString* ExpandSaveLoadLabelNativeAt4DC660(RuntimeCString* fresh,
    const char* input, ISaveLoadLabelNativeSource& source);
// 参数不固化为40/2；保留任意有符号int、当前CRT lead规则和结尾NUL索引。
RuntimeCString* TruncateSaveLoadLabelNativeAt4DA8E0(RuntimeCString* fresh,
    const char* input, std::int32_t maxBytesPerLine, std::int32_t maxLines,
    ISaveLoadLabelNativeSource& source);
}
