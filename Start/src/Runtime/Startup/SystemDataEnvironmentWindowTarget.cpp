#include <Runtime/SystemDataEnvironmentWindowTarget.h>
#include <Runtime/SystemDataEnvironmentArchive.h>
#include <Runtime/EnvironmentRootRuntime.h>

#include <cstring>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
void SystemDataEnvironmentWindowTarget::Apply(const ParamsGameSystem& gameSystem) const
{
    if (const auto snapshot = std::get_if<SystemDataEnvironmentOwnerInitialization*>(&target_))
    {
        // 兼容入口不声称执行4180C0；当前Startup迁移前保留其可观察字段。
        if (!*snapshot) return;
        auto& target = **snapshot;
        target.widthAt18 = gameSystem.width;
        target.heightAt1C = gameSystem.height;
        target.valueAt20 = 0x02CB0000;
        target.valueAt24 = 0x200;
        target.valueAt28 = 0x12000000;
        const std::string_view title = gameSystem.gameTitle.View();
        target.stringAt10 = title.substr(0, title.find('\0'));
        const std::string_view append = gameSystem.appendTitle.View();
        target.stringAt14 = append.substr(0, append.find('\0'));
        return;
    }

    auto& owner = *std::get<EnvironmentRootOwner*>(target_);
    const auto height = gameSystem.height; // 4963C0/4963C3：捕获前先高后宽。
    const auto width = gameSystem.width;
    auto* dimensions = owner.CaptureAt4180C0();
    dimensions->widthAt18 = width;
    dimensions->heightAt1C = height;
    owner.CaptureAt4180C0()->styleAt20 = 0x02CB0000;
    owner.CaptureAt4180C0()->exStyleAt24 = 0x200;
    owner.CaptureAt4180C0()->fullscreenStyleAt28 = 0x12000000;

    // 496414先借源raw，49641A捕获后固定目标，再测长；回调可改旧raw所指内容。
    // 这里借用chars而非复制CString：不能增持源header或提前取header长度。
    const char* const title = gameSystem.gameTitle.Native().GetString();
    auto& titleTarget = owner.CaptureAt4180C0()->titleAt10;
    const int titleLength = title ? static_cast<int>(std::strlen(title)) : 0;
    SetRuntimeCStringAt437680(titleTarget, title, titleLength);

    // 必须在首串写完之后才读取副标题，不能预存两个string_view/长度。
    const char* const append = gameSystem.appendTitle.Native().GetString();
    auto& appendTarget = owner.CaptureAt4180C0()->appendTitleAt14;
    const int appendLength = append ? static_cast<int>(std::strlen(append)) : 0;
    SetRuntimeCStringAt437680(appendTarget, append, appendLength);
}
}
