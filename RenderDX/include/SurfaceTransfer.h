#pragma once
#include <Windows.h>
#include <d3d9.h>

namespace KaGuYa::RenderDX
{
// 原1930/02F90：上传画布到纹理level0，区域与原始目标点联动裁剪。
// 像素转换仍复用已验的XRGB叶；本层负责实际表面/锁/临时资源与失败返回。
bool TransferPrimarySurface(IDirect3DTexture9* texture, IDirect3DSurface9* source,
    const RECT& dirty);
}
