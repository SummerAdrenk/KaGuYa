#include <Runtime/SoundParamsNamesNative.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
SoundParamsNamesNative::SoundParamsNamesNative(IRuntimeStringManagerSource& strings)
{
    // 原PE 5B28A8/5AE13C/5B28AC/5AE144，长度3/6/12/4；均为字面串而非资源ID。
    constexpr std::array<const char*, 4> values{{"BGM", "\x8C\xF8\x89\xCA\x89\xB9",
        "\x83\x8B\x81\x5B\x83\x76\x8C\xF8\x89\xCA\x89\xB9", "\x89\xB9\x90\xBA"}};
    constexpr std::array<int, 4> lengths{{3, 6, 12, 4}};
    const std::array<RuntimeCString*, 4> fields{{&musicAt00, &normalEffectAt04, &loopEffectAt08, &voiceAt0C}};
    unsigned constructed = 0;
    try
    {
        for (unsigned index = 0; index != fields.size(); ++index)
        {
            InitializeEmptyRuntimeCString(*fields[index], strings);
            // FuncInfo5E7D28：GetNil成功便进入奇数state，赋值抛出也要释放当前串。
            ++constructed;
            SetRuntimeCStringAt437680(*fields[index], values[index], lengths[index]);
        }
    }
    catch (...)
    {
        // 40CD20与40CD40都是对当前CStringData减持；展开时二次异常应终止。
        const auto unwind = [&]() noexcept {
            while (constructed) DestroyRuntimeCStringAt40CD40(fields[--constructed]);
        };
        unwind();
        throw;
    }
}

SoundParamsNamesNative::~SoundParamsNamesNative() noexcept
{
    DestroyRuntimeCStringAt40CD40(&voiceAt0C);
    DestroyRuntimeCStringAt40CD40(&loopEffectAt08);
    DestroyRuntimeCStringAt40CD40(&normalEffectAt04);
    DestroyRuntimeCStringAt40CD40(&musicAt00);
}
}
