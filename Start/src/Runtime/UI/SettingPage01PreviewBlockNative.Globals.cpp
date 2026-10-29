#include <Runtime/SettingPage01PreviewBlockNative.h>
#include <Runtime/SettingTextDisplayResourceProfile.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
void Construct(RuntimeCString& fresh, const char* bytes, int length, IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(fresh, strings);
    struct Unwind final
    {
        RuntimeCString* value;
        ~Unwind() noexcept { if (value) DestroyRuntimeCStringAt40CD40(value); }
    } unwind{&fresh};
    SetRuntimeCStringAt437680(fresh, bytes, length);
    unwind.value = nullptr;
}
}
void ConstructSettingPage01PreviewFontFaceAt407310(RuntimeCString& fresh, IRuntimeStringManagerSource& strings)
{
    const auto face = CurrentSettingTextDisplayProfile().preview.fontFace;
    Construct(fresh, face.data(), static_cast<int>(face.size()), strings);
}
void ConstructSettingPage01PreviewSampleAt4073A0(RuntimeCString& fresh, IRuntimeStringManagerSource& strings)
{
    // Original 17:405DF0 / 18:4073A0 use CP932 and Size=0x0E.
    // The 18 profile explicitly preserves the separately verified 14-byte GBK customization.
    const auto sample = CurrentSettingTextDisplayProfile().preview.sample;
    Construct(fresh, sample.data(), static_cast<int>(sample.size()), strings);
}
}
