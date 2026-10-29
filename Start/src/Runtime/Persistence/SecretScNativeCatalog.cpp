#include <Runtime/SecretScNativeCatalog.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/ParamsThumbnailCatalogRuntime.h>
#include <bit>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Group = ParamsRegistSceneGroupRuntime;
using Item = ParamsRegistSceneItemRuntime;
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
template<class T> T* Advance(T* pointer, int index) noexcept
{ return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(pointer) + std::uint32_t(index) * sizeof(T)); }
template<class T> T* Resolve(const Slot& reference) noexcept
{
    const auto base = Read(reference.baseAt00);
    if (!base) return nullptr;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(reference.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + Read(reference.biasAt04) * stride);
}
int Add(int a, int b) noexcept
{ return std::bit_cast<int>(std::uint32_t(a) + std::uint32_t(b)); }
int RefCount(const EnvironmentVectorStorage<Slot>& value) noexcept
{
    auto* last = Read(value.end);
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(last) -
        reinterpret_cast<std::uintptr_t>(Read(value.begin)));
    return std::bit_cast<int>(bytes) / 12;
}
const ParamsGameSystem& Capture(unsigned site)
{
    const auto* root = static_cast<const ParamsRuntimeRoot*>(CaptureBoundSecretCgParamsAt418480(site));
    // 原直接解Ref，不把null正文改成可捕获异常或成功空表。
    return *Resolve<ParamsGameSystem>(root->gameSystemAt04.Native());
}
Group* Category(const ParamsGameSystem& system, int category) noexcept
{ return Resolve<Group>(*Advance(Read(system.registScene.storage.begin),category)); }
Item* Selected(const ParamsGameSystem& system, int category, int item) noexcept
{ return Resolve<Item>(*Advance(Read(Category(system,category)->itemsAt04.storage.begin),item)); }
int FindScene(const ParamsGameSystem& system, const char* query)
{
    const auto& names = system.sceneNames.storage;
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(names.end)) -
        reinterpret_cast<std::uintptr_t>(Read(names.begin)));
    // 原SAR2；负数不是C++除4的向零取整。空/负范围不检查query。
    if ((std::bit_cast<int>(bytes) >> 2) <= 0) return -1;
    int index = 0;
    for (;;)
    {
        if (!query) throw ATL::CAtlException(E_FAIL);
        auto* name = Advance(Read(names.begin),index);
        if (_mbscmp(reinterpret_cast<const unsigned char*>(name->GetString()),
            reinterpret_cast<const unsigned char*>(query)) == 0) return index;
        index = Add(index,1);
        const auto currentBytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(names.end)) -
            reinterpret_cast<std::uintptr_t>(Read(names.begin)));
        if (index >= (std::bit_cast<int>(currentBytes) >> 2)) return -1;
    }
}
bool Enabled(const SecretScArchive& self, int index) noexcept
{
    // 原直接读this+0C位表，不检查ready/count，也不走+0C虚槽。
    auto* words = Read(self.bitsAt0C.beginAt00);
    const auto word = Read(*Advance(words,int(std::uint32_t(index) >> 5)));
    return (word & (1u << (std::uint32_t(index) & 31u))) != 0;
}
int Percentage(int enabled, int total) noexcept
{
    const auto numerator = std::bit_cast<int>(std::uint32_t(enabled) * 100u);
    const auto result = numerator / (total ? total : 1);
    return result == 0 && enabled ? 1 : result;
}
const char* const* ThumbnailAlias(const ParamsGameSystem& system, const char* name)
{
    Slot temporary;
    // 477800返回前没有此局部Ref展开资格，不加RAII或异常回滚。
    static_cast<void>(FindParamsThumbnailReferenceAt477800(system.thumbnails,temporary,name));
    const auto base = Read(temporary.baseAt00);
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(temporary.controlAt08));
    const char* const* result = nullptr;
    if (base)
    {
        const auto stride = control ? Read(control->strideAt0C) : 0u;
        result = reinterpret_cast<const char* const*>(base + Read(temporary.biasAt04) * stride + 4u);
    }
    // 478655 state2 -> 541E55：正常尾缓存control，payload后仍删同一control。
    [&]() noexcept
    {
        if (control && --control->referenceCountAt04 == 0)
        {
            if (Read(control->owningAt08)) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }();
    return result;
}
}

const char* const* ReadSecretScThumbnailNativeAt478230(const SecretScArchive& self, int category, int item)
{
    if (category >= RefCount(Capture(0x478262).registScene.storage)) return nullptr;
    if (item >= RefCount(Category(Capture(0x4782AF),category)->itemsAt04.storage)) return nullptr;
    auto* cursor = Read(Selected(Capture(0x478328),category,item)->scenesAt00.begin);
    for (;;)
    {
        // 4783A8每轮独立捕获end，不能把整组选定记录/begin/end一起缓存。
        if (cursor == Read(Selected(Capture(0x4783A8),category,item)->scenesAt00.end)) break;
        const auto* name = cursor->GetString();
        const auto index = FindScene(Capture(0x478443),name);
        if (index < 0 || !Enabled(self,index)) return nullptr;
        cursor = Advance(cursor,1);
    }
    const auto* thumbnailName = Selected(Capture(0x47858A),category,item)->nameAt0C.GetString();
    return ThumbnailAlias(Capture(0x478602),thumbnailName);
}
int ReadSecretScPercentCategoryNativeAt47C050(const SecretScArchive& self, int category)
{
    int enabled = 0, total = 0;
    if (category < RefCount(Capture(0x47C082).registScene.storage))
    {
        total = RefCount(Category(Capture(0x47C0D4),category)->itemsAt04.storage);
        for (int item = 0;; item = Add(item,1))
        {
            // 原内联418480以Enter调用站点标识；同源owner叶保留锁/普通-EH资格。
            if (item >= RefCount(Category(Capture(0x47C169),category)->itemsAt04.storage)) break;
            auto* cursor = Read(Selected(Capture(0x47C281),category,item)->scenesAt00.begin);
            bool complete = true;
            for (;;)
            {
                if (cursor == Read(Selected(Capture(0x47C3BB),category,item)->scenesAt00.end)) break;
                const auto* name = cursor->GetString();
                const auto index = FindScene(Capture(0x47C4E3),name);
                if (index < 0 || !Enabled(self,index)) { complete = false; break; }
                cursor = Advance(cursor,1);
            }
            if (complete) enabled = Add(enabled,1);
        }
    }
    return Percentage(enabled,total);
}
int ReadSecretScPercentAllNativeAt47D830(const SecretScArchive& self)
{
    int enabled = 0, total = 0;
    auto* group = Read(Capture(0x47D865).registScene.storage.begin);
    while (group != Read(Capture(0x47D8B3).registScene.storage.end))
    {
        auto* item = Read(Resolve<Group>(*group)->itemsAt04.storage.begin);
        while (item != Read(Resolve<Group>(*group)->itemsAt04.storage.end))
        {
            total = Add(total,1);
            bool complete = true;
            auto* cursor = Read(Resolve<Item>(*item)->scenesAt00.begin);
            while (cursor != Read(Resolve<Item>(*item)->scenesAt00.end))
            {
                const auto* name = cursor->GetString();
                const auto index = FindScene(Capture(0x47DA36),name);
                if (index < 0 || !Enabled(self,index)) { complete = false; break; }
                cursor = Advance(cursor,1);
            }
            if (complete) enabled = Add(enabled,1);
            item = Advance(item,1);
        }
        group = Advance(group,1);
    }
    return Percentage(enabled,total);
}
const char* const* SecretScArchive::ThumbnailAtVslot1C(int category, int item)
{ return ReadSecretScThumbnailNativeAt478230(*this,category,item); }
int SecretScArchive::PercentAllAtVslot20() const
{ return ReadSecretScPercentAllNativeAt47D830(*this); }
int SecretScArchive::PercentCategoryAtVslot24(int category) const
{ return ReadSecretScPercentCategoryNativeAt47C050(*this,category); }
}
