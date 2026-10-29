#pragma once
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct SceneManagerNativeBody48h;
// Root/manager均是已选定的原生业务身份；三个叶分开，不能提前捕获或借旧Host快照。
class IReadScriptCommandNativeSource
{
public:
    virtual ~IReadScriptCommandNativeSource()=default;
    virtual void EnsureCommandRootAt413BE0(std::uint32_t caller)=0;
    virtual void* CommandRootAt6198C4(std::uint32_t caller)=0;
    virtual SceneManagerNativeBody48h* CommandManagerAt4186D0(void* capturedRoot,std::uint32_t caller)=0;
};
enum class ReadScriptCommandRelease{CachedHeader,ReturnedCString};
void* CaptureReadScriptCommandRoot(IReadScriptCommandNativeSource&,std::uint32_t ensure,std::uint32_t read);
// 414540/4ADAC0取得fresh临时串。原命令串没有caller EH资格，不加析构守卫或重试释放。
bool CompareReadScriptCommandNative(IReadScriptCommandNativeSource&,void* capturedRoot,
    std::uint32_t managerCall,const char* expected,ReadScriptCommandRelease);
void SetReadScriptCommandNative(IReadScriptCommandNativeSource&,void* capturedRoot,
    std::uint32_t managerCall,const char* text,int length);
}
