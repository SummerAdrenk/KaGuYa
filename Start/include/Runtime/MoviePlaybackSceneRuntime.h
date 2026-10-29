#pragma once

#include <Runtime/SceneControllerRuntime.h>
#include <Runtime/SceneFactoryRuntime.h>

#include <cstdint>
#include <optional>
#include <string>

namespace KaGuYa::Start::Runtime
{
class IMoviePlaybackSceneServices
{
public:
    virtual ~IMoviePlaybackSceneServices() = default;

    [[nodiscard]] virtual std::uint32_t ReadScriptStateDwordAt04() const = 0;
    virtual void WriteScriptStateDwordAt04(std::uint32_t value) = 0;

    // 0x4A0B45..0x4A0C1D：对象创建成功后立即装入 root 的 DirectMovie 引用。
    virtual void CreateAndInstallDirectMovie() = 0;
    [[nodiscard]] virtual bool ReadMovieMutedFlag() const = 0;
    virtual void SetInstalledMovieMuted(bool muted) = 0;
    [[nodiscard]] virtual std::int32_t ReadMovieVolumeValue() const = 0;

    // 4A0E44检查Sound包装Ref：Ref为空才直接采用-10000；非空调用
    // SoundCalcValueToVolume(100, value)。包装存在但缺DLL/导出不能伪装成空Ref。
    [[nodiscard]] virtual std::optional<long> CalculateSoundVolume(
        std::int32_t maximum,
        std::int32_t value) const = 0;
    virtual void SetInstalledMovieVolume(long volume) = 0;

    [[nodiscard]] virtual std::string ResolveProgramParam00ResourceNameCp932() const = 0;
    [[nodiscard]] virtual std::string PrimaryMovieBaseDirectoryCp932() const = 0;
    [[nodiscard]] virtual std::string SecondaryMovieBaseDirectoryCp932() const = 0;
    [[nodiscard]] virtual bool FileExists(const std::string& pathCp932) const = 0;
    virtual void AppendMissingMovieError(const std::string& resourceNameCp932) = 0;
    virtual void DisplayPendingErrors() = 0;
    virtual void LoadInstalledMovie(const std::string& pathCp932, bool loop) = 0;

    // 0x4B9520 中三组条件具有短路语义，不能折叠成一个“是否跳过”兜底接口。
    [[nodiscard]] virtual bool PollMovieAbortActionAtSlot0() = 0;
    [[nodiscard]] virtual bool ReadInputByteAt48() const = 0;
    [[nodiscard]] virtual bool ReadInputByteAt16() const = 0;
    [[nodiscard]] virtual std::uint32_t ReadInputDwordAt40() const = 0;
    [[nodiscard]] virtual bool ReadInputByteAt49() const = 0;
    [[nodiscard]] virtual bool ReadInputByteAt17() const = 0;

    [[nodiscard]] virtual bool InstalledMovieIsActive() const = 0;
    virtual void FinishInstalledMoviePlayback() = 0;

    virtual void AdvanceActiveSceneFromPending() = 0;
    virtual void InvalidateScreenRect(std::int32_t argument) = 0;
    virtual void ReleaseInstalledDirectMovie() = 0;
};

// Scene::CMovie（0x4A0B00、虚表 off_5B5330）的可证明行为边界。
class MoviePlaybackSceneRuntime final : public ISceneTask
{
public:
    explicit MoviePlaybackSceneRuntime(IMoviePlaybackSceneServices& services);

    // ITask 包装 0x4C3630 忽略传入参数，固定执行 Update 和空 Draw。
    void Run(std::int32_t argument) override;

    [[nodiscard]] std::uint32_t SavedScriptStateDwordAt04() const noexcept;

private:
    static long ClampDirectShowVolume(long value) noexcept;
    [[nodiscard]] static std::string BuildMoviePath(
        const std::string& baseDirectoryCp932,
        const std::string& resourceNameCp932);

    void RestoreScriptAdvanceAndInvalidate();
    void Update();
    void Draw() noexcept;

    IMoviePlaybackSceneServices* services_ = nullptr;
    std::uint32_t savedScriptStateDwordAt04_ = 0u;
};

// SceneFactory 的电影播放分支。DirectMovie 的建立、安装和释放仍全部由
// 已审计的 IMoviePlaybackSceneServices 完成，不在工厂层复制第二份状态。
class MoviePlaybackSceneConstructionServicesRuntime final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    MoviePlaybackSceneConstructionServicesRuntime(
        ISceneConstructionServices& upstream,
        IMoviePlaybackSceneServices& movieServices) noexcept;

    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateMoviePlayback() override;

private:
    IMoviePlaybackSceneServices* movieServices_ = nullptr;
};
}
