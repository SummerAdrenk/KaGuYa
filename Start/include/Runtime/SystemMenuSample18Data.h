#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>

#include <array>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
// 18号PE：先606978背景，再6069B0起四项2Ch按钮记录，保持4F41A0的创建顺序。
// 复用既有typed描述；返回资源名为CP932原字节，不是UTF-8或宿主ACP字符串。
[[nodiscard]] const std::array<CgOperationChildDescriptor, 5>&
SystemMenuChildDescriptorsSample18At606978();

// 5095BD..5095C7向SceneManager+1C复制的八字节名称，不包含终止NUL。
[[nodiscard]] std::string_view SystemMenuSettingsSceneNameSample18At5B4E68() noexcept;
}
