#include <Runtime/ReadScriptPacketEffects.h>

#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/TransitRuntime.h>

#include <array>
#include <bit>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
DrawTransitionView NativeDrawTransitionViewSource::CaptureDrawTransitionAt413BE0()
{
    return DrawTransitionView(root_.CaptureDrawManagerAfter413BE0(), &graphics_);
}

namespace
{
std::int32_t ReadStepAt497900(const SaveDataEnvironmentArchive& save) noexcept
{
    // 497903..49790C：无符号 cmp/sbb/and；6018A0 的表值为 4/8/16/32。
    constexpr std::array<std::int32_t, 4> steps{4, 8, 16, 32};
    const auto index = save.valueAt70;
    return steps[index < steps.size() ? index : 0];
}

GraphicsSurfaceHandle ReadHandle(RuntimeGraphicsSurface* surface, const char* error)
{
    // 在原 [surface+4] 解引用点明确拒绝空对象，不提前检查 source/句柄。
    // 该错误仍归外层原 ctor try 处理。
    if (!surface) throw std::logic_error(error);
    return surface->Handle();
}

class PacketEffect : public IReadScriptEffectRuntime
{
public:
    std::uint8_t ReadyAtVslot08() const final { return ready_; }

protected:
    PacketEffect(DrawTransitionView draw, IDrawTransitionViewSource* drawSource,
        IGraphicsScreenUpdateApi& screen, std::int32_t opcode) noexcept
        : draw_(draw), drawSource_(drawSource), screen_(screen)
    {
        operation_.opcode = opcode;
    }

    void SetReady(const SaveDataEnvironmentArchive& save) noexcept
    {
        step_ = ReadStepAt497900(save);
        ready_ = 1;
    }

    void RequireTickable() const
    {
        // 未 ready 的原对象没有已初始化的 +38；owner 必须先查 Ready。
        // 对非法 owner 调用明确拒绝，不读取未定值，也不返回伪成功。
        if (!ready_ || finishAttempted_)
            throw std::logic_error("ReadScript packet effect is not tickable");
    }

    void Render()
    {
        // 4BB774/4BB8EA/4BBEC4/4BBF59：门控后才取根；Finish先写最终current。
        // 捕获完整manager后由view晚解+20/+24/+28；不二次取根，也不缓存CTransit。
        const auto draw = drawSource_ ? drawSource_->CaptureDrawTransitionAt413BE0() : draw_;
        // 4BB7BD/4BB935/4BBF0D/4BBFA4均丢弃分派AL，0也继续推进和失效。
        static_cast<void>(draw.RenderTransitionAt4C39E0(operation_));
    }

    void Invalidate() { screen_.InvalidatedRectIfLoaded(nullptr); }

    void AddStep() noexcept
    {
        operation_.current = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(operation_.current) + static_cast<std::uint32_t>(step_));
    }

    bool TickAscendingToTarget()
    {
        RequireTickable();
        if (operation_.opcode == -1 || operation_.current >= operation_.target) return false;
        Render();
        AddStep();
        if (operation_.current >= operation_.target) operation_.current = operation_.target;
        Invalidate();
        return true;
    }

    void FinishAt(std::int32_t current)
    {
        // 四个完成函数的 FuncInfo 均 EHFlags=5。外层具体 Finish 的 noexcept
        // 对齐原 CRT 543CB9 -> 543D62 -> 56A3EF，不让任何完成异常返回 owner。
        // 重复调用的现代 guard 同样终止，不重放绘制，也不伪报成功。
        if (finishAttempted_)
            throw std::logic_error("ReadScript packet effect Finish already attempted");
        finishAttempted_ = true;
        if (!ready_) return;
        operation_.current = current;
        Render();
        Invalidate();
    }

    TransitOperation operation_;
    std::int32_t step_ = 0;

private:
    DrawTransitionView draw_;
    IDrawTransitionViewSource* drawSource_;
    IGraphicsScreenUpdateApi& screen_;
    std::uint8_t ready_ = 0;
    bool finishAttempted_ = false;
};

class BlendEffect final : public PacketEffect
{
public:
    BlendEffect(DrawTransitionView draw, IDrawTransitionViewSource* drawSource,
        IGraphicsScreenUpdateApi& screen,
        const SaveDataEnvironmentArchive& save,
        RuntimeGraphicsSurface* source, RuntimeGraphicsSurface* previous)
        : PacketEffect(draw, drawSource, screen, 0)
    {
        // 49BB68..49BC75：仅捕获 state 1（FuncInfo 5EED3C）。
        try
        {
            operation_.source = source ? &source->NativeBody() : nullptr;
            operation_.previous = ReadHandle(previous, "CEffectBlend previous surface is missing");
            SetReady(save);
        }
        catch (...) {} // 49BC77 保留 ready=0；分配及成员初始化在 try 外。
    }
    bool TickAtVslot00() override { return TickAscendingToTarget(); } // 4BB750
    void DrawAtVslot04() override {} // 4B5520 原实现为空 ret。
    void Finish() noexcept override { FinishAt(255); } // 4A3FC0；FuncInfo 5EED60。
};

class MosaicEffect final : public PacketEffect
{
public:
    MosaicEffect(DrawTransitionView draw, IDrawTransitionViewSource* drawSource,
        IGraphicsScreenUpdateApi& screen,
        const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
        RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* work)
        : PacketEffect(draw, drawSource, screen, 2)
    {
        // 49BF88..49C0C5：state 1，FuncInfo 5EF2AC。
        try
        {
            operation_.source = source ? &source->NativeBody() : nullptr;
            operation_.previous = ReadHandle(previous, "CEffectMosaic previous surface is missing");
            operation_.destination = ReadHandle(work, "CEffectMosaic work surface is missing");
            operation_.current = 0;
            SetReady(save);
        }
        catch (...) {} // 49C0C7
    }
    bool TickAtVslot00() override
    {
        RequireTickable();
        if (operation_.current >= 512) return false; // 4BB8D3：有符号比较。
        Render();
        AddStep(); // 不截到 255 或 512。
        Invalidate();
        return true;
    }
    void DrawAtVslot04() override {} // 4B5540 原实现为空 ret。
    void Finish() noexcept override { FinishAt(512); } // 4A41C0；5EF2D0；此时不补画 source。
};

class TransitEffect final : public PacketEffect
{
public:
    TransitEffect(DrawTransitionView draw, IDrawTransitionViewSource* drawSource,
        IGraphicsScreenUpdateApi& screen,
        const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source, std::int32_t opcode)
        : PacketEffect(draw, drawSource, screen, opcode)
    {
        // 49D6E5..49D7CB：state 1，FuncInfo 5EEDEC。此处不新建 CTransit。
        try
        {
            operation_.source = source ? &source->NativeBody() : nullptr;
            SetReady(save);
        }
        catch (...) {} // 49D7CD；当前实体依赖使这段主体没有可抛调用。
    }
    bool TickAtVslot00() override { return TickAscendingToTarget(); } // 4BBEA0
    void DrawAtVslot04() override {} // 4B5840 原实现为空 ret。
    void Finish() noexcept override { FinishAt(255); } // 4A4590；FuncInfo 5EEE10。
};

class UniversalEffect final : public PacketEffect
{
public:
    UniversalEffect(DrawTransitionView draw, IDrawTransitionViewSource* drawSource,
        IGraphicsScreenUpdateApi& screen,
        const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
        RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* mask)
        : PacketEffect(draw, drawSource, screen, 1)
    {
        // 49D898..49D9E3：state 1，FuncInfo 5EEE9C。
        try
        {
            operation_.source = source ? &source->NativeBody() : nullptr;
            operation_.previous = ReadHandle(previous, "CEffectUniversal previous surface is missing");
            operation_.mask = ReadHandle(mask, "CEffectUniversal mask surface is missing");
            operation_.current = 255;
            operation_.range = 128;
            operation_.target = -128;
            SetReady(save);
        }
        catch (...) {} // 49D9E5
    }
    bool TickAtVslot00() override
    {
        RequireTickable();
        if (operation_.current < operation_.target) return false; // 4BBF43..4BBF49
        Render();
        operation_.current = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(operation_.current) - static_cast<std::uint32_t>(step_));
        Invalidate();
        return true;
    }
    void DrawAtVslot04() override {} // 4B5850 原实现为空 ret。
    void Finish() noexcept override { FinishAt(operation_.target); } // 4A4650；5EEEC0。
};

static_assert(noexcept(std::declval<BlendEffect&>().Finish()));
static_assert(noexcept(std::declval<MosaicEffect&>().Finish()));
static_assert(noexcept(std::declval<TransitEffect&>().Finish()));
static_assert(noexcept(std::declval<UniversalEffect&>().Finish()));
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptBlendEffect(
    IDrawTransitionViewSource& draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save,
    RuntimeGraphicsSurface* source, RuntimeGraphicsSurface* previous)
{
    return std::make_unique<BlendEffect>(DrawTransitionView{}, &draw, screen, save, source, previous);
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptMosaicEffect(
    IDrawTransitionViewSource& draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* work)
{
    return std::make_unique<MosaicEffect>(DrawTransitionView{}, &draw, screen, save, source, previous, work);
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptTransitEffect(
    IDrawTransitionViewSource& draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source, std::int32_t opcode)
{
    return std::make_unique<TransitEffect>(DrawTransitionView{}, &draw, screen, save, source, opcode);
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptUniversalEffect(
    IDrawTransitionViewSource& draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* mask)
{
    return std::make_unique<UniversalEffect>(DrawTransitionView{}, &draw, screen, save, source, previous, mask);
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptBlendEffect(
    DrawTransitionView draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save,
    RuntimeGraphicsSurface* source, RuntimeGraphicsSurface* previous)
{
    return std::make_unique<BlendEffect>(draw, nullptr, screen, save, source, previous);
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptMosaicEffect(
    DrawTransitionView draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* work)
{
    return std::make_unique<MosaicEffect>(draw, nullptr, screen, save, source, previous, work);
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptTransitEffect(
    DrawTransitionView draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source, std::int32_t opcode)
{
    return std::make_unique<TransitEffect>(draw, nullptr, screen, save, source, opcode);
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptUniversalEffect(
    DrawTransitionView draw, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* mask)
{
    return std::make_unique<UniversalEffect>(draw, nullptr, screen, save, source, previous, mask);
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptBlendEffect(
    DrawManagerRuntime& manager, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save,
    RuntimeGraphicsSurface* source, RuntimeGraphicsSurface* previous)
{
    return CreateReadScriptBlendEffect(DrawTransitionView(manager), screen, save, source, previous);
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptMosaicEffect(
    DrawManagerRuntime& manager, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* work)
{
    return CreateReadScriptMosaicEffect(DrawTransitionView(manager), screen, save, source, previous, work);
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptTransitEffect(
    DrawManagerRuntime& manager, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source, std::int32_t opcode)
{
    return CreateReadScriptTransitEffect(DrawTransitionView(manager), screen, save, source, opcode);
}

std::unique_ptr<IReadScriptEffectRuntime> CreateReadScriptUniversalEffect(
    DrawManagerRuntime& manager, IGraphicsScreenUpdateApi& screen,
    const SaveDataEnvironmentArchive& save, RuntimeGraphicsSurface* source,
    RuntimeGraphicsSurface* previous, RuntimeGraphicsSurface* mask)
{
    return CreateReadScriptUniversalEffect(DrawTransitionView(manager), screen, save, source, previous, mask);
}
}
