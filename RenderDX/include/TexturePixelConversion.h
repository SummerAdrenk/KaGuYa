#pragma once

#include <Windows.h>
#include <d3d9.h>

#include <cstddef>
#include <cstdint>

namespace KaGuYa::RenderDX
{
// 仅接受原纹理协商从 X8R8G8B8 可达的 15 种格式；其它格式返回 0。
std::size_t TexturePixelBytes(D3DFORMAT format) noexcept;

// 原 10002F90 的 filter=1、colorKey=0、等尺寸逐像素上传，不包含缩放或抖动。
// 调用者负责锁定、裁剪、有效内存及行跨度；源为 B,G,R,X，每像素 4 字节。
// 源/目标区间须不重叠，唯 22->22 允许同一指针；只写本行像素，不写 padding。
// 不支持的格式、非空行的空指针或字节数溢出返回 false，且不写目标。
// 支持的格式在 pixelCount=0 时返回 true，允许空指针。
bool ConvertXrgbPixels(D3DFORMAT destinationFormat, const std::uint8_t* source,
    std::uint8_t* destination, std::size_t pixelCount) noexcept;

// 原 100089F3/100137CB/100065C2 的有限整区域转换，源固定为 22/X8R8G8B8。
// 两指针已指向裁后区域起点；pitch 为正，width/height 为相同的裁后尺寸。
// 调用者保持双锁和有效存储，区域不重叠；22 同指针同 pitch 亦可。
// 无效格式、空区域、空指针、跨度不足或尺寸/偏移溢出返回 false，不分配或写入。
// 正常返回 true；先建目的再建源视图，均为 Win32 的 128 字节真实对象。
// 22 同格式不建 float 行；异格式建立并使用一条 16*width 字节 RGBA 行。
// 分配失败保持 std::bad_alloc 传播，异常清理真实 owner，不转换为 false。
bool ConvertXrgbRegion(D3DFORMAT destinationFormat, const std::uint8_t* source,
    std::ptrdiff_t sourcePitch, std::uint8_t* destination, std::ptrdiff_t destinationPitch,
    std::size_t width, std::size_t height);
}
