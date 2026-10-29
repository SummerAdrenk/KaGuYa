#pragma once

namespace KaGuYa::Start::Runtime
{
struct MusicButtonNativeServices;
class IMusicSceneNativeButtonConstruction
{
public:
    virtual ~IMusicSceneNativeButtonConstruction()=default;
    // 接收唯一70h存储，callee负责成员展开；不得返回现代shared_ptr正文。
    virtual void* ConstructAt4A1C70(void*)=0;
    virtual MusicButtonNativeServices* NativeButtonServices() const noexcept{return nullptr;}
};
}
