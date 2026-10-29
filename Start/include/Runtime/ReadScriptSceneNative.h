#pragma once
#include <Runtime/ReadScriptCommandNative.h>
#include <Runtime/ReadScriptReturnTitleNative.h>
#include <Runtime/ReadScriptInteractionNativeHost.h>

#include <Runtime/FreeScrollNative.h>
#include <Runtime/ReadScriptEffectOwnerNative.h>
#include <Runtime/RuntimeCString.h>
#include <Runtime/ScrVmYieldDispatcher.h>
#include <cstddef>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct ReadScriptSceneNativeBodyBCh;
// 18号源PE 5B5344，只有四槽；5B5354起是名字常量，不是Finish槽。
struct ReadScriptSceneNativeVtable final
{
    std::uint8_t (__thiscall* runAt00)(ReadScriptSceneNativeBodyBCh*, std::int32_t);
    ReadScriptSceneNativeBodyBCh* (__thiscall* deleteAt04)(ReadScriptSceneNativeBodyBCh*, std::uint32_t);
    void (__thiscall* updateAt08)(ReadScriptSceneNativeBodyBCh*);
    void (__thiscall* drawAt0C)(ReadScriptSceneNativeBodyBCh*);
};
struct ReadScriptSceneNativeBodyBCh final
{
    const ReadScriptSceneNativeVtable* vptrAt00;
    std::int32_t mainStateAt04;
    std::uint8_t drawEnabledAt08;
    std::uint8_t unwrittenAt09[3];
    std::int32_t substateAt0C;
    std::int32_t interactionValueAt10;
    std::int32_t interactionValueAt14;
    std::uint32_t unwrittenAt18;
    union { ReadScriptEffectSlotsNative30h effectsAt1C; };
    std::int32_t savedMainStateAt4C;
    std::int32_t savedSubstateAt50;
    std::int32_t neutralDwordAt54;
    std::int32_t waitValueAt58;
    std::int32_t waitValueAt5C;
    union { FreeScrollNativeBody50h scrollAt60; };
    union { RuntimeCString errorMessageAtB0; };
    std::int32_t suspendValueAtB4;
    std::uint8_t interactionInitializedAtB8;
    std::uint8_t unwrittenAtB9[3];
    // 工厂placement-new不提前构造Ref/FreeScroll/CString或清未写标量。
    ReadScriptSceneNativeBodyBCh() noexcept {}
    ~ReadScriptSceneNativeBodyBCh() noexcept {}
    ReadScriptSceneNativeBodyBCh(const ReadScriptSceneNativeBodyBCh&) = delete;
    ReadScriptSceneNativeBodyBCh& operator=(const ReadScriptSceneNativeBodyBCh&) = delete;
};
static_assert(sizeof(void*) == 4);
static_assert(sizeof(ReadScriptSceneNativeVtable) == 0x10);
static_assert(std::is_standard_layout_v<ReadScriptSceneNativeBodyBCh>);
static_assert(sizeof(ReadScriptSceneNativeBodyBCh) == 0xBC);
static_assert(alignof(ReadScriptSceneNativeBodyBCh) == 4);
#define KAGUYA_READSCRIPT_OFFSET(member, offset) \
    static_assert(offsetof(ReadScriptSceneNativeBodyBCh, member) == offset)
KAGUYA_READSCRIPT_OFFSET(vptrAt00, 0x00);
KAGUYA_READSCRIPT_OFFSET(mainStateAt04, 0x04);
KAGUYA_READSCRIPT_OFFSET(drawEnabledAt08, 0x08);
KAGUYA_READSCRIPT_OFFSET(unwrittenAt09, 0x09);
KAGUYA_READSCRIPT_OFFSET(substateAt0C, 0x0C);
KAGUYA_READSCRIPT_OFFSET(interactionValueAt10, 0x10);
KAGUYA_READSCRIPT_OFFSET(interactionValueAt14, 0x14);
KAGUYA_READSCRIPT_OFFSET(unwrittenAt18, 0x18);
KAGUYA_READSCRIPT_OFFSET(effectsAt1C, 0x1C);
KAGUYA_READSCRIPT_OFFSET(savedMainStateAt4C, 0x4C);
KAGUYA_READSCRIPT_OFFSET(savedSubstateAt50, 0x50);
KAGUYA_READSCRIPT_OFFSET(neutralDwordAt54, 0x54);
KAGUYA_READSCRIPT_OFFSET(waitValueAt58, 0x58);
KAGUYA_READSCRIPT_OFFSET(waitValueAt5C, 0x5C);
KAGUYA_READSCRIPT_OFFSET(scrollAt60, 0x60);
KAGUYA_READSCRIPT_OFFSET(errorMessageAtB0, 0xB0);
KAGUYA_READSCRIPT_OFFSET(suspendValueAtB4, 0xB4);
KAGUYA_READSCRIPT_OFFSET(interactionInitializedAtB8, 0xB8);
KAGUYA_READSCRIPT_OFFSET(unwrittenAtB9, 0xB9);
#undef KAGUYA_READSCRIPT_OFFSET

class IReadScriptSceneRestoreSource;
class IReadScriptWaitStateSource;
class IReadScriptSoundWaitSource;
class IReadScriptInteractionStateSource;
class IReadScriptMenuInteractionHost;
class IReadScriptToolServices;
class IReadScriptTextServices;
class IReadScriptMenuServices;
class IReadScriptEffectMaskNativeServices;
class IMoveEffectNativeConstructionSource;
class IReadScriptFlashShakeNativeSource;
class IDrawLayersViewSource;
class IOpacityWindowInputSource;
class IStartupConfigGameSystemSource;
class ISpriteAnimationInitializationSource;
struct ReadScriptEffectFactoryNativeServices;
struct FreeScrollNativeDrawServices;
struct FreeScrollNativeInputServices;

struct ReadScriptSceneNativeScrollServices final
{
    IDrawManagerNativeRootSource& draw;
    const IStartupConfigGameSystemSource& params;
    FreeScrollNativeDrawServices& graphics;
    PatternLayerNativeRenderServices& patterns;
    FreeScrollNativeInputServices& input;
    const ISpriteAnimationInitializationSource& registry;
    IGraphicsScreenCalls& screen;
    IEnvironmentRootMemory& memory;
    IOpacityWindowInputSource& opacity;
};
struct ReadScriptSceneNativeErrorTarget final { HWND window; const char* caption; };

// 全部为必填业务叶。服务访问器仅借稳定服务，不提前Ensure或捕获业务状态。
// 原取根由被调用叶在对应调用点完成；禁止返回现代SceneState或整段Run回调。
// 生产实现由RootReadScriptSceneNativeServicesRuntime持有并连接同根来源。
class IReadScriptSceneNativeSource:public IReadScriptCommandNativeSource,public IReadScriptReturnTitleNativeServicesSource
{
public:
    virtual ~IReadScriptSceneNativeSource() = default;
    // 只复用返回值分派；原4BFFD0不调用此接口里的PrepareCurrentScript。
    virtual IScrVmYieldBoundary& VmYield() = 0;
    // 一次413BE0后47CD00选定receiver，再走既有完整4D3FF0；不得重取Script或假route。
    // nativeParse/UserCode普通异常先完成叶自己的原EH资格展开，再原类型外传；
    // 未取得资格的command/reader/name不由Scene补扫，叶的noexcept错误也不能转成route。
    virtual std::int32_t ExecuteCurrentScriptAt4D3FF0() = 0;
    virtual ReadScriptEffectFactoryNativeServices& EffectFactory() = 0;
    virtual IReadScriptEffectMaskNativeServices& EffectMasks() = 0;
    virtual IMoveEffectNativeConstructionSource& MoveConstruction() = 0;
    virtual IReadScriptFlashShakeNativeSource& FlashShakeSource() = 0;
    virtual IReadScriptWaitStateSource& WaitState() = 0;
    virtual IReadScriptSoundWaitSource& SoundWait() = 0;
    virtual IReadScriptInteractionStateSource& InteractionState() = 0;
    virtual IReadScriptInteractionNativeHost& InteractionHost() = 0;
    // 4B163F已捕获Root；随后Env/498020回调不得让47BDB0改用另一根。
    virtual bool ReadCommandReplayAt4C73A0(void* capturedRoot)=0;
    virtual IReadScriptToolServices& Tools() = 0;
    virtual IReadScriptTextServices& Text() = 0;
    virtual IReadScriptMenuServices& Menu() = 0;
    virtual IReadScriptMenuServices& CommandMenu();
    virtual ReadScriptSceneNativeScrollServices ScrollServices() = 0;
    virtual IDrawLayersViewSource& DrawLayers() = 0;
    virtual IGraphicsScreenCalls& Screen() = 0;
    virtual PatternLayerNativeRenderServices& Patterns() = 0;
    virtual bool SuppressFrameAdvanceAt4C3350() = 0;
    virtual ReadScriptSceneNativeErrorTarget CaptureErrorTargetAt4180C0() = 0;
    virtual int MessageBoxErrorA(HWND, const char* text, const char* caption, unsigned type) = 0;
    virtual void CleanupCurrentEnvironmentAt4AC6E0() = 0;
};
struct ReadScriptSceneNativeServices final
{
    IGraphicsScreenSizeApi& sizes;
    IRuntimeStringManagerSource& strings;
    IReadScriptSceneRestoreSource& restore;
    IReadScriptSceneNativeSource& source;
    // 完整生产必须提供。仅无资源夹具允许null；不产生备用资源/现代owner。
    // 用同根EffectFactory服务覆盖Run/普通销毁/ctor EH里的所有原生资源虚调用。
    ReadScriptEffectFactoryNativeServices* effectCalls = nullptr;
};
class ReadScriptSceneNativeScope final
{
public:
    explicit ReadScriptSceneNativeScope(ReadScriptSceneNativeServices&) noexcept;
    ~ReadScriptSceneNativeScope() noexcept;
    ReadScriptSceneNativeScope(const ReadScriptSceneNativeScope&) = delete;
    ReadScriptSceneNativeScope& operator=(const ReadScriptSceneNativeScope&) = delete;
private:
    ReadScriptSceneNativeServices* previous_;
};
const ReadScriptSceneNativeVtable& ReadScriptSceneNativeMethods() noexcept;
// storage由raw工厂分配，placement-new后交本入口；本入口不申请第二个task。
ReadScriptSceneNativeBodyBCh* ConstructReadScriptSceneNativeAt4A14D0(
    ReadScriptSceneNativeBodyBCh&, ReadScriptSceneNativeServices&);
void RestoreReadScriptSceneNativeBodyAt4A1646(ReadScriptSceneNativeBodyBCh&, IReadScriptSceneRestoreSource&);
void DestroyReadScriptSceneNativeAt4A4920(ReadScriptSceneNativeBodyBCh&, ReadScriptSceneNativeServices&) noexcept;
ReadScriptSceneNativeBodyBCh* DeleteReadScriptSceneNativeAt4A6F60(
    ReadScriptSceneNativeBodyBCh*, std::uint32_t, ReadScriptSceneNativeServices&) noexcept;
std::uint8_t RunReadScriptSceneNativeAt4C3630(ReadScriptSceneNativeBodyBCh&, std::int32_t, ReadScriptSceneNativeServices&);
void UpdateReadScriptSceneNativeAt4BA530(ReadScriptSceneNativeBodyBCh&, ReadScriptSceneNativeServices&);
void DrawReadScriptSceneNativeAt4B5A20(ReadScriptSceneNativeBodyBCh&, ReadScriptSceneNativeServices&);
}
