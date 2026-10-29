#pragma once

#include <Runtime/LegacyTitleWorkflow.h>
#include <Runtime/CgUiButtonReaderNative.h>
#include <Runtime/ModeButtonNative.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
struct LegacyTitleMenuNavigation final
{
    std::int32_t item;
    // 顺序与动作1..4一致；只使用显式候选，不按资源坐标猜测空间排序。
    std::array<std::span<const std::int32_t>, 4> candidatesByDirection;
};

// Baku 569690/569780：七槽记录和邻接数据，不包含原 CTitleBtn 的对象布局。
struct LegacyTitleMenuData final
{
    std::array<CgUiButtonRecordNative28h, 6> buttons;
    std::array<std::int32_t, 24> neighbors;
    std::array<std::int32_t, 4> directionActions;
    // 49D9BD可达的通用fallback；Baku原构造没有填充，默认必须为空。
    std::span<const LegacyTitleMenuNavigation> navigation{};
    bool useNeighborTable = true;
};

struct LegacyTitleMenuNames final
{
    const char* background;
    const char* overlay;
    const char* clickCue;
    const char* selectCue;
};

enum class LegacyTitleMenuCue { Click, Select };

// 真实根服务的强制装配边界。所有 output 都是菜单自己的唯一拥有槽；服务不能偷留借用。
// overlay 是动画对象的 YTL Ref，不能用静态 Surface 或空实现代替。
class ILegacyTitleMenuHost
{
public:
    virtual ~ILegacyTitleMenuHost() = default;
    virtual const char* BackgroundName() = 0;
    virtual const char* OverlayName() = 0;
    // 49CB58/49CBB8：失败可留下已取得的引用；由菜单保留至正常释放。
    virtual bool LoadBackground(YtlReferenceSlotRuntime& output, const char* nameCp932) = 0;
    virtual bool LoadOverlay(YtlReferenceSlotRuntime& output, const char* nameCp932, int mode) = 0;
    // 49DD81：原返回0才失效；不是“true表示需要重画”。
    virtual std::int32_t UpdateOverlay(const YtlReferenceSlotRuntime&) = 0;
    virtual RECT OverlayBounds(const YtlReferenceSlotRuntime&) = 0;
    virtual void BlitOverlay(const YtlReferenceSlotRuntime&, GraphicsSurfaceNativeBody&, const RECT*) = 0;
    virtual void DrawOverlay(const YtlReferenceSlotRuntime&, const RECT*) = 0;
    // 49DD12/49DDD7/49DE2A：先读音量，再读当前cue名，再播放；普通返回被忽略。
    virtual std::int32_t ReadCueVolume() = 0;
    virtual const char* CueName(LegacyTitleMenuCue) = 0;
    virtual std::uint8_t PlayCue(const char* nameCp932, int channelOffset, int volume, int enabled) = 0;
};

struct LegacyTitleMenuServices final
{
    ILegacyTitleMenuHost& host;
    CgUiButtonNativeServices& buttons;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsScreenCalls& screen;
    IEnvironmentRootMemory& memory;
    IArchiveResourceSource& archives;
    const FileTransactionContext* transaction;
    IStartFileApi& files;
    const IEnvironmentKeyTableSource& key;
    ICgUiButtonResourceErrorDisplay& errors;
    ArchiveCodecServices codecs;
};

class LegacyTitleMenu final : public ILegacyTitleMenu
{
public:
    // data/services及其依赖必须覆盖菜单寿命；base构造失败外传，资源阶段失败返回 !Ready()。
    LegacyTitleMenu(const LegacyTitleMenuData&, LegacyTitleMenuServices&);
    ~LegacyTitleMenu() override;
    LegacyTitleMenu(const LegacyTitleMenu&) = delete;
    LegacyTitleMenu& operator=(const LegacyTitleMenu&) = delete;

    [[nodiscard]] bool Ready() const noexcept { return ready_; }
    std::int32_t PollSelection() override;
    void FlushDirty() override;
    // 49DE80：构造frame/显式重合成用Blit通道，不commit，也不清dirty。
    void Compose(GraphicsSurfaceNativeBody& destination, const RECT* clip = nullptr);
    // 普通释放可抛；owner应在进入ILegacyTitleMenu的noexcept析构边界前调用。
    // 正常顺序49DF60/49CE90：overlay、background、children、navigation、dirty、children存储。
    void ReleaseResources();

    [[nodiscard]] const CgUiButtonNativeBody6Ch* Button(std::int32_t index) const noexcept;
    [[nodiscard]] std::int32_t ChildCount() const noexcept;

private:
    std::int32_t ReadInput();
    CgUiButtonNativeBody6Ch* AppendButton();
    void AppendNavigation(const LegacyTitleMenuNavigation&);
    void ReleaseBase();
    void Cue(LegacyTitleMenuCue);
    void RequireReady() const;
    void DrawButton(CgUiButtonNativeBody6Ch&, const RECT*);

    const LegacyTitleMenuData& data_;
    LegacyTitleMenuServices& services_;
    // 只复用共享ModeButton状态/导航存储；不调用其现代input/process/release虚槽。
    ModeButtonNativeBody50h state_;
    YtlReferenceSlotRuntime background_;
    YtlReferenceSlotRuntime overlay_;
    bool ready_ = false;
    bool released_ = false;
};
}
