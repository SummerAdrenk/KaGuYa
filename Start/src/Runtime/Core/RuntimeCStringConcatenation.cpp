#include <Runtime/RuntimeCStringConcatenation.h>

#include <cstddef>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class Function>
Function ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    const std::byte* table;
    // 读取原生vptr，保留Clone/GetNil的原始虚槽调用。
    std::memcpy(&table, static_cast<const void*>(manager), sizeof(table));
    Function result;
    std::memcpy(&result, table + offset, sizeof(result));
    return result;
}
}

RuntimeCStringConcatenation::RuntimeCStringConcatenation(RuntimeStringBorrow prefix,
    const char* suffix, std::uint32_t suffixLength, IRuntimeStringManagerSource& strings)
{
    Initialize(prefix, suffix, &suffixLength, strings);
}

RuntimeCStringConcatenation::RuntimeCStringConcatenation(RuntimeStringBorrow prefix,
    const char* suffix, IRuntimeStringManagerSource& strings)
{
    Initialize(prefix, suffix, nullptr, strings);
}

void RuntimeCStringConcatenation::Initialize(RuntimeStringBorrow prefix,
    const char* suffix, const std::uint32_t* explicitSuffixLength, IRuntimeStringManagerSource& strings)
{
    const auto* source = prefix.GetIf<RuntimeCString>();
    using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
    ATL::IAtlStringMgr* manager = nullptr;
    if (source)
    {
        const auto* header = reinterpret_cast<const ATL::CStringData*>(
            reinterpret_cast<std::uintptr_t>(source->GetString()) - sizeof(ATL::CStringData));
        manager = header->pStringMgr;
        // 4667A2的Clone可抛；不能经过v143 GetManager()的throw()边界。
        if (manager) manager = ManagerSlot<Clone>(manager, 16)(manager);
    }
    if (!manager)
    {
        auto* global = strings.CaptureAt417360();
        // 原入口在空全局manager处会非法解引用；现代安全边界显式拒绝，不造替代manager。
        if (!global) throw ATL::CAtlException(E_FAIL);
        manager = ManagerSlot<Clone>(global, 16)(global);
        if (!manager) throw ATL::CAtlException(E_FAIL);
    }
    using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    auto* const nil = ManagerSlot<GetNil>(manager, 12)(manager);
    // 4667BF返回后才写输出并取得清理资格；不经过ATL构造，也不替换空nil返回。
    const auto chars = reinterpret_cast<std::uintptr_t>(nil) + sizeof(ATL::CStringData);
    std::memcpy(static_cast<void*>(&storage_.value), &chars, sizeof(chars));
    struct ConstructionCleanup
    {
        RuntimeCString* value;
        ~ConstructionCleanup() noexcept { if (value) DestroyRuntimeCStringAt40CD40(value); }
    } cleanup{&storage_.value};
    // Clone/GetNil回调可能改写同一个路径对象，原调用随后重新读取字符指针和长度。
    const auto suffixLength = explicitSuffixLength ? *explicitSuffixLength :
        suffix ? static_cast<std::uint32_t>(std::strlen(suffix)) : 0u;
    const auto current = prefix.View();
    ConcatenateRuntimeCStringAt4145E0(storage_.value, current.data(),
        static_cast<std::uint32_t>(current.size()), suffix, suffixLength);
    cleanup.value = nullptr;
}

RuntimeCStringConcatenation::~RuntimeCStringConcatenation() noexcept(false)
{
    DestroyRuntimeCStringAt40CD40(&storage_.value);
}
void ConstructRuntimeCStringPrefixAt4CA280(RuntimeCString* out,const char* prefix,
    const RuntimeCString& suffix,IRuntimeStringManagerSource& strings)
{
    const auto* header=reinterpret_cast<const ATL::CStringData*>(suffix.GetString())-1;
    using Clone=ATL::IAtlStringMgr*(__thiscall*)(ATL::IAtlStringMgr*);
    auto* manager=header->pStringMgr;
    if(manager)manager=ManagerSlot<Clone>(manager,16)(manager);
    if(!manager)
    {
        auto* global=strings.CaptureAt417360();
        manager=ManagerSlot<Clone>(global,16)(global);
        if(!manager)throw ATL::CAtlException(E_FAIL);
    }
    using GetNil=ATL::CStringData*(__thiscall*)(ATL::IAtlStringMgr*);
    auto* nil=ManagerSlot<GetNil>(manager,12)(manager);
    const auto chars=reinterpret_cast<std::uintptr_t>(nil)+sizeof(ATL::CStringData);
    // GetNil引用只按原生指针发布；后续cleanup的取得及撤销时点不变。
    std::memcpy(static_cast<void*>(out),&chars,sizeof(chars));
    struct Unwind final{RuntimeCString* value;~Unwind() noexcept{if(value)DestroyRuntimeCStringAt40CD40(value);}} cleanup{out};
    const char* second=suffix.GetString();
    const auto firstLength=prefix?std::uint32_t(std::strlen(prefix)):0u;
    const auto secondLength=std::uint32_t((reinterpret_cast<const ATL::CStringData*>(second)-1)->nDataLength);
    ConcatenateRuntimeCStringAt4145E0(*out,prefix,firstLength,second,secondLength);cleanup.value=nullptr;
}
}
