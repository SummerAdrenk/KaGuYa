#pragma once
#include <Runtime/AppModuleNative.h>
#include <Runtime/AtlBaseModuleNative.h>

namespace KaGuYa::Start::Runtime
{
// 只组合已有叶服务，不进入3Ch/0Ch正文；allocation仅调用Recalloc。
struct AppModuleMessageLoopsNativeServices final
{
    AppModuleNativeServices module;
    IWindowClassNativeApi& allocation;
    IAtlBaseModuleNativeDiagnostics& diagnostics;
};
AppModuleMessageLoopsNativeServices& DefaultAppModuleMessageLoopsNativeServices() noexcept;

// 包含4342F9..434373的捕锁/解锁。固定全局619828的+34即61985C，
// 不能改读publication的+34；loop借用caller的CIdleLoop，不代管其寿命。
// 原inline没有成功返回值，第二次Recalloc失败保留第一次发布。
void RegisterAppModuleMessageLoopNativeAt434310(AppModuleNativeBody3Ch& fixedGlobalAt619828,
    AppModuleNativeBody3Ch*& publicationAt6197DC, void* loop,
    AppModuleMessageLoopsNativeServices& = DefaultAppModuleMessageLoopsNativeServices());
void* FindAppModuleMessageLoopNativeAt417F50(AppModuleNativeBody3Ch& fixedReceiver,
    AppModuleNativeBody3Ch*& publicationAt6197DC, DWORD threadId,
    AppModuleMessageLoopsNativeServices& = DefaultAppModuleMessageLoopsNativeServices());
BOOL RemoveAppModuleMessageLoopNativeAt432800(AppModuleNativeBody3Ch& fixedReceiver,
    AppModuleNativeBody3Ch*& publicationAt6197DC,
    AppModuleMessageLoopsNativeServices& = DefaultAppModuleMessageLoopsNativeServices());
// 不补空+34兜底或事务回滚。移除仅在Enter成功后具有已证明的锁清理资格；
// 原Enter内部异常时未初始化的栈标志不在等价声明内，详见专项审计。
}
