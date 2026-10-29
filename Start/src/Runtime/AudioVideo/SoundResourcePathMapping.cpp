#include <Runtime/SoundResourcePathMapping.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/GameBuildProfile.h>
#include <bit>
#include <cstring>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Byte = unsigned char;
template<class T> T& Body(const Slot& slot)
{
    auto* value = ResolveEnvironmentObjectReference<T>(slot);
    if (!value) throw std::invalid_argument("sound path query has no payload (safe boundary)");
    return *value;
}
ATL::CStringData* Header(const char* text) noexcept
{ return reinterpret_cast<ATL::CStringData*>(reinterpret_cast<std::uintptr_t>(text) - 16u); }
void Publish(RuntimeCString* out, ATL::CStringData* header) noexcept
{
    const auto data = reinterpret_cast<std::uintptr_t>(header) + 16u;
    std::memcpy(out, &data, sizeof(data));
}
template<class Function> Function ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    const std::byte* table;
    std::memcpy(&table, manager, sizeof(table));
    Function function;
    std::memcpy(&function, table + offset, sizeof(function));
    return function;
}
class ReturnCleanup final
{
public:
    explicit ReturnCleanup(RuntimeCString* out) noexcept : out_(out) {}
    ~ReturnCleanup() noexcept { if (out_) DestroyRuntimeCStringAt40CD40(out_); }
    void Dismiss() noexcept { out_ = nullptr; }
private:
    RuntimeCString* out_;
};
void Normalize(RuntimeCString& text, IRuntimeStringManagerSource& strings)
{
    LowerRuntimeCStringAt481690(text);
    if (text.GetLength() < 0) return;
    const auto* begin = reinterpret_cast<const Byte*>(text.GetString());
    const auto* found = _mbsstr(begin, reinterpret_cast<const Byte*>(":\\"));
    if (!found) return;
    const auto index = static_cast<std::int32_t>(found - begin);
    if (index < 0) return;
    auto tail = ArchiveResourceNameMidAt420C80(text, index + 2, text.GetLength() - index - 2, strings);
    AssignRuntimeCStringAt410220(text, tail.Value());
    tail.ReleaseNormal();
}
void AppendByte(RuntimeCString& text, char byte, bool cacheCharacters)
{
    auto* data = const_cast<char*>(text.GetString());
    const int oldLength = Header(data)->nDataLength;
    const auto length = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(oldLength) + 1u);
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    const auto prepare = (1u - static_cast<std::uint32_t>(Header(data)->nRefs)) |
        (static_cast<std::uint32_t>(Header(data)->nAllocLength) - static_cast<std::uint32_t>(length));
    if (std::bit_cast<std::int32_t>(prepare) < 0)
    {
        PrepareRuntimeCStringWriteAt42D590(text, length);
        data = const_cast<char*>(text.GetString());
    }
    // 476DCE的反斜杠使用缓存chars；477050的点及后续写入重读返回槽。
    (cacheCharacters ? data : const_cast<char*>(text.GetString()))[oldLength] = byte;
    auto* header = Header(cacheCharacters ? data : text.GetString());
    if (length > header->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    header->nDataLength = length;
    (cacheCharacters ? data : const_cast<char*>(text.GetString()))[length] = 0;
}
void MapMatch(RuntimeCString& result, const Slot& registration, const Slot& key,
    IRuntimeStringManagerSource& strings)
{
    ArchiveRawResourceNameRuntime file(ArchiveRawResourceNameRuntime::ProducerTag{}, [&](RuntimeCString* out) {
        Publish(out, CloneRuntimeCStringDataAt414540(Header(Body<SoundFileKeyNative>(key).fileAt0C.GetString())));
    });
    if (file.Value().GetLength() == 0)
        AssignRuntimeCStringAt410220(file.Value(), Body<SoundSeNative>(registration).textAt10);
    if (CompiledGameProfile().soundSettings == GameBuildProfile::SoundSettings::Categorized)
    {
        // Baku 45A0E0的试听名规则：空File不变；ext替换后缀；old:new只替换old。
        // 旧表的File不是新版的archive路径，不能给空File拼出“:\\”。
        const std::string expression(file.Value().GetString(), file.Value().GetLength());
        if (!expression.empty())
        {
            std::string name(result.GetString(), result.GetLength());
            const auto colon = expression.find(':');
            const auto dot = name.find_last_of('.');
            const std::string oldExtension = colon == std::string::npos ? "" : expression.substr(0, colon);
            const auto currentExtension = dot == std::string::npos ? name : name.substr(dot + 1);
            if (oldExtension.empty() || _mbsicmp(reinterpret_cast<const Byte*>(oldExtension.c_str()),
                reinterpret_cast<const Byte*>(currentExtension.c_str())) == 0)
            {
                if (dot != std::string::npos) name.resize(dot);
                name += '.';
                name += colon == std::string::npos ? expression : expression.substr(colon + 1);
                SetRuntimeCStringAt437680(result, name.c_str(), static_cast<int>(name.size()));
            }
        }
        file.ReleaseNormal();
        return;
    }
    const auto* cachedFile = file.Value().GetString();
    const auto* colon = _mbsrchr(reinterpret_cast<const Byte*>(cachedFile), ':');
    const auto offset = colon ? static_cast<std::int32_t>(colon - reinterpret_cast<const Byte*>(cachedFile)) : -1;
    if (offset >= 0)
    {
        if (offset == Header(cachedFile)->nDataLength - 1) AppendByte(file.Value(), '\\', true);
    }
    else AppendRuntimeCStringAt413550(file.Value(), ":\\", 2);
    cachedFile = file.Value().GetString();

    // FuncInfo5E7EDC/state3或6：GetNil成功才取得concat临时清理资格。
    // Clone/GetNil之后prefix仍用缓存chars，suffix则重新读返回槽，不复用通用拼接owner。
    ArchiveRawResourceNameRuntime combined(ArchiveRawResourceNameRuntime::ProducerTag{}, [&](RuntimeCString* out) {
        using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
        using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
        auto* manager = Header(cachedFile)->pStringMgr;
        if (manager) manager = ManagerSlot<Clone>(manager, 16)(manager);
        if (!manager)
        {
            manager = strings.CaptureAt417360();
            manager = ManagerSlot<Clone>(manager, 16)(manager);
            if (!manager) throw ATL::CAtlException(E_FAIL);
        }
        Publish(out, ManagerSlot<GetNil>(manager, 12)(manager));
    });
    ConcatenateRuntimeCStringAt4145E0(combined.Value(), cachedFile,
        static_cast<std::uint32_t>(Header(cachedFile)->nDataLength), result.GetString(),
        static_cast<std::uint32_t>(result.GetLength()));
    const auto* cachedCombined = combined.Value().GetString();
    AssignRuntimeCStringAt410220(result, combined.Value());
    combined.ReleaseNormalCached(cachedCombined);
    file.ReleaseNormalCached(cachedFile);

    // 扩展名取源在两个临时Free之后，回调可能改写Key；不缓存早先的扩展名。
    if (_mbscmp(reinterpret_cast<const Byte*>(Body<SoundFileKeyNative>(key).extensionAt08.GetString()),
        reinterpret_cast<const Byte*>("")) == 0) return;
    const auto* dot = _mbsrchr(reinterpret_cast<const Byte*>(result.GetString()), '.');
    if (dot)
    {
        const auto index = static_cast<std::int32_t>(dot - reinterpret_cast<const Byte*>(result.GetString()));
        if (index >= 0)
        {
            auto left = ArchiveResourceNameLeftAt419D00(result, index, strings);
            const auto* cached = left.Value().GetString();
            AssignRuntimeCStringAt410220(result, left.Value());
            left.ReleaseNormalCached(cached);
        }
    }
    AppendByte(result, '.', false);
    const auto& extension = Body<SoundFileKeyNative>(key).extensionAt08;
    AppendRuntimeCStringAt413550(result, extension.GetString(), extension.GetLength());
}
bool MapKeys(RuntimeCString& name, const Slot& registration, IRuntimeStringManagerSource& strings)
{
    bool matched = false;
    for (auto* key = Body<SoundSeNative>(registration).fileKeysAt14.begin;
         key != Body<SoundSeNative>(registration).fileKeysAt14.end && !matched; ++key)
    {
        const auto* prefix = Body<SoundFileKeyNative>(*key).keyAt04.GetString();
        if (!prefix || name.GetLength() < 0) continue;
        const auto* found = _mbsstr(reinterpret_cast<const Byte*>(name.GetString()), reinterpret_cast<const Byte*>(prefix));
        if (!found || found != reinterpret_cast<const Byte*>(name.GetString())) continue;
        MapMatch(name, registration, *key, strings);
        matched = true;
    }
    return matched;
}
}
void ConstructSoundResourcePathAt476A80(RuntimeCString* out, const char* resourceName,
    const Slot& characters, const Slot& types, IRuntimeStringManagerSource& strings)
{
    ConstructArchiveResourceNameAt40B530(out, resourceName, strings);
    ReturnCleanup cleanup(out); // state0/bit1在40B530成功返回后置位。
    Normalize(*out, strings);
    bool matched = false;
    if (characters.baseAt00)
    {
        for (auto* character = Body<SoundCharacterGroupNative>(characters).charactersAt08.begin;
             character != Body<SoundCharacterGroupNative>(characters).charactersAt08.end && !matched; ++character)
        {
            for (auto* registration = Body<SoundCharacterNative>(*character).parametersAt0C.begin;
                 registration != Body<SoundCharacterNative>(*character).parametersAt0C.end && !matched; ++registration)
                matched = MapKeys(*out, *registration, strings);
        }
    }
    if (!matched && types.baseAt00)
    {
        for (auto* registration = Body<SoundTypeGroupNative>(types).parametersAt08.begin;
             registration != Body<SoundTypeGroupNative>(types).parametersAt08.end && !matched; ++registration)
            matched = MapKeys(*out, *registration, strings);
    }
    cleanup.Dismiss();
}
}
