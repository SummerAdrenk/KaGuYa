#pragma once

#include <Runtime/CgDisplayButtonGroupFactoryRuntime.h>
#include <Runtime/SceneFactoryRuntime.h>
#include <Runtime/SceneManagerDispatchRuntime.h>
#include <Runtime/SceneManagerPublicDialogRuntime.h>
#include <Runtime/RuntimeCStringResult.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
// 样本 18 CSceneManager(0x45B890) 的生产所有权骨架。
// SceneFactory 必须早于依赖它的 SceneController 构造；公共 Dialog 放在最后，
// 使 C++ 逆序析构保持原 0x463AF0 的 Dialog -> SceneController 顺序。
class SceneManagerRuntime final
    : public ICgDisplayButtonGroupSharedControlsAt499DC0
{
public:
    SceneManagerRuntime(
        ISceneConstructionServices& sceneServices,
        ISceneManagerPublicDialogFactory& publicDialogFactory,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~SceneManagerRuntime() noexcept;
    SceneManagerRuntime(const SceneManagerRuntime&) = delete;
    SceneManagerRuntime& operator=(const SceneManagerRuntime&) = delete;

    [[nodiscard]] SceneManagerDispatchRuntime& Dispatch() noexcept;
    [[nodiscard]] const SceneManagerDispatchRuntime& Dispatch() const noexcept;
    [[nodiscard]] SceneManagerPublicDialogRuntime& PublicDialogs() noexcept;
    [[nodiscard]] const SceneManagerPublicDialogRuntime& PublicDialogs() const noexcept;

    // 真实 +1C CString，由 ReadScript 命令与 LoadAfter 清名称共用；不是 pending 栈。
    [[nodiscard]] std::string_view ReadPendingCommandAt1C() const noexcept;
    [[nodiscard]] const RuntimeCString& PendingCommandSlotAt1C() const noexcept;
    [[nodiscard]] RuntimeCStringResult ClonePendingCommandAt414540() const;
    void WritePendingCommandAt1C(std::string_view commandCp932);
    void PopThenActivatePendingAt4C1760();

    // 45B957 的独立 CString；PROGRAM 00 写入，Ending 49DA7E 从同一槽 Clone。
    [[nodiscard]] const RuntimeCString& DemoNameSlotAt3C() const noexcept;
    void SetDemoNameAt4D6820(const char* value);

    [[nodiscard]] std::int32_t ReadSceneMarkerAt38() const;
    void WriteSceneMarkerAt38(std::int32_t value) noexcept;

    [[nodiscard]] std::shared_ptr<ICgDisplayButtonGroupSelector>
        AcquireSelectorAt478A30() override;
    [[nodiscard]] std::vector<std::shared_ptr<CgUiButtonRuntime>>
        AcquireModeButtonsAt47B7B0() override;

private:
    // 45B890 未写 +38；恢复前必须由真实写入方赋值，不能默认成正文分支 0。
    std::optional<std::int32_t> sceneMarkerAt38_;
    SceneFactoryRuntime sceneFactory_;
    SceneManagerDispatchRuntime dispatch_;
    // 45BC5D 清为空串；463BAD 在公共 Dialog 释放后、Controller 释放前销毁。
    RuntimeCStringResult pendingCommandAt1C_;
    SceneManagerPublicDialogRuntime publicDialogs_;
    // 仅在原始 GetNil 成功后取得资格；正常释放先于公共 Dialog，不经 ATL 自动析构。
    union DemoNameStorage
    {
        DemoNameStorage() noexcept {}
        ~DemoNameStorage() noexcept {}
        RuntimeCString value;
    } demoNameAt3C_;
};
}
