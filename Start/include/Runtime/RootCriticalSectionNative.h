#pragma once

#include <Runtime/WinHookNative.h>

namespace KaGuYa::Start::Runtime
{
// vptr由原静态数据预设；4010C0不补写表，Initialize失败也不提前写thread/depth。
void InitializeRootCriticalSectionNativeAt4010C0(CriticalSectionNativeBody24h&, WinHookNativeServices&);
// 5AA1D0只退役原全局锁，不删24h外块、不释放Root或progress。
void RetireRootCriticalSectionNativeAt5AA1D0(CriticalSectionNativeBody24h&, WinHookNativeServices&);

// 正文外借用。没有析构业务，不建第二把锁；每次读当前原生表，不缓存虚槽。
class RootCriticalSectionNativeReference final
{
public:
    RootCriticalSectionNativeReference(CriticalSectionNativeBody24h&, WinHookNativeServices&) noexcept;
    void Enter();
    void Leave() noexcept;
    [[nodiscard]] bool HeldByCurrentThreadAt4076D0() const;
    [[nodiscard]] CriticalSectionNativeBody24h& Body() const noexcept { return body_; }
private:
    CriticalSectionNativeBody24h& body_;
    WinHookNativeServices& services_;
};

// 仅在原5AD4FC初始化完成后借用；此getter不初始化、不取Root/Env。
RootCriticalSectionNativeReference& ProcessRootCriticalSectionNativeReference() noexcept;
// 原静态数据正文，不开始/结束锁生命期；仅供原CRT分期及只读验证按原点读取。
CriticalSectionNativeBody24h& ProcessRootCriticalSectionNativeBody() noexcept;
}
