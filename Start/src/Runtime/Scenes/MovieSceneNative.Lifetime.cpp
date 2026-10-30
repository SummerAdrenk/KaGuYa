#include "MovieSceneNative.Internal.h"
#include <Runtime/SceneTaskRunNative.h>
#include <exception>
#include <new>
#include <stdexcept>

extern "C" int __cdecl _purecall();
namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace MovieSceneNativeDetail;
thread_local MovieSceneNativeServices* active=nullptr;
void __fastcall Update(Body* body,void*)
{UpdateMovieSceneNativeAt4B9520(*body,RequireMovieSceneNativeServices());}
Body* __fastcall Delete(Body* body,void*,unsigned flags)noexcept
{return DeleteMovieSceneNativeAt4A6EA0(body,flags);}
std::uint8_t __fastcall Pure(Body*,void*,std::int32_t){_purecall();std::terminate();}
Body* __fastcall DeleteTask(Body* body,void*,unsigned flags)noexcept;
struct Retired final
{
    decltype(MovieSceneNativeVtable4::runAt00) run;
    decltype(MovieSceneNativeVtable4::deleteAt04) destroy;
};
static_assert(sizeof(Retired)==8);
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Retired retired{
    reinterpret_cast<decltype(Retired::run)>(&Pure),
    reinterpret_cast<decltype(Retired::destroy)>(&DeleteTask)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const MovieSceneNativeVtable4 methods{
    reinterpret_cast<decltype(MovieSceneNativeVtable4::runAt00)>(&RunSceneTaskNativeAt4C3630),
    reinterpret_cast<decltype(MovieSceneNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(MovieSceneNativeVtable4::updateAt08)>(&Update),
    reinterpret_cast<decltype(MovieSceneNativeVtable4::drawAt0C)>(&DrawSceneTaskNativeAt4B5E20)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
Body* __fastcall DeleteTask(Body* body,void*,unsigned flags)noexcept
{Retire(*body);if(flags&1u)::operator delete(body,std::size_t{4});return body;}
constexpr std::uint32_t table[]{0x4C3630,0x4A6EA0,0x4B9520,0x4B5E20};
constexpr std::uint32_t unwind[]{
    0xFFFFFFFF,0x59CBD0,0,0x59CBD8,1,0x59CBE7,0,0x59CBEF,
    0,0x59CBF7,4,0x59CC10,4,0x59CC18,6,0x59CC20};
const MovieSceneNativeEhEvidence evidence[]{
    {0x4A0B00,0x5F056C,{0x19930522,8,0x5F0590,0,0,0,0,0,1},unwind}};
}

MovieSceneNativeScope::MovieSceneNativeScope(MovieSceneNativeServices& services)noexcept
    :previous_(active),movie_(services.movie){active=&services;}
MovieSceneNativeScope::~MovieSceneNativeScope()noexcept{active=previous_;}
MovieSceneNativeServices& RequireMovieSceneNativeServices()
{if(!active)throw std::logic_error("Movie native scene services are required");return *active;}
const MovieSceneNativeVtable4& MovieSceneNativeMethods()noexcept{return methods;}
namespace MovieSceneNativeDetail
{
void Retire(Body& body)noexcept
{body.vptrAt00=reinterpret_cast<const MovieSceneNativeVtable4*>(&retired);}
}
void DestroyMovieSceneNativeAt4A6EA0(MovieSceneNativeBody0Ch& body)noexcept
{MovieSceneNativeDetail::Retire(body);}
MovieSceneNativeBody0Ch* DeleteMovieSceneNativeAt4A6EA0(MovieSceneNativeBody0Ch* body,unsigned flags)noexcept
{DestroyMovieSceneNativeAt4A6EA0(*body);if(flags&1u)::operator delete(body,std::size_t{0x0C});return body;}
std::span<const std::uint32_t,4> MovieSceneNativeOriginalTableSample18()noexcept{return table;}
std::span<const MovieSceneNativeEhEvidence,1> MovieSceneNativeEhSample18()noexcept{return evidence;}
const char* MovieSceneMissingFileFormatSample18()noexcept
{
    return "\x81\x77 %s \x81\x78\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xAA\x8C\xA9"
        "\x82\xC2\x82\xA9\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49";
}
}
