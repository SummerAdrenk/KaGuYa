#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <Runtime/PersistenceFileAccess.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsSurfaceApi;

// 511D90：直接装入唯一24h正文；flags是BMP上传参数，AP分支不使用。
// memory仅分配14h reader/18h control，存储必须兼容全局sized delete。
// 不吞读取/命名/上传异常，不将OOM或非法Ref伪装为预览读取失败。
[[nodiscard]] std::uint32_t LoadSavePreviewFileNativeAt511D90(
    GraphicsSurfaceNativeBody& destination, const char* path, std::uint32_t flags,
    IStartFileApi& files, IGraphicsSurfaceApi& graphics,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
