#include <Runtime/DemoSequenceResourceRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>

#include <bit>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
EnvironmentRootStorage& RequireEnvironment(EnvironmentRootStorage* receiver)
{
    if (!receiver) throw 1;
    return *receiver;
}

bool EmptyChars(const char* name)
{
    return _mbscmp(reinterpret_cast<const unsigned char*>(name),
        reinterpret_cast<const unsigned char*>("")) == 0;
}
}

DemoSequenceRootSource::DemoSequenceRootSource(RootRuntimeOwner& root,
    EnvironmentRootOwner& environment, IDemoSequenceEnvironmentArchives& archives) noexcept
    : root_(root), environment_(environment), archives_(archives) {}

RootRuntimePayload* DemoSequenceRootSource::CaptureRootAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.Published();
}
EnvironmentRootStorage* DemoSequenceRootSource::CaptureEnvironmentAt4180C0()
{ return environment_.CaptureAt4180C0(); }
DemoSoundManagerReceiver DemoSequenceRootSource::ResolveSoundManagerAt4188F0(RootRuntimePayload* receiver)
{
    if (!receiver) throw 1;
    const auto sound = receiver->PostWindow().DemoSoundReceiverAt24();
    if (!sound.Object()) throw 1;
    return sound;
}
EnvironmentSoundParamsRuntime& DemoSequenceRootSource::ResolveSoundParamsAt4188C0(
    EnvironmentRootStorage& receiver)
{ return archives_.ResolveSoundParamsAt4188C0(receiver); }
RuntimeCString& DemoSequenceRootSource::ResolveMusicNameAt4BF74E(EnvironmentRootStorage& receiver)
{ return archives_.ResolveMusicNameAt4BF74E(receiver); }

Win32DemoResourceErrorDisplay::Win32DemoResourceErrorDisplay(ProgramWindowHandleReference window,
    const std::string& gameTitleCp932) noexcept : window_(window), title_(gameTitleCp932) {}
void Win32DemoResourceErrorDisplay::Show(const char* message)
{
    static_cast<void>(MessageBoxA(window_.Read(), message, title_.c_str(), MB_ICONERROR));
}

DemoSequenceArchiveResourceServices::DemoSequenceArchiveResourceServices(
    IDemoSequenceRuntimeSource& source, IGraphicsSurfaceApi& graphics,
    IArchiveResourceSource& archives, const FileTransactionContext* transaction,
    IStartFileApi& files, const IEnvironmentKeyTableSource& key,
    IDemoResourceErrorDisplay& errors, ArchiveCodecServices codecs) noexcept
    : source_(source), graphics_(graphics), archives_(archives), transaction_(transaction),
      files_(files), key_(key), errors_(errors), codecs_(codecs) {}

EnvironmentRootStorage* DemoSequenceArchiveResourceServices::CaptureEnvironmentAt4180C0()
{ return source_.CaptureEnvironmentAt4180C0(); }

void* DemoSequenceArchiveResourceServices::CaptureDrawManagerAt413BE0()
{
    auto* const root = source_.CaptureRootAt413BE0();
    if (!root) throw 1;
    return root->PostWindow().DrawManagerAt18();
}

bool DemoSequenceArchiveResourceServices::LoadDemoMaskAt480000(void* receiver,
    DemoSurfaceSlot& target, const char* borrowedName)
{
    // 480000 保存 receiver 后仅转交4800A0；后者没有读取传入ECX的字段。
    // getter 时点仍由 caller 保留，目标槽的创建/替换不提前至 getter 之前。
    static_cast<void>(receiver);
    // Surface 仍由既有现代 owner 承接，不将 make_unique 宣称为原24h正文/Ref EH。
    if (!target) target = std::make_unique<RuntimeGraphicsSurface>(graphics_);
    return LoadDrawManagerStaticBorrowedAt4800A0(*target, borrowedName,
        archives_, transaction_, files_, key_, errors_, codecs_);
}

bool DemoSequenceArchiveResourceServices::LoadDemoSpriteAt4814F0(void* receiver,
    SpriteRuntime& target, const char* borrowedName, std::uint8_t flag)
{
    // 4814F0 不消费传入ECX；内部483D90在读取try内另取root，不在此处提前捕获。
    static_cast<void>(receiver);
    return LoadDrawManagerSpriteBorrowedAt4814F0(target, borrowedName, flag,
        archives_, transaction_, files_, key_, errors_, codecs_);
}

void DemoSequenceArchiveResourceServices::ExecuteDemoSoundAt4BF692(const ParamsDemoCmdSound& body)
{
    const auto mode = body.modeAt04;
    if (mode == 0)
    {
        if (EmptyChars(body.nameAt08.GetString()))
        {
            auto* const root = source_.CaptureRootAt413BE0();
            auto* const environment = source_.CaptureEnvironmentAt4180C0();
            auto& name = source_.ResolveMusicNameAt4BF74E(RequireEnvironment(environment));
            // 4BF6F8 以后才从之前捕获的root解析SoundManager，不重取当前发布根。
            source_.ResolveSoundManagerAt4188F0(root).StopDemoMusicAt4C3500(name, archives_);
            return;
        }
        auto* const first = source_.CaptureEnvironmentAt4180C0();
        auto& soundParams = source_.ResolveSoundParamsAt4188C0(RequireEnvironment(first));
        const auto* const name = body.nameAt08.GetString();
        auto* const root = source_.CaptureRootAt413BE0();
        const auto* const parameter = soundParams.BorrowPlaybackParameterAt477D50();
        auto* const second = source_.CaptureEnvironmentAt4180C0();
        // 4BF74E：字段指针跨第二次Environment getter保留，到这里才读取DWORD。
        if (!parameter) throw 1;
        const auto value = std::bit_cast<std::int32_t>(*parameter);
        auto& output = source_.ResolveMusicNameAt4BF74E(RequireEnvironment(second));
        static_cast<void>(source_.ResolveSoundManagerAt4188F0(root).PlayDemoMusicAt4BCCC0(output, name, value, 0, archives_));
        return;
    }
    if (mode == 1)
    {
        if (EmptyChars(body.nameAt08.GetString()))
        {
            auto* const root = source_.CaptureRootAt413BE0();
            source_.ResolveSoundManagerAt4188F0(root).StopAndClearAllVoiceSlots();
        }
        else
        {
            const auto* const name = body.nameAt08.GetString();
            auto* const root = source_.CaptureRootAt413BE0();
            static_cast<void>(source_.ResolveSoundManagerAt4188F0(root).PlayDemoVoiceAt4BCE10(name, archives_));
        }
        return;
    }
    // mode>=2 且非空无动作；+05从不消费，不补未知mode拒绝。
    if (EmptyChars(body.nameAt08.GetString()))
    {
        auto* const root = source_.CaptureRootAt413BE0();
        source_.ResolveSoundManagerAt4188F0(root).StopEffectsKeepingMappingsAt413270(archives_);
    }
}
}
