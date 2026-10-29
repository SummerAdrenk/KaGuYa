#pragma once
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class SpriteRuntime;
struct SpriteNativeBodyA0h;
struct DrawSpriteViewMethods final
{
    std::int32_t(*nameLength)(void*);
    std::uint8_t(*repeat)(void*);
    std::uint8_t(*flags)(void*);
    std::int32_t(*result)(void*);
    void(*finishWait)(void*);
};
// 固定所选Sprite正文，只借方法表；不缓存动画字段、映射或原Ref。
class DrawSpriteView final
{
public:
    DrawSpriteView(SpriteRuntime*) noexcept;
    DrawSpriteView(SpriteNativeBodyA0h*) noexcept;
    const void* Receiver() const noexcept{return body_;}
    std::int32_t LogicalNameLengthAt0C() const{return methods_->nameLength(body_);}
    std::uint8_t RepeatByteAt44() const{return methods_->repeat(body_);}
    std::uint8_t FlagsByteAt34() const{return methods_->flags(body_);}
    std::int32_t ResultAt40() const{return methods_->result(body_);}
    void FinishForWaitAt4C540E() const{methods_->finishWait(body_);}
private:
    void* body_;const DrawSpriteViewMethods* methods_;
};
}
