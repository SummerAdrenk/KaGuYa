#include <Runtime/SaveFileFactoryNativeIo.h>

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RuntimeCStringConcatenation.h>

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
struct TextUnwind final
{
    RuntimeCString& value;
    bool active = true;
    // 展开时第二次抛出终止；正常 Finish 先撤资格，首次 Free 异常交给入口 catch。
    ~TextUnwind() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&value); }
    void Finish()
    {
        active = false;
        DestroyRuntimeCStringAt40CD40(&value);
    }
};
}

std::int32_t SaveFileFactoryNativeAt488CF0(SaveFileDeleteFactoryNativeBody0Ch& body,
    std::int32_t slot, std::int32_t mode, ISaveFileFactoryNativeIoSource& source)
{
    try
    {
        TextSlot dataName;
        ConstructSaveFileFactoryDataNameNativeAt47AF10(dataName.value, body, slot, source);
        TextUnwind dataUnwind{dataName.value};
        const char* const dataChars = dataName.value.GetString();
        source.EnsureRootAt413BE0();
        void* const params = source.ParamsAt47BDB0(source.RootAt6198C4());
        const bool writeFailed = static_cast<std::uint8_t>(
            source.WriteContainerAt4C7B10(params, dataChars)) == 0;
        // 488D59 回到 state0；先释放 dat 名称，再按已锁存 AL 决定是否 throw 1。
        dataUnwind.Finish();
        if (writeFailed) throw 1;

        TextSlot previewName;
        ConstructSaveFileFactoryPreviewNameNativeAt47AFA0(previewName.value, body, slot, source);
        TextUnwind previewUnwind{previewName.value};
        const char* const previewChars = previewName.value.GetString();
        auto* const environment = source.CaptureEnvironmentAt4180C0();
        auto* const paths = ResolveEnvironmentObjectReference<std::byte>(environment->pathsAt48);
        auto& directory = *reinterpret_cast<RuntimeCString*>(
            reinterpret_cast<std::uintptr_t>(paths) + 0x10u);
        // 复用 466760 核：借用目录，Clone/GetNil 后晚读；不持有环境 Ref。
        RuntimeCStringConcatenation previewPath(directory, previewChars, source.Strings());
        // state3 -> state4：完整路径已构造，正常释放名称先撤资格，不再重试名称。
        previewUnwind.Finish();
        if (mode == 0)
        {
            // 原入口先生成完整 BMP 路径，再独立重取目录并删除 BMP；FALSE 被删除核忽略。
            DeleteSaveFileFactorySlotNativeAt476460(body, slot, 0, source);
        }
        else
        {
            if (mode == 2)
            {
                source.EnsureRootAt413BE0();
                source.CapturePreviewAt48DBE0(source.RootAt6198C4());
            }
            // 路径在 Capture 前已固定；保存前仍独立 Ensure/读取当时的 Root。
            const char* const pathChars = previewPath.GetString();
            source.EnsureRootAt413BE0();
            if (static_cast<std::uint8_t>(
                source.SavePreviewAt489420(source.RootAt6198C4(), pathChars)) == 0)
                throw 3;
        }
        // concat 的既有 noexcept(false) 析构承担正常释放；展开时再抛出由 C++ 终止。
        // 正常释放抛出时该对象已不具再次析构资格，且仍处于本 try 内。
        return 0;
    }
    catch (int code) { return code; }
    catch (...) { return -1; }
}

std::int32_t LoadFileFactoryNativeAt480830(SaveFileDeleteFactoryNativeBody0Ch& body,
    std::int32_t slot, ISaveFileFactoryNativeIoSource& source)
{
    try
    {
        TextSlot dataName;
        ConstructSaveFileFactoryDataNameNativeAt47AF10(dataName.value, body, slot, source);
        TextUnwind dataUnwind{dataName.value};
        const char* const chars = dataName.value.GetString();
        source.EnsureRootAt413BE0();
        void* const params = source.ParamsAt47BDB0(source.RootAt6198C4());
        const bool readFailed = static_cast<std::uint8_t>(
            source.ReadContainerAt4C7420(params, chars, 1u)) == 0;
        dataUnwind.Finish();
        if (readFailed) throw 1;
        return 0;
    }
    catch (int code) { return code; }
    catch (...) { return -1; }
}
}
