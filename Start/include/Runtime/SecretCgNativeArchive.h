#pragma once

#include <Runtime/SecretCgConstructionRuntime.h>
#include <Runtime/SystemDataEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime
{
struct SecretCgConstructionInputs;
struct SecretCgResolvedMapping;

// 非拥有索引视图；每次按原Ref的bias/stride解析，不复制数组。
class SecretCgIndicesView final
{
public:
    explicit SecretCgIndicesView(const YtlReferenceSlotRuntime& reference) noexcept : reference_(&reference) {}
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::int32_t operator[](std::size_t index) const noexcept;
private:
    const YtlReferenceSlotRuntime* reference_;
};
// 真实Params来源；每个原callSite独立执行418480，不持有GS/Pattern快照。
class SecretCgParamsRootSource final : public ISecretCgParamsRootSource
{
public:
    explicit SecretCgParamsRootSource(ParamsRuntimeRootOwner& owner) noexcept : owner_(owner) {}
    const void* CaptureAt418480(std::uint32_t callSite) override;
private:
    ParamsRuntimeRootOwner& owner_;
};

// 原+1C虚槽没有服务参数；宿主在调用范围绑定真实进程来源，不把服务塞入24h正文。
// 不自动创建假Params根；未绑定时明确报装配错误。不同线程不得交错更换进程绑定。
class SecretCgParamsBinding final
{
public:
    explicit SecretCgParamsBinding(ISecretCgParamsRootSource& source) noexcept;
    ~SecretCgParamsBinding() noexcept;
    SecretCgParamsBinding(const SecretCgParamsBinding&) = delete;
    SecretCgParamsBinding& operator=(const SecretCgParamsBinding&) = delete;
private:
    ISecretCgParamsRootSource* previous_;
};
// CG/SC原业务槽均取同一进程Params来源；不缓存正文或另设默认根。
const void* CaptureBoundSecretCgParamsAt418480(std::uint32_t callSite);

// 派生类虽封闭，5B291C的追加业务虚槽仍是原生ABI的一部分，不能删virtual。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunnecessary-virtual-specifier"
#endif
class SecretCgNativeArchive final : public IEnvironmentArchiveRuntime
{
public:
    static constexpr std::uint32_t OriginalObjectSize = 0x24;
    static constexpr std::uint32_t OriginalVtableAddress = 0x5B291C;
    explicit SecretCgNativeArchive(ISecretCgParamsRootSource& source,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    // 仅显式工具/profile夹具入口；生产注册必须使用上面的真实Params来源。
    explicit SecretCgNativeArchive(const SecretCgConstructionInputs& inputs);
    ~SecretCgNativeArchive() noexcept;
    SecretCgNativeArchive(const SecretCgNativeArchive&) = delete;
    SecretCgNativeArchive& operator=(const SecretCgNativeArchive&) = delete;

    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    [[nodiscard]] virtual std::int32_t CountAtVslot08() const noexcept;
    [[nodiscard]] virtual std::uint8_t GetFlagAtVslot0C(std::uint32_t index) const noexcept;
    virtual void SetFlagAtVslot10(std::uint32_t index, std::uint8_t value) noexcept;
    virtual void SetKeyBaseAtVslot14(std::uint32_t value) noexcept;
    virtual void FillFlagsAtVslot18(std::uint8_t value) noexcept;
    [[nodiscard]] virtual const char* const* ThumbnailAtVslot1C(std::int32_t category, std::int32_t item);
    [[nodiscard]] virtual std::int32_t PercentAllAtVslot20() const noexcept;
    [[nodiscard]] virtual std::int32_t PercentCategoryAtVslot24(std::int32_t category) const noexcept;

    // 与虚槽使用同一个执行体；显式来源口供尚未组装进程全局的调用者使用。
    [[nodiscard]] const char* const* ThumbnailAt477FD0(
        ISecretCgParamsRootSource& source, std::int32_t category, std::int32_t item);
    [[nodiscard]] std::size_t CategoryCount() const noexcept;
    [[nodiscard]] std::size_t ItemCount(std::size_t category) const noexcept;
    [[nodiscard]] SecretCgIndicesView Indices(std::size_t category, std::size_t item) const noexcept;
    [[nodiscard]] const SecretCgFlagRecord& RecordAt(std::size_t index) const noexcept;

    bool readyAt04;
    std::uint8_t unwrittenAt05[3];
    std::uint32_t baseKeyOffsetAt08;
    SecretCgReferenceVector recordsAt0C;
    SecretCgMappingVectorStorage mappingsAt18;
};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
static_assert(sizeof(SecretCgNativeArchive) == 0x24);
static_assert(alignof(SecretCgNativeArchive) == 4);
static_assert(offsetof(SecretCgNativeArchive, readyAt04) == 4);
static_assert(offsetof(SecretCgNativeArchive, baseKeyOffsetAt08) == 8);
static_assert(offsetof(SecretCgNativeArchive, recordsAt0C) == 0x0C);
static_assert(offsetof(SecretCgNativeArchive, mappingsAt18) == 0x18);

// 47E349：24h正文构造后才捕获Environment；ready=0也登记，捕获失败不补回收。
void RegisterSecretCgArchiveAt47E349(EnvironmentRootOwner& owner,
    ISecretCgParamsRootSource& params,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
