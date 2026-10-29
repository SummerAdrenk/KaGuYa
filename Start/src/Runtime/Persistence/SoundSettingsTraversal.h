#pragma once
#include <Runtime/SoundParameterNative.h>
#include <Runtime/SettingTagErrorRuntime.h>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime::SoundSettingsTraversal
{
using Slot = YtlReferenceSlotRuntime;
inline const ParamsSettingTagRuntime& RequireNode(const Slot& slot)
{
    const auto* node = ResolveParamsSettingTagAt410560(slot);
    if (!node) throw MissingSettingsRoot{};
    return *node;
}
template<class T> T& Body(const Slot& slot)
{
    auto* body = ResolveEnvironmentObjectReference<T>(slot);
    if (!body) throw std::invalid_argument("sound native group has no payload (safe boundary)");
    return *body;
}
inline bool Same(const RuntimeCString& text, const char* key)
{
    if (!key) throw ATL::CAtlException(E_FAIL);
    return _mbscmp(reinterpret_cast<const unsigned char*>(text.GetString()),
        reinterpret_cast<const unsigned char*>(key)) == 0;
}
inline SettingTagNodeCapture NextChild(const Slot& parent, Slot*& cursor, const char* name)
{
    // 47D340：无匹配返回空Ref，不抛missing；仅外层47D2E0重置游标/清空错误。
    while (cursor != RequireNode(parent).childrenAt10.end)
    {
        if (Same(RequireNode(*cursor).nameAt00, name))
        {
            SettingTagNodeCapture result(*cursor);
            ++cursor;
            return result;
        }
        ++cursor;
    }
    return {};
}
inline SettingTagNodeCapture FirstChild(RuntimeCString* error, const Slot& parent,
    Slot*& cursor, const char* name)
{
    if (!error) _invalid_parameter_noinfo_noreturn();
    SetRuntimeCStringAt437680(*error, "", 0);
    cursor = RequireNode(parent).childrenAt10.begin;
    return NextChild(parent, cursor, name);
}
inline void AdvanceChild(SettingTagNodeCapture& selected, const Slot& parent, Slot*& cursor,
    const char* name, ISettingTagErrorSource& errors)
{
    // 47573B/475B99只捕获error owner，不清空；4757E3/475C32先释放旧selected。
    (void)errors.CaptureAt47BCF0();
    auto next = NextChild(parent, cursor, name);
    selected = {};
    selected = SettingTagNodeCapture(next.Native());
}

// 调用点决定默认值写入时机和catch范围。
bool ReadBool(RuntimeCString* error, const Slot& tag, const char* key);
std::uint32_t ReadDword(RuntimeCString* error, const Slot& tag, const char* key);
}
