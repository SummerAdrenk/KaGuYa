#include <Runtime/DrawSpriteView.h>
#include <Runtime/SpriteRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
const DrawSpriteViewMethods methods{
    [](void* p){return static_cast<std::int32_t>(static_cast<SpriteRuntime*>(p)->LogicalName().size());},
    [](void* p){return static_cast<std::uint8_t>(static_cast<SpriteRuntime*>(p)->AnimationState().repeatCommands);},
    [](void* p){return static_cast<std::uint8_t>(static_cast<SpriteRuntime*>(p)->AnimationState().stateFlags);},
    [](void* p){return static_cast<SpriteRuntime*>(p)->AnimationState().resultCode;},
    [](void* p){static_cast<SpriteRuntime*>(p)->FinishNonRepeatingAnimationAt4C4FC0();}};
}
DrawSpriteView::DrawSpriteView(SpriteRuntime* body) noexcept:body_(body),methods_(&methods){}
}
