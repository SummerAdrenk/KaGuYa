#include <Runtime/SavePreviewFileNative.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/ArchiveReaderNative.h>
#include <Runtime/FileStreamRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <bit>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 511D90在511F54之前始终state=-1，不能给局部流增加自动展开清理。
union PreviewFileStorage
{
    PreviewFileStorage() noexcept : stream() {}
    ~PreviewFileStorage() noexcept {}
    FileStreamRuntime stream;
};
static_assert(sizeof(PreviewFileStorage) == 0x10);

struct PreviewReaderLocal final
{
    PreviewReaderLocal() noexcept {}
    ~PreviewReaderLocal() noexcept
    {
        // state0 -> 5A6860 -> 40D750：回调后重取槽中的control。
        if (armed) reference.~ArchiveReaderReference();
    }
    void ReleaseNormal(ArchiveReaderReferenceControl* cached) noexcept
    {
        // 511F63改state1；5F2C34 -> 541E55，失败终止且不再展开Ref。
        // 511F6E..86始终使用ESI缓存，正文回调换槽也不改为释放新control。
        armed = false;
        if (cached && --cached->referenceCountAt04 == 0)
        {
            if (cached->owningAt08) cached->DestroyPayloadAtVslot00();
            cached->DeleteControlAtVslot04(1);
        }
    }
    union { ArchiveReaderReference reference; };
    bool armed = false;
};
}

std::uint32_t LoadSavePreviewFileNativeAt511D90(GraphicsSurfaceNativeBody& destination,
    const char* path, std::uint32_t flags, IStartFileApi& files, IGraphicsSurfaceApi& graphics,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    // 此入口没有483D90的state2外块guard；构造/读取/发布异常不得补删reader。
    auto* const raw = memory.AllocateBody(0x14);
    auto* const reader = raw ? ::new(raw) ArchiveFileReaderRuntime(
        ArchiveFileReaderRuntime::InlineBeforeClearTag{}, strings) : nullptr;
    if (reader)
    {
        // 构造已完成，caller仍state=-1；Clear抛出不自动析构基类或回收外块。
        reader->ClearAt4327B0();
        PreviewFileStorage file;
        SECURITY_ATTRIBUTES attributes{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
        static_cast<void>(file.stream.OpenWithApi(
            {path, GENERIC_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, &attributes}, files));
        // 511E5F只测Ref.base，不按42BAE0的返回值或HANDLE有效性门控。
        if (file.stream.fileAt04.baseAt00)
        {
            const auto size = static_cast<std::uint32_t>(file.stream.SizeWithApi(files));
            // 417A80返回EDX:EAX；511E6D只取低DWORD并按signed比较。
            if (std::bit_cast<std::int32_t>(size) >= 1)
            {
                reader->RawBytesAt04().AllocateAt412700(size);
                const auto length = path ? static_cast<std::uint32_t>(std::strlen(path)) : 0u;
                SetRuntimeCStringAt437680(reader->RawNameAt10(), path,
                    std::bit_cast<std::int32_t>(length));
                // 命名回调可改byte Ref；Read目标在此重解，不能缓存分配时指针。
                if (!file.stream.ReadExactWithApi(reader->RawBytesAt04().Data(), size, files))
                    reader->ClearAt4327B0();
            }
        }
        // 511EDD先析构/退表/Close，再511EE6调Ready；返回值被后续xor覆盖。
        DestroyFileStreamAt40ED70(&file.stream, files);
        static_cast<void>(reader->ReadyAt419360());
    }

    ArchiveReaderReferenceSlot slot;
    ArchiveReaderReferenceControl* cached = nullptr;
    if (reader)
    {
        slot.baseAt00 = reinterpret_cast<std::uintptr_t>(reader);
        slot.biasAt04 = 0;
        cached = ConstructArchiveReaderControl(memory.AllocateBody(0x18), reader);
        // 511F2C的null control是原非法写入；不新增bad_alloc/成功fallback。
        cached->referenceCountAt04 = 1;
        cached->owningAt08 = 1;
        cached->strideAt0C = 0x14;
        slot.controlAt08 = cached;
        cached->extentAt10 = 1;
    }
    PreviewReaderLocal local;
    ::new (static_cast<void*>(&local.reference)) ArchiveReaderReference(ArchiveReaderReference::Adopt(slot));
    local.armed = true; // 511F54：仅419D70及之后拥有Ref展开资格。
    const bool result = LoadGraphicsSurfaceArchiveReaderAt419D70(destination, local.reference, graphics, flags);
    local.ReleaseNormal(cached);
    return result ? 1u : 0u; // 原只约定AL；冻结公开入口标准化为完整uint32 0/1。
}
}
