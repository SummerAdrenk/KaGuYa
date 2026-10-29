#pragma once
#include <Platform/InputViews.h>

#include <Platform/Input.h>
#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/PatternLayerRuntime.h>

#include <memory>
#include <optional>
#include <vector>

namespace KaGuYa::Start::Runtime
{
struct ParamsGameSystem;
class ParamsRuntimeRootOwner;
class SaveDataEnvironmentArchive;

// 每次调用只借用当前 GS；不在循环之间保留对象或配置 count。
class IStartupConfigGameSystemSource
{
public:
    virtual ~IStartupConfigGameSystemSource() = default;
    [[nodiscard]] virtual const ParamsGameSystem* CaptureGameSystemAt418480() const = 0;
};

class RootStartupConfigGameSystemSource final : public IStartupConfigGameSystemSource
{
public:
    explicit RootStartupConfigGameSystemSource(ParamsRuntimeRootOwner& params) noexcept : params_(params) {}
    [[nodiscard]] const ParamsGameSystem* CaptureGameSystemAt418480() const override;
private:
    ParamsRuntimeRootOwner& params_;
};

// 所有依赖均借用生产 owner；window 是 Program+2C 的实时 HWND，不是构造快照。
struct FreeScrollDependencies final
{
    DrawManagerLayerRuntime& manager;
    const IStartupConfigGameSystemSource& gameSystem;
    const SaveDataEnvironmentArchive& save;
    Platform::MouseView mouse;
    Platform::KeyboardView keyboard;
    Platform::CursorView cursor;
    const HWND& window;
    const Platform::ICursorApi& windowApi;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsPatternFilterApi& filters;
    IGraphicsScreenSizeApi& screenSize;
    IGraphicsScreenUpdateApi& screen;
};

// 语义状态，不是原 x86 ABI。4A14D0/4ADF70 未初始化两个 repeat 字段。
struct FreeScrollState final
{
    std::uint32_t axesAt0C = 0;
    std::int32_t stepAt10 = 40;
    std::int32_t xAt14 = 0;
    std::int32_t yAt18 = 0;
    RECT viewportAt1C{};
    RECT limitsAt2C{};
    std::optional<std::int32_t> repeatWaitAt3C;
    std::optional<std::int32_t> repeatCountdownAt40;
    std::uint8_t draggingAt44 = 0;
    POINT lastPointerAt48{};
};

struct FreeScrollEntryView final
{
    const PatternLayerRuntime* pattern;
    POINT unionOrigin;
    const RuntimeGraphicsSurface* main;
    const RuntimeGraphicsSurface* processed;
};

// 隐藏窗口调用合同，只借业务入口；不表示原CFreeScroll对象布局。
class IFreeScrollOperations
{
public:
    virtual ~IFreeScrollOperations()=default;
    virtual bool BeginAt4ADF70()=0;
    virtual bool UpdateAt4BDD00()=0;
    virtual void EndAt4AC970()=0;
};
// CReadScript+60 的显式现代模型；manager/Pattern/Graphics 必须比本对象长寿。
class FreeScrollRuntime final:public IFreeScrollOperations
{
public:
    explicit FreeScrollRuntime(FreeScrollDependencies dependencies);
    ~FreeScrollRuntime() override;
    FreeScrollRuntime(const FreeScrollRuntime&) = delete;
    FreeScrollRuntime& operator=(const FreeScrollRuntime&) = delete;

    [[nodiscard]] bool BeginAt4ADF70() override;
    [[nodiscard]] bool UpdateAt4BDD00() override;
    void EndAt4AC970() override;

    [[nodiscard]] const FreeScrollState& State() const noexcept;
    [[nodiscard]] std::size_t ScrollCount() const noexcept;
    [[nodiscard]] FreeScrollEntryView ScrollAt(std::size_t index) const;

private:
    struct Entry final
    {
        explicit Entry(PatternLayerRuntime& value) noexcept : pattern(value) {}
        PatternLayerRuntime& pattern;
        POINT unionOrigin{};
        std::unique_ptr<RuntimeGraphicsSurface> main;
        // 4A4E90：processed 必须先于 main 析构。
        std::unique_ptr<RuntimeGraphicsSurface> processed;
    };

    [[nodiscard]] LayerWindowRuntime& WindowZero() const;
    [[nodiscard]] PatternLayerRuntime* PatternAt(std::size_t index) const;
    [[nodiscard]] Platform::CursorView Cursor() const;
    void ReleaseEntriesForward() noexcept;
    void UpdateMouse();
    void UpdateKeyboard();
    void UpdateWheel();
    void RestorePatternsAt4C4330();

    void CreateCanvasAt4AE574(Entry& entry);
    void RenderAt4AF200(bool changed);
    void DrawSpritesInRect(Entry& entry, const RECT& rect);
    void RefreshFilterInRect(Entry& entry, const RECT& rect);
    void RedrawMarginAt4C43D0(Entry& entry, const RECT& rect);
    void CollectAnimationDirtyAt4A7A30(Entry& entry, DirtyRectCollection& dirty);
    [[nodiscard]] static RECT LocalUnionAt4ADDE0(const PatternLayerRuntime& pattern);
    [[nodiscard]] static RECT SpriteRect(const SpriteRuntime& sprite) noexcept;
    [[nodiscard]] static const RuntimeGraphicsSurface& SpriteSurface(
        const SpriteRuntime& sprite, std::size_t index);
    [[nodiscard]] static POINT ScreenOffset(const Entry& entry, POINT delta) noexcept;
    [[nodiscard]] static std::int32_t Add(std::int32_t a, std::int32_t b) noexcept;
    [[nodiscard]] static std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept;
    [[nodiscard]] static RECT Offset(const RECT& rect, std::int32_t x, std::int32_t y) noexcept;
    static void AddDirty(DirtyRectCollection& dirty, const RECT& rect);
    static void CoalesceDirty(DirtyRectCollection& dirty);

    FreeScrollDependencies dependencies_;
    GameGuiInputStateRuntime input_;
    FreeScrollState state_;
    std::vector<std::unique_ptr<Entry>> entries_;
};
}
