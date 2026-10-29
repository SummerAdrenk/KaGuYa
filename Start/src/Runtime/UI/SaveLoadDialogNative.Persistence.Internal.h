#pragma once
#include <Runtime/SaveLoadDialogNativePersistence.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime::SaveLoadPersistenceDetail
{
using Slot = YtlReferenceSlotRuntime;
union TextSlot { RuntimeCString value; TextSlot() noexcept {} ~TextSlot() noexcept {} };
struct TextUnwind final
{
    RuntimeCString& value;
    bool armed = true;
    ~TextUnwind() noexcept { if (armed) DestroyRuntimeCStringAt40CD40(&value); }
    void Finish()
    { armed = false; DestroyRuntimeCStringAt40CD40(&value); }
    void FinishCached(const char* chars)
    { armed = false; ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(chars)) - 1); }
};
struct ReferenceUnwind final
{
    Slot& slot;
    bool armed = true;
    ~ReferenceUnwind() noexcept { if (armed) ReleaseYtlReference(slot); }
    void FinishClearing() noexcept { armed = false; ReleaseYtlReference(slot); }
    void FinishUncleared() noexcept
    {
        armed = false;
        auto* control = slot.controlAt08;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        }
    }
    void FinishCached(YtlReferenceControlRuntime* control) noexcept
    {
        armed = false;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }
};
struct Format final
{
    Slot name, voices, body;
    bool armed = true;
    MessageFormatSelectionRawTarget Target() noexcept { return {name, voices, body}; }
    void Finish() noexcept
    {
        armed = false;
        // 4100C0：body/voices/name，各槽正常释放后重读control，整个析构为终止边界。
        ReleaseYtlReference(body); ReleaseYtlReference(voices); ReleaseYtlReference(name);
    }
    ~Format() noexcept { if (armed) Finish(); }
};
inline std::int32_t Increment(std::int32_t value) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(value) + 1u); }
inline void AssignChars(RuntimeCString& target, const char* source)
{ SetRuntimeCStringAt437680(target, source, source ? int(std::strlen(source)) : 0); }
inline std::int32_t& PageIndex(GuiDialogNativeBody5Ch* page) noexcept
{ return *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(page) + 0x7C); }
inline void NotifyPage(GuiDialogNativeBody5Ch* page)
{
    using Call = void (__thiscall*)(GuiDialogNativeBody5Ch*, std::int32_t, std::int32_t, std::int32_t);
    const auto* table = *reinterpret_cast<const std::byte* const*>(page);
    const auto call = *reinterpret_cast<const Call*>(table + 0x8C);
    const auto previous = PageIndex(page); PageIndex(page) = -1;
    call(page, previous, 1, 0);
}
inline RuntimeCString* ReadMessageText(void* text, RuntimeCString* fresh)
{
    using Read = RuntimeCString* (__thiscall*)(void*, RuntimeCString*);
    const auto* table = *reinterpret_cast<const std::byte* const*>(text);
    return (*reinterpret_cast<const Read*>(table + 4))(text, fresh);
}
// 两条MessageBox原CP932字节来自5C0C7C/5C0CA8，不能借本机ACP重新编码。
inline constexpr char saveFailure[] = "\x83\x47\x83\x89\x81\x5B \x81\x46 \x83\x5A\x81\x5B\x83\x75\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD!!";
inline constexpr char loadFailure[] = "\x83\x47\x83\x89\x81\x5B \x81\x46 \x83\x8D\x81\x5B\x83\x68\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD!!";
}
