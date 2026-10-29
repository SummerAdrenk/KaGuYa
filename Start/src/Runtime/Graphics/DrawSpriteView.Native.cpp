#include <Runtime/DrawSpriteView.h>
#include <Runtime/SpriteAnimationNativeExecution.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=SpriteNativeBodyA0h;
const DrawSpriteViewMethods methods{
    [](void* p){return static_cast<Body*>(p)->logicalNameAt0C.GetLength();},
    [](void* p){return static_cast<const volatile Body*>(p)->animationAt34.repeatAt10;},
    [](void* p){return *reinterpret_cast<const volatile std::uint8_t*>(&static_cast<Body*>(p)->animationAt34.flagsAt00);},
    [](void* p){return static_cast<const volatile Body*>(p)->animationAt34.resultAt0C;},
    [](void* p){
        auto& body=*static_cast<Body*>(p);
        // 4C540E..4C5433：名字只比较length==0，mode2/非repeat才清index、flags并复制最终映射。
        if(body.logicalNameAt0C.GetLength()==0||body.modeAt04!=2||body.animationAt34.repeatAt10)return;
        body.animationAt34.indexAt08=0;body.animationAt34.flagsAt00=0;
        CopySpriteFinalMappingsNativeAt4C23A0(body.animationAt34);}};
}
DrawSpriteView::DrawSpriteView(Body* body) noexcept:body_(body),methods_(&methods){}
}
