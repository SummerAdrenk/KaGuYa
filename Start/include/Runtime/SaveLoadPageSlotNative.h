#pragma once
#include <Runtime/SaveLoadPageSecondNative.h>
#include <Runtime/SaveLoadDialogNative.h>
#include <Runtime/SaveLoadLabelNative.h>
#include <Runtime/SaveLoadTimestampNative.h>
#include <Runtime/SaveFileDeleteFactoryNative.h>
#include <Runtime/GraphicsTextNativeApi.h>

namespace KaGuYa::Start::Runtime
{
class RootSaveLoadNativeSource;
class NativeGraphicsRuntimeLibrary;

// 只允许精确原子叶；52A470/528D90整段算法不进入来源接口。
class ISaveLoadPageSlotNativeSource : public ISaveLoadLabelNativeSource,
    public ISaveFileDeleteFactoryNativeSource, public ISaveLoadTimestampNativeSource
{
public:
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override = 0;
    virtual IRuntimeStringManagerSource& Strings() override = 0;
    virtual GraphicsSurfaceNativeBody* ConstructPreviewSurfaceAt40C2E0(void*) = 0;
    virtual std::uint32_t LoadPreviewAt511D90(GraphicsSurfaceNativeBody*, const char*, const void*) = 0;
    virtual bool CreateTextFontAt4388B0(const GraphicsTextNativeFontParameters*) = 0;
    virtual bool InitializeTextAt4389D0(const char*, std::int32_t, std::int32_t) = 0;
    virtual bool BlitAllTextAt53A980(GraphicsSurfaceHandle, RECT*) = 0;
};

struct SaveLoadPageSlotNativeServices final
{
    ISaveLoadPageSlotNativeSource& source;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
};

// 同一Root/Environment/Graphics的非拥有peer，不复制catalog、代际或页面状态。
class RootSaveLoadPageSlotNativeSource final : public ISaveLoadPageSlotNativeSource
{
public:
    RootSaveLoadPageSlotNativeSource(RootSaveLoadNativeSource& source,
        NativeGraphicsRuntimeLibrary& graphics) noexcept : source_(source), graphics_(graphics) {}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    IRuntimeStringManagerSource& Strings() override;
    BOOL DeleteFileA(const char*) override;
    void ConstructInputAt40B530(RuntimeCString*, const char*) override;
    int LocalTime64At5697BB(std::tm*, const std::int64_t*) override;
    GraphicsSurfaceNativeBody* ConstructPreviewSurfaceAt40C2E0(void*) override;
    std::uint32_t LoadPreviewAt511D90(GraphicsSurfaceNativeBody*, const char*, const void*) override;
    bool CreateTextFontAt4388B0(const GraphicsTextNativeFontParameters*) override;
    bool InitializeTextAt4389D0(const char*, std::int32_t, std::int32_t) override;
    bool BlitAllTextAt53A980(GraphicsSurfaceHandle, RECT*) override;
private:
    RootSaveLoadNativeSource& source_;
    NativeGraphicsRuntimeLibrary& graphics_;
};

// 原ECX=AC，ret24h：page(int32),slot(uint32),page Ref12,factory Ref12,isNew(int32)。
// 参数已增持一次，本入口消费；page数值原核不读。EAX无业务返回合同。
// preview向量不做补齐/限8；原无界槽/空必需正文为非法调用边界，不转NoData。
void RenderSaveLoadPageSlotNativeAt52A470(SaveLoadPageSecondNativeBodyACh&,
    std::int32_t page, std::uint32_t slot, YtlReferenceSlotRuntime ownedPage,
    YtlReferenceSlotRuntime ownedFactory, std::int32_t isNew, SaveLoadPageSlotNativeServices&);

// 原ECX=parent184但不读；ret14h。surface是消费参数，label是已构造借用CString。
// 空label仅消费surface；非空字体/文字叶错误由原catch(...)吞掉。
void DrawSaveLoadPageSlotLabelNativeAt528D90(SaveLoadDialogNativeBody184h&,
    YtlReferenceSlotRuntime ownedSurface, const RuntimeCString& label,
    std::int32_t saveMode, SaveLoadPageSlotNativeServices&);
void RefreshSaveLoadPageHoverNative(SaveLoadPageSecondNativeBodyACh&,
    SaveLoadPageSlotNativeServices&, bool force = false);
}
