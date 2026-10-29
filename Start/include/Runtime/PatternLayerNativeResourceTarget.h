#pragma once

#include <Runtime/PatternLayerNative.h>
#include <Runtime/PatternResourceExecutor.h>
#include <Runtime/GraphicsRootNative.h>

namespace KaGuYa::Start::Runtime
{
// 只借用80h及调用服务；不持有Sprite Ref，不在原正文旁维护名称/画布镜像。
struct PatternLayerNativeResourceServices final
{
    const PatternResourceLoadServices& loads;
    IGraphicsSurfaceApi& surfaces;
    GraphicsRootOwner& graphics;
    IGraphicsModuleNativeWin32Api& win32;
};

// 443650：空rect借当前主画布尺寸，非空rect原样交当前dirty对象的+08槽。
void InvalidatePatternLayerNativeAt443650(PatternLayerNativeBody80h& body, const RECT* rect);

class PatternLayerNativeResourceTarget final : public IPatternResourceExecutionTarget
{
public:
    PatternLayerNativeResourceTarget(PatternLayerNativeBody80h& body,
        PatternLayerNativeResourceServices& services) noexcept;

    std::size_t SpriteSlotCount() const noexcept override;
    std::int32_t LayerX() const noexcept override;
    std::int32_t LayerY() const noexcept override;
    const char* LogicalNameCharacters(std::size_t slot) const override;
    std::int32_t LogicalNameLength(std::size_t slot) const override;
    PatternSpriteRectResult LocalDrawableRect(std::size_t slot) const override;
    void ClearSlot(std::size_t slot) override;
    PatternSpriteReceiver CaptureSpriteReceiver(std::uint32_t slot) const override;
    void LoadResource(std::uint32_t slot, PatternSpriteReceiver receiver,
        const PatternResourceLoadRequest& request, const IPatternResourceFailureName& failureName,
        IPatternResourceLoadCommit& commit, IRuntimeStringManagerSource& strings) override;
    const char* ReceiverPrimaryNameCharacters(PatternSpriteReceiver receiver) const override;
    PatternResourceNamesBorrow ReceiverAuxiliaryNames(PatternSpriteReceiver receiver) const override;
    void PrepareReceiverFileGroup(PatternSpriteReceiver receiver, FileGroupChange changes) override;
    void SetReceiverLogicalName(PatternSpriteReceiver receiver, const RuntimeCString& source) override;
    void SetReceiverOrigin(PatternSpriteReceiver receiver, std::int32_t x, std::int32_t y) override;
    void SetReceiverActive(PatternSpriteReceiver receiver, bool active) override;
    PatternSpriteRectResult ReceiverDrawableRect(PatternSpriteReceiver receiver) const override;
    void Invalidate(const RECT& rect) override;

private:
    PatternLayerNativeBody80h& body_;
    PatternLayerNativeResourceServices& services_;
};
}
