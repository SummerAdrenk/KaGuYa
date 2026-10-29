#pragma once
#include <Windows.h>

namespace KaGuYa::Graphics
{
class SurfaceInfo;

// 原 Graphics.dll:100083C0 mode0：Lock 成功后读取当前 clearColor，仅清格式 7/12。
// 正常 Unlock 忽略返回值，无异常补偿；返回 0（含不支持格式的空操作）或 1（Lock 失败）。
int ClearTextWorkSurface(SurfaceInfo& surface);

// 原 Graphics.dll:100111C0 / 10011A30：文字源格式 12，目标格式 7/12；blend 选择 mode10，否则 mode0。
// 返回 0 正常（含空裁剪）、1 锁失败、2 不支持格式；同步借用 Info，不新增所有权或像素快照。
int SubmitTextSurface(SurfaceInfo& dst, SurfaceInfo& src, int x, int y, bool blend);

// GDI Draw上传的mode0：零偏移、源全图，dirty只作目的clip；真实Info锁与0/1/2状态。
int CopyGdiSurface(SurfaceInfo& dst, SurfaceInfo& src, const RECT* clip);
}
