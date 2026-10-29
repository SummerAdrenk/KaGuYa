#include <Runtime/FontManagerRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
BOOL Win32FontResourcePlatformApi::RemoveFontResource(const char* name, DWORD flags, void* reserved)
{
    return ::RemoveFontResourceExA(name, flags, reserved);
}

FontManagerRuntime::FontManagerRuntime(IFontResourcePlatformApi& platform) noexcept
    : platform_(platform)
{
    ConstructFontManagerAt48ACA9(body_);
}

FontManagerRuntime::~FontManagerRuntime() noexcept
{
    FontManagerNativeScope scope(platform_);
    DestroyFontManagerModernOwner(body_, secondaryStorageDestroyed_);
}

void FontManagerRuntime::AdoptResourceReference(FontResourceReferenceSlotRuntime reference)
{
    AdoptFontResourceReferenceModern(body_.resourcesAt04, reference);
}
void FontManagerRuntime::AdoptSecondaryReference(YtlReferenceSlotRuntime reference)
{
    if (secondaryStorageDestroyed_)
        throw std::logic_error("Cannot adopt a font secondary reference after 462140 destruction");
    AdoptFontSecondaryReferenceModern(body_.secondaryAt10, reference);
}
}
