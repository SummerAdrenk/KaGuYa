#include <Runtime/DemoSequenceRuntime.h>
#include <Runtime/ParamsDemoLookupRuntime.h>

#include <bit>
#include <cstring>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 样本 18 原 PE 5B4E94/5B4EBC/5B4EE4，保持 char* 异常及 CP932 内容。
char missingDemo[] = "\x83\x66\x83\x82\x83\x66\x81\x5B\x83\x5E\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49\x81\x49 : [CDemo]";
char unsupportedWait[] = "Wait\x83\x52\x83\x7D\x83\x93\x83\x68\x82\xAA\x96\xA2\x91\xCE\x89\x9E\x82\xC5\x82\xB7\x81\x49\x81\x49 : [CDemo]";
char missingPicture[] = "\x89\xE6\x91\x9C\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xAA\x82\xC8\x82\xA2\x82\xC8\x81\x49\x81\x49 : [CDemo]";

std::int32_t Signed(std::uint32_t value) noexcept
{
    return std::bit_cast<std::int32_t>(value);
}

bool EqualChars(const char* first, const char* second)
{
    return _mbscmp(reinterpret_cast<const unsigned char*>(first),
        reinterpret_cast<const unsigned char*>(second)) == 0;
}

RuntimeGraphicsSurface& RequireSurface(const DemoSurfaceSlot& slot)
{
    if (!slot) throw 1;
    return *slot;
}
}

DemoSequenceRuntime::DemoSequenceRuntime(IDemoSequenceResourceServices& resources, IGraphicsSurfaceApi& graphics,
    IGraphicsSurfaceCompositionApi& composition, IGraphicsScreenUpdateApi& screen,
    IGraphicsSurfaceTransitionApi& transition, const DrawManagerRuntime& manager,
    const std::int32_t width, const std::int32_t height)
    : resources_(resources), graphics_(graphics), composition_(composition),
      screen_(screen), transitionApi_(transition), bounds_{0, 0, width, height},
      transition_(manager, screen)
{
    // 49B6F0：三张画布依次建立，任一次 Create 返回 0 均抛整数 1。
    canvas_ = std::make_unique<RuntimeGraphicsSurface>(graphics);
    if (!canvas_->Create(width, height, false, true)) throw 1;
    previous_ = std::make_unique<RuntimeGraphicsSurface>(graphics);
    if (!previous_->Create(width, height, false, true)) throw 1;
    alphaCanvas_ = std::make_unique<RuntimeGraphicsSurface>(graphics);
    if (!alphaCanvas_->Create(width, height, true, true)) throw 1;
    sprites_ = std::make_unique<DemoSpriteLayerRuntime>(graphics, bounds_);
}

DemoSequenceRuntime::~DemoSequenceRuntime()
{
    // 4A3CA0 flags5：Data Ref 先释放，失败立即终止。图形 owner 仍是现有现代边界。
    ReleaseParamsDemoLookupAt4A32A0(demoData_);
    sprites_.reset();
    alphaCanvas_.reset();
    previous_.reset();
    mask_.reset();
    canvas_.reset();
}

void DemoSequenceRuntime::BindDemoDataAt4C2220(const YtlReferenceSlotRuntime& source)
{
    // 4C2220 按槽地址判自赋值；旧 payload 回调之后才读 source，不先增持或快照。
    if (&demoData_ != &source)
    {
        ReleaseParamsDemoDataAt43B7D0(demoData_);
        demoData_.baseAt00 = source.baseAt00;
        demoData_.biasAt04 = source.biasAt04;
        demoData_.controlAt08 = source.controlAt08;
        auto* const control = demoData_.controlAt08;
        if (control) ++control->referenceCountAt04;
    }
    // 旧释放抛出不走这里；成功只清 index，不清 wait/sprite/transition。
    commandIndex_ = 0;
}

bool DemoSequenceRuntime::LoadMask(const char* borrowedName)
{
    auto* const receiver = resources_.CaptureDrawManagerAt413BE0();
    return resources_.LoadDemoMaskAt480000(receiver, mask_, borrowedName);
}

std::int32_t DemoSequenceRuntime::Run(const std::uint8_t completionRequested)
{
    if (completionRequested) waitTicks_ = 0;
    else if (waitTicks_ != 0)
    {
        waitTicks_ = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(waitTicks_) - 1u);
        return 2;
    }
    // 4BDAE0：隐藏精灵的未完成运动同样阻塞 completion。
    if (sprites_->AnyMoving()) return 2;
    if (completionRequested)
    {
        const auto previous = RequireSurface(previous_).Handle();
        transitionApi_.BackupScreenToSurfaceIfLoaded(previous);
        if (!LoadMask("bg_black.bgd")) return -1;
        transition_.Reset();
        auto& state = transition_.State();
        state.opcode = 0;
        state.source = mask_ ? &mask_->NativeBody() : nullptr;
        state.previous = RequireSurface(previous_).Handle();
        transition_.SetStep(8);
        return 1;
    }
    for (;;)
    {
        const auto result = ExecuteCommandAt4BF5A0();
        // 4BDBF7：catch 返回 -1 也是正常返回，照常 DWORD wrap 自增。
        ++commandIndex_;
        if (result >= -1 && result <= 2) return result;
    }
}

DemoSpriteRuntime& DemoSequenceRuntime::ExistingSprite(const std::size_t index)
{
    if (index >= sprites_->Count()) throw 1;
    return sprites_->At(index);
}

void DemoSequenceRuntime::BeginTransition(const ParamsDemoCmdTransit& command)
{
    transition_.Reset();
    transition_.SetStep(Signed(command.stepAt08));
    auto& state = transition_.State();
    if (EqualChars(command.effectAt04.GetString(), "BLEND_BLT"))
    {
        state.opcode = 0;
        const auto previous = RequireSurface(previous_).Handle();
        transitionApi_.BackupScreenToSurfaceIfLoaded(previous);
        state.previous = RequireSurface(previous_).Handle();
    }
    else if (EqualChars(command.effectAt04.GetString(), "UNIVERSAL_BLT"))
    {
        state.opcode = 1;
        if (EqualChars(command.maskAt0C.GetString(), "")) throw 1;
        const auto* const maskName = command.maskAt0C.GetString();
        if (!LoadMask(maskName)) throw 1;
        state.mask = RequireSurface(mask_).Handle();
        state.current = 255;
        state.range = 128;
        state.target = -128;
        const auto previous = RequireSurface(previous_).Handle();
        transitionApi_.BackupScreenToSurfaceIfLoaded(previous);
        state.previous = RequireSurface(previous_).Handle();
        sprites_->ComposeInto(RequireSurface(alphaCanvas_), composition_, screen_);
        state.source = alphaCanvas_ ? &alphaCanvas_->NativeBody() : nullptr;
        return;
    }
    else
    {
        state.opcode = -1;
        transition_.SetStep(256);
        state.opcode = 3;
    }
    sprites_->ComposeInto(RequireSurface(canvas_), composition_, screen_);
    state.source = canvas_ ? &canvas_->NativeBody() : nullptr;
}

void* DemoSequenceRuntime::ResolveCommandBody(std::uint32_t byteOffset) const
{
    auto* const data = ResolveParamsDemoDataAt4A52A0(demoData_);
    if (!data) throw 1;
    const auto begin = reinterpret_cast<std::uintptr_t>(data->commandsAt00.begin);
    const auto end = reinterpret_cast<std::uintptr_t>(data->commandsAt00.end);
    // 原无边界检查；只在实际访问点拒绝非法范围，不预解码、不补隐含 END。
    if (!begin || end < begin || end - begin < sizeof(YtlReferenceSlotRuntime) ||
        byteOffset > end - begin - sizeof(YtlReferenceSlotRuntime))
        throw std::out_of_range("Demo command reference");
    auto* const slot = reinterpret_cast<const YtlReferenceSlotRuntime*>(begin + byteOffset);
    auto* const body = ResolveParamsDemoCommand(*slot);
    if (!body) throw 1;
    return body;
}

std::int32_t DemoSequenceRuntime::ExecuteCommandAt4BF5A0()
{
    try
    {
        // 5EF720 的 catch 覆盖缺 Demo 检查；command 本身没有额外 AddRef。
        if (!demoData_.baseAt00) throw missingDemo;
        const std::uint32_t offset = commandIndex_ * 12u;
        std::uint32_t type;
        std::memcpy(&type, ResolveCommandBody(offset), sizeof(type));
        switch (type)
        {
        default:
            return 0;
        case 1:
            return 2;
        case 2:
        {
            const auto& body = *static_cast<const ParamsDemoCmdWait*>(ResolveCommandBody(offset));
            if (body.modeAt04 != 0) throw unsupportedWait;
            waitTicks_ = Signed(body.ticksAt08);
            return 2;
        }
        case 3:
            resources_.ExecuteDemoSoundAt4BF692(
                *static_cast<const ParamsDemoCmdSound*>(ResolveCommandBody(offset)));
            return 3;
        case 4:
        {
            const auto& body = *static_cast<const ParamsDemoCmdLoad*>(ResolveCommandBody(offset));
            for (;;)
            {
                const auto count = Signed(static_cast<std::uint32_t>(sprites_->Count()));
                if (Signed(body.slotAt04) < count) break;
                // 一次只创建/追加一个原 Ref；下一轮重新读正文 slot 和当前 count。
                sprites_->AppendOneDemoSpriteAt4BF864();
            }
            const auto* const name = body.nameAt08.GetString();
            auto* const receiver = resources_.CaptureDrawManagerAt413BE0();
            auto& sprite = ExistingSprite(body.slotAt04).Sprite();
            if (!resources_.LoadDemoSpriteAt4814F0(receiver, sprite, name, 1)) throw missingPicture;
            return 3;
        }
        case 5:
            BeginTransition(*static_cast<const ParamsDemoCmdTransit*>(ResolveCommandBody(offset)));
            return 1;
        case 6:
        {
            const auto& body = *static_cast<const ParamsDemoCmdDisp*>(ResolveCommandBody(offset));
            const auto count = static_cast<std::uint32_t>(sprites_->Count());
            const auto slot = Signed(body.slotAt04);
            if (slot < 0)
            {
                // 首轮只判 count==0；4BFC0C 的回边是 unsigned jb。
                if (count == 0) return 3;
                std::uint32_t index = 0;
                do
                {
                    auto& sprite = ExistingSprite(index).Sprite();
                    sprite.SetActiveByte(body.visibleAt08);
                    ++index;
                } while (index < static_cast<std::uint32_t>(sprites_->Count()));
            }
            else
            {
                if (slot >= Signed(count)) throw 1;
                auto& sprite = ExistingSprite(static_cast<std::uint32_t>(slot)).Sprite();
                sprite.SetActiveByte(body.visibleAt08);
            }
            return 3;
        }
        case 7:
        {
            // 4BFC3A 先取高，4BFC42 再取宽；不使用构造时 bounds_。
            auto* const first = resources_.CaptureEnvironmentAt4180C0();
            if (!first) throw 1;
            const auto height = Signed(first->heightAt1C);
            auto* const second = resources_.CaptureEnvironmentAt4180C0();
            if (!second) throw 1;
            const auto width = Signed(second->widthAt18);
            sprites_->DirtyRects().Add(RECT{0, 0, width, height});
            return 3;
        }
        case 8:
        {
            const auto& body = *static_cast<const ParamsDemoCmdMove*>(ResolveCommandBody(offset));
            const auto count = Signed(static_cast<std::uint32_t>(sprites_->Count()));
            if (Signed(body.slotAt04) >= count) throw 1;
            const auto previous = ExistingSprite(body.slotAt04).Bounds();
            const auto duration = Signed(body.durationAt08);
            const auto startX = Signed(body.startXAt0C);
            const auto slot = body.slotAt04;
            const auto endX = Signed(body.endXAt14);
            const auto endY = Signed(body.endYAt18);
            const auto startY = Signed(body.startYAt10);
            auto& sprite = ExistingSprite(slot);
            // 4BFD02 先重读 Y，再重读 X；判等值与实际运动所用的缓存末坐标分开。
            const auto comparedEndY = Signed(body.endYAt18);
            const auto comparedEndX = Signed(body.endXAt14);
            if (startX == comparedEndX && startY == comparedEndY) throw 1;
            sprite.CommitMotionAt4BFD18(duration, startX, startY, endX, endY);
            sprites_->DirtyRects().Add(previous);
            return 3;
        }
        case 9:
        {
            const auto& body = *static_cast<const ParamsDemoCmdPos*>(ResolveCommandBody(offset));
            // 4BFE5C 先缓存 end/begin，后读 slot；寻址沿用该 begin，不重新解析整个表。
            const auto* const end = sprites_->NativeSprites().end;
            const auto* const begin = sprites_->NativeSprites().begin;
            const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
                reinterpret_cast<std::uintptr_t>(begin));
            const auto count = Signed(bytes) / 12;
            const auto slot = Signed(body.slotAt04);
            if (slot >= count) throw 1;
            if (slot < 0 || !begin) throw 1; // 原非法下标的明确现代访问保护。
            auto* const sprite = ResolveEnvironmentObjectReference<DemoSpriteRuntime>(begin[slot]);
            const auto x = Signed(body.xAt08);
            const auto y = Signed(body.yAt0C);
            if (!sprite) throw 1;
            sprite->SetPosition(x, y);
            return 3;
        }
        }
    }
    catch (...) { return -1; }
}

bool DemoSequenceRuntime::AdvanceTransition() { return transition_.Advance(); }
void DemoSequenceRuntime::UpdateSpritesAndRedraw()
{
    sprites_->UpdateSpritesAndRedraw(composition_, screen_);
}
std::uint32_t DemoSequenceRuntime::CommandIndex() const noexcept { return commandIndex_; }
std::int32_t DemoSequenceRuntime::WaitTicks() const noexcept { return waitTicks_; }
DemoSpriteLayerRuntime& DemoSequenceRuntime::Sprites() noexcept { return *sprites_; }
const DemoTransitionRuntime& DemoSequenceRuntime::Transition() const noexcept { return transition_; }
YtlReferenceSlotRuntime& DemoSequenceRuntime::DemoDataReference() noexcept { return demoData_; }
const YtlReferenceSlotRuntime& DemoSequenceRuntime::DemoDataReference() const noexcept { return demoData_; }

DemoSequenceFactoryRuntime::DemoSequenceFactoryRuntime(IDemoSequenceResourceServices& resources, IGraphicsSurfaceApi& graphics,
    IGraphicsSurfaceCompositionApi& composition, IGraphicsScreenUpdateApi& screen,
    IGraphicsSurfaceTransitionApi& transition, const DrawManagerRuntime& manager,
    const std::int32_t width, const std::int32_t height) noexcept
    : resources_(resources), graphics_(graphics), composition_(composition),
      screen_(screen), transition_(transition), manager_(manager), width_(width), height_(height) {}

std::unique_ptr<DemoSequenceRuntime> DemoSequenceFactoryRuntime::Create()
{
    return std::make_unique<DemoSequenceRuntime>(resources_, graphics_, composition_,
        screen_, transition_, manager_, width_, height_);
}
std::unique_ptr<ILogoDemoSequenceRuntime> DemoSequenceFactoryRuntime::CreateLogoDemoSequence()
{
    return Create();
}
std::unique_ptr<IEndingDemoSequenceRuntime> DemoSequenceFactoryRuntime::CreateEndingDemoSequence()
{
    return Create();
}
}
