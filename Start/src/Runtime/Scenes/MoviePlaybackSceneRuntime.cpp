#include <Runtime/MoviePlaybackSceneRuntime.h>

#include <algorithm>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kMovieDirectoryCp932[] = "Movie\\";
constexpr long kMinimumDirectShowVolume = -10000L;
constexpr long kMaximumDirectShowVolume = 0L;
}

MoviePlaybackSceneRuntime::MoviePlaybackSceneRuntime(
    IMoviePlaybackSceneServices& services)
    : services_(&services)
{
    // 0x4A0B18..0x4A0B42：先保存并清零 CScript+4；后续失败不回滚这两步。
    savedScriptStateDwordAt04_ = services_->ReadScriptStateDwordAt04();
    services_->WriteScriptStateDwordAt04(0u);

    services_->CreateAndInstallDirectMovie();
    services_->SetInstalledMovieMuted(services_->ReadMovieMutedFlag());

    const std::int32_t volumeValue = services_->ReadMovieVolumeValue();
    const std::optional<long> calculatedVolume =
        services_->CalculateSoundVolume(100, volumeValue);
    const long volume = ClampDirectShowVolume(
        calculatedVolume.value_or(kMinimumDirectShowVolume));
    services_->SetInstalledMovieVolume(volume);

    const std::string resourceNameCp932 =
        services_->ResolveProgramParam00ResourceNameCp932();
    std::string moviePathCp932 = BuildMoviePath(
        services_->PrimaryMovieBaseDirectoryCp932(), resourceNameCp932);

    if (!services_->FileExists(moviePathCp932))
    {
        moviePathCp932 = BuildMoviePath(
            services_->SecondaryMovieBaseDirectoryCp932(), resourceNameCp932);
        if (!services_->FileExists(moviePathCp932))
        {
            // 0x4A0E07..0x4A0EAB：缺文件时报告资源名、重置选择并立即恢复脚本；
            // 此处不释放刚装入 root 的 DirectMovie，释放发生在下一次 Update。
            services_->AppendMissingMovieError(resourceNameCp932);
            services_->DisplayPendingErrors();
            RestoreScriptAdvanceAndInvalidate();
            moviePathCp932.clear();
        }
    }

    if (!moviePathCp932.empty())
        services_->LoadInstalledMovie(moviePathCp932, false);
}

void MoviePlaybackSceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
    Draw();
}

std::uint32_t MoviePlaybackSceneRuntime::SavedScriptStateDwordAt04() const noexcept
{
    return savedScriptStateDwordAt04_;
}

long MoviePlaybackSceneRuntime::ClampDirectShowVolume(const long value) noexcept
{
    return (std::max)(kMinimumDirectShowVolume,
        (std::min)(kMaximumDirectShowVolume, value));
}

std::string MoviePlaybackSceneRuntime::BuildMoviePath(
    const std::string& baseDirectoryCp932,
    const std::string& resourceNameCp932)
{
    // 0x4A0D17/0x4A0D7B：原程序按 base + 固定 "Movie\\" + 资源名直接拼接。
    return baseDirectoryCp932 + kMovieDirectoryCp932 + resourceNameCp932;
}

void MoviePlaybackSceneRuntime::RestoreScriptAdvanceAndInvalidate()
{
    services_->WriteScriptStateDwordAt04(savedScriptStateDwordAt04_);
    services_->AdvanceActiveSceneFromPending();
    services_->InvalidateScreenRect(0);
}

void MoviePlaybackSceneRuntime::Update()
{
    // 0x4B9528..0x4B95B1：只有活动中的电影才读取跳过输入；三个条件保持原短路顺序。
    if (services_->InstalledMovieIsActive())
    {
        const bool abortRequested =
            services_->PollMovieAbortActionAtSlot0() ||
            (!services_->ReadInputByteAt48() &&
                services_->ReadInputByteAt16() &&
                services_->ReadInputDwordAt40() == 0u) ||
            (!services_->ReadInputByteAt49() &&
                services_->ReadInputByteAt17() &&
                services_->ReadInputDwordAt40() == 0u);
        if (abortRequested)
            services_->FinishInstalledMoviePlayback();
    }

    // 0x4B95B6..0x4B961D：再次读取播放状态；结束帧依次恢复、提交、释放引用。
    if (!services_->InstalledMovieIsActive())
    {
        RestoreScriptAdvanceAndInvalidate();
        services_->ReleaseInstalledDirectMovie();
    }
}

void MoviePlaybackSceneRuntime::Draw() noexcept
{
    // 0x4B5E20 是严格空函数。
}

MoviePlaybackSceneConstructionServicesRuntime::
MoviePlaybackSceneConstructionServicesRuntime(
    ISceneConstructionServices& upstream,
    IMoviePlaybackSceneServices& movieServices) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream),
      movieServices_(&movieServices)
{
}

std::shared_ptr<ISceneTask>
MoviePlaybackSceneConstructionServicesRuntime::CreateMoviePlayback()
{
    return std::make_shared<MoviePlaybackSceneRuntime>(*movieServices_);
}
}
