#include <Runtime/CountDownWindowNativeResourceProfile.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Name = std::array<char, 80>;

// 原6014D0的60项及601470的12项均逐byte核对至NUL。
constexpr std::string_view prefix =
    "\x8E\xCB\x90\xB8\x83\x4A\x83\x45\x83\x93\x83\x67\x83\x5F\x83\x45\x83\x93_";
constexpr std::array<std::string_view, 4> styles{{
    "\x83\x7A\x83\x8F\x83\x43\x83\x67", "\x83\x4F\x83\x8C\x81\x5B",
    "\x83\x7A\x83\x8F\x83\x43\x83\x67\x94\xBC\x93\xA7\x96\xBE",
    "\x83\x4F\x83\x8C\x81\x5B\x94\xBC\x93\xA7\x96\xBE"}};
constexpr std::array<std::string_view, 3> sizes{{"\x8F\xAC", "\x92\x86", "\x91\xE5"}};
constexpr std::array<std::string_view, 5> digits{{"1", "2", "3", "4", "5"}};
constexpr std::string_view suffix = "\x8E\xCB\x90\xB8\x82\xDC\x82\xC5";
constexpr std::string_view extension = ".vrm";

template<std::size_t N>
consteval std::size_t LongestPart(const std::array<std::string_view, N>& parts)
{
    std::size_t longest = 0;
    for (const auto part : parts)
        if (part.size() > longest) longest = part.size();
    return longest;
}

constexpr auto longestNumberWithNul = prefix.size() + LongestPart(digits) +
    LongestPart(styles) + LongestPart(sizes) + extension.size() + 1;
constexpr auto longestSuffixWithNul = prefix.size() + LongestPart(styles) +
    suffix.size() + LongestPart(sizes) + extension.size() + 1;
static_assert(longestNumberWithNul == 41 && longestNumberWithNul <= Name{}.size());
static_assert(longestSuffixWithNul == 48 && longestSuffixWithNul <= Name{}.size());

consteval Name JoinName(const std::array<std::string_view, 5>& parts)
{
    Name result{};
    std::size_t offset = 0;
    for (const auto part : parts)
        for (const char byte : part) result.at(offset++) = byte;
    // 越长只允许编译期失败，不截断或在运行期补默认名。
    result.at(offset) = '\0';
    return result;
}

consteval CountDownWindowNativeResourceProfile MakeSample18Profile()
{
    CountDownWindowNativeResourceProfile result{};
    for (std::size_t style = 0; style < styles.size(); ++style)
        for (std::size_t size = 0; size < sizes.size(); ++size)
        {
            // 两原getter均按size + 3*(color + 2*alpha)寻址。
            const auto variant = style * sizes.size() + size;
            result.suffixNamesCp932.at(variant) =
                JoinName({prefix, styles[style], suffix, sizes[size], extension});
            for (std::size_t digit = 0; digit < digits.size(); ++digit)
                result.numberNamesCp932.at(variant * digits.size() + digit) =
                    JoinName({prefix, digits[digit], styles[style], sizes[size], extension});
        }
    // 原6015C0/6014A0各六POINT，按size+3*mode读取。
    // 45867E..4586BA构造边界分别读取两表第2项。
    result.numberPositions = {{{1222, 640}, {1210, 627}, {1198, 614},
        {36, 640}, {24, 627}, {12, 614}}};
    result.suffixPositions = {{{1222, 627}, {1210, 610}, {1198, 593},
        {36, 627}, {24, 610}, {12, 593}}};
    return result;
}

constexpr CountDownWindowNativeResourceProfile profile = MakeSample18Profile();

struct NameDigest final
{
    std::uint64_t hash = 14695981039346656037ull;
    std::size_t bytes = 0;
};

consteval NameDigest DigestNames(const CountDownWindowNativeResourceProfile& value)
{
    NameDigest result;
    const auto append = [&result](const auto& names) {
        for (const auto& name : names)
            for (const char byte : name)
            {
                result.hash = (result.hash ^ static_cast<unsigned char>(byte)) * 1099511628211ull;
                ++result.bytes;
                if (byte == '\0') break;
            }
    };
    append(value.numberNamesCp932);
    append(value.suffixNamesCp932);
    return result;
}

consteval std::uint64_t DigestPositions(const CountDownWindowNativeResourceProfile& value)
{
    std::uint64_t hash = 14695981039346656037ull;
    const auto append = [&hash](const auto& positions) {
        for (const POINT point : positions)
            for (const LONG coordinate : {point.x, point.y})
                for (unsigned shift = 0; shift < 32; shift += 8)
                    hash = (hash ^ ((static_cast<std::uint32_t>(coordinate) >> shift) & 255u)) *
                        1099511628211ull;
    };
    append(value.numberPositions);
    append(value.suffixPositions);
    return hash;
}

// 独立原PE字节流见证亦存于CountDownWindowSelfTest的5810/5822。
constexpr auto namesDigest = DigestNames(profile);
static_assert(namesDigest.bytes == 2748);
static_assert(namesDigest.hash == 0x72B61810B9324541ull);
static_assert(DigestPositions(profile) == 0xC87A88BAFDA9BBC9ull);
}

const CountDownWindowNativeResourceProfile& Sample18CountDownWindowNativeResourceProfile() noexcept
{
    return profile;
}
}
