#include <Runtime/EnvironmentRootVersionSource.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

namespace KaGuYa::Start::Runtime
{
const ResourceVersionRuntime& EnvironmentRootVersionSource::CaptureResourceVersionAt4180C0() const
{
    // 4964A6/496503/49655E每次进入4180C0，解锁后才从本次根的+54解析版本对象。
    // 原空Ref是非法调用，不改为备用对象/空串成功；无版本资源由对象自身分支处理。
    return *ResolveEnvironmentObjectReference<ResourceVersionRuntime>(root_.CaptureAt4180C0()->versionAt54);
}
}
