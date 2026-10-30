#pragma once

#include "RendererReference.h"
#include "Surface.h"
#include "SurfaceManager.h"
#include <Windows.h>
#include <array>

namespace KaGuYa::Graphics
{
// 原dirty对象的vptr占首DWORD，析构虚槽不可因final而删除。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunnecessary-virtual-specifier"
#endif
struct DirtyRectState final
{
    virtual ~DirtyRectState() = default;
    std::array<RECT, 6> rectangles;
    std::array<int, 6> areas;
    int count = 0;
};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
static_assert(sizeof(DirtyRectState) == 0x80);

struct BorrowedPrimaryPixels
{
    int pitch;
    std::uint8_t* bits;
};

// 此接口只在重写模块内部使用；真实资源成员属于各renderer，不另放全局owner。
class RendererState
{
public:
    virtual ~RendererState() noexcept(false) = default;
    virtual int Kind() const noexcept = 0;
    virtual SurfaceInfo* Screen() const noexcept = 0;
    virtual RendererReference<DirtyRectState>& Dirty() noexcept = 0;
    virtual RendererReference<SurfaceManager>& Manager() noexcept = 0;
};
}
