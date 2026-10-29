#include <Runtime/TitleDialogChildObjectGraphRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const std::array<CgOperationChildDescriptor, 7>&
TitleDialogChildDescriptorsSample18At608358()
{
    static const auto descriptors = []
    {
        // 6028A4 与 604030：CP932 原始字节，不能把资源前缀或全角英文字母归一化。
        const std::string prefix = "vrm:\\\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA";
        const auto base = prefix + "\x83\x78\x81\x5B\x83\x58.bmp";
        const std::array<const char*, 6> commands{
            "\x82\xCD\x82\xB6\x82\xDF\x82\xA9\x82\xE7",
            "\x82\xC2\x82\xC3\x82\xAB\x82\xA9\x82\xE7",
            "\x82\x62\x82\x66\x8A\xD3\x8F\xDC",
            "\x82\x72\x82\x83\x82\x85\x82\x8E\x82\x85\x8A\xD3\x8F\xDC",
            "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC",
            "\x83\x51\x81\x5B\x83\x80\x8F\x49\x97\xB9"};
        std::array<CgOperationChildDescriptor, 7> result;
        result[0] = {CgOperationControlKind::Static, 1,
            {{base, {}, {}, base, base, {}, {}}, std::nullopt}, 7, -1};
        for (std::size_t i = 0; i < commands.size(); ++i)
        {
            const auto name = prefix + '_' + commands[i];
            const auto target = name + "_target.alp";
            // 描述 +24h=0 送入 Button+60h，命中槽是 off 图，不是范围槽 6。
            result[i + 1] = {CgOperationControlKind::Button, static_cast<std::int32_t>(i + 3),
                {{name + "_off.alp", name + "_lock.alp", {}, target, target,
                    name + "_click.alp", {}}, std::nullopt}, 0, -1};
        }
        return result;
    }();
    return descriptors;
}

std::vector<GuiDialogNavigationGroupRef> CreateTitleDialogNavigationSample18At6083E0()
{
    std::vector<GuiDialogNavigationGroupRef> result;
    result.push_back(std::make_shared<GuiDialogNavigationGroupRuntime>(1,
        std::vector<GuiDialogNavigationMapping>{{3, 8}, {4, 3}, {5, 4}, {6, 5}, {7, 6}, {8, 7}}));
    result.push_back(std::make_shared<GuiDialogNavigationGroupRuntime>(2,
        std::vector<GuiDialogNavigationMapping>{{3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 8}, {8, 3}}));
    return result;
}

TitleDialogChildObjectGraphRuntime::TitleDialogChildObjectGraphRuntime()
    : events_(CgOperationClickedEventManagerRuntime::CreateEmptyAt4D7B90())
{
}

TitleDialogChildObjectGraphRuntime::TitleDialogChildObjectGraphRuntime(
    ICgOperationSurfaceFactory& factory, ICgOperationButtonInputServices& input)
    : TitleDialogChildObjectGraphRuntime()
{
    PopulateChildrenAt4F4530(factory, input);
}

void TitleDialogChildObjectGraphRuntime::PopulateChildrenAt4F4530(
    ICgOperationSurfaceFactory& factory, ICgOperationButtonInputServices& input)
{
    if (populationStarted_) throw std::logic_error("Title children were already constructed");
    populationStarted_ = true;
        for (const auto& descriptor : TitleDialogChildDescriptorsSample18At608358())
        {
            auto resources = std::make_shared<CgOperationControlResourceGroupRuntime>(factory, descriptor.resources);
            auto child = std::make_shared<CgOperationChildControlRuntime>(descriptor.kind,
                descriptor.eventValueAt04, descriptor.childIdAt0C, descriptor.initialResourceSlotAt24,
                std::move(resources));
            if (descriptor.kind == CgOperationControlKind::Button) child->BindButtonInputServicesAt4C(input);
            auto event = std::make_shared<CgOperationClickedEventRuntime>(child->EventValueAt04());
            child->BindClickedEventAt3C(event);
            child->WriteClickedEventEnabledAt48(1);
            // 4F4530：绑定完成后先插 child vector，再插 ClickedEvent vector。
            children_.push_back(child);
            events_->AppendBoundEventAt4D9620(std::move(event));
        }
}

TitleDialogChildObjectGraphRuntime::~TitleDialogChildObjectGraphRuntime()
{
    ReleaseClickedEventsAtC8();
    ReleaseChildrenAt0C();
}

void TitleDialogChildObjectGraphRuntime::ReleaseClickedEventsAtC8() noexcept
{
    events_.reset();
}

void TitleDialogChildObjectGraphRuntime::ReleaseChildrenAt0C() noexcept
{
    // 4F6C60 -> 4D7F00 从 begin 向 end 逐项释放，不依赖 STL 的元素析构顺序。
    for (auto& child : children_) child.reset();
    children_.clear();
}
}
