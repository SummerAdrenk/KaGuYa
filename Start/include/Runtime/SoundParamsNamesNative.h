#pragma once
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
// 45D611..45D73A：四个连续CString，各自重新捕获管理器；不是配置生成的名字副本。
class SoundParamsNamesNative final
{
public:
    explicit SoundParamsNamesNative(
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~SoundParamsNamesNative() noexcept;
    SoundParamsNamesNative(const SoundParamsNamesNative&) = delete;
    SoundParamsNamesNative& operator=(const SoundParamsNamesNative&) = delete;

    union { RuntimeCString musicAt00; };
    union { RuntimeCString normalEffectAt04; };
    union { RuntimeCString loopEffectAt08; };
    union { RuntimeCString voiceAt0C; };
};
static_assert(sizeof(SoundParamsNamesNative) == 0x10);
static_assert(offsetof(SoundParamsNamesNative, voiceAt0C) == 0x0C);
}
