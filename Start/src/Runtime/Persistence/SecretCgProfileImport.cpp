#include <Runtime/EnvironmentStateArchives.h>

#include <algorithm>
#include <bit>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
const SecretCgFlagGroupSource* Find(const std::vector<SecretCgFlagGroupSource>& groups,
    const std::string& name)
{
    for (const auto& group : groups)
        if (_mbsicmp(reinterpret_cast<const unsigned char*>(group.name.c_str()),
                reinterpret_cast<const unsigned char*>(name.c_str())) == 0) return &group;
    return nullptr;
}

void AppendRecord(SecretCgReferenceVector& records, std::uint32_t id)
{
    auto& memory = DefaultEnvironmentRootMemory();
    auto* record = static_cast<SecretCgFlagRecord*>(::operator new(sizeof(SecretCgFlagRecord)));
    record->idAt00 = id;
    record->byteFlagAt04 = 0;
    YtlReferenceSlotRuntime reference;
    try { AdoptSecretCgFlagAt45C0F0(reference, record, memory); }
    catch (...) { ::operator delete(record, sizeof(SecretCgFlagRecord)); throw; }
    try { AppendSecretCgReferenceAt4551B0(records, reference, memory); }
    catch (...) { ReleaseYtlReference(reference); throw; }
    ReleaseYtlReference(reference);
}
}

SecretCgNativeArchive::SecretCgNativeArchive(const SecretCgConstructionInputs& inputs)
    : readyAt04(false), baseKeyOffsetAt08(0), recordsAt0C{}, mappingsAt18{}
{
    // 显式工具导入：只有这一入口消费值模型。原45BCA0构造不经过该路径。
    auto& memory = DefaultEnvironmentRootMemory();
    try
    {
        for (const auto& name : inputs.selectedFlagGroupNamesAtEC)
            if (const auto* group = Find(inputs.availableFlagGroupsAt1C, name))
                for (auto id : group->flagIds) AppendRecord(recordsAt0C, id);
        ResizeSecretCgMappingVectorAt4577F0(mappingsAt18,
            static_cast<std::uint32_t>(inputs.mappingsAt104.size()),
            DefaultRuntimeStringManagerSource(), memory);
        for (std::size_t category = 0; category < inputs.mappingsAt104.size(); ++category)
        {
            const auto& value = inputs.mappingsAt104[category];
            auto& mapping = mappingsAt18.begin[category];
            SetRuntimeCStringAt437680(mapping.nameAt00, value.name.c_str(), static_cast<int>(value.name.size()));
            for (const auto& listName : value.flagListNames)
            {
                const auto* list = Find(inputs.resolvableFlagListsAt1C, listName);
                YtlReferenceSlotRuntime reference{};
                if (list)
                {
                    const auto count = static_cast<std::uint32_t>(list->flagIds.size());
                    try { AllocateSecretCgIndicesAt45C825(reference, count, memory); }
                    catch (...)
                    {
                        ::operator delete[](reinterpret_cast<void*>(reference.baseAt00));
                        throw;
                    }
                    auto* indices = reinterpret_cast<std::int32_t*>(reference.baseAt00);
                    for (std::uint32_t index = 0; index < count; ++index)
                    {
                        indices[index] = -1;
                        for (std::int32_t flag = 0; flag < CountAtVslot08(); ++flag)
                            if (RecordAt(flag).idAt00 == list->flagIds[index])
                            { indices[index] = flag; break; }
                    }
                }
                try { AppendSecretCgReferenceAt4551B0(mapping.referencesAt04, reference, memory); }
                catch (...) { ReleaseYtlReference(reference); throw; }
                ReleaseYtlReference(reference);
            }
        }
        readyAt04 = true;
    }
    catch (...)
    {
        DestroySecretCgMappingVectorAt461500(mappingsAt18, memory);
        DestroySecretCgReferencesAt461440(recordsAt0C, memory);
        throw;
    }
}
}
