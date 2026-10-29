#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>

#include <cstdint>
#include <span>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
// 仅保存样本18原PE数据；不加载资源、不创建控件、不替父对象安装输入。
[[nodiscard]] std::span<const CgOperationChildDescriptor> PopupMenuStaticDescriptorsSample18();
[[nodiscard]] std::span<const CgOperationChildDescriptor> PopupMenuCheckBoxDescriptorsSample18();

struct PopupMenuCheckStampData
{
    std::string_view normalNameCp932;
    std::string_view slot1NameCp932;
    POINT origin;
};
[[nodiscard]] PopupMenuCheckStampData PopupMenuCheckStampsSample18() noexcept;

struct PopupMenuNavigationDescriptor
{
    std::int32_t input;
    std::span<const GuiDialogNavigationMapping> mappings;
};
struct PopupMenuHoverPair
{
    std::int32_t controlEvent;
    std::int32_t hintEvent;
};

// 50E044..50E072：mode==1取第一套，其余值取第二套，不新增模式限制。
[[nodiscard]] std::span<const std::int32_t> PopupMenuLayoutEventsSample18(std::int32_t mode) noexcept;
[[nodiscard]] std::span<const PopupMenuNavigationDescriptor> PopupMenuNavigationSample18(std::int32_t mode) noexcept;
[[nodiscard]] std::span<const PopupMenuHoverPair> PopupMenuHoverMapSample18() noexcept;
[[nodiscard]] POINT PopupMenuHintOffsetSample18() noexcept;
[[nodiscard]] POINT PopupMenuCursorOffsetSample18() noexcept;

// 509120仅结果2..5写场景命令；其余结果不具有此字符串映射。
[[nodiscard]] std::string_view PopupMenuSceneCommandSample18(std::int32_t result) noexcept;
}
