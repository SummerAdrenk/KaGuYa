#pragma once
#include <Runtime/SceneManagerView.h>
#include <Runtime/SceneManagerPublicDialogRuntime.h>
#include <variant>

namespace KaGuYa::Start::Runtime
{
// 标题/鉴赏等已选manager的公共操作借用，不维护控件owning槽的第二份镜像。
class SceneManagerPublicDialogView final
{
public:
    SceneManagerPublicDialogView(SceneManagerView selected) noexcept:source_(selected){}
    SceneManagerPublicDialogView(SceneManagerPublicDialogRuntime& controlled) noexcept:source_(&controlled){}
    void ClearAt46DB90() const
    {
        std::visit([](const auto& source)
        {if constexpr(std::is_same_v<std::decay_t<decltype(source)>,SceneManagerView>)source.ClearPublicAt46DB90();
            else source->ClearAt46DB90();},source_);
    }
    void ConfigureAt4703D0(std::int32_t mode) const
    {
        std::visit([&](const auto& source)
        {if constexpr(std::is_same_v<std::decay_t<decltype(source)>,SceneManagerView>)source.ConfigurePublicAt4703D0(mode);
            else source->ConfigureAt4703D0(mode);},source_);
    }
private:
    std::variant<SceneManagerView,SceneManagerPublicDialogRuntime*> source_;
};
}
