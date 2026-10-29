#pragma once

#include <Runtime/DrawTransitionView.h>
#include <Runtime/ReadScriptEffectRuntime.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
class DrawManagerRuntime;
class IDrawManagerNativeRootSource;
class IGraphicsScreenUpdateApi;
class RuntimeGraphicsSurface;
class SaveDataEnvironmentArchive;

// 原四类Tick/Finish均在Render处413BE0后读取root+18，再解同一manager的+20。
// 不是构造时固定receiver：4BB76F/4BB8E5/4BBEBF/4BBF54，
// 以及4A3FFE/4A41FE/4A45CE/4A468D。一次Render只捕获一次，不跨帧缓存。
// Source负责该点的根捕获，只借用完整manager的view，不提前解析/保活Transit。
class IDrawTransitionViewSource
{
public:
    virtual ~IDrawTransitionViewSource() = default;
    [[nodiscard]] virtual DrawTransitionView CaptureDrawTransitionAt413BE0() = 0;
};

// 借用唯一原生Draw根来源；每次Render才取2Ch正文，构造时不捕获/增持。
class NativeDrawTransitionViewSource final : public IDrawTransitionViewSource
{
public:
    NativeDrawTransitionViewSource(IDrawManagerNativeRootSource& root, IGraphicsTransitApi& graphics) noexcept
        : root_(root), graphics_(graphics) {}
    DrawTransitionView CaptureDrawTransitionAt413BE0() override;
private:
    IDrawManagerNativeRootSource& root_;
    IGraphicsTransitApi& graphics_;
};

// 这里只构造 Effect，不包含 main1 的捕获/合成工厂。
// 所有 Surface 均为借用，与原不持有 Ref 的 0x30 packet 一致。
// packet 使用期间依赖 owner 的 source/previous/work/mask 槽存活，本对象不 retain。
// 正常流程先在 main2 的 4C0990 完成并清空旧四 Ref，再进入后续 main1 准备。
// 原 main1 是先覆盖图像槽、再构造新 Effect、最后释放旧 Effect；本契约不要求
// 为残留旧 packet 额外保活，不前移 Finish，也不将准备改为事务。
// 若绕过正常清理，旧 source/句柄可能悬空；显式检测仅能算越界保护，非原行为。
// 4C0990 顺序：Finish -> 销毁 Effect -> source -> previous -> work/mask。
// 四个具体 Finish 均 noexcept（原 EHFlags=5）；绘制/失效异常 terminate，
// 不外传、不继续推进状态或解绑后续槽；共享基类的异常说明不改变。
// 固定view的manager、Source及screen须满足借用寿命；save仅在构造时读取。
// 空 source 按原 ctor 仅存指针，不提前解引用。
// 空 previous/work/mask 在原 [surface+4] 读取点由既有现代guard拒绝，
// 异常在对应ctor try内转成Ready()==0；原指令本身并未判空。
// 非空 Surface 的 Handle()==0 不拒绝。
// 这些空对象/生命周期guard是既有现代偏差，不等于原AV或原0x3C拥有布局。
// Save/screen仍是既有固定注入；本次不迁原Environment/Graphics根或Effect/Surface拥有。

// 晚取入口：构造/Ready/空Draw/早退Tick/未ready的Finish不捕获Source。
// native失败原样传播（Finish终止），不得重试或回到固定/现代manager。
[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptBlendEffect(
    IDrawTransitionViewSource& draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save,
    RuntimeGraphicsSurface* source, RuntimeGraphicsSurface* previous);

[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptMosaicEffect(
    IDrawTransitionViewSource& draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* work);

[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptTransitEffect(
    IDrawTransitionViewSource& draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    std::int32_t opcode);

[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptUniversalEffect(
    IDrawTransitionViewSource& draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* mask);

// 显式固定receiver入口，native view无需现代manager；不冒充上述逐点取根。
[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptBlendEffect(
    DrawTransitionView draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save,
    RuntimeGraphicsSurface* source, RuntimeGraphicsSurface* previous);

[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptMosaicEffect(
    DrawTransitionView draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* work);

[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptTransitEffect(
    DrawTransitionView draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    std::int32_t opcode);

[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptUniversalEffect(
    DrawTransitionView draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* mask);

// 既有caller的显式现代兼容入口，只构造Controlled view；不参与native失败处理。
[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptBlendEffect(
    DrawManagerRuntime& manager, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save,
    RuntimeGraphicsSurface* source, RuntimeGraphicsSurface* previous);

[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptMosaicEffect(
    DrawManagerRuntime& manager, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* work);

// 49D640 原样存入 opcode；main1 的 4C3650 selector 检查仍由 owner 执行。
// 本构造工厂不缩小合法 CTransit 编号范围，也不为无效编号伪造兜底。
[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptTransitEffect(
    DrawManagerRuntime& manager, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    std::int32_t opcode);

[[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptUniversalEffect(
    DrawManagerRuntime& manager, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* mask);
}
