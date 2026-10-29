#pragma once

namespace KaGuYa::Graphics
{
struct Surface;
bool BoxBlurSurface(Surface& destination, Surface& source, int radius, int rectAddress);
bool GaussianBlurSurface(Surface& destination, Surface& source, int radius, int rectAddress);
}
