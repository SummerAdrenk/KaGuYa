#pragma once
namespace KaGuYa::Start::Runtime
{
struct ParamsDemoCmdSound;
class IDemoSoundCommandServices
{
public:
    virtual ~IDemoSoundCommandServices() = default;
    virtual void ExecuteDemoSoundAt4BF692(const ParamsDemoCmdSound&) = 0;
};
}
