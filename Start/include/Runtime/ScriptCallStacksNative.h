#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/SystemDataEnvironmentArchive.h>

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 5B5BC0：CStack只有Serialize与flags-delete两槽，正文不能退化为值帧。
class ScriptStackFrameNative final : public IEnvironmentArchiveRuntime
{
public:
    ScriptStackFrameNative(std::int32_t scriptId, std::uint32_t returnPc) noexcept;
    void Serialize(SerializeBuffer& archive);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    std::int32_t scriptIdAt04;
    std::uint32_t returnPcAt08;
};
static_assert(sizeof(ScriptStackFrameNative) == 0x0C);
static_assert(offsetof(ScriptStackFrameNative, scriptIdAt04) == 4);
static_assert(offsetof(ScriptStackFrameNative, returnPcAt08) == 8);

// 4CD430..4CD49B：先发布base，control分配失败不补回收孤立正文。
void CreateScriptStackReference(YtlReferenceSlotRuntime& slot, ScriptStackFrameNative* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ReleaseScriptStackReferenceAt4C9330(YtlReferenceSlotRuntime& slot) noexcept;
// 4CD4E5：正常临时释放固定使用捕获control，不重读临时Ref。
void ReleaseCapturedScriptStackControl(YtlReferenceControlRuntime* control) noexcept;

// CParams+24调用栈和+64候选表共用453A70/48F6C0的DWORD容器。
class ScriptDwordVectorStorage final
{
public:
    ScriptDwordVectorStorage() noexcept = default;
    ~ScriptDwordVectorStorage() noexcept;
    ScriptDwordVectorStorage(const ScriptDwordVectorStorage&) = delete;
    ScriptDwordVectorStorage& operator=(const ScriptDwordVectorStorage&) = delete;
    void Push(std::uint32_t value, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    void GrowAndInsertAt453A70(std::uint32_t* position, const std::uint32_t& source,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    [[nodiscard]] std::uint32_t Size() const noexcept;
    void clear() noexcept;
    void DestroyStorage(IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
    EnvironmentVectorStorage<std::uint32_t> storage{};
};
static_assert(sizeof(ScriptDwordVectorStorage) == 12);
using ScriptLocalCallStorage = ScriptDwordVectorStorage;

class ScriptFileCallStorage final
{
public:
    ScriptFileCallStorage() noexcept = default;
    ~ScriptFileCallStorage() noexcept;
    ScriptFileCallStorage(const ScriptFileCallStorage&) = delete;
    ScriptFileCallStorage& operator=(const ScriptFileCallStorage&) = delete;
    void Append(const YtlReferenceSlotRuntime& source,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    void GrowAndInsertAt4C8800(YtlReferenceSlotRuntime* position, const YtlReferenceSlotRuntime& source,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    // 隐藏返回槽原地初始化，不先复制正文；调用者负责释放这个保活Ref。
    void PopReferenceAt4D4B40(YtlReferenceSlotRuntime& result) noexcept;
    [[nodiscard]] std::uint32_t Size() const noexcept;
    void clear() noexcept;
    void DestroyStorage(IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> storage{};
};
static_assert(sizeof(ScriptFileCallStorage) == 12);
}
