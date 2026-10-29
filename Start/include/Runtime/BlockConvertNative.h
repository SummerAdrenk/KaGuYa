#pragma once

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct ArchiveCodecManagerRuntime;

// 445220只申请10h裸正文，不写虚表、尺寸或默认block；由447A8D逐字段配置。
struct BlockConvertNativeBody10h final
{
    std::int32_t widthAt00;
    std::int32_t heightAt04;
    std::int32_t channelsAt08;
    std::int32_t blockSizeAt0C;
};
static_assert(sizeof(BlockConvertNativeBody10h) == 0x10);
static_assert(offsetof(BlockConvertNativeBody10h, blockSizeAt0C) == 0x0C);

[[nodiscard]] BlockConvertNativeBody10h* GetBlockConvertAt445220(ArchiveCodecManagerRuntime& manager);

// 原入口没有span或输出长度；调用者保证所读写范围有效且block为正。
// 目标旧字节是输入状态的一部分；未知映射不得预清零或复制previous覆盖。
// 命令区耗尽按445722抛原CP932的char*，不是现代格式枚举或std::exception。
void DecodeBlockConvertNativeAt445300(BlockConvertNativeBody10h& body,
    const std::uint8_t* previous, std::uint8_t* destination, const std::uint8_t* payload);
}
