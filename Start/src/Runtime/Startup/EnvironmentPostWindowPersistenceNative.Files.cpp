#include "EnvironmentPostWindowPersistenceNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime::EnvironmentPostWindowPersistenceNativeDetail
{
namespace
{
union StreamStorage final
{
    FileStreamRuntime stream;
    StreamStorage() noexcept { ::new (&stream) FileStreamRuntime; }
    ~StreamStorage() noexcept {}
};
struct StreamUnwind final
{
    StreamStorage& storage;
    IStartFileApi& files;
    bool active = true;
    ~StreamUnwind() noexcept { if (active) DestroyFileStreamAt40ED70(&storage.stream, files); }
    void Normal() noexcept
    { active = false; DestroyFileStreamAt40ED70(&storage.stream, files); }
};
}

int LoadFileAt480550(void* serializer, const char* path, Services& services)
{
    StreamStorage storage;
    SECURITY_ATTRIBUTES attributes{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
    // 原新建CAtlFile的事务槽为null；非空context仅转发既有显式FileStream注入，不作失败兜底。
    storage.stream.OpenWithApi({path, GENERIC_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, &attributes},
        services.files, services.transaction);
    StreamUnwind unwind{storage, services.files}; // 4805CE：Open返回后才取得stream资格。
    int result = 0;
    if (storage.stream.fileAt04.baseAt00)
    {
        const U count = static_cast<U>(storage.stream.SizeWithApi(services.files));
        if (count)
        {
            auto* buffer = Slot<decltype(NativeISerializeVtable29::bufferAt14)>(serializer, 0x14)(serializer);
            ResizeNativeSerialize18ByteVectorAt4570D0(buffer, count, buffer);
            buffer = Slot<decltype(NativeISerializeVtable29::bufferAt14)>(serializer, 0x14)(serializer);
            result = storage.stream.ReadExactWithApi(buffer->begin, count, services.files);
        }
    }
    unwind.Normal(); // 48060B/48062B：在后续归档Capture之前销毁流。
    return result == 0 ? 1 : 0; // 空file/未打开/size0均失败，不改mode、不补字节。
}
}
