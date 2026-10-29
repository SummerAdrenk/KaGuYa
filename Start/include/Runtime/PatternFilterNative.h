#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/SystemDataEnvironmentArchive.h>

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 5B2448 = [48AFF0, 468AB0]。构造器仅建立两槽vptr；字段由下列raw入口管理。
// 不得普通delete或按值复制；Pattern内嵌时也必须调用同一构造/析构叶。
class PatternFilterNativeBody20h final : public IEnvironmentArchiveRuntime
{
public:
    PatternFilterNativeBody20h() noexcept {}
    PatternFilterNativeBody20h(const PatternFilterNativeBody20h&) = delete;
    PatternFilterNativeBody20h& operator=(const PatternFilterNativeBody20h&) = delete;
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;

    std::uint32_t modeAt04;
    EnvironmentVectorStorage<std::uint32_t> argumentsAt08;
    union { YtlReferenceSlotRuntime temporaryAt14; };

private:
    ~PatternFilterNativeBody20h() noexcept = default;
};
static_assert(sizeof(PatternFilterNativeBody20h) == 0x20);
static_assert(offsetof(PatternFilterNativeBody20h, modeAt04) == 0x04);
static_assert(offsetof(PatternFilterNativeBody20h, argumentsAt08) == 0x08);
static_assert(offsetof(PatternFilterNativeBody20h, temporaryAt14) == 0x14);

[[nodiscard]] PatternFilterNativeBody20h* ConstructPatternFilterNativeAt45B1CB(void* raw) noexcept;
// 46F245..46F281：先mode/vector深复制，分配返回后才复制temp三字段并AddRef。
// 失败仅执行40F860基类退表；不取得raw块的删除资格。
[[nodiscard]] PatternFilterNativeBody20h* CopyConstructPatternFilterNativeAt46F245(
    void* raw, const PatternFilterNativeBody20h& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 4CA040：先mode，再原vector赋值，最后释放旧temp并晚取源Ref；失败不回滚已写前缀。
// 扩容先删缓存旧块再分配，与Resize/CopyConstruct的取源及失败状态不同。
PatternFilterNativeBody20h* AssignPatternFilterNativeAt4CA040(
    PatternFilterNativeBody20h& body, const PatternFilterNativeBody20h& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

void DestroyPatternFilterNativeAt462FC0(PatternFilterNativeBody20h& body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
void* DeletePatternFilterNativeAt468AB0(PatternFilterNativeBody20h* body, std::uint32_t flags,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;

// SerializeBuffer仍是现代适配边界，不宣称拥有原CSerialize虚表或逐字节故障语义。
// 直接操作正文三指针；不借std::vector别名，也不构造第二份拥有容器。
void SerializePatternFilterNativeAt48AFF0(PatternFilterNativeBody20h& body,
    SerializeBuffer& serializer, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ResizePatternFilterArgumentsAt4573E0(PatternFilterNativeBody20h& body, std::uint32_t count,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
