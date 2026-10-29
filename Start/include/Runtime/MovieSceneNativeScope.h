#pragma once
#include <Runtime/DirectMovieNative.h>

namespace KaGuYa::Start::Runtime
{
struct MovieSceneNativeServices;
class MovieSceneNativeScope final
{
public:
    explicit MovieSceneNativeScope(MovieSceneNativeServices&)noexcept;
    ~MovieSceneNativeScope()noexcept;
    MovieSceneNativeScope(const MovieSceneNativeScope&)=delete;
    MovieSceneNativeScope& operator=(const MovieSceneNativeScope&)=delete;
private:
    MovieSceneNativeServices* previous_;
    DirectMovieNativeScope movie_;
};
}
