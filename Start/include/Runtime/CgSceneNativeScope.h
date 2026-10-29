#pragma once
#include <Runtime/CgButtonNative.h>
#include <Runtime/CgDisplayWindowNative.h>
#include <Runtime/CgOperationDialogNative.h>
#include <Runtime/SecretCgNativeArchive.h>

namespace KaGuYa::Start::Runtime
{
struct CgSceneNativeServices;
// 与SceneManager对象声明分离，任务调度器可绑定服务而不产生Scene/Factory头循环。
class CgSceneNativeScope final
{
public:
    explicit CgSceneNativeScope(CgSceneNativeServices&) noexcept;
    ~CgSceneNativeScope() noexcept;
    CgSceneNativeScope(const CgSceneNativeScope&)=delete;
    CgSceneNativeScope& operator=(const CgSceneNativeScope&)=delete;
private:
    CgSceneNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    SecretCgParamsBinding params_;
    CgButtonNativeScope buttons_;
    CgDisplayWindowNativeScope display_;
    CgOperationDialogNativeScope operation_;
};
}
