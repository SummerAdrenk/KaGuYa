#pragma once
#include <Runtime/LikabilityDialogNative.h>

namespace KaGuYa::Start::Runtime
{
struct LikabilitySceneNativeServices;
class LikabilitySceneNativeScope final
{
public:
    explicit LikabilitySceneNativeScope(LikabilitySceneNativeServices&)noexcept;
    ~LikabilitySceneNativeScope()noexcept;
    LikabilitySceneNativeScope(const LikabilitySceneNativeScope&)=delete;
    LikabilitySceneNativeScope& operator=(const LikabilitySceneNativeScope&)=delete;
private:
    LikabilitySceneNativeServices* previous_;
    LikabilityDialogNativeScope gui_;
};
}
