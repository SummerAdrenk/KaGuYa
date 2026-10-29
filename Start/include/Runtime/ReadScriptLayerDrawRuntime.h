#pragma once
#include <Platform/InputViews.h>

#include <Runtime/LayerWindowRuntime.h>
#include <Runtime/ReadScriptSceneRuntime.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsScreenUpdateApi;

// 4B5A20的四个413BE0点分别取当前manager；drawEnabled门控仍由场景负责。
class ReadScriptLayerDrawRuntime final : public IReadScriptDrawBoundary
{
public:
    ReadScriptLayerDrawRuntime(IDrawManagerLayerSource& source,
        Platform::KeyboardView keyboard, const UnSaveDataRuntime& unsaved,
        const SaveDataEnvironmentArchive& saved, IGraphicsScreenUpdateApi& screen) noexcept;
    void PrepareRootOwners() override;
    void SubmitCurrentDirtyRect() override;
    void SubmitDrawRecords() override;
    void CompleteRootOwnerDraw() override;
    [[nodiscard]] bool SuppressFrameAdvanceAt4C3350() const;
private:
    [[nodiscard]] DrawWindowView WindowZero() const;
    IDrawManagerLayerSource& source_;
    const UnSaveDataRuntime& unsaved_;
    const SaveDataEnvironmentArchive& saved_;
    LayerWindowDrawStateRuntime input_;
    IGraphicsScreenUpdateApi& screen_;
};
}
