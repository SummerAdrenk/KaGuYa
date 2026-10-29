#pragma once
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 原4C3630：按当前vptr先+08再+0C；更新回调可以替换vptr，故第二次必须重取。
std::uint8_t __fastcall RunSceneTaskNativeAt4C3630(void*,void*,std::int32_t);
// 原4B5E20是单条C3；空Draw也共用同一入口，不改变EAX或清额外栈。
void __fastcall DrawSceneTaskNativeAt4B5E20(void*,void*);
}
