#include <Runtime/SaveLoadSystemPageNative.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
void RootSaveLoadSystemPageNativeSource::EnsureRootAt413BE0() { root_.EnsureCreatedAt413BE0(); }
void* RootSaveLoadSystemPageNativeSource::RootAt6198C4() { return root_.ReadPublishedAt6198C4(); }
std::uint8_t RootSaveLoadSystemPageNativeSource::RootByteAt218(void* root)
{ return static_cast<RootRuntimePayload*>(root)->ReadByteAt218(); }
InputRootNativeBody24h* RootSaveLoadSystemPageNativeSource::InputAt418210()
{ return input_.CaptureAt418210(); }
}
