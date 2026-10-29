#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace KaGuYa::Start::Runtime
{
// 4ADAE0 返回新建的二 DWORD 对象，不借用可变的选择槽。
struct ScenePlaybackSelection final
{
    std::int32_t fileIndex = 0;
    std::int32_t entry = 0;
};
using ScenePlaybackSelectionRef = std::shared_ptr<const ScenePlaybackSelection>;

// CModeReplay 的脚本边界与窗口宿主分开装配，共用同一 CScript 状态。
class IScenePlaybackScriptServices
{
public:
    virtual ~IScenePlaybackScriptServices() = default;
    virtual ScenePlaybackSelectionRef FindSelectionAt4ADAE0(
        std::int32_t group, std::int32_t item, std::int32_t segment) = 0;
    // 4B8919先捕获Pattern，4B893F才读selection.fileIndex，随后按首NUL复制CString。
    virtual std::string ResolveScriptNameAt4AD1B0(const ScenePlaybackSelection& selection) = 0;
    virtual void LoadReplayScriptAt4D53D0(const char* name, std::int32_t entry) = 0;
    virtual std::int32_t ExecuteScriptAt4D3FF0() = 0;
    virtual void WriteScriptDwordAt04(std::uint32_t value) = 0;
    virtual void WriteScriptByteAt08(std::uint8_t value) = 0;
};
}
