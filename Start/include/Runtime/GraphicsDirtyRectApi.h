#pragma once

namespace KaGuYa::Start::Runtime
{
// 4AA780：仅按 Graphics 模块引用门控，不要求图形初始化完成。
class IGraphicsDirtyRectApi
{
public:
    virtual ~IGraphicsDirtyRectApi() = default;
    virtual void ClearDirtyRectIfLoaded() = 0;
};
}
