#include <Runtime/SavePreviewCaptureRuntime.h>

#include <Runtime/CountDownWindowResources.h>
#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/PatternLayerRuntime.h>
#include <Runtime/ParamsPatternSource.h>
#include <Runtime/ParamsPatternNativeAccess.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/WaitMarkServicesRuntime.h>

#include <bit>
#include <mbstring.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 46ED9A局部Ref12：仅增持FileGroup内数组，不保活整个Pattern。
struct PreviewComponents final
{
    YtlReferenceSlotRuntime slot{};
    YtlReferenceControlRuntime* savedControl = nullptr;
    bool armed = true;
    ~PreviewComponents() noexcept { if (armed) ReleaseYtlReference(slot); }
    void Capture(const RuntimeCStringArrayReference& source) noexcept
    {
        slot = source.RawSlot();
        savedControl = slot.controlAt08;
        if (savedControl) ++savedControl->referenceCountAt04;
    }
    [[nodiscard]] const RuntimeCString* Get() const noexcept
    { return ResolveEnvironmentObjectReference<RuntimeCString>(slot); }
    void FinishAt46EFA1() noexcept
    {
        // 46EF95先撤资格，正常路径使用46EE1B缓存的EDI；state0B要求释放异常终止。
        armed = false;
        if (!savedControl) return;
        if (--savedControl->referenceCountAt04 == 0)
        {
            if (savedControl->owningAt08) savedControl->DestroyPayloadAtVslot00();
            savedControl->DeleteControlAtVslot04(1);
        }
        slot.baseAt00 = 0;
        slot.controlAt08 = nullptr;
    }
};

std::int32_t Add(std::int32_t x, std::int32_t y) noexcept
    { return std::bit_cast<std::int32_t>(std::uint32_t(x) + std::uint32_t(y)); }

bool HasSuffixAt45DA40(const std::string& name)
{
    const auto* dot = _mbsrchr(reinterpret_cast<const unsigned char*>(name.c_str()), '.');
    return dot != nullptr && dot[1] != 0;
}

std::shared_ptr<RuntimeGraphicsSurface> CloneAt444470(RuntimeGraphicsSurface& source,
    IGraphicsSurfaceApi& surfaces, IGraphicsPatternFilterApi& filters, bool copy)
{
    auto result = std::make_shared<RuntimeGraphicsSurface>(surfaces);
    if (!result->Create(source.Width(), source.Height(), source.HasAlpha(), true)) return {};
    if (copy)
    {
        filters.SurfaceCopyBlitRaw(result->Handle(), source.Handle(), 0, 0, nullptr, nullptr, nullptr, 0);
        result->SetOrigin(source.OriginX(), source.OriginY());
    }
    return result;
}
}

SavePreviewCaptureRuntime::SavePreviewCaptureRuntime(SavePreviewCaptureDependencies dependencies) noexcept
    : dependencies_(dependencies) {}

std::shared_ptr<RuntimeGraphicsSurface> SavePreviewCaptureRuntime::SnapshotPatternAt46EA70(
    PatternLayerRuntime& pattern)
{
    // 46EAAB..46EAD9 的对象及Ref构造不在try(states2..17h)中。
    auto snapshot = std::make_shared<RuntimeGraphicsSurface>(dependencies_.surfaces);
    try
    {
        const SIZE screen = dependencies_.sizes.ReadScreenSizeAt47CC00();
        if (!snapshot->Create(screen.cx, screen.cy, pattern.MainSurfaceAt18().HasAlpha(), true)) throw 1;
        WaitMarkRendererRuntime renderer(dependencies_.drawing);
        for (std::size_t index = 0; index < pattern.SpriteSlotCount(); ++index)
        {
            auto& sprite = pattern.SpriteAt(index);
            if (sprite.LogicalName().empty()) continue;
            const auto* begin = reinterpret_cast<const unsigned char*>(sprite.LogicalName().c_str());
            // PE 5B3330=8D62、5B3334=89B3，替换首次完整MBCS匹配，不normalize。
            const auto* match = _mbsstr(begin, reinterpret_cast<const unsigned char*>("\x8D\x62"));
            if (match != nullptr)
            {
                const auto offset = static_cast<std::size_t>(match - begin);
                std::string name = sprite.LogicalName().substr(0, offset) + "\x89\xB3" +
                    sprite.LogicalName().substr(offset + 2);
                bool useExplicitPosition = false;
                std::int32_t x = 0, y = 0;
                if (!HasSuffixAt45DA40(name))
                {
                    // 46ED5B..46EE61只借第一次捕获的Pattern；反查和成员读取不能各自重取根。
                    const auto* const firstPattern = dependencies_.patternResources.CapturePattern();
                    PreviewComponents components;
                    if (!firstPattern) throw std::logic_error("preview first Pattern body is null");
                    const auto first = FindPatternItemByResourceName(*firstPattern, name);
                    if (first && ParamsPatternItemKindAt(*firstPattern, *first) == 1)
                        components.Capture(ParamsPatternFileGroupAt(*firstPattern, *first).stringsAt08);
                    // 46EE33只看base；无control的部分发布状态不能按count=0跳过。
                    if (const auto* const firstComponent = components.Get())
                    {
                        name.assign(RuntimeStringView(*firstComponent));
                    }
                    else
                    {
                        // 46EE84重新取源；46EF25赋值后仍从同一Pattern重解条目再读两个坐标。
                        std::string explicitName;
                        const auto* const secondPattern = dependencies_.patternResources.CapturePattern();
                        if (!secondPattern) throw std::logic_error("preview second Pattern body is null");
                        const auto second = FindPatternItemByResourceName(*secondPattern, name);
                        if (!second || ParamsPatternItemKindAt(*secondPattern, *second) != 2) throw 2;
                        explicitName.assign(RuntimeStringView(ParamsPatternExcPositionAt(*secondPattern, *second).subNameAt08));
                        const auto& position = ParamsPatternExcPositionAt(*secondPattern, *second);
                        x = std::bit_cast<std::int32_t>(position.xAt0C);
                        y = std::bit_cast<std::int32_t>(position.yAt10);
                        name = explicitName;
                        // 46EEC6暂存12*index，但46EF75成功后覆写1；首项同样使用显式坐标。
                        useExplicitPosition = true;
                    }
                    // 46EF95先撤销EH清理资格再按保存control释放；只有此前抛出才走45FE00。
                    components.FinishAt46EFA1();
                }
                // 46EFD6先取根/+18，再由4813B0分配Sprite；4814F0不消费manager的this。
                // 因而空manager不能在这里虚构失败，但取根的创建/异常副作用不可省略。
                static_cast<void>(dependencies_.runtime.CaptureDrawLayersAt413BE0());
                auto replacement = std::make_unique<SpriteRuntime>(dependencies_.surfaces);
                if (!dependencies_.spriteResources.LoadSpriteAt4814F0(*replacement, name, false)) throw 2;
                if (!useExplicitPosition) { x = replacement->X(); y = replacement->Y(); }
                replacement->SetOrigin(Add(x, pattern.X()), Add(y, pattern.Y()));
                static_cast<void>(renderer.DrawToSurface(*replacement, snapshot->Handle(), nullptr));
            }
            else
            {
                const auto x = sprite.X(), y = sprite.Y();
                sprite.SetOrigin(Add(x, pattern.X()), Add(y, pattern.Y()));
                static_cast<void>(renderer.DrawToSurface(sprite, snapshot->Handle(), nullptr));
                // 46F1EC..46F229 仅正常返回恢复；异常由外层catch处理并保留暂写坐标。
                pattern.SpriteAt(index).SetOrigin(x, y);
            }
        }

        const auto mode = pattern.FilterMode();
        if (mode > 0 && mode != 10)
        {
            const auto arguments = pattern.FilterArguments();
            const std::vector<std::uint32_t> copiedArguments(arguments.begin(), arguments.end());
            auto temporary = pattern.CopyTemporaryFilterSurfaceAt54();
            auto clone = CloneAt444470(*snapshot, dependencies_.surfaces, dependencies_.filters, mode != 3);
            if (mode == 6)
            {
                // 46F3C9..3ED先取Params/Pattern；46F3F5才读457F50已深拷贝的局部参数。
                const auto root = dependencies_.filterResources.CaptureParamsForNames();
                const auto names = root.CapturePatternNames();
                if (copiedArguments.empty()) throw std::out_of_range("capture resource filter has no resource ID");
                const char* const rawName = names.FindNameAt4AD1B0(
                    std::bit_cast<std::int32_t>(copiedArguments.front()));
                const std::string name(rawName ? rawName : "");
                if (!temporary) temporary = std::make_shared<RuntimeGraphicsSurface>(dependencies_.surfaces);
                if (dependencies_.filterResources.LoadInto(*temporary, name))
                    static_cast<void>(ApplyPatternPixelFilterAt442900(dependencies_.filters, mode,
                        copiedArguments, clone.get(), nullptr, temporary.get(), nullptr));
            }
            else
                static_cast<void>(ApplyPatternPixelFilterAt442900(dependencies_.filters, mode,
                    copiedArguments, clone.get(), mode == 3 ? snapshot.get() : nullptr, temporary.get(), nullptr));
            // 46F5B8 先释放原画布，再发布clone；local Filter的共享temp稍后释放。
            snapshot = std::move(clone);
        }
    }
    catch (...)
    {
        // PE5ECB2C ->5ECB18 ->5ECB08(flags40,type0) ->46F620，仅清本层局部Ref。
        snapshot.reset();
    }
    return snapshot;
}

void SavePreviewCaptureRuntime::CaptureIntoAt48DBE0(
    YtlReferenceSlotRuntime& targetAt1E4)
{
    GraphicsSurfaceNativeScope scope(dependencies_.surfaces);
    const SIZE screen = dependencies_.sizes.ReadScreenSizeAt47CC00();
    auto* body = NewGraphicsSurfaceAt40C2E0(dependencies_.surfaces);
    AdoptGraphicsSurfaceAt407A80(targetAt1E4, body);
    auto* target = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(targetAt1E4);
    if (!target) throw std::logic_error("capture destination body is null");
    if (!target->CreateAt415010(dependencies_.surfaces, screen.cx, screen.cy, false, true)) throw 2;
    const auto initialManager = dependencies_.runtime.CaptureDrawLayersAt413BE0();
    if (!initialManager) throw std::logic_error("capture initial DrawManager body is null");
    const auto window = initialManager.WindowZero();
    for (std::uint32_t index = 0;; ++index)
    {
        // 48DCE0..48DDBE逐轮重取Params/GameSystem；只保留最初借到的图层0。
        const auto* const gameSystem = dependencies_.runtime.CaptureGameSystemAt418480();
        if (!gameSystem) throw std::logic_error("capture GameSystem body is null");
        const auto count = static_cast<std::uint32_t>(gameSystem->startupConfigBytes.size());
        if (index >= count) break; // 48DDBE 每次按unsigned重读GameSystem数量。
        if (window.PatternLayerCount() < index) continue;
        const auto layer = window.PatternAt4AD2A0(index); // 保留显式受控模型的at边界，不min。
        if (!layer || layer.EnabledByteAt0C() == 0) continue;
        auto snapshot = SnapshotPatternAt46EA70(layer.ControlledBody());
        if (snapshot)
        {
            // 4425C0 先解引用目标，再检查两handle；局部快照在下一层之前释放。
            target = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(targetAt1E4);
            if (!target) throw std::logic_error("capture destination ref disappeared");
            if (target->Handle() != 0 && snapshot->Handle() != 0)
                static_cast<void>(dependencies_.drawing.SurfaceBlitRaw(target->Handle(),
                    snapshot->Handle(), 0, 0, nullptr, nullptr, nullptr, 0));
        }
    }
    for (std::uint32_t index = 1;; ++index)
    {
        // 48DED6读数量与48DF24取元素是两次独立取根，不缓存第一个manager。
        const auto countManager = dependencies_.runtime.CaptureDrawLayersAt413BE0();
        if (!countManager) throw std::logic_error("capture count DrawManager body is null");
        if (index >= countManager.OwnedLayerCount()) break;
        const auto itemManager = dependencies_.runtime.CaptureDrawLayersAt413BE0();
        if (!itemManager) throw std::logic_error("capture item DrawManager body is null");
        const auto layer = itemManager.LayerAt418C10(index);
        if (!layer || layer.EnabledByteAt0C() == 0) continue;
        // 48DFA3 和48DFD3：不看registered顺序，不调Draw/Advance/Prepare。
        target = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(targetAt1E4);
        layer.CompositeToSurface(target ? GraphicsSurfaceDestination::FromNative(*target) :
            GraphicsSurfaceDestination(0), nullptr);
    }
}
}
