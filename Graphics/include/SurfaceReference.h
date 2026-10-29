#pragma once

#include "NativeObjectReference.h"

namespace KaGuYa::Graphics
{
struct Surface;
using SurfaceReference = NativeObjectReference<Surface, 0x34>;
static_assert(sizeof(SurfaceReference) == 12);
}
