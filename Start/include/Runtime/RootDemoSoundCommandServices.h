#pragma once
#include <Runtime/DemoSoundCommandServices.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class EnvironmentRootOwner;
class IArchiveResourceSource;
// 4BF692的命令叶；只借现有Root/Env/VFS，不创建50h或持有名字/参数镜像。
class RootDemoSoundCommandServices final:public IDemoSoundCommandServices
{
public:
    RootDemoSoundCommandServices(RootRuntimeOwner& root,EnvironmentRootOwner& environment,
        IArchiveResourceSource& archives)noexcept:root_(root),environment_(environment),archives_(archives){}
    void ExecuteDemoSoundAt4BF692(const ParamsDemoCmdSound&)override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    IArchiveResourceSource& archives_;
};
}
