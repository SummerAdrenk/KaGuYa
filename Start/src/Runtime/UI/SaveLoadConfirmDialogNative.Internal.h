#pragma once
#include <Runtime/SaveLoadConfirmDialogNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime::SaveLoadConfirmNativeDetail
{
using Body = SaveLoadConfirmDialogNativeBodyE4h;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
inline Body& Dialog(Common* common) noexcept { return *reinterpret_cast<Body*>(common); }
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) - std::uint32_t(b)); }
inline auto* Surface(const Slot& slot) noexcept
{ return ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slot); }
inline void Copy(Slot& out, const Slot& source) noexcept
{ CopyGraphicsSurfaceReferenceAt47D740(out, source); }
void ReleaseUncleared(Slot&);
// 普通尾可选缓存control或重读槽；EH永远走原Ref析构，不能合并两种协议。
struct RefLifetime final
{
    Slot& slot;
    bool armed = true;
    ~RefLifetime() noexcept { if (armed) ReleaseYtlReference(slot); }
    void Finish(bool clear = false) noexcept;
    void FinishCaptured(YtlReferenceControlRuntime*, bool clear = true) noexcept;
};
struct RawLifetime final
{
    void* raw;
    std::size_t size;
    bool armed = true;
    ~RawLifetime() noexcept { if (armed) ::operator delete(raw, size); }
};
// CString的正常Free可抛；撤销清理资格后再调用，首次异常不重复减持。
struct TextLifetime final
{
    RuntimeCString& value;
    bool armed = true;
    ~TextLifetime() noexcept { if (armed) DestroyRuntimeCStringAt40CD40(&value); }
    void Finish() { armed = false; DestroyRuntimeCStringAt40CD40(&value); }
};
union TextStorage
{
    RuntimeCString value;
    TextStorage() noexcept {}
    ~TextStorage() noexcept {}
};
void ConstructText(TextStorage&, const char*, int);
void TrimLeft(RuntimeCString&);
void TrimRight(RuntimeCString&);
std::int32_t ImmutableDword(std::uint32_t address);
void DestroyMembers(Body&) noexcept;
const GuiDialogNativeVtable35& ModalBaseTable() noexcept;
void __fastcall Begin(Common*, void*);
std::int32_t __fastcall Poll(Common*, void*);
std::int32_t __fastcall End(Common*, void*);
std::int32_t __fastcall ActiveInput(Common*, void*);
void __fastcall ResetInput(Common*, void*);
std::int32_t __fastcall Result(Common*, void*);
void __fastcall Flush(Common*, void*);
std::int32_t __fastcall ReadEvents(Common*, void*);
void __fastcall NullHook(Common*, void*);
void __fastcall Mode(Common*, void*, std::int32_t);
void BeginModalBase(Common*);
std::int32_t EndModalBase(Common*);
}
