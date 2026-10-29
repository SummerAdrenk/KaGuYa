#pragma once
#include <Runtime/ReplayButtonNative.h>
#include <Runtime/SecretCgNativeArchive.h>

namespace KaGuYa::Start::Runtime
{
struct ReplaySceneNativeServices;
class ReplaySceneNativeScope final
{
public:
    explicit ReplaySceneNativeScope(ReplaySceneNativeServices&) noexcept;
    ~ReplaySceneNativeScope() noexcept;
    ReplaySceneNativeScope(const ReplaySceneNativeScope&)=delete;
    ReplaySceneNativeScope& operator=(const ReplaySceneNativeScope&)=delete;
private:
    ReplaySceneNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    SecretCgParamsBinding params_;
    ReplayButtonNativeScope buttons_;
};
}
