#include <Runtime/CgDisplayButtonGroupRuntime.h>
#include <Runtime/GraphicsTransitionSourceRuntime.h>
#include <Runtime/GuiDirtyRectNative.h>

#include <algorithm>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::int32_t kPreviousPageAction = 18;
constexpr std::int32_t kNextPageAction = 19;

class CgDisplayButtonGroupDirtyRectsRuntime final
    : public ICgDisplayButtonGroupDirtyRects
{
public:
    CgDisplayButtonGroupDirtyRectsRuntime()
    {
        auto& memory = DefaultEnvironmentRootMemory();
        void* storage = memory.AllocateBody(0x80);
        auto* body = storage ? ::new(storage) GuiDirtyRectNativeBody80h : nullptr;
        AdoptGuiDirtyRectReferenceAt498860(reference_, body, memory);
    }

    ~CgDisplayButtonGroupDirtyRectsRuntime() override
    {
        ReleaseGuiDirtyRectReferenceAt460500(reference_);
    }

    void PrepareAtVslot04() override
    {
        auto* body = ResolveGuiDirtyRectReference(reference_);
        body->vptrAt00->coalesceAt04(body);
    }

    [[nodiscard]] std::size_t CountAtVslot18() const noexcept override
    {
        auto* body = ResolveGuiDirtyRectReference(reference_);
        return body->vptrAt00->countAt18(body);
    }

    [[nodiscard]] std::span<const RECT> RectsAtVslot1C() const noexcept override
    {
        auto* body = ResolveGuiDirtyRectReference(reference_);
        const auto count = body->vptrAt00->countAt18(body);
        return {body->vptrAt00->rectsAt1C(body), count};
    }

    [[nodiscard]] std::int32_t AddAtVslot08(const RECT& rect) override
    {
        auto* body = ResolveGuiDirtyRectReference(reference_);
        body->vptrAt00->addRectAt08(body, &rect);
        // 原Add没有稳定的EAX业务合同，现代接口保留原有哨兵返回值。
        return 1;
    }

    void ClearAtVslot00() noexcept override
    {
        auto* body = ResolveGuiDirtyRectReference(reference_);
        body->vptrAt00->clearAt00(body);
    }

    [[nodiscard]] const YtlReferenceSlotRuntime* BorrowNativeReference() const noexcept override
    { return &reference_; }

private:
    YtlReferenceSlotRuntime reference_;
};
}

GraphicsSurfaceNativeBody*
ICgDisplayButtonGroupRuntimeServices::ResolveTransitionTargetNativeBody(
    const CgDisplayTransitionSourceRef& target) const
{
    const auto surface = std::dynamic_pointer_cast<GraphicsTransitionSourceRuntime>(target);
    return surface ? &surface->Surface().NativeBody() : nullptr;
}

std::unique_ptr<ICgDisplayButtonGroupDirtyRects> CreateCgDisplayButtonGroupDirtyRectsAt4A2E60()
{
    return std::make_unique<CgDisplayButtonGroupDirtyRectsRuntime>();
}

CgDisplayButtonGroupRuntime::CgDisplayButtonGroupRuntime(
    ICgDisplayButtonGroupRuntimeServices& services,
    IGraphicsSurfaceCompositionApi& graphics,
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> pageSurfaces,
    std::vector<std::shared_ptr<CgUiButtonRuntime>> mainButtons,
    std::vector<std::array<std::int32_t, 4>> navigationTable,
    CgDisplayPublicModeButtonsRuntime modeButtons,
    const std::int32_t currentModeIndex,
    const std::int32_t categoryIndex,
    std::vector<std::shared_ptr<CgUiButtonRuntime>> pageButtons,
    CgDisplayPublicSelectorRuntime selector)
    : CgDisplayButtonGroupRuntime(
          services,
          graphics,
          CreateCgDisplayButtonGroupDirtyRectsAt4A2E60(),
          std::move(pageSurfaces),
          std::move(mainButtons),
          std::move(navigationTable),
          std::move(modeButtons),
          currentModeIndex,
          categoryIndex,
          std::move(pageButtons),
          std::move(selector))
{
}

CgDisplayButtonGroupRuntime::CgDisplayButtonGroupRuntime(
    ICgDisplayButtonGroupRuntimeServices& services,
    IGraphicsSurfaceCompositionApi& graphics,
    std::unique_ptr<ICgDisplayButtonGroupDirtyRects> dirtyRects,
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> pageSurfaces,
    std::vector<std::shared_ptr<CgUiButtonRuntime>> mainButtons,
    std::vector<std::array<std::int32_t, 4>> navigationTable,
    CgDisplayPublicModeButtonsRuntime modeButtons,
    const std::int32_t currentModeIndex,
    const std::int32_t categoryIndex,
    std::vector<std::shared_ptr<CgUiButtonRuntime>> pageButtons,
    CgDisplayPublicSelectorRuntime selector)
    : services_(&services),
      graphics_(&graphics),
      dirtyRects_(std::move(dirtyRects)),
      state_(*this),
      pageSurfacesAt50_(std::move(pageSurfaces)),
      modeButtonsAt44_(std::move(modeButtons)),
      currentModeIndexAt40_(currentModeIndex),
      selectorAt78_(std::move(selector))
{
    if (!dirtyRects_)
        throw std::invalid_argument("CCGdispBtn dirty rect owner is null");
    if (selectorAt78_.IsNative() && !dirtyRects_->BorrowNativeReference())
        throw std::invalid_argument("CCGdispBtn Native selector requires a native DIR owner");
    const std::size_t expectedPages = (mainButtons.size() + 8u) / 9u;
    // 49F270 的缺失 group 得到 count=-1，原 signed 除法算出一页但零个项目。
    const bool missingGroupPage = mainButtons.empty() && pageSurfacesAt50_.size() == 1u;
    if ((!missingGroupPage && pageSurfacesAt50_.size() != expectedPages) || expectedPages > 3u)
    {
        throw std::invalid_argument(
            "CCGdispBtn page surface count differs from ceil(itemCount / 9)");
    }
    if ((!pageButtons.empty() && pageButtons.size() != 2u) ||
        (expectedPages >= 2u && pageButtons.size() != 2u) ||
        (expectedPages < 2u && !pageButtons.empty()))
    {
        throw std::invalid_argument(
            "CCGdispBtn page buttons do not match original page-count rule");
    }
    if (!modeButtonsAt44_.IsNative() && (currentModeIndexAt40_ < 0 ||
        currentModeIndexAt40_ >= modeButtonsAt44_.Count()))
    {
        throw std::out_of_range("CCGdispBtn current mode index is outside mode buttons");
    }

    state_.InstallMainButtons(std::move(mainButtons));
    state_.InstallNavigationTable(std::move(navigationTable));
    state_.InstallPageButtons(std::move(pageButtons));
    state_.SetCategoryIndexAt74(categoryIndex);
}

CgDisplayButtonGroupRuntime::~CgDisplayButtonGroupRuntime()
{
    // 0x4A4C60：严格保持 selector、翻页按钮、分页 Surface、模式按钮、
    // 主按钮与导航表的释放次序，不能依赖成员声明的默认逆序析构。
    selectorAt78_.Release();
    state_.ReleasePageButtonsAt5C();
    pageSurfacesAt50_.clear();
    modeButtonsAt44_.Release();
    state_.ReleaseMainButtonsAt14();
    state_.ClearNavigationTableAt20();
}

std::int32_t CgDisplayButtonGroupRuntime::ProcessSelectionAtVslot04()
{
    std::int32_t result = ProcessPageKeysAt4A9FC0();
    if (result == -1)
    {
        for (std::int32_t index = 0; index < modeButtonsAt44_.Count(); ++index)
        {
            if (index == currentModeIndexAt40_)
                continue;
            if (!modeButtonsAt44_.HasButton(index))
                continue;
            const std::int32_t oldState = modeButtonsAt44_.StateAt04(index);
            if (modeButtonsAt44_.ProcessAt4A98E0(index, false) >= 2)
            {
                result = ItemCountAt3C() + index;
            }
            if (oldState != modeButtonsAt44_.StateAt04(index))
                static_cast<void>(state_.AddDirtyRectAtVslot08(modeButtonsAt44_.VisibleRect(index)));
        }

        if (selectorAt78_)
        {
            if (selectorAt78_.ProcessAtVslot48() == 0)
            {
                // 4BEC88..4BEC9E先读模式count，再调+50，最后读项目数量。
                const auto modeCount = modeButtonsAt44_.Count();
                const auto selection = selectorAt78_.SelectionAtVslot50();
                result = ItemCountAt3C() + modeCount + selection;
            }
            selectorAt78_.AttachDirtyRectsAtVslot94(*dirtyRects_);
        }

        const auto& pageButtons = state_.PageButtonsAt5C();
        if (!pageButtons.empty())
        {
            if (!services_->PageAxisModeAt2C() &&
                services_->PageAxisDeltaAt34() != 0)
            {
                const std::int32_t oldPage = state_.CurrentPageAt6C();
                const std::int32_t delta = services_->PageAxisDeltaAt34();
                const std::int32_t lastPage =
                    static_cast<std::int32_t>(pageSurfacesAt50_.size()) - 1;
                state_.SetCurrentPageAt6C((std::clamp)(
                    oldPage + (delta < 0 ? 1 : -1), 0, lastPage));
                if (oldPage != state_.CurrentPageAt6C())
                {
                    state_.ResetGroupAt4C1260(true);
                    return -1;
                }
            }
            else
            {
                for (std::size_t index = 0; index < pageButtons.size(); ++index)
                {
                    if (!PageButtonVisible(index) || !pageButtons[index])
                        continue;
                    const std::int32_t oldState = pageButtons[index]->StateAt04();
                    if (pageButtons[index]->ProcessAt4A98E0(false) >= 2)
                    {
                        std::int32_t page = state_.CurrentPageAt6C() +
                            (index == 0u ? -1 : 1);
                        const std::int32_t pageCount =
                            static_cast<std::int32_t>(pageSurfacesAt50_.size());
                        if (page < 0)
                            page = pageCount - 1;
                        if (page >= pageCount)
                            page = 0;
                        state_.SetCurrentPageAt6C(page);
                        state_.ResetGroupAt4C1260(true);
                        EmitPageSound();
                        return -1;
                    }
                    if (oldState != pageButtons[index]->StateAt04())
                    {
                        static_cast<void>(state_.AddDirtyRectAtVslot08(
                            pageButtons[index]->VisibleRect()));
                    }
                }
            }
        }

        const auto& mainButtons = state_.MainButtonsAt14();
        const std::int32_t first =
            state_.CurrentPageAt6C() * state_.ItemsPerPageAt70();
        const std::int32_t end = first + state_.ItemsPerPageAt70();
        for (std::size_t index = 0; index < mainButtons.size(); ++index)
        {
            const std::int32_t signedIndex = static_cast<std::int32_t>(index);
            const auto& button = mainButtons[index];
            if (!button || signedIndex < first || signedIndex >= end)
                continue;
            const std::int32_t oldState = button->StateAt04();
            if (result == -1 && button->ProcessAt4A98E0(false) >= 2)
                result = signedIndex;
            if (oldState != button->StateAt04())
            {
                static_cast<void>(state_.AddDirtyRectAtVslot08(button->VisibleRect()));
                if (button->StateAt04() == 1 && result != signedIndex)
                {
                    services_->EmitSoundAt4BCD70(
                        CgDisplayButtonSoundAt4BCD70::HoverOrInvalidAt61A568, 1);
                }
            }
        }
    }

    if (result >= ItemCountAt3C())
    {
        services_->EmitSoundAt4BCD70(
            CgDisplayButtonSoundAt4BCD70::ExitOrModeAt61A56C, 0);
    }
    else if (result >= 0)
    {
        EmitPageSound();
    }
    else if (result == -4)
    {
        services_->EmitSoundAt4BCD70(
            CgDisplayButtonSoundAt4BCD70::HoverOrInvalidAt61A568, 1);
    }
    return result;
}

bool CgDisplayButtonGroupRuntime::PollInputReleaseGateAtVslot0C()
{
    // 0x4A9C60 读取的是聚合输入对象的 +0x11/+0x28/+0x31，不能用
    // 某个动作键的 edge/held 状态替代这三个字段。
    switch (state_.ReleaseGateStateAt10())
    {
    case 0:
        if (services_->ReadInputEdgeAt47F4E0(0))
        {
            state_.SetReleaseGateStateAt10(2);
            return false;
        }
        if (!services_->ReadAggregateInputByteAt31() &&
            services_->ReadAggregateInputByteAt11() &&
            services_->ReadAggregateInputDwordAt28() == 0)
        {
            state_.SetReleaseGateStateAt10(1);
        }
        return false;

    case 1:
        if (!services_->ReadAggregateInputByteAt11() ||
            services_->ReadAggregateInputDwordAt28() != 0)
        {
            state_.SetReleaseGateStateAt10(0);
            return true;
        }
        return false;

    case 2:
        if (!services_->ReadInputHeldAt4198D0(0))
        {
            state_.SetReleaseGateStateAt10(0);
            return true;
        }
        return false;

    default:
        return false;
    }
}

std::int32_t CgDisplayButtonGroupRuntime::MoveSelectionAtVslot30()
{
    return state_.MoveSelectionAtVslot30();
}

std::int32_t CgDisplayButtonGroupRuntime::ItemCountAt3C() const
{
    return static_cast<std::int32_t>(state_.ItemCountAt3C());
}

void CgDisplayButtonGroupRuntime::RenderDirtyRegionsAtVslot34()
{
    dirtyRects_->PrepareAtVslot04();
    const std::size_t count = dirtyRects_->CountAtVslot18();
    const std::span<const RECT> rects = count == 0u
        ? std::span<const RECT>{}
        : dirtyRects_->RectsAtVslot1C();
    if (count > rects.size())
        throw std::out_of_range("CCGdispBtn dirty rect count exceeds source span");

    if (count != 0u && pageSurfacesAt50_.empty())
        throw std::out_of_range("CCGdispBtn has dirty rects without a page surface");
    for (std::size_t index = 0; index < count; ++index)
    {
        const RECT& clip = rects[index];
        const auto& page = pageSurfacesAt50_.at(
            static_cast<std::size_t>(state_.CurrentPageAt6C()));
        if (!page)
            throw std::runtime_error("CCGdispBtn current page surface is null");
        static_cast<void>(graphics_->DrawSurface(
            page->Handle(), 0, 0, nullptr, nullptr, &clip, 0));

        const auto& mainButtons = state_.MainButtonsAt14();
        const std::size_t first = static_cast<std::size_t>(
            state_.CurrentPageAt6C() * state_.ItemsPerPageAt70());
        const std::size_t end = (std::min)(
            first + static_cast<std::size_t>(state_.ItemsPerPageAt70()),
            mainButtons.size());
        for (std::size_t buttonIndex = first; buttonIndex < end; ++buttonIndex)
        {
            if (mainButtons[buttonIndex])
                static_cast<void>(mainButtons[buttonIndex]->DrawToScreenAt441980(&clip));
        }
        if (selectorAt78_)
            selectorAt78_.DrawAtVslot60(clip);
        DrawButtonsToScreen(clip);
        const auto& pageButtons = state_.PageButtonsAt5C();
        for (std::size_t buttonIndex = 0; buttonIndex < pageButtons.size(); ++buttonIndex)
        {
            if (PageButtonVisible(buttonIndex) && pageButtons[buttonIndex])
                static_cast<void>(pageButtons[buttonIndex]->DrawToScreenAt441980(&clip));
        }
        services_->SubmitInvalidatedRect(clip);
    }
    dirtyRects_->ClearAtVslot00();
}

void CgDisplayButtonGroupRuntime::CompositeIntoAtVslot38(
    const CgDisplayTransitionSourceRef& target,
    const RECT* const destinationClip)
{
    if (pageSurfacesAt50_.empty())
        throw std::out_of_range("CCGdispBtn has no page surface to composite");
    auto* targetBody = services_->ResolveTransitionTargetNativeBody(target);
    const bool nativeControls = modeButtonsAt44_.IsNative() || selectorAt78_.IsNative();
    if (nativeControls && !targetBody)
        throw std::invalid_argument("CCGdispBtn Native controls require the actual Surface24h target");
    const GraphicsSurfaceDestination destination = targetBody
        ? GraphicsSurfaceDestination::FromNative(*targetBody)
        : GraphicsSurfaceDestination(services_->ResolveTransitionTargetHandle(target));
    const auto& page = pageSurfacesAt50_.at(
        static_cast<std::size_t>(state_.CurrentPageAt6C()));
    if (!page)
        throw std::runtime_error("CCGdispBtn current page surface is null");
    // 页面先于控件提交；后续叶端重新读取同一真实目标正文的当前句柄。
    const auto destinationHandle = destination.Handle();
    if (!targetBody || destinationHandle != 0)
    {
        const auto sourceHandle = page->Handle();
        if (!targetBody || sourceHandle != 0)
            static_cast<void>(graphics_->BlitSurface(destinationHandle, sourceHandle, 0, 0,
                nullptr, nullptr, destinationClip, 0));
    }

    const auto& mainButtons = state_.MainButtonsAt14();
    const std::size_t first = static_cast<std::size_t>(
        state_.CurrentPageAt6C() * state_.ItemsPerPageAt70());
    const std::size_t end = (std::min)(
        first + static_cast<std::size_t>(state_.ItemsPerPageAt70()),
        mainButtons.size());
    for (std::size_t index = first; index < end; ++index)
    {
        if (mainButtons[index])
            static_cast<void>(mainButtons[index]->DrawIntoAt4B5290(
                destination, destinationClip));
    }
    if (selectorAt78_)
        selectorAt78_.CompositeAtVslot5C(target, destination, destinationClip);
    DrawButtonsInto(destination, destinationClip);
    const auto& pageButtons = state_.PageButtonsAt5C();
    for (std::size_t index = 0; index < pageButtons.size(); ++index)
    {
        if (PageButtonVisible(index) && pageButtons[index])
            static_cast<void>(pageButtons[index]->DrawIntoAt4B5290(
                destination, destinationClip));
    }
}

void CgDisplayButtonGroupRuntime::ResetGroupAt4C1260(
    const std::int32_t invalidateFullScreen)
{
    state_.ResetGroupAt4C1260(invalidateFullScreen != 0);
}

CgDisplayButtonGroupStateRuntime& CgDisplayButtonGroupRuntime::State() noexcept
{
    return state_;
}

const CgDisplayButtonGroupStateRuntime&
CgDisplayButtonGroupRuntime::State() const noexcept
{
    return state_;
}

std::size_t CgDisplayButtonGroupRuntime::PageCountAt50() const noexcept
{
    return pageSurfacesAt50_.size();
}

std::int32_t CgDisplayButtonGroupRuntime::ProcessPageKeysAt4A9FC0()
{
    const std::int32_t result = services_->PollBaseInputAt4AA200();
    if (result != -2 && result != -1)
        return result;

    const auto& pageButtons = state_.PageButtonsAt5C();
    if (state_.InputStateAt08() == 10)
    {
        if (!services_->ReadInputHeldAt4198D0(kPreviousPageAction))
        {
            std::int32_t page = state_.CurrentPageAt6C() - 1;
            if (page < 0)
                page = static_cast<std::int32_t>(pageSurfacesAt50_.size()) - 1;
            state_.SetCurrentPageAt6C(page);
            state_.SetInputStateAt08(-1);
            state_.ResetGroupAt4C1260(true);
            EmitPageSound();
        }
        return result;
    }
    if (state_.InputStateAt08() == 11)
    {
        if (!services_->ReadInputHeldAt4198D0(kNextPageAction))
        {
            std::int32_t page = state_.CurrentPageAt6C() + 1;
            if (page >= static_cast<std::int32_t>(pageSurfacesAt50_.size()))
                page = 0;
            state_.SetCurrentPageAt6C(page);
            state_.SetInputStateAt08(-1);
            state_.ResetGroupAt4C1260(true);
            EmitPageSound();
        }
        return result;
    }

    if (services_->ReadInputEdgeAt47F4E0(kPreviousPageAction))
    {
        if (!pageButtons.empty() && state_.CurrentPageAt6C() != 0)
        {
            state_.SetInputStateAt08(10);
            pageButtons[0]->SetStateAt04(2);
            static_cast<void>(state_.AddDirtyRectAtVslot08(
                pageButtons[0]->VisibleRect()));
            return -2;
        }
    }
    else if (services_->ReadInputEdgeAt47F4E0(kNextPageAction))
    {
        if (!pageButtons.empty() &&
            state_.CurrentPageAt6C() !=
                static_cast<std::int32_t>(pageSurfacesAt50_.size()) - 1)
        {
            state_.SetInputStateAt08(11);
            pageButtons[1]->SetStateAt04(2);
            static_cast<void>(state_.AddDirtyRectAtVslot08(
                pageButtons[1]->VisibleRect()));
            return -2;
        }
    }
    return result;
}

bool CgDisplayButtonGroupRuntime::PageButtonVisible(
    const std::size_t index) const noexcept
{
    if (index == 0u && state_.CurrentPageAt6C() == 0)
        return false;
    if (index == 1u &&
        state_.CurrentPageAt6C() ==
            static_cast<std::int32_t>(pageSurfacesAt50_.size()) - 1)
    {
        return false;
    }
    return true;
}

void CgDisplayButtonGroupRuntime::DrawButtonsToScreen(
    const RECT& clip)
{
    for (std::int32_t index = 0; index < modeButtonsAt44_.Count(); ++index)
    {
        if (index == currentModeIndexAt40_)
        {
            continue;
        }
        if (modeButtonsAt44_.HasButton(index))
            static_cast<void>(modeButtonsAt44_.DrawToScreenAt441980(index, &clip));
    }
}

void CgDisplayButtonGroupRuntime::DrawButtonsInto(
    const GraphicsSurfaceDestination destination,
    const RECT* const clip)
{
    for (std::int32_t index = 0; index < modeButtonsAt44_.Count(); ++index)
    {
        if (index == currentModeIndexAt40_)
        {
            continue;
        }
        if (modeButtonsAt44_.HasButton(index))
            static_cast<void>(modeButtonsAt44_.DrawIntoAt4B5290(index, destination, clip));
    }
}

void CgDisplayButtonGroupRuntime::EmitPageSound()
{
    services_->EmitSoundAt4BCD70(
        CgDisplayButtonSoundAt4BCD70::ConfirmOrPageAt61A564, 0);
}

std::int32_t CgDisplayButtonGroupRuntime::SubmitDirtyRectAtVslot08(
    const RECT& rect)
{
    return dirtyRects_->AddAtVslot08(rect);
}

void CgDisplayButtonGroupRuntime::ResetCategorySelectorAtVslot90()
{
    if (selectorAt78_)
        selectorAt78_.ResetAtVslot90();
}

RECT CgDisplayButtonGroupRuntime::ReadScreenRectAt4180C0() const
{
    return services_->ReadScreenRectAt4180C0();
}
}
