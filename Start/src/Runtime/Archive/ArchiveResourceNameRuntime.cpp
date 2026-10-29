#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/ArchiveTitleScannerRuntime.h>

#include <mbstring.h>

#include <bit>
#include <cstdint>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Byte = unsigned char;

// 只在producer完成GetNil/写槽后建立；构造失败时清当前槽，再抛则终止展开。
class ConstructionCleanup final
{
public:
    explicit ConstructionCleanup(RuntimeCString& value) noexcept : value_(&value) {}
    ~ConstructionCleanup() noexcept { if (value_) DestroyRuntimeCStringAt40CD40(value_); }
    ConstructionCleanup(const ConstructionCleanup&) = delete;
    ConstructionCleanup& operator=(const ConstructionCleanup&) = delete;
    void Dismiss() noexcept { value_ = nullptr; }
private:
    RuntimeCString* value_;
};

ATL::CStringData* Header(const char* characters) noexcept
{
    return reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(characters) - sizeof(ATL::CStringData));
}

void ReleaseCurrent(RuntimeCString& value, bool& active)
{
    if (!active) return;
    active = false;
    DestroyRuntimeCStringAt40CD40(&value);
}

void ReleaseCached(const char* characters, bool& active)
{
    if (!active) return;
    active = false;
    ReleaseRuntimeCStringHeader(Header(characters));
}

void SetCharacters(RuntimeCString& destination, const char* source)
{
    const int length = source ? static_cast<int>(std::strlen(source)) : 0;
    SetRuntimeCStringAt437680(destination, source, length);
}

void AssignRawAt40B530(RuntimeCString& destination, const char* source)
{
    const auto address = reinterpret_cast<std::uintptr_t>(source);
    if (address != 0 && (address & 0xFFFF0000u) == 0)
        static_cast<void>(LoadRuntimeCStringResourceAt420510(destination, static_cast<std::uint32_t>(address)));
    else
        SetCharacters(destination, source);
}
}

void ConstructArchiveResourceNameAt40B530(RuntimeCString* destination, const char* source,
    IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(*destination, strings);
    ConstructionCleanup cleanup(*destination);
    // 40B56F..40B5A8：源分类、测长及资源加载均晚于nil附着。
    AssignRawAt40B530(*destination, source);
    cleanup.Dismiss();
}

ArchiveRawResourceNameRuntime::ArchiveRawResourceNameRuntime(
    const char* source, IRuntimeStringManagerSource& strings)
{
    ConstructArchiveResourceNameAt40B530(&storage_.value, source, strings);
    active_ = true;
}
void ArchiveRawResourceNameRuntime::ReleasePreservingQualification()
{if(active_)DestroyRuntimeCStringAt40CD40(&storage_.value);}

ArchiveRawResourceNameRuntime::ArchiveRawResourceNameRuntime(LeftTag,
    const RuntimeCString& source, int count, IRuntimeStringManagerSource& strings)
{
    ConstructRuntimeCStringLeftAt419D00(&storage_.value, source, count, strings);
    active_ = true;
}

ArchiveRawResourceNameRuntime::ArchiveRawResourceNameRuntime(MidTag,
    const RuntimeCString& source, int start, int count, IRuntimeStringManagerSource& strings)
{
    ConstructRuntimeCStringMidAt420C80(&storage_.value, source, start, count, strings);
    active_ = true;
}

ArchiveRawResourceNameRuntime::~ArchiveRawResourceNameRuntime() noexcept
{
    ReleaseCurrent(storage_.value, active_);
}

void ArchiveRawResourceNameRuntime::ReleaseNormal()
{
    ReleaseCurrent(storage_.value, active_);
}

void ArchiveRawResourceNameRuntime::ReleaseNormalCached(const char* characters)
{
    ReleaseCached(characters, active_);
}

ArchiveRawResourceNameRuntime ArchiveResourceNameAt40B530(
    const char* source, IRuntimeStringManagerSource& strings)
{
    return ArchiveRawResourceNameRuntime(source, strings);
}

ArchiveRawResourceNameRuntime ArchiveResourceNameLeftAt419D00(const RuntimeCString& source, int count,
    IRuntimeStringManagerSource& strings)
{
    return ArchiveRawResourceNameRuntime(ArchiveRawResourceNameRuntime::LeftTag{}, source, count, strings);
}

ArchiveRawResourceNameRuntime ArchiveResourceNameMidAt420C80(const RuntimeCString& source, int start, int count,
    IRuntimeStringManagerSource& strings)
{
    return ArchiveRawResourceNameRuntime(ArchiveRawResourceNameRuntime::MidTag{}, source, start, count, strings);
}

namespace
{
// caller先用40B530("")完成40C760同序的raw nil与437680空初始化。
// scanner异常析构为flags5；正常内联释放由caller显式撤资格，不能走该析构。
void ScanFileNameAt45D950(RuntimeCString& fileName, const char* source,
    IRuntimeStringManagerSource& strings)
{
    SetCharacters(fileName, source);
    if (fileName.GetLength() == 0) return;
    const auto* slash = _mbsrchr(reinterpret_cast<const Byte*>(fileName.GetString()), '\\');
    if (!slash) return;
    const auto offset = static_cast<int>(slash - reinterpret_cast<const Byte*>(fileName.GetString()));
    if (offset < 0 || offset + 1 >= fileName.GetLength()) return;
    auto tail = ArchiveResourceNameMidAt420C80(fileName, offset + 1,
        fileName.GetLength() - offset - 1, strings);
    AssignRuntimeCStringAt410220(fileName, tail.Value());
    tail.ReleaseNormal(); // 45D9FD：Assign之后读取临时槽。
}

void ScanTitleAt40C130(RuntimeCString& title, const char* source,
    IRuntimeStringManagerSource& strings,bool cachedLeft=false)
{
    {
        ArchiveRawResourceNameRuntime fileName("", strings);
        ScanFileNameAt45D950(fileName.Value(), source, strings);
        SetCharacters(title, fileName.Value().GetString());
        fileName.ReleaseNormal();
    } // 40C238..40C25A：正常Free失败不重试，成功之后才查询title里的点。
    const auto* dot = _mbsrchr(reinterpret_cast<const Byte*>(title.GetString()), '.');
    if (dot)
    {
        const auto offset = static_cast<int>(dot - reinterpret_cast<const Byte*>(title.GetString()));
        if (offset >= 0 && offset + 1 < title.GetLength())
        {
            auto left = ArchiveResourceNameLeftAt419D00(title, offset, strings);
            const char* const captured=cachedLeft?left.Value().GetString():nullptr;
            AssignRuntimeCStringAt410220(title, left.Value());
            // 494039在Assign前缓存ESI；40C299则在Assign后读取临时槽。
            if(cachedLeft)left.ReleaseNormalCached(captured);
            else left.ReleaseNormal();
            return;
        }
    }
    SetRuntimeCStringAt437680(title, "", 0);
}

void ScanSuffixAt416D04(RuntimeCString& suffix, const char* source,
    IRuntimeStringManagerSource& strings)
{
    SetCharacters(suffix, source);
    const auto* dot = _mbsrchr(reinterpret_cast<const Byte*>(suffix.GetString()), '.');
    if (dot)
    {
        const auto offset = static_cast<int>(dot - reinterpret_cast<const Byte*>(suffix.GetString()));
        if (offset >= 0 && offset + 1 < suffix.GetLength())
        {
            auto tail = ArchiveResourceNameMidAt420C80(suffix, offset + 1,
                suffix.GetLength() - offset - 1, strings);
            AssignRuntimeCStringAt410220(suffix, tail.Value());
            tail.ReleaseNormal();
            return;
        }
    }
    SetRuntimeCStringAt437680(suffix, "", 0);
}
}

void ScanArchiveFileNameAt45D950(RuntimeCString& initializedScannerText,const char* source,
    IRuntimeStringManagerSource& strings)
{ScanFileNameAt45D950(initializedScannerText,source,strings);}

void ScanArchiveTitleAt40C130(RuntimeCString& title,const char* source,IRuntimeStringManagerSource& strings)
{ScanTitleAt40C130(title,source,strings);}
void ScanLinkTitleAt493F67(RuntimeCString& title,const char* source,IRuntimeStringManagerSource& strings)
{ScanTitleAt40C130(title,source,strings,true);}

ArchiveInstalledNamespaceRuntime::ArchiveInstalledNamespaceRuntime(
    const char* installedFileName, IRuntimeStringManagerSource& strings)
{
    ArchiveRawResourceNameRuntime fileName("", strings);
    ScanFileNameAt45D950(fileName.Value(), installedFileName, strings);
    const char* const titleSource = fileName.Value().GetString();
    ArchiveRawResourceNameRuntime title("", strings);
    ScanTitleAt40C130(title.Value(), titleSource, strings);
    const char* const keySource = title.Value().GetString();
    ConstructArchiveResourceNameAt40B530(&namespace_.value, keySource, strings);
    active_ = true;
    // 459190/state13：title撤资格后Free若抛，namespace必须先于外filename清理。
    ConstructionCleanup namespaceCleanup(namespace_.value);
    title.ReleaseNormal();
    fileName.ReleaseNormal();
    namespaceCleanup.Dismiss();
}

ArchiveInstalledNamespaceRuntime::~ArchiveInstalledNamespaceRuntime() noexcept
{
    ReleaseCurrent(namespace_.value, active_);
}

void ArchiveInstalledNamespaceRuntime::ReleaseNormalAt4592EA()
{
    ReleaseCurrent(namespace_.value, active_);
}

ArchiveQueryNameRuntime::ArchiveQueryNameRuntime(
    const char* resourceName, IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(namespace_.value, strings);
    namespaceActive_ = true;
    ConstructionCleanup namespaceCleanup(namespace_.value);
    InitializeEmptyRuntimeCString(name_.value, strings);
    nameActive_ = true;
    ConstructionCleanup nameCleanup(name_.value);
    ArchiveRawResourceNameRuntime input(resourceName, strings);
    int colonOffset = -1;
    if (input.Value().GetLength() > 0)
    {
        if (const auto* colon = _mbschr(reinterpret_cast<const Byte*>(input.Value().GetString()), ':'))
            colonOffset = static_cast<int>(colon - reinterpret_cast<const Byte*>(input.Value().GetString()));
    }
    if (colonOffset >= 0)
    {
        {
            auto left = ArchiveResourceNameLeftAt419D00(input.Value(), colonOffset, strings);
            const char* const cachedLeft = left.Value().GetString(); // 416C80，早于Assign。
            AssignRuntimeCStringAt410220(namespace_.value, left.Value());
            left.ReleaseNormalCached(cachedLeft);
        }
        {
            const int start = std::bit_cast<int>(static_cast<std::uint32_t>(colonOffset) + 2u);
            const int count = std::bit_cast<int>(static_cast<std::uint32_t>(input.Value().GetLength()) -
                static_cast<std::uint32_t>(start));
            auto tail = ArchiveResourceNameMidAt420C80(input.Value(), start, count, strings);
            AssignRuntimeCStringAt410220(name_.value, tail.Value());
            tail.ReleaseNormal();
        }
        explicit_ = true;
    }
    else
    {
        {
            ArchiveRawResourceNameRuntime suffix("", strings);
            ScanSuffixAt416D04(suffix.Value(), resourceName, strings);
            SetCharacters(namespace_.value, suffix.Value().GetString());
            suffix.ReleaseNormal();
        }
        {
            ArchiveRawResourceNameRuntime title("", strings);
            ScanTitleAt40C130(title.Value(), resourceName, strings);
            SetCharacters(name_.value, title.Value().GetString());
            title.ReleaseNormal();
        }
    }
    // 416E52先撤input资格；Free抛出仅展开name、namespace，不开始map查找。
    input.ReleaseNormal();
    nameCleanup.Dismiss();
    namespaceCleanup.Dismiss();
}

ArchiveQueryNameRuntime::~ArchiveQueryNameRuntime() noexcept
{
    ReleaseCurrent(name_.value, nameActive_);
    ReleaseCurrent(namespace_.value, namespaceActive_);
}

void ArchiveQueryNameRuntime::ReleaseNormalAt416F9C(const char* cachedNameCharacters)
{
    ReleaseCached(cachedNameCharacters, nameActive_);
    // 416FB7在name的Free正常返回后才重读namespace，不提前缓存它的header。
    ReleaseCurrent(namespace_.value, namespaceActive_);
}
}
