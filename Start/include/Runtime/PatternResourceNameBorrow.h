#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/RuntimeCString.h>

#include <cstdint>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 4CBCAC/4CBD18：本轮固定begin，元素字符在实际比较点才读取，不复制或增持名称。
struct PatternResourceNamesRange final
{
    using ReadCharacters = const char* (*)(const void* begin, std::uint32_t index);

    const void* begin = nullptr;
    std::uint32_t count = 0; // 原DWORD位型；调用点决定signed还是unsigned比较。
    ReadCharacters readCharacters = nullptr;

    [[nodiscard]] const char* Characters(std::uint32_t index) const
        { return readCharacters(begin, index); }
};

// 仅借容器头及无捕获读取函数；不读头、不分配、不AddRef，也不保存跨轮range。
class PatternResourceNamesBorrow final
{
public:
    using ReadRange = PatternResourceNamesRange (*)(const void* header);

    explicit PatternResourceNamesBorrow(const EnvironmentVectorStorage<RuntimeCString>& names) noexcept;
    explicit PatternResourceNamesBorrow(const std::vector<std::string>& names) noexcept;
    constexpr PatternResourceNamesBorrow(const void* header, ReadRange readRange) noexcept
        : header_(header), readRange_(readRange) {}

    // 4CBCA8/4CBD18/4CBE76/4A889D：只在原重读区间捕获当前begin/count。
    // native读核统一先begin后end；不声称初次4CBCA8先end的逐指令读序等价。
    [[nodiscard]] PatternResourceNamesRange CaptureRange() const { return readRange_(header_); }

private:
    const void* header_;
    ReadRange readRange_;
};
}
