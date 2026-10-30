#include <Runtime/SaveFileDeleteFactoryNative.h>

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <Runtime/SaveLoadResourceProfile.h>

#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
union TextSlot
{
    TextSlot() noexcept {}
    ~TextSlot() noexcept {}
    RuntimeCString value;
};
ATL::CStringData* Header(const char* chars) noexcept
{
    return reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(chars) - sizeof(ATL::CStringData));
}
struct TextUnwind final
{
    RuntimeCString& value;
    bool active = true;
    // 仅展开时调用；再次抛出终止。正常 Finish 先撤销资格，再允许首次 Free 异常外传。
    ~TextUnwind() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&value); }
    void Disarm() noexcept { active = false; }
    void Finish()
    {
        Disarm();
        DestroyRuntimeCStringAt40CD40(&value);
    }
};
struct FactoryUnwind final
{
    SaveFileDeleteFactoryNativeBody0Ch& body;
    bool active = true;
    ~FactoryUnwind() noexcept { if (active) DestroySaveFileDeleteFactoryNativeAt463A90(body); }
};

void ConstructAt45B800(SaveFileDeleteFactoryNativeBody0Ch& body,
    ISaveFileDeleteFactoryNativeSource& source)
{
    body.vptrAt00 = &BaseSaveFileDeleteFactoryNativeVtable();
    InitializeEmptyRuntimeCString(body.prefixAt04, source.Strings());
    FactoryUnwind unwind{body};
    body.vptrAt00 = &RegularSaveFileDeleteFactoryNativeVtable();
    body.slotCountAt08 = 0;
    SetRuntimeCStringAt437680(body.prefixAt04, "save", 4);
    unwind.active = false;
}

void ConstructName(RuntimeCString& output, const SaveFileDeleteFactoryNativeBody0Ch& body,
    std::int32_t slot, const char* format, ISaveFileDeleteFactoryNativeSource& source)
{
    InitializeEmptyRuntimeCString(output, source.Strings());
    TextUnwind unwind{output};
    // GetNil 成功后才读 prefix chars；格式化两遍均沿用该实参指针。
    FormatRuntimeCStringAt417130(output, format, body.prefixAt04.GetString(), slot);
    unwind.Disarm();
}

RuntimeCString& CaptureDirectory(ISaveFileDeleteFactoryNativeSource& source)
{
    auto* const root = source.CaptureEnvironmentAt4180C0();
    auto* const paths = ResolveEnvironmentObjectReference<std::byte>(root->pathsAt48);
    const auto address = reinterpret_cast<std::uintptr_t>(paths) + 0x10u;
    // 仅借用：原 476460/5022E0 不对 pathsAt48 增加引用或执行释放。
    return *reinterpret_cast<RuntimeCString*>(address);
}

void AssignFilePath(RuntimeCString& output, const SaveFileDeleteFactoryNativeBody0Ch& body,
    std::int32_t slot, bool preview, ISaveFileDeleteFactoryNativeSource& source)
{
    TextSlot name;
    if (preview) ConstructSaveFileFactoryPreviewNameNativeAt47AFA0(name.value, body, slot, source);
    else ConstructSaveFileFactoryDataNameNativeAt47AF10(name.value, body, slot, source);
    TextUnwind nameUnwind{name.value};
    const char* const suffix = name.value.GetString();
    auto& directory = CaptureDirectory(source);
    {
        // 466760 与 4C7210 的 CString/char* 合法域核心已证等价；非法域残余见工作文档。
        RuntimeCStringConcatenation path(directory, suffix, source.Strings());
        AssignRuntimeCStringAt410220(output, path.Value());
    }
    nameUnwind.Finish();
}

void* DeleteSized(SaveFileDeleteFactoryNativeBody0Ch& body, std::uint32_t flags,
    std::size_t bytes, IEnvironmentRootMemory& memory)
{
    void* const result = &body;
    DestroySaveFileDeleteFactoryNativeAt463A90(body);
    if ((flags & 1u) != 0) memory.DeleteBody(result, bytes);
    return result;
}
void* __fastcall DeleteBase(SaveFileDeleteFactoryNativeBody0Ch* body, void*, std::uint32_t flags)
{ return DeleteBaseSaveFileDeleteFactoryNativeAt469960(*body, flags); }
void* __fastcall DeleteRegular(SaveFileDeleteFactoryNativeBody0Ch* body, void*, std::uint32_t flags)
{ return DeleteRegularSaveFileDeleteFactoryNativeAt469100(*body, flags); }
void* __fastcall DeleteQuick(SaveFileDeleteFactoryNativeBody0Ch* body, void*, std::uint32_t flags)
{ return DeleteQuickSaveFileDeleteFactoryNativeAt469030(*body, flags); }
void* __fastcall DeleteAuto(SaveFileDeleteFactoryNativeBody0Ch* body, void*, std::uint32_t flags)
{ return DeleteAutoSaveFileDeleteFactoryNativeAt4686D0(*body, flags); }
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 void* 仅占 EDX、不读取其输入。
// flags 仍为同一 4 字节栈实参并由被调方清栈；void* 返回及可抛异常约定一致。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const SaveFileDeleteFactoryNativeVtable1 baseTable{
    reinterpret_cast<decltype(SaveFileDeleteFactoryNativeVtable1::deletingDestructorAt00)>(&DeleteBase)};
const SaveFileDeleteFactoryNativeVtable1 regularTable{
    reinterpret_cast<decltype(SaveFileDeleteFactoryNativeVtable1::deletingDestructorAt00)>(&DeleteRegular)};
const SaveFileDeleteFactoryNativeVtable1 quickTable{
    reinterpret_cast<decltype(SaveFileDeleteFactoryNativeVtable1::deletingDestructorAt00)>(&DeleteQuick)};
const SaveFileDeleteFactoryNativeVtable1 autoTable{
    reinterpret_cast<decltype(SaveFileDeleteFactoryNativeVtable1::deletingDestructorAt00)>(&DeleteAuto)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

const SaveFileDeleteFactoryNativeVtable1& BaseSaveFileDeleteFactoryNativeVtable() noexcept { return baseTable; }
const SaveFileDeleteFactoryNativeVtable1& RegularSaveFileDeleteFactoryNativeVtable() noexcept { return regularTable; }
const SaveFileDeleteFactoryNativeVtable1& QuickSaveFileDeleteFactoryNativeVtable() noexcept { return quickTable; }
const SaveFileDeleteFactoryNativeVtable1& AutoSaveFileDeleteFactoryNativeVtable() noexcept { return autoTable; }

void ConstructRegularSaveFileDeleteFactoryNativeAt4DCDB0(SaveFileDeleteFactoryNativeBody0Ch& body,
    std::int32_t page, ISaveFileDeleteFactoryNativeSource& source)
{
    body.vptrAt00 = &BaseSaveFileDeleteFactoryNativeVtable();
    InitializeEmptyRuntimeCString(body.prefixAt04, source.Strings());
    FactoryUnwind bodyUnwind{body};
    body.vptrAt00 = &RegularSaveFileDeleteFactoryNativeVtable();
    body.slotCountAt08 = 0;
    TextSlot formatted;
    InitializeEmptyRuntimeCString(formatted.value, source.Strings());
    TextUnwind formattedUnwind{formatted.value};
    FormatRuntimeCStringAt417130(formatted.value, "save%04d_", page);
    const char* const chars = formatted.value.GetString();
    const int length = chars ? static_cast<int>(std::strlen(chars)) : 0;
    SetRuntimeCStringAt437680(body.prefixAt04, chars, length);
    // 4DCE40 在 Set 前缓存 ESI：EH 读取当前槽，正常释放使用缓存 header，不重试临时串。
    formattedUnwind.Disarm();
    ReleaseRuntimeCStringHeader(Header(chars));
    bodyUnwind.active = false;
}

void ConstructRollingSaveFileDeleteFactoryNativeAt45B800(
    SaveFileDeleteFactoryNativeRollingBody10h& body, bool automatic,
    ISaveFileDeleteFactoryNativeSource& source)
{
    ConstructAt45B800(body.base, source);
    FactoryUnwind unwind{body.base};
    body.base.vptrAt00 = automatic ? &AutoSaveFileDeleteFactoryNativeVtable() :
        &QuickSaveFileDeleteFactoryNativeVtable();
    body.previewAt0C = 1;
    body.base.slotCountAt08 = CurrentSaveLoadResourceProfile().catalog.slotsPerPage;
    SetRuntimeCStringAt437680(body.base.prefixAt04, automatic ? "asave" : "qsave", 5);
    unwind.active = false;
}

void DestroySaveFileDeleteFactoryNativeAt463A90(SaveFileDeleteFactoryNativeBody0Ch& body)
{
    auto* const header = Header(body.prefixAt04.GetString());
    body.vptrAt00 = &BaseSaveFileDeleteFactoryNativeVtable();
    ReleaseRuntimeCStringHeader(header);
}

void ConstructSaveFileFactoryDataNameNativeAt47AF10(RuntimeCString& output,
    const SaveFileDeleteFactoryNativeBody0Ch& body, std::int32_t slot,
    ISaveFileDeleteFactoryNativeSource& source)
{ ConstructName(output, body, slot, "%s%02d.dat", source); }

void ConstructSaveFileFactoryPreviewNameNativeAt47AFA0(RuntimeCString& output,
    const SaveFileDeleteFactoryNativeBody0Ch& body, std::int32_t slot,
    ISaveFileDeleteFactoryNativeSource& source)
{ ConstructName(output, body, slot, "%s%02d.bmp", source); }

void DeleteSaveFileFactorySlotNativeAt476460(SaveFileDeleteFactoryNativeBody0Ch& body,
    std::int32_t slot, std::uint32_t deleteData, ISaveFileDeleteFactoryNativeSource& source)
{
    TextSlot path;
    InitializeEmptyRuntimeCString(path.value, source.Strings());
    TextUnwind pathUnwind{path.value};
    if (static_cast<std::uint8_t>(deleteData) != 0)
    {
        AssignFilePath(path.value, body, slot, false, source);
        static_cast<void>(source.DeleteFileA(path.value.GetString()));
    }
    AssignFilePath(path.value, body, slot, true, source);
    const char* const chars = path.value.GetString();
    static_cast<void>(source.DeleteFileA(chars));
    pathUnwind.Disarm();
    ReleaseRuntimeCStringHeader(Header(chars));
}

void DeleteSaveFileFactoryAllNativeAt5022E0(SaveFileDeleteFactoryNativeBody0Ch& body,
    std::int32_t* latest, ISaveFileDeleteFactoryNativeSource& source)
{
    *latest = -1;
    if (body.slotCountAt08 <= 0) return;
    std::int32_t slot = 1;
    do
    {
        TextSlot path;
        InitializeEmptyRuntimeCString(path.value, source.Strings());
        TextUnwind pathUnwind{path.value};
        AssignFilePath(path.value, body, slot, false, source);
        static_cast<void>(source.DeleteFileA(path.value.GetString()));
        AssignFilePath(path.value, body, slot, true, source);
        const char* const chars = path.value.GetString();
        static_cast<void>(source.DeleteFileA(chars));
        pathUnwind.Disarm();
        ReleaseRuntimeCStringHeader(Header(chars));
        slot = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(slot) + 1u);
    } while (slot <= body.slotCountAt08);
}

void* DeleteBaseSaveFileDeleteFactoryNativeAt469960(SaveFileDeleteFactoryNativeBody0Ch& body,
    std::uint32_t flags, IEnvironmentRootMemory& memory)
{ return DeleteSized(body, flags, 8, memory); }
void* DeleteRegularSaveFileDeleteFactoryNativeAt469100(SaveFileDeleteFactoryNativeBody0Ch& body,
    std::uint32_t flags, IEnvironmentRootMemory& memory)
{ return DeleteSized(body, flags, 12, memory); }
void* DeleteQuickSaveFileDeleteFactoryNativeAt469030(SaveFileDeleteFactoryNativeBody0Ch& body,
    std::uint32_t flags, IEnvironmentRootMemory& memory)
{ return DeleteSized(body, flags, 16, memory); }
void* DeleteAutoSaveFileDeleteFactoryNativeAt4686D0(SaveFileDeleteFactoryNativeBody0Ch& body,
    std::uint32_t flags, IEnvironmentRootMemory& memory)
{ return DeleteSized(body, flags, 16, memory); }
}
