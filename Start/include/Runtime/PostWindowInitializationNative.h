#pragma once
#include <Runtime/PostWindowLifecycle.h>

namespace KaGuYa::Start::Runtime
{
// 原489D70..489E56的分界：局部Ref构造成功后才能执行59AB87。
// 不把进度正文/control申请失败的部分发布交给现代回滚。
class IPostWindowInitializationNativeFiles
{
public:
    virtual ~IPostWindowInitializationNativeFiles() = default;
    virtual void CreateInitializationProgressAt489D70() = 0;
    virtual void CreateFileManagerAt489E56() = 0;
    virtual void ReleaseInitializationProgressAt460730() noexcept = 0;
};
bool RunPostWindowInitializationNativeAt489C90(IPostWindowRuntimeServices&, IPostWindowInitializationNativeFiles&);
}
