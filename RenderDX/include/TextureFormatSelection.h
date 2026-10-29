#pragma once
#include <d3d9.h>

namespace KaGuYa::RenderDX
{
// 原04735：请求固定为X8R8G8B8，按原表、兼容矩阵及分量损失评分选择。
D3DFORMAT SelectPrimaryTextureFormat(IDirect3DDevice9* device) noexcept;
// 原0491E在本调用链固定为2D/managed/usage=0/请求完整mip链；输出只在成功尾提交。
HRESULT PreparePrimaryTexture(IDirect3DDevice9* device, UINT& width, UINT& height,
    UINT& levels, D3DFORMAT& format) noexcept;
}

