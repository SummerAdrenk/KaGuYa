#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct UpdateUiNativeMap04h final
{
    std::uint16_t commandAt00;
    std::uint16_t flagsAt02;
};

struct UpdateUiNativeData08h final
{
    std::uint16_t flagsAt00;
    std::uint16_t wordAt02;
    void* textAt04;
};

// 原 CMainFrame+3Ch 的18h正文，不携带服务、虚表或自动析构资格。
// caller须保留原始存储字节；不能用{}或成员初值抹掉未写的+17。
struct UpdateUiNativeBody18h final
{
    void* elementsAt00;
    std::uint32_t countAt04;
    std::uint32_t capacityAt08;
    const UpdateUiNativeMap04h* mapAt0C;
    UpdateUiNativeData08h* dataAt10;
    std::uint16_t wordAt14;
    std::uint8_t byteAt16;
    std::uint8_t unwrittenAt17;
};

static_assert(sizeof(void*) == 4 && sizeof(std::size_t) == 4);
static_assert(sizeof(UpdateUiNativeMap04h) == 4 && alignof(UpdateUiNativeMap04h) == 2);
static_assert(offsetof(UpdateUiNativeMap04h, commandAt00) == 0);
static_assert(offsetof(UpdateUiNativeMap04h, flagsAt02) == 2);
static_assert(sizeof(UpdateUiNativeData08h) == 8 && alignof(UpdateUiNativeData08h) == 4);
static_assert(offsetof(UpdateUiNativeData08h, flagsAt00) == 0);
static_assert(offsetof(UpdateUiNativeData08h, wordAt02) == 2);
static_assert(offsetof(UpdateUiNativeData08h, textAt04) == 4);
static_assert(sizeof(UpdateUiNativeBody18h) == 0x18 && alignof(UpdateUiNativeBody18h) == 4);
static_assert(offsetof(UpdateUiNativeBody18h, elementsAt00) == 0);
static_assert(offsetof(UpdateUiNativeBody18h, countAt04) == 4);
static_assert(offsetof(UpdateUiNativeBody18h, capacityAt08) == 8);
static_assert(offsetof(UpdateUiNativeBody18h, mapAt0C) == 0x0C);
static_assert(offsetof(UpdateUiNativeBody18h, dataAt10) == 0x10);
static_assert(offsetof(UpdateUiNativeBody18h, wordAt14) == 0x14);
static_assert(offsetof(UpdateUiNativeBody18h, byteAt16) == 0x16);
static_assert(offsetof(UpdateUiNativeBody18h, unwrittenAt17) == 0x17);
static_assert(std::is_standard_layout_v<UpdateUiNativeMap04h>);
static_assert(std::is_standard_layout_v<UpdateUiNativeData08h>);
static_assert(std::is_trivially_copyable_v<UpdateUiNativeMap04h>);
static_assert(std::is_trivially_copyable_v<UpdateUiNativeData08h>);
static_assert(std::is_standard_layout_v<UpdateUiNativeBody18h>);
static_assert(std::is_trivially_copyable_v<UpdateUiNativeBody18h>);
static_assert(std::is_trivially_default_constructible_v<UpdateUiNativeBody18h>);
static_assert(std::is_trivially_destructible_v<UpdateUiNativeBody18h>);

// 三个原叶均为单栈参数cdecl；服务借用且不入正文。注入叶允许抛C++异常，
// 不能因默认CRT delete/free不抛就把虚接口或Release标为noexcept。
class IUpdateUiNativeMemory
{
protected:
    ~IUpdateUiNativeMemory() = default;
public:
    virtual void* NewArrayAt53FC09(std::size_t bytes) = 0;
    virtual void DeleteArrayAt53FC12(void* block) = 0;
    virtual void FreeAt54B2C0(void* block) = 0;
};

IUpdateUiNativeMemory& DefaultUpdateUiNativeMemory() noexcept;
const std::array<UpdateUiNativeMap04h, 11>& UpdateUiNativeMapAt5AE9A0() noexcept;

// 原ctor ECX=this/EAX=this；本接口显式传正文，不添加raw thiscall/TLS分派层。
UpdateUiNativeBody18h* InitializeUpdateUiNativeAt40B600(
    UpdateUiNativeBody18h&, IUpdateUiNativeMemory& = DefaultUpdateUiNativeMemory());

// 原40EFB0无本地catch。map空不释放data；正常尾只清数组三字段，不清data。
void ReleaseUpdateUiNativeAt40EFB0(
    UpdateUiNativeBody18h&, IUpdateUiNativeMemory& = DefaultUpdateUiNativeMemory());
}
