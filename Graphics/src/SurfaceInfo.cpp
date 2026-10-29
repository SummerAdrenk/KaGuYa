#include "Surface.h"

#include <stdexcept>

namespace KaGuYa::Graphics
{
SurfaceInfo::SurfaceInfo()
{
    // 原100079F5/10007A1D：先构造局部locker Ref；控制块正常建立后才复制进Info。
    RendererReference<SurfaceLocker>::Captured created;
    created.Publish(new NullSurfaceLocker, 4);
    locker.Assign(created);
}

SurfaceInfo::~SurfaceInfo() noexcept(false) { locker.RetireOwned(); }

int SurfaceInfo::Lock()
{
    // 原1002F3D0：状态先改，底层抛出或失败都不回滚locked。
    if (!valid) return -1;
    if (locked) return 1;
    locked = true;
    auto captured = locker.Capture();
    if (!captured) throw std::logic_error("SurfaceInfo has no locker");
    return captured->Lock();
}

int SurfaceInfo::Unlock()
{
    // 原1002F4B0：已经解锁返回1，不重复调用底层；临时Ref覆盖实际虚调用。
    if (!valid) return -1;
    if (!locked) return 1;
    locked = false;
    auto captured = locker.Capture();
    if (!captured) throw std::logic_error("SurfaceInfo has no locker");
    return captured->Unlock();
}

Surface::Surface()
{
    // 原CMemSurface10006B7D/10006BB3：Info构造完成后发布独立Ref。
    info.Publish(new SurfaceInfo);
}

Surface::~Surface() noexcept(false)
{
    // 像素属于CMemSurface；不能让最后的Info引用替代Surface的像素释放时点。
    if (info)
    {
        info->pixels.Release();
        info->valid = false;
    }
    info.RetireOwned();
}
}
