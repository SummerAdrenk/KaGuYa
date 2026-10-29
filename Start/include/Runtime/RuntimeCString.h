#pragma once

#include <atlbase.h>
#include <atlsimpstr.h>
#include <cstdarg>
#include <cstdint>
#include <string>
#include <string_view>
#include <Runtime/RuntimeStringBorrow.h>

namespace KaGuYa::Start::Runtime
{
// 417360/413430/431500：进程堆管理器、16-byte 头、共享 nil 与写时分离。
// 使用基础 ATL 字符串；新版 CStringT::FormatV 会额外复制格式串，不可直接代替417150。
using RuntimeCString = ATL::CSimpleStringT<char, false>;
static_assert(sizeof(RuntimeCString) == 4);
static_assert(sizeof(ATL::CStringData) == 16);

// 40CD40：原子减持后直接调用管理器虚槽 +04，允许原始 ABI 目标传播异常。
// 仅用于已构造的字符串；成功后不得再执行 ATL 析构。
// Free抛出后的清理资格由原调用方EH决定：多数入口不重试，43D5D7仍需展开时再减持。
void DestroyRuntimeCStringAt40CD40(RuntimeCString* element);
// 原调用点已缓存chars/header时使用，不在Free之前重新解析TEXT槽。
void ReleaseRuntimeCStringHeader(ATL::CStringData* header);

// 414540：Clone/Allocate使用原始虚槽；调用方负责返回header的引用，不提前发布目标。
[[nodiscard]] ATL::CStringData* CloneRuntimeCStringDataAt414540(ATL::CStringData* source);

// 410220：同manager的共享赋值先CloneData再释放旧串；旧Free可抛，发布必须在其后。
void AssignRuntimeCStringAt410220(RuntimeCString& destination, const RuntimeCString& source);

// 437680：显式长度写入；保留管理器原始虚槽、写时分离及异常后的发布顺序。
void SetRuntimeCStringAt437680(RuntimeCString& destination, const char* source, int length);
// 481690：按原长度PrepareWrite后调用当前线程代码页的_mbslwr_s，保留4136D0错误映射。
void LowerRuntimeCStringAt481690(RuntimeCString& destination);
// 413550：追加至多 limit 个首 NUL 前字节；分配后重定位指向自身的 source。
void AppendRuntimeCStringAt413550(RuntimeCString& destination, const char* source, int limit);
// 原调用者在分支中已判定需写时分离/扩容，直接进入42D590，不穿过新版ATL throw()。
void PrepareRuntimeCStringWriteAt42D590(RuntimeCString& destination, int length);
// 417260/486B10：外部流直接写缓冲，先发布请求长度，再按当前容量提交NUL前长度。
char* GetRuntimeCStringBufferSetLengthAt417260(RuntimeCString& destination,int length);
void ReleaseRuntimeCStringBufferAt486B10(RuntimeCString& destination,int length=-1);

// 4102D0：两遍CP_THREAD_ACP转换；第二遍容量不含NUL，忽略返回值，再提交首遍长度。
// PrepareWrite/Empty复用原始manager槽，允许异常外传，不经过ATL GetBuffer的throw()边界。
void AssignRuntimeCStringWideAt4102D0(RuntimeCString& destination, const wchar_t* source);
// 可空目标保留4102EE第一次Win32查询，直到原目标首次访问处才明确终止。
void AssignRuntimeCStringWideAt4102D0(RuntimeCString* destination, const wchar_t* source);

// 43D9B0：重叠安全复制；参数错误时的清零顺序不同于当前UCRT的memmove_s。
int MoveRuntimeBytesAt43D9B0(void* destination, std::uint32_t capacity,
    const void* source, std::uint32_t length);

class IRuntimeStringManagerSource
{
public:
    virtual ~IRuntimeStringManagerSource() = default;
    virtual ATL::IAtlStringMgr* CaptureAt417360() = 0;
};

IRuntimeStringManagerSource& DefaultRuntimeStringManagerSource() noexcept;

// 41D473/41D565：仅写尚未构造TEXT的4-byte指针；GetNil原始槽允许异常外传。
void InitializeEmptyRuntimeCString(RuntimeCString& destination,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 40B490：out为未构造槽；直接Allocate，零长度也不取nil。
// 先发布再校验长度/容量；失败不回滚，调用方仅在成功返回后取得清理资格。
void ConstructRuntimeCStringAt40B490(RuntimeCString* out,
    const char* source, int length, ATL::IAtlStringMgr* manager);
// 419D00/420C80：完整范围用414540，其余raw Clone选manager后重读源chars。
void ConstructRuntimeCStringLeftAt419D00(RuntimeCString* out,
    const RuntimeCString& source, int count,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void ConstructRuntimeCStringMidAt420C80(RuntimeCString* out,
    const RuntimeCString& source, int start, int count,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
// 420510：资源miss不改目标；显式WCHAR长度两遍转换，不减1、不补nil或回滚。
[[nodiscard]] bool LoadRuntimeCStringResourceAt420510(RuntimeCString& destination, std::uint32_t id);

// 420650：在未构造的返回槽生成 Demo 短串，显式 byteLength/2 个 WCHAR，允许内嵌 NUL。
// 源地址由调用者提前捕获并保证存活；不是普通 S 的 NUL 结尾转换，也不接管输入 owner。
struct DemoShortTextInputTruncated final {};
void InitializeDemoShortRuntimeCStringAt420650(RuntimeCString& result,
    const wchar_t* source, std::int32_t byteLength,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    std::size_t availableSourceBytes = static_cast<std::size_t>(-1));

[[nodiscard]] inline std::string_view RuntimeStringView(const RuntimeCString& value) noexcept
{
    return {value.GetString(), static_cast<std::size_t>(value.GetLength())};
}

// 417130 -> 417150：先测长，再扩容、写入，最后提交第一遍长度。
void FormatRuntimeCStringVAt417150(RuntimeCString& destination,
    _Printf_format_string_ const char* format, va_list args);
void __cdecl FormatRuntimeCStringAt417130(RuntimeCString& destination,
    _Printf_format_string_ const char* format, ...);

// 479F48..479F85：缓存旧长度，原位追加LF；分配及提交均使用raw写入路径。
void AppendRuntimeCStringLineFeedAt479F48(RuntimeCString& destination);

// 4145E0：两段按显式长度复制，保留内嵌NUL；DWORD相加后按有符号值拒绝负长度。
void ConcatenateRuntimeCStringAt4145E0(RuntimeCString& destination,
    const char* first, std::uint32_t firstLength, const char* second, std::uint32_t secondLength);
// 4CA280：manager来自suffix，输出独立GetNil；之后缓存suffix chars再测prefix长。
void ConstructRuntimeCStringPrefixAt4CA280(RuntimeCString* out,const char* prefix,
    const RuntimeCString& suffix,IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource());
}
