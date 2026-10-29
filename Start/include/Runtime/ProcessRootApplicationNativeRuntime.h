#pragma once

namespace KaGuYa::Start::Runtime
{
struct CountDownWindowResourceProfile;
class RootRuntimeOwner;
class RootApplicationNativeServicesRuntime;
class RootMainFrameNativeServicesRuntime;

// 正式入口在CRT全部完成后显式调用一次，不是getter中的懒初始化或失败兜底。
// countdown仅借用，其profile必须保留到进程结束；不在装配期间复制72个资源串。
// 所有元数据使用显式存储，5AB3C0只释放Root正文，不自动析构长图或补删晚建根。
RootApplicationNativeServicesRuntime& ConstructProcessRootApplicationNativeRuntimeAtEntry(
    const CountDownWindowResourceProfile& countdown);
// 正式入口直接选择已确认的Sample18静态表，不构造72个拥有串。
RootApplicationNativeServicesRuntime& ConstructProcessRootApplicationNativeRuntimeAtEntry();
RootRuntimeOwner& ProcessRootRuntimeOwner() noexcept;
RootApplicationNativeServicesRuntime& ProcessRootApplicationNativeServices() noexcept;
RootMainFrameNativeServicesRuntime& ProcessRootMainFrameNativeServices() noexcept;
}
