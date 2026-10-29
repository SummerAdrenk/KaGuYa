#include "SaveLoadPopupNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 权威18号PE的CP932字节、null与重复指针直接编译，绝不按名字猜测或运行期读PE。
constexpr char name5B94AC[] = "\x83\x75\x83\x89\x83\x62\x83\x4E\x83\x41\x83\x45\x83\x67\x82\x50\x82\x55\x82\x4F\x2E\x62\x67\x64";
constexpr char name5B94C8[] = "\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x6F\x81\x5B\x2E\x76\x72\x6D";
constexpr char name5B94E0[] = "\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x6F\x81\x5B\x2E\x76\x72\x6D";
constexpr char name5B94F8[] = "\x83\x58\x83\x89\x83\x43\x83\x68\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9510[] = "\x83\x58\x83\x89\x83\x43\x83\x68\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9528[] = "\x4C\x83\x58\x83\x89\x83\x43\x83\x68\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9540[] = "\x4C\x83\x58\x83\x89\x83\x43\x83\x68\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9558[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9574[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B958C[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B95A4[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B95C0[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B95DC[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B95F4[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B960C[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9628[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9644[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B965C[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9674[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9690[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B96AC[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B96C4[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B96DC[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B96F8[] = "\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9718[] = "\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9734[] = "\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9754[] = "\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B9770[] = "\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x49\x83\x74\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9790[] = "\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B97B0[] = "\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B97CC[] = "\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B97EC[] = "\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B9808[] = "\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x49\x83\x74\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9828[] = "\x70\x30\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9838[] = "\x70\x31\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9848[] = "\x70\x32\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9858[] = "\x70\x33\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9868[] = "\x70\x34\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9878[] = "\x70\x35\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9888[] = "\x70\x36\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9898[] = "\x70\x37\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B98A8[] = "\x70\x38\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B98B8[] = "\x70\x39\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B98C8[] = "\x70\x30\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B98D4[] = "\x70\x31\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B98E0[] = "\x70\x32\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B98EC[] = "\x70\x33\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B98F8[] = "\x70\x34\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9904[] = "\x70\x35\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9910[] = "\x70\x36\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B991C[] = "\x70\x37\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9928[] = "\x70\x38\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9934[] = "\x70\x39\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9940[] = "\x70\x30\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9950[] = "\x70\x31\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9960[] = "\x70\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9970[] = "\x70\x33\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9980[] = "\x70\x34\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9990[] = "\x70\x35\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B99A0[] = "\x70\x36\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B99B0[] = "\x70\x37\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B99C0[] = "\x70\x38\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B99D0[] = "\x70\x39\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B99E0[] = "\x4C\x70\x30\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B99F0[] = "\x4C\x70\x31\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9A00[] = "\x4C\x70\x32\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9A10[] = "\x4C\x70\x33\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9A20[] = "\x4C\x70\x34\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9A30[] = "\x4C\x70\x35\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9A40[] = "\x4C\x70\x36\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9A50[] = "\x4C\x70\x37\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9A60[] = "\x4C\x70\x38\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9A70[] = "\x4C\x70\x39\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9A80[] = "\x4C\x70\x30\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9A8C[] = "\x4C\x70\x31\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9A98[] = "\x4C\x70\x32\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9AA4[] = "\x4C\x70\x33\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9AB0[] = "\x4C\x70\x34\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9ABC[] = "\x4C\x70\x35\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9AC8[] = "\x4C\x70\x36\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9AD4[] = "\x4C\x70\x37\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9AE0[] = "\x4C\x70\x38\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9AEC[] = "\x4C\x70\x39\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9AF8[] = "\x4C\x70\x30\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9B08[] = "\x4C\x70\x31\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9B18[] = "\x4C\x70\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9B28[] = "\x4C\x70\x33\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9B38[] = "\x4C\x70\x34\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9B48[] = "\x4C\x70\x35\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9B58[] = "\x4C\x70\x36\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9B68[] = "\x4C\x70\x37\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9B78[] = "\x4C\x70\x38\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9B88[] = "\x4C\x70\x39\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9B98[] = "\x83\x7C\x83\x62\x83\x76\x4E\x45\x57\x2E\x76\x72\x6D";
constexpr char name5B9BA8[] = "\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x81\x96\x2E\x76\x72\x6D";
constexpr char name5B9BB8[] = "\x83\x5E\x83\x51\x81\x96\x2E\x76\x72\x6D";
constexpr char name5B9BC4[] = "\x4C\x83\x7C\x83\x62\x83\x76\x4E\x45\x57\x2E\x76\x72\x6D";
constexpr char name5B9BD4[] = "\x4C\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x81\x96\x2E\x76\x72\x6D";
constexpr char name5B9BE4[] = "\x4C\x83\x5E\x83\x51\x81\x96\x2E\x76\x72\x6D";
constexpr std::array<std::uint32_t, 9> words606534{{0x37u,0x5B94ACu,0x0u,0x0u,0x5B94ACu,0x5B94ACu,0x0u,0x0u,0x0u}};
constexpr std::array<std::uint32_t, 27> words606560{{0x39u,0x5B94F8u,0x0u,0x0u,0x5B9510u,0x5B9510u,0x5B9510u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x1u,0x0u,0x4BEu,0xCFu,0x4D7u,0x25Du,0x0u,0x0u,0x0u,0xFFFFFFCDu}};
constexpr std::array<std::uint32_t, 27> words6065CC{{0x39u,0x5B9528u,0x0u,0x0u,0x5B9540u,0x5B9540u,0x5B9540u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x1u,0x0u,0x4BEu,0xCFu,0x4D7u,0x25Du,0x0u,0x0u,0x0u,0xFFFFFFCDu}};
constexpr std::array<std::uint32_t, 11> words606638{{0x3Au,0x5B9558u,0x5B9574u,0x0u,0x5B958Cu,0x5B958Cu,0x5B95A4u,0x0u,0x0u,0x0u,0xFFFFFFFFu}};
constexpr std::array<std::uint32_t, 11> words606664{{0x3Bu,0x5B95C0u,0x5B95DCu,0x0u,0x5B95F4u,0x5B95F4u,0x5B960Cu,0x0u,0x0u,0x0u,0xFFFFFFFFu}};
constexpr std::array<std::uint32_t, 11> words606690{{0x3Au,0x5B9628u,0x5B9644u,0x0u,0x5B965Cu,0x5B965Cu,0x5B9674u,0x0u,0x0u,0x0u,0xFFFFFFFFu}};
constexpr std::array<std::uint32_t, 11> words6066BC{{0x3Bu,0x5B9690u,0x5B96ACu,0x0u,0x5B96C4u,0x5B96C4u,0x5B96DCu,0x0u,0x0u,0x0u,0xFFFFFFFFu}};
constexpr std::array<std::uint32_t, 18> words6066E8{{0x3Cu,0x5B96F8u,0x0u,0x0u,0x5B9718u,0x5B9718u,0x5B9734u,0x0u,0x5B9754u,0x0u,0x0u,0x5B9770u,0x5B9770u,0x5B9770u,0x0u,0x0u,0x3u,0xFFFFFFFFu}};
constexpr std::array<std::uint32_t, 18> words606730{{0x3Cu,0x5B9790u,0x0u,0x0u,0x5B97B0u,0x5B97B0u,0x5B97CCu,0x0u,0x5B97ECu,0x0u,0x0u,0x5B9808u,0x5B9808u,0x5B9808u,0x0u,0x0u,0x3u,0xFFFFFFFFu}};
constexpr std::array<std::uint32_t, 2> words606558{{0x5B94C8u,0x5B94E0u}};
constexpr std::array<std::uint32_t, 60> words606778{{0x5B9828u,0x5B9838u,0x5B9848u,0x5B9858u,0x5B9868u,0x5B9878u,0x5B9888u,0x5B9898u,0x5B98A8u,0x5B98B8u,0x5B98C8u,0x5B98D4u,0x5B98E0u,0x5B98ECu,0x5B98F8u,0x5B9904u,0x5B9910u,0x5B991Cu,0x5B9928u,0x5B9934u,0x5B9940u,0x5B9950u,0x5B9960u,0x5B9970u,0x5B9980u,0x5B9990u,0x5B99A0u,0x5B99B0u,0x5B99C0u,0x5B99D0u,0x5B99E0u,0x5B99F0u,0x5B9A00u,0x5B9A10u,0x5B9A20u,0x5B9A30u,0x5B9A40u,0x5B9A50u,0x5B9A60u,0x5B9A70u,0x5B9A80u,0x5B9A8Cu,0x5B9A98u,0x5B9AA4u,0x5B9AB0u,0x5B9ABCu,0x5B9AC8u,0x5B9AD4u,0x5B9AE0u,0x5B9AECu,0x5B9AF8u,0x5B9B08u,0x5B9B18u,0x5B9B28u,0x5B9B38u,0x5B9B48u,0x5B9B58u,0x5B9B68u,0x5B9B78u,0x5B9B88u}};
constexpr std::array<std::uint32_t, 6> words606880{{0x5B9B98u,0x5B9BA8u,0x5B9BB8u,0x5B9BC4u,0x5B9BD4u,0x5B9BE4u}};
constexpr std::array<std::uint32_t, 16> constants606868{{99,4,79,4,59,4,0x5B9B98u,0x5B9BA8u,0x5B9BB8u,0x5B9BC4u,0x5B9BD4u,0x5B9BE4u,129,1,31,7}};
const GuiStaticDescriptorNative24h background{55, {{name5B94AC, nullptr, nullptr, name5B94AC, name5B94AC, nullptr, nullptr}}, nullptr};
const GuiSliderDescriptorNative6Ch slider0{57, {{name5B94F8, nullptr, nullptr, name5B9510, name5B9510, name5B9510, nullptr}}, {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}}, nullptr, 0, 1, 0, {1214,207,1239,605}, {0,0,0,-51}};
const SaveLoadPopupNativeDetail::CheckDescriptor check0{60, {{name5B96F8, nullptr, nullptr, name5B9718, name5B9718, name5B9734, nullptr}}, {{name5B9754, nullptr, nullptr, name5B9770, name5B9770, name5B9770, nullptr}}, nullptr, 3, -1};
const GuiButtonDescriptorNative2Ch arrows0[2]{
    {{58, {{name5B9558, name5B9574, nullptr, name5B958C, name5B958C, name5B95A4, nullptr}}, nullptr}, 0, -1},
    {{59, {{name5B95C0, name5B95DC, nullptr, name5B95F4, name5B95F4, name5B960C, nullptr}}, nullptr}, 0, -1},
};
const char* const digits0[30]{name5B9828,name5B9838,name5B9848,name5B9858,name5B9868,name5B9878,name5B9888,name5B9898,name5B98A8,name5B98B8,name5B98C8,name5B98D4,name5B98E0,name5B98EC,name5B98F8,name5B9904,name5B9910,name5B991C,name5B9928,name5B9934,name5B9940,name5B9950,name5B9960,name5B9970,name5B9980,name5B9990,name5B99A0,name5B99B0,name5B99C0,name5B99D0};
const char* const markers0[3]{name5B9B98,name5B9BA8,name5B9BB8};
const SaveLoadPopupNativeDetail::Data data0{background, name5B94C8, slider0, arrows0, check0, digits0, markers0};
const GuiSliderDescriptorNative6Ch slider1{57, {{name5B9528, nullptr, nullptr, name5B9540, name5B9540, name5B9540, nullptr}}, {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}}, nullptr, 0, 1, 0, {1214,207,1239,605}, {0,0,0,-51}};
const SaveLoadPopupNativeDetail::CheckDescriptor check1{60, {{name5B9790, nullptr, nullptr, name5B97B0, name5B97B0, name5B97CC, nullptr}}, {{name5B97EC, nullptr, nullptr, name5B9808, name5B9808, name5B9808, nullptr}}, nullptr, 3, -1};
const GuiButtonDescriptorNative2Ch arrows1[2]{
    {{58, {{name5B9628, name5B9644, nullptr, name5B965C, name5B965C, name5B9674, nullptr}}, nullptr}, 0, -1},
    {{59, {{name5B9690, name5B96AC, nullptr, name5B96C4, name5B96C4, name5B96DC, nullptr}}, nullptr}, 0, -1},
};
const char* const digits1[30]{name5B99E0,name5B99F0,name5B9A00,name5B9A10,name5B9A20,name5B9A30,name5B9A40,name5B9A50,name5B9A60,name5B9A70,name5B9A80,name5B9A8C,name5B9A98,name5B9AA4,name5B9AB0,name5B9ABC,name5B9AC8,name5B9AD4,name5B9AE0,name5B9AEC,name5B9AF8,name5B9B08,name5B9B18,name5B9B28,name5B9B38,name5B9B48,name5B9B58,name5B9B68,name5B9B78,name5B9B88};
const char* const markers1[3]{name5B9BC4,name5B9BD4,name5B9BE4};
const SaveLoadPopupNativeDetail::Data data1{background, name5B94E0, slider1, arrows1, check1, digits1, markers1};
constexpr POINT digitPositions[3]{{99,4},{79,4},{59,4}};
constexpr GuiDialogNavigationMapping navigation0[12]{{60,-1},{61,60},{62,61},{63,62},{64,63},{65,64},{66,65},{67,66},{68,67},{69,68},{70,69},{-1,0}};
constexpr std::array<std::uint32_t,24> navigationWords0{{0x3Cu,0xFFFFFFFFu,0x3Du,0x3Cu,0x3Eu,0x3Du,0x3Fu,0x3Eu,0x40u,0x3Fu,0x41u,0x40u,0x42u,0x41u,0x43u,0x42u,0x44u,0x43u,0x45u,0x44u,0x46u,0x45u,0xFFFFFFFFu,0x0u}};
constexpr GuiDialogNavigationMapping navigation1[12]{{70,-1},{69,70},{68,69},{67,68},{66,67},{65,66},{64,65},{63,64},{62,63},{61,62},{60,61},{-1,0}};
constexpr std::array<std::uint32_t,24> navigationWords1{{0x46u,0xFFFFFFFFu,0x45u,0x46u,0x44u,0x45u,0x43u,0x44u,0x42u,0x43u,0x41u,0x42u,0x40u,0x41u,0x3Fu,0x40u,0x3Eu,0x3Fu,0x3Du,0x3Eu,0x3Cu,0x3Du,0xFFFFFFFFu,0x0u}};
constexpr std::array<std::uint32_t,4> navigationHeaders{{1,0x606908,2,0x6068A8}};
constexpr std::array<std::uint32_t,9> eh5F65D4{{0x19930522u,0x33u,0x5F65F8u,0x0u,0x0u,0x0u,0x0u,0x0u,0x1u}};
constexpr std::array<std::uint32_t,102> eh5F65F8{{0xFFFFFFFFu,0x5A2A20u,0x0u,0x5A2A2Bu,0x1u,0x5A2A3Cu,0x2u,0x5A2A4Du,0x3u,0x5A2A5Eu,0x4u,0x5A2A66u,0x5u,0x5A2A71u,0x4u,0x5A2A71u,0x7u,0x541E55u,0x7u,0x5A2A7Cu,0x7u,0x541E55u,0x4u,0x541E55u,0x4u,0x5A2A84u,0x4u,0x541E55u,0x3u,0x541E55u,0x3u,0x5A2A8Cu,0xFu,0x5A2A94u,0xFu,0x541E55u,0xFu,0x5A2A9Fu,0xFu,0x541E55u,0x3u,0x541E55u,0x3u,0x541E55u,0x3u,0x5A2AAAu,0x16u,0x5A2AB2u,0x16u,0x541E55u,0x16u,0x5A2ABDu,0x16u,0x541E55u,0x16u,0x5A2AC8u,0x1Bu,0x5A2AD0u,0x1Cu,0x5A2AE2u,0x1Bu,0x5A2AF0u,0x1Bu,0x5A2AFBu,0x1Bu,0x5A2B0Du,0x1Bu,0x5A2B18u,0x16u,0x5A2B2Au,0x3u,0x5A2B35u,0x23u,0x5A2B3Du,0x23u,0x541E55u,0x23u,0x5A2B48u,0x23u,0x541E55u,0x3u,0x541E55u,0x3u,0x541E55u,0x3u,0x5A2B53u,0x2Au,0x5A2B5Bu,0x3u,0x541E55u,0x3u,0x541E55u,0x3u,0x5A2B6Du,0x2Eu,0x5A2B75u,0x3u,0x541E55u,0x3u,0x5A2B87u,0x3u,0x541E55u}};
constexpr std::array<std::uint32_t,9> eh5F6460{{0x19930522u,0x8u,0x5F6484u,0x0u,0x0u,0x0u,0x0u,0x0u,0x1u}};
constexpr std::array<std::uint32_t,16> eh5F6484{{0xFFFFFFFFu,0x5A7280u,0x0u,0x5A7288u,0x1u,0x5A7290u,0x1u,0x5A7298u,0x1u,0x5A72A0u,0x1u,0x5A72A8u,0x0u,0x541E55u,0xFFFFFFFFu,0x541E55u}};
constexpr std::array<std::uint32_t,9> eh5F65B0{{0x19930522u,0x1u,0x5F65A8u,0x0u,0x0u,0x0u,0x0u,0x0u,0x1u}};
constexpr std::array<std::uint32_t,2> eh5F65A8{{0xFFFFFFFFu,0x5A6700u}};
constexpr std::array<std::uint32_t,9> eh5F67C4{{0x19930522u,0x1u,0x5F67BCu,0x0u,0x0u,0x0u,0x0u,0x0u,0x1u}};
constexpr std::array<std::uint32_t,2> eh5F67BC{{0xFFFFFFFFu,0x5A8770u}};
constexpr std::array<std::uint32_t,9> eh5F632C{{0x19930522u,0x3u,0x5F6314u,0x0u,0x0u,0x0u,0x0u,0x0u,0x5u}};
constexpr std::array<std::uint32_t,6> eh5F6314{{0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x541E55u}};
constexpr std::array<std::uint32_t,2> numberReferenceSlots{{0x53DA20,0x4FBCC0}};
constexpr std::array<std::uint32_t,9> eh5F64C4{{0x19930522u,0x18u,0x5F64E8u,0x0u,0x0u,0x0u,0x0u,0x0u,0x1u}};
constexpr std::array<std::uint32_t,48> eh5F64E8{{0xFFFFFFFFu,0x5A7690u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x5A7698u,0x3u,0x5A76A0u,0x3u,0x541E55u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x5A76A8u,0x7u,0x5A76B0u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x5A76B8u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x5A76C0u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x5A76C8u,0x10u,0x5A76D0u,0x10u,0x541E55u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x5A76D8u,0x14u,0x5A76E0u,0x14u,0x541E55u,0xFFFFFFFFu,0x541E55u}};
constexpr std::array<std::uint32_t,9> eh5F67F8{{0x19930522u,0x2u,0x5F67E8u,0x0u,0x0u,0x0u,0x0u,0x0u,0x1u}};
constexpr std::array<std::uint32_t,4> eh5F67E8{{0xFFFFFFFFu,0x5A7DA0u,0xFFFFFFFFu,0x541E55u}};
constexpr std::array<std::uint32_t,9> eh5F63A0{{0x19930522u,0xDu,0x5F63C4u,0x0u,0x0u,0x0u,0x0u,0x0u,0x1u}};
constexpr std::array<std::uint32_t,26> eh5F63C4{{0xFFFFFFFFu,0x5A83E0u,0xFFFFFFFFu,0x5A83E8u,0xFFFFFFFFu,0x5A83F0u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x5A83F8u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x5A8400u,0xFFFFFFFFu,0x5A8408u,0xFFFFFFFFu,0x5A8410u,0xFFFFFFFFu,0x5A8418u,0xFFFFFFFFu,0x541E55u,0xFFFFFFFFu,0x5A8420u,0xFFFFFFFFu,0x541E55u}};
constexpr std::array<std::uint32_t,9> eh5F6824{{0x19930522u,0x1u,0x5F681Cu,0x0u,0x0u,0x0u,0x0u,0x0u,0x5u}};
constexpr std::array<std::uint32_t,2> eh5F681C{{0xFFFFFFFFu,0x541E55u}};
constexpr std::array<std::uint32_t,9> eh5F643C{{0x19930522u,0x2u,0x5F642Cu,0x0u,0x0u,0x0u,0x0u,0x0u,0x1u}};
constexpr std::array<std::uint32_t,4> eh5F642C{{0xFFFFFFFFu,0x5A9350u,0xFFFFFFFFu,0x541E55u}};
const SaveLoadPopupNativeWords evidence[]{
    {0x5F64C4u, eh5F64C4},
    {0x5F64E8u, eh5F64E8},
    {0x5F67F8u, eh5F67F8},
    {0x5F67E8u, eh5F67E8},
    {0x5F63A0u, eh5F63A0},
    {0x5F63C4u, eh5F63C4},
    {0x5F6824u, eh5F6824},
    {0x5F681Cu, eh5F681C},
    {0x5F643Cu, eh5F643C},
    {0x5F642Cu, eh5F642C},
    {0x5F65D4u, eh5F65D4},
    {0x5F65F8u, eh5F65F8},
    {0x5F6460u, eh5F6460},
    {0x5F6484u, eh5F6484},
    {0x5F65B0u, eh5F65B0},
    {0x5F65A8u, eh5F65A8},
    {0x5F67C4u, eh5F67C4},
    {0x5F67BCu, eh5F67BC},
    {0x5F632Cu, eh5F632C},
    {0x5F6314u, eh5F6314},
    {0x5C0DDC, numberReferenceSlots},
    {0x606534u, words606534},
    {0x606560u, words606560},
    {0x6065CCu, words6065CC},
    {0x606638u, words606638},
    {0x606664u, words606664},
    {0x606690u, words606690},
    {0x6066BCu, words6066BC},
    {0x6066E8u, words6066E8},
    {0x606730u, words606730},
    {0x606558u, words606558},
    {0x606778u, words606778},
    {0x606880u, words606880},
    {0x606868, constants606868}, {0x606908, navigationWords0}, {0x6068A8, navigationWords1}, {0x606968, navigationHeaders}
};
const SaveLoadPopupNativeString strings[]{
    {0x5B94ACu, name5B94AC},
    {0x5B94C8u, name5B94C8},
    {0x5B94E0u, name5B94E0},
    {0x5B94F8u, name5B94F8},
    {0x5B9510u, name5B9510},
    {0x5B9528u, name5B9528},
    {0x5B9540u, name5B9540},
    {0x5B9558u, name5B9558},
    {0x5B9574u, name5B9574},
    {0x5B958Cu, name5B958C},
    {0x5B95A4u, name5B95A4},
    {0x5B95C0u, name5B95C0},
    {0x5B95DCu, name5B95DC},
    {0x5B95F4u, name5B95F4},
    {0x5B960Cu, name5B960C},
    {0x5B9628u, name5B9628},
    {0x5B9644u, name5B9644},
    {0x5B965Cu, name5B965C},
    {0x5B9674u, name5B9674},
    {0x5B9690u, name5B9690},
    {0x5B96ACu, name5B96AC},
    {0x5B96C4u, name5B96C4},
    {0x5B96DCu, name5B96DC},
    {0x5B96F8u, name5B96F8},
    {0x5B9718u, name5B9718},
    {0x5B9734u, name5B9734},
    {0x5B9754u, name5B9754},
    {0x5B9770u, name5B9770},
    {0x5B9790u, name5B9790},
    {0x5B97B0u, name5B97B0},
    {0x5B97CCu, name5B97CC},
    {0x5B97ECu, name5B97EC},
    {0x5B9808u, name5B9808},
    {0x5B9828u, name5B9828},
    {0x5B9838u, name5B9838},
    {0x5B9848u, name5B9848},
    {0x5B9858u, name5B9858},
    {0x5B9868u, name5B9868},
    {0x5B9878u, name5B9878},
    {0x5B9888u, name5B9888},
    {0x5B9898u, name5B9898},
    {0x5B98A8u, name5B98A8},
    {0x5B98B8u, name5B98B8},
    {0x5B98C8u, name5B98C8},
    {0x5B98D4u, name5B98D4},
    {0x5B98E0u, name5B98E0},
    {0x5B98ECu, name5B98EC},
    {0x5B98F8u, name5B98F8},
    {0x5B9904u, name5B9904},
    {0x5B9910u, name5B9910},
    {0x5B991Cu, name5B991C},
    {0x5B9928u, name5B9928},
    {0x5B9934u, name5B9934},
    {0x5B9940u, name5B9940},
    {0x5B9950u, name5B9950},
    {0x5B9960u, name5B9960},
    {0x5B9970u, name5B9970},
    {0x5B9980u, name5B9980},
    {0x5B9990u, name5B9990},
    {0x5B99A0u, name5B99A0},
    {0x5B99B0u, name5B99B0},
    {0x5B99C0u, name5B99C0},
    {0x5B99D0u, name5B99D0},
    {0x5B99E0u, name5B99E0},
    {0x5B99F0u, name5B99F0},
    {0x5B9A00u, name5B9A00},
    {0x5B9A10u, name5B9A10},
    {0x5B9A20u, name5B9A20},
    {0x5B9A30u, name5B9A30},
    {0x5B9A40u, name5B9A40},
    {0x5B9A50u, name5B9A50},
    {0x5B9A60u, name5B9A60},
    {0x5B9A70u, name5B9A70},
    {0x5B9A80u, name5B9A80},
    {0x5B9A8Cu, name5B9A8C},
    {0x5B9A98u, name5B9A98},
    {0x5B9AA4u, name5B9AA4},
    {0x5B9AB0u, name5B9AB0},
    {0x5B9ABCu, name5B9ABC},
    {0x5B9AC8u, name5B9AC8},
    {0x5B9AD4u, name5B9AD4},
    {0x5B9AE0u, name5B9AE0},
    {0x5B9AECu, name5B9AEC},
    {0x5B9AF8u, name5B9AF8},
    {0x5B9B08u, name5B9B08},
    {0x5B9B18u, name5B9B18},
    {0x5B9B28u, name5B9B28},
    {0x5B9B38u, name5B9B38},
    {0x5B9B48u, name5B9B48},
    {0x5B9B58u, name5B9B58},
    {0x5B9B68u, name5B9B68},
    {0x5B9B78u, name5B9B78},
    {0x5B9B88u, name5B9B88},
    {0x5B9B98u, name5B9B98},
    {0x5B9BA8u, name5B9BA8},
    {0x5B9BB8u, name5B9BB8},
    {0x5B9BC4u, name5B9BC4},
    {0x5B9BD4u, name5B9BD4},
    {0x5B9BE4u, name5B9BE4},
};
}
std::span<const SaveLoadPopupNativeWords> SaveLoadPopupNativeOriginalData() noexcept { return evidence; }
std::span<const SaveLoadPopupNativeString> SaveLoadPopupNativeOriginalStrings() noexcept { return strings; }
namespace SaveLoadPopupNativeDetail
{
Data DataForId(std::int32_t id)
{
    if(id!=6&&id!=7)throw std::out_of_range("save popup id outside profile");
    const auto& profile=CurrentSaveLoadResourceProfile().popup;
    const auto& theme=profile.themes[id==6?0:1];
    if(!theme.slider)throw std::logic_error("save popup slider is not bound");
    if (profile.visibleRows <= 0 || (profile.digitStates != 3 && profile.digitStates != 4) ||
        theme.digits.size() != static_cast<std::size_t>(profile.digitStates * 10) ||
        theme.markers.size() != 3 || profile.digitPositions.size() != (profile.tiledFrame ? 4u : 3u))
        throw std::logic_error("save popup profile has incomplete resources");
    return {theme.background,theme.bar,*theme.slider,theme.arrows.data(),theme.checkbox,
        theme.digits.data(),theme.markers.data()};
}
int PageCount()
{
    auto* save = Save(RequireSaveLoadPopupNativeServices().source.CaptureEnvironmentAt4180C0());
    if (!save) throw std::logic_error("save popup catalog is not bound");
    return static_cast<int>(save->RegularPages().size());
}
std::span<const POINT> DigitPositions() { return CurrentSaveLoadResourceProfile().popup.digitPositions; }
POINT LatestPosition() { return CurrentSaveLoadResourceProfile().popup.latestOrigin; }
POINT UsedPosition() { return CurrentSaveLoadResourceProfile().popup.usedOrigin; }
const GuiDialogNavigationMapping* NavigationPairs(unsigned i)
{ return CurrentSaveLoadResourceProfile().popup.navigation[i].pairs.data(); }
}
}
