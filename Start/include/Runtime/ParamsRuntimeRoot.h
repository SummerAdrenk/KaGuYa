#pragma once

#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ParamsGameSystemReference.h>
#include <Runtime/ParamsPatternNative.h>
#include <Runtime/ParamsSceneLabelNative.h>
#include <Runtime/ParamsRuntimeRootDiagnostic.h>
#include <Runtime/SystemDataEnvironmentArchive.h>
#include <cstddef>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class IParamsSerializeInput;
class IParamsHeaderSource;
// 唯一30h正文；两个虚槽之外没有诊断指针或拥有者。
class ParamsRuntimeRoot final : public IEnvironmentArchiveRuntime
{
public:
    struct RawTag final {};
    static constexpr std::size_t OriginalObjectSize = 0x30;
    static constexpr std::uint32_t OriginalVtableAddress = 0x5AE12C;

    // 4184E7..418525只初始化vptr和九个Ref DWORD，不写+28，也不注册诊断。
    explicit ParamsRuntimeRoot(RawTag) noexcept;
    ~ParamsRuntimeRoot() noexcept;
    ParamsRuntimeRoot(const ParamsRuntimeRoot&) = delete;
    ParamsRuntimeRoot& operator=(const ParamsRuntimeRoot&) = delete;

    // Legacy slot00 stays unchanged during the staged native input integration.
    void Serialize(SerializeBuffer& input);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    void SerializeAt434940(SerializeBuffer& input);
    // Nonvirtual native input leaf; slot00 remains Legacy until mainline integration.
    void SerializeNativeAt434940(void* native18);
    void SerializeAt434940(IParamsSerializeInput& input, const IParamsHeaderSource& headers);
    // 无外部绑定或尚未写版本时返回nullopt，不能读取原未初始化double。
    [[nodiscard]] std::optional<double> Version() const noexcept;
    [[nodiscard]] const auto& GameSystem() const noexcept { return gameSystemAt04; }
    [[nodiscard]] std::uint64_t GameSystemPublicationEpoch() const noexcept;
    [[nodiscard]] const auto& Pattern() const noexcept { return patternAt10; }
    [[nodiscard]] const auto& SceneLabels() const noexcept { return sceneLabelsAt1C; }
    [[nodiscard]] bool ReadCompleted() const noexcept;
    [[nodiscard]] ParamsRuntimeProfile Snapshot() const;

    // 不让编译器在412360的手工减持之后再自动析构这些Ref。
    union { ParamsGameSystemReference gameSystemAt04; };
    union { ParamsPatternReference patternAt10; };
    union { ParamsSceneLabelReference sceneLabelsAt1C; };
    double versionAt28;
};
static_assert(sizeof(ParamsRuntimeRoot) == 0x30);
static_assert(offsetof(ParamsRuntimeRoot, gameSystemAt04) == 0x04);
static_assert(offsetof(ParamsRuntimeRoot, patternAt10) == 0x10);
static_assert(offsetof(ParamsRuntimeRoot, sceneLabelsAt1C) == 0x1C);
static_assert(offsetof(ParamsRuntimeRoot, versionAt28) == 0x28);

// 原30h申请叶。成功存储须兼容全局delete；memory只注入申请，不驻留正文。
[[nodiscard]] ParamsRuntimeRoot* CreateParamsRuntimeRootAt4184D6(
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

class ParamsRuntimeRootOwner final
{
public:
    struct DiagnosticTag final {};
    // 原始拥有入口，不自动添加诊断；锁必须独立于Environment/SettingTag根。
    explicit ParamsRuntimeRootOwner(IEnvironmentRootLock& lock,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
    // 现有Version/Snapshot/epoch调用者显式选择外部诊断适配。
    // 只绑定本Owner最近创建的正文；回调安装外部正文须自行提供其诊断记录。
    explicit ParamsRuntimeRootOwner(DiagnosticTag,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    ParamsRuntimeRootOwner(DiagnosticTag, IEnvironmentRootLock& lock,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
    ~ParamsRuntimeRootOwner() noexcept;
    ParamsRuntimeRootOwner(const ParamsRuntimeRootOwner&) = delete;
    ParamsRuntimeRootOwner& operator=(const ParamsRuntimeRootOwner&) = delete;

    [[nodiscard]] ParamsRuntimeRoot* CaptureAt418480();
    [[nodiscard]] ParamsRuntimeRoot& GetOrCreateAt418480();
    [[nodiscard]] const ParamsRuntimeRoot* Published() const noexcept { return published_; }
    [[nodiscard]] ParamsRuntimeRoot*& PublicationSlot() noexcept { return published_; }
    void ReleaseAt5ABE20() noexcept;

private:
    void LeaveNoexcept() noexcept;
    IEnvironmentRootLock& lock_;
    IEnvironmentRootMemory& memory_;
    ParamsRuntimeRoot* published_ = nullptr;
    std::optional<ParamsRuntimeRootDiagnostic> diagnostic_;
};

// 替代现代栈上默认root；绑定过程不属于418480/RawTag的构造语义。
class ParamsRuntimeRootFixture final
{
public:
    ParamsRuntimeRootFixture() noexcept;
    ~ParamsRuntimeRootFixture() noexcept;
    ParamsRuntimeRootFixture(const ParamsRuntimeRootFixture&) = delete;
    ParamsRuntimeRootFixture& operator=(const ParamsRuntimeRootFixture&) = delete;
    [[nodiscard]] ParamsRuntimeRoot& Root() noexcept { return root_; }
    [[nodiscard]] const ParamsRuntimeRoot& Root() const noexcept { return root_; }
private:
    ParamsRuntimeRootDiagnostic diagnostic_;
    union { ParamsRuntimeRoot root_; };
};
}
