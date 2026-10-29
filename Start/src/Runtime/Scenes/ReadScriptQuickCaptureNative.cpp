#include <Runtime/ReadScriptQuickCaptureNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct CallerArgumentUnwind final
{
    YtlReferenceSlotRuntime& argument;
    bool armed = true;

    ~CallerArgumentUnwind() noexcept
    {
        // 59E4B0/59E4B8/59E460/59E468 -> 40D2F0，原 flags5：释放失败即终止。
        if (armed) ReleaseYtlReference(argument);
    }
};
}

void CaptureReadScriptQuickNativeAt4C1C60(ReadScriptQuickCaptureNativeServices& services)
{
    auto& source = services.source;
    source.EnsureRootAt413BE0();
    auto* const captureRoot = source.RootAt6198C4();
    const auto layers = source.LayersAt4175A0(captureRoot);

    GraphicsSurfaceNativeScope scope(services.surfaces);
    // 原隐藏返回直接落在按值参数区；合成返回前 caller 没有 Ref 析构资格。
    union Argument
    {
        YtlReferenceSlotRuntime ref;
        Argument() noexcept {}
        ~Argument() noexcept {}
    } argument;
    CompositeGraphicsFrameAt4A8340(argument.ref, services.surfaces, services.sizes, layers, 0);
    CallerArgumentUnwind unwind{argument.ref};

    source.EnsureRootAt413BE0();
    auto* const publishRoot = source.RootAt6198C4();
    auto& frame = source.FrameAt1F0(publishRoot);
    // Frame getter 仍在 caller 资格内；进入 setter 前撤销，由 callee 独占参数释放。
    unwind.armed = false;
    AssignGraphicsSurfaceReferenceAt4C1C60(frame, argument.ref);
}
}
