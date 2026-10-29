#include "GdiBackendState.h"
#include "Surface.h"
#include "SurfaceManager.h"

#include <stdexcept>

namespace KaGuYa::Graphics
{
GdiBackendState::~GdiBackendState() noexcept(false)
{
    // 原05AF0：manager先退，异常只展开尚未执行的dirty/primary/Draw。
    ReferenceRetirement draw(draw_);
    ReferenceRetirement primary(primary_);
    ReferenceRetirement dirty(dirty_);
    manager_.RetireOwned();
    dirty.Retire();
    primary.Retire();
    draw.Retire();
}

SurfaceInfo* GdiBackendState::Screen() const noexcept
{ return primary_ ? primary_->info.get() : nullptr; }

void GdiBackendState::Close()
{
    // 原02480先primary，后024B3整个Draw；dirty/manager不属于Close。
    primary_.RetireOwned();
    draw_.RetireOwned();
}

GdiDraw& GdiBackendState::Draw() const
{
    if (!draw_) throw std::logic_error("GDI Draw reference is absent");
    return *draw_.get();
}

bool GdiBackendState::InitializeDraw(HWND window, int width, int height)
{
    // 原02117/02175：新body先于退旧；两种实际Draw的control stride分别40/28。
    draw_.Publish(new GdiPlusDraw, 0x28);
    if (Draw().Initialize(window, width, height) == 0) return true;
    // 只有原正常非零返回才替换；异常直接交renderer handler，不尝试DIB。
    draw_.Publish(new DibDraw, 0x1C);
    return Draw().Initialize(window, width, height) == 0;
}
}
