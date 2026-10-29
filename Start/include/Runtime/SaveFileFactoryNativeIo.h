#pragma once

#include <Runtime/SaveFileDeleteFactoryNative.h>

namespace KaGuYa::Start::Runtime
{
// Source 仅提供独立必填叶；入口顺序、CString 生命周期、mode 分支和 catch 由本模块执行。
// 尚未提供真实 Root 装配，不以既有现代 factory 或整段保存/加载回调代替原生入口。
class ISaveFileFactoryNativeIoSource : public ISaveFileDeleteFactoryNativeSource
{
public:
    virtual void EnsureRootAt413BE0() = 0;
    virtual void* RootAt6198C4() = 0;
    virtual void* ParamsAt47BDB0(void* root) = 0;
    // 保留原 EAX 原值；入口只判 AL，不能把任意非零 DWORD 当作成功。
    virtual std::uint32_t WriteContainerAt4C7B10(void* params, const char*) = 0;
    virtual std::uint32_t ReadContainerAt4C7420(void* params, const char*, std::uint32_t mode) = 0;
    // 488E48: ECX=this，无栈参数；不接收反编译伪代码中的 v18。
    virtual void CapturePreviewAt48DBE0(void* root) = 0;
    virtual std::uint32_t SavePreviewAt489420(void* root, const char*) = 0;
};

// body 必须已由现有 0Ch/10h 构造核成功构造；本入口不取得 prefix 的销毁资格。
// mode 保留完整有符号 int：0 另删 BMP，2 捕获后保存，其余非零只保存预览。
std::int32_t SaveFileFactoryNativeAt488CF0(SaveFileDeleteFactoryNativeBody0Ch& body,
    std::int32_t slot, std::int32_t mode, ISaveFileFactoryNativeIoSource& source);
std::int32_t LoadFileFactoryNativeAt480830(SaveFileDeleteFactoryNativeBody0Ch& body,
    std::int32_t slot, ISaveFileFactoryNativeIoSource& source);
}
