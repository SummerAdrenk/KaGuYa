#pragma once
#include <Runtime/GeneralConfirmDialogNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime::GeneralConfirmNativeDetail
{
using Body = GeneralConfirmDialogNativeBody84h;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
// profile由当前编译游戏提供；不在原正文追加指针或修改Root/modal所有权。
inline const GeneralConfirmResourceProfile& Resources() { return CurrentGeneralConfirmResourceProfile(); }
inline Body& Dialog(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
inline auto* Surface(const Slot& r) noexcept
{ return ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(r); }
inline void Copy(Slot& out, const Slot& source) noexcept
{ CopyGraphicsSurfaceReferenceAt47D740(out, source); }
void ReleaseUncleared(Slot&);
// EH重读槽；正常尾分别按原选点使用缓存control或重读槽，不能混成移动语义。
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
const GeneralConfirmDialogNativeVtable38& ModalBaseTable() noexcept;
void DestroyMembers(Body&) noexcept;
void __fastcall Begin(Common*, void*);
std::int32_t __fastcall Poll(Common*, void*);
std::int32_t __fastcall End(Common*, void*);
std::int32_t __fastcall Result(Common*, void*);
std::int32_t __fastcall ActiveInput(Common*, void*);
void __fastcall ResetInput(Common*, void*);
void __fastcall Flush(Common*, void*);
std::int32_t __fastcall ReadEvents(Common*, void*);
void __fastcall BeginHook(Common*, void*);
void __fastcall EndHook(Common*, void*);
}
