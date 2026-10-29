#pragma once

#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
// 40B530：目标必须是对齐、未构造的存储；先捕获manager并附着nil，再读取源。
// 构造失败自行清理；成功后调用者负责析构一次，不能传入已构造的CString。
void ConstructArchiveResourceNameAt40B530(RuntimeCString* destination, const char* source,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// scanner+04已完成40C760的nil/空串初始化；仅操作其原CString字段，不创建额外owner。
void ScanArchiveFileNameAt45D950(RuntimeCString& initializedScannerText,const char* source,
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource());

// 43CEF0每层的raw输入owner；不允许复制成另一个CString而额外触发CloneData。
class ArchiveRawResourceNameRuntime final
{
public:
    struct ProducerTag final {};
    // producer只写未构造槽；成功返回才获得清理资格，不能提前附着nil。
    template<class Producer>
    ArchiveRawResourceNameRuntime(ProducerTag, Producer&& producer)
    {
        producer(&storage_.value);
        active_ = true;
    }
    explicit ArchiveRawResourceNameRuntime(const char* source,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~ArchiveRawResourceNameRuntime() noexcept;
    ArchiveRawResourceNameRuntime(const ArchiveRawResourceNameRuntime&) = delete;
    ArchiveRawResourceNameRuntime& operator=(const ArchiveRawResourceNameRuntime&) = delete;
    ArchiveRawResourceNameRuntime(ArchiveRawResourceNameRuntime&&) = delete;
    ArchiveRawResourceNameRuntime& operator=(ArchiveRawResourceNameRuntime&&) = delete;

    [[nodiscard]] RuntimeCString& Value() & noexcept { return storage_.value; }
    [[nodiscard]] const RuntimeCString& Value() const & noexcept { return storage_.value; }
    const RuntimeCString& Value() const && = delete;
    // 正常出口先撤资格；失败不由本owner重试，展开仍清其它活跃前缀。
    void ReleaseNormal();
    void ReleaseNormalCached(const char* characters);
    // 仅原4D3C90错误尾state7：普通Free不撤资格，另一Free失败时按原EH再次减持。
    void ReleasePreservingQualification();
    void DismissQualification() noexcept{active_=false;}

private:
    struct LeftTag {};
    struct MidTag {};
    ArchiveRawResourceNameRuntime(LeftTag, const RuntimeCString& source, int count,
        IRuntimeStringManagerSource& strings);
    ArchiveRawResourceNameRuntime(MidTag, const RuntimeCString& source, int start, int count,
        IRuntimeStringManagerSource& strings);
    friend ArchiveRawResourceNameRuntime ArchiveResourceNameLeftAt419D00(
        const RuntimeCString&, int, IRuntimeStringManagerSource&);
    friend ArchiveRawResourceNameRuntime ArchiveResourceNameMidAt420C80(
        const RuntimeCString&, int, int, IRuntimeStringManagerSource&);
    union Storage
    {
        Storage() noexcept {}
        ~Storage() noexcept {}
        RuntimeCString value;
    } storage_;
    bool active_ = false;
};

// 返回同类型prvalue，保证直接构造owner，不依赖可选NRVO；消费者用auto持有后借Value()。
[[nodiscard]] ArchiveRawResourceNameRuntime ArchiveResourceNameAt40B530(const char* source,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 保留原CloneData和零长Allocate行为；临时须跨过赋值或递归调用，随后立即释放。
[[nodiscard]] ArchiveRawResourceNameRuntime ArchiveResourceNameLeftAt419D00(
    const RuntimeCString& source, int count,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] ArchiveRawResourceNameRuntime ArchiveResourceNameMidAt420C80(
    const RuntimeCString& source, int start, int count,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 45915A..4591DF：LoadInfo成功后构造，namespace存活到插表临时pair释放之后。
class ArchiveInstalledNamespaceRuntime final
{
public:
    explicit ArchiveInstalledNamespaceRuntime(const char* installedFileName,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~ArchiveInstalledNamespaceRuntime() noexcept;
    ArchiveInstalledNamespaceRuntime(const ArchiveInstalledNamespaceRuntime&) = delete;
    ArchiveInstalledNamespaceRuntime& operator=(const ArchiveInstalledNamespaceRuntime&) = delete;
    ArchiveInstalledNamespaceRuntime(ArchiveInstalledNamespaceRuntime&&) = delete;
    ArchiveInstalledNamespaceRuntime& operator=(ArchiveInstalledNamespaceRuntime&&) = delete;

    [[nodiscard]] const RuntimeCString& Namespace() const noexcept { return namespace_.value; }
    void ReleaseNormalAt4592EA();

private:
    // basename/title扫描器之前不构造nil，也不提前捕获manager。
    union Storage
    {
        Storage() noexcept {}
        ~Storage() noexcept {}
        RuntimeCString value;
    } namespace_;
    bool active_ = false;
};

// 416B90..416E6B：构造返回前已释放输入临时；本对象必须跨过+38结果发布。
// 析构按name、namespace顺序释放，不增持归档Ref。
class ArchiveQueryNameRuntime final
{
public:
    explicit ArchiveQueryNameRuntime(const char* resourceName,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~ArchiveQueryNameRuntime() noexcept;
    ArchiveQueryNameRuntime(const ArchiveQueryNameRuntime&) = delete;
    ArchiveQueryNameRuntime& operator=(const ArchiveQueryNameRuntime&) = delete;
    ArchiveQueryNameRuntime(ArchiveQueryNameRuntime&&) = delete;
    ArchiveQueryNameRuntime& operator=(ArchiveQueryNameRuntime&&) = delete;

    [[nodiscard]] const RuntimeCString& Namespace() const noexcept { return namespace_.value; }
    [[nodiscard]] const RuntimeCString& Name() const noexcept { return name_.value; }
    [[nodiscard]] bool IsExplicit() const noexcept { return explicit_; }
    void ReleaseNormalAt416F9C(const char* cachedNameCharacters);

private:
    union Storage
    {
        Storage() noexcept {}
        ~Storage() noexcept {}
        RuntimeCString value;
    } namespace_, name_;
    bool namespaceActive_ = false;
    bool nameActive_ = false;
    bool explicit_ = false;
};
}
