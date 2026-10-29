#include "MovieSceneNative.Internal.h"

namespace KaGuYa::Start::Runtime::MovieSceneNativeDetail
{
void RestoreAdvanceInvalidate(Body& body,IMovieSceneNativeSource& source,const CompletionSites& sites)
{
    auto* environment=source.CaptureEnvironmentAt4180C0(sites.environment);
    auto* script=ResolveEnvironmentArchiveAt417990(*environment,2);
    Field<volatile std::uint32_t>(script,4)=Read(body.savedModeAt08);
    auto* root=Root(source,sites.ensureRoot,sites.rootRead);
    auto* scenes=Resolve<void>(source.RootScenesSlotAt30(root,sites.scenesSlot));
    source.AdvanceScenesAt4C1760(root,scenes,sites.advance);
    auto* graphics=source.CaptureGraphicsAt4182F0(sites.graphics);
    source.InvalidateAt4B02E0(graphics,nullptr,sites.invalidate);
}
namespace
{
bool Abort(IMovieSceneNativeSource& source)
{
    auto* input=source.CaptureInputAt418210(0x4B955E);
    auto* keyboard=Resolve<KeyboardManagerNativeBody438h>(input->keyboardAt18);
    if(ReadKeyboardPressedAt47F4E0(*keyboard,0))return true;
    input=source.CaptureInputAt418210(0x4B958F);
    auto* mouse=Resolve<MouseManagerNativeBody3Ch>(input->mouseAt00);
    if(!Read(mouse->previousAt30[0])&&Read(mouse->pressedAt10[0])&&!Read(mouse->suppressionAt28))return true;
    input=source.CaptureInputAt418210(0x4B95C6);
    mouse=Resolve<MouseManagerNativeBody3Ch>(input->mouseAt00);
    return !Read(mouse->previousAt30[1])&&Read(mouse->pressedAt10[1])&&!Read(mouse->suppressionAt28);
}
}
}

namespace KaGuYa::Start::Runtime
{
void UpdateMovieSceneNativeAt4B9520(MovieSceneNativeBody0Ch& body,MovieSceneNativeServices& services)
{
    using namespace MovieSceneNativeDetail;
    MovieSceneNativeScope scope(services);auto& source=services.source;
    auto* movie=Movie(source,0x4B9529,0x4B952E,0x4B9533);
    if(Read(movie->stateAt2C)&&Abort(source))
    {
        movie=Movie(source,0x4B9602,0x4B9607,0x4B960D);
        Read(movie->vptrAt00)->finishAt1C(movie);
    }
    movie=Movie(source,0x4B9638,0x4B963D,0x4B9642);
    if(Read(movie->stateAt2C))return;
    RestoreAdvanceInvalidate(body,source,updateCompletion);
    // Advance/Invalidate可更换Root及movie；必须释放随后重新取得的当前槽。
    auto* root=Root(source,0x4B96CC,0x4B96D1);
    source.ReleaseMovieAt4B96D7(root,0x4B96D7);
}
}
