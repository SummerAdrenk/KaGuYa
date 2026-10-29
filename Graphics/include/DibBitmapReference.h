#pragma once

#include "NativeObjectReference.h"

namespace KaGuYa::Graphics
{
class DibBitmap;

using DibBitmapReference = NativeObjectReference<DibBitmap, 0x1C>;
static_assert(sizeof(DibBitmapReference) == 12);
}
