#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/RuntimeCString.h>

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct SaveFileDeleteFactoryNativeBody0Ch;
struct SaveFileDeleteFactoryNativeVtable1 final
{
    void* (__thiscall* deletingDestructorAt00)(SaveFileDeleteFactoryNativeBody0Ch*, std::uint32_t);
};

struct SaveFileDeleteFactoryNativeBody0Ch final
{
    SaveFileDeleteFactoryNativeBody0Ch() noexcept {}
    ~SaveFileDeleteFactoryNativeBody0Ch() noexcept {}
    SaveFileDeleteFactoryNativeBody0Ch(const SaveFileDeleteFactoryNativeBody0Ch&) = delete;
    SaveFileDeleteFactoryNativeBody0Ch& operator=(const SaveFileDeleteFactoryNativeBody0Ch&) = delete;

    const SaveFileDeleteFactoryNativeVtable1* vptrAt00;
    union { RuntimeCString prefixAt04; };
    std::int32_t slotCountAt08;
};

struct SaveFileDeleteFactoryNativeRollingBody10h final
{
    SaveFileDeleteFactoryNativeBody0Ch base;
    std::uint8_t previewAt0C;
    std::byte unwrittenAt0D[3];
};
static_assert(sizeof(SaveFileDeleteFactoryNativeVtable1) == 4);
static_assert(sizeof(SaveFileDeleteFactoryNativeBody0Ch) == 0x0C);
static_assert(offsetof(SaveFileDeleteFactoryNativeBody0Ch, prefixAt04) == 4);
static_assert(offsetof(SaveFileDeleteFactoryNativeBody0Ch, slotCountAt08) == 8);
static_assert(sizeof(SaveFileDeleteFactoryNativeRollingBody10h) == 0x10);
static_assert(offsetof(SaveFileDeleteFactoryNativeRollingBody10h, previewAt0C) == 0x0C);
static_assert(offsetof(SaveFileDeleteFactoryNativeRollingBody10h, unwrittenAt0D) == 0x0D);

class ISaveFileDeleteFactoryNativeSource
{
public:
    virtual ~ISaveFileDeleteFactoryNativeSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    virtual IRuntimeStringManagerSource& Strings() = 0;
    virtual BOOL DeleteFileA(const char*) = 0;
};

// 正文由调用者持有；C++ 空析构不释放 prefixAt04，必须按原正常/EH路径显式销毁。
// 原生构造失败已完成对应 EH 清理，调用者不可再对失败正文执行销毁。
// regular 的 slotCountAt08 保持原构造值 0，不在本入口补写为 8。
void ConstructRegularSaveFileDeleteFactoryNativeAt4DCDB0(
    SaveFileDeleteFactoryNativeBody0Ch&, std::int32_t page, ISaveFileDeleteFactoryNativeSource&);
// 包含 45B800 的第一次 "save" Set，以及 506CE0 内 quick/auto 的第二次 Set。
void ConstructRollingSaveFileDeleteFactoryNativeAt45B800(
    SaveFileDeleteFactoryNativeRollingBody10h&, bool automatic, ISaveFileDeleteFactoryNativeSource&);
void DestroySaveFileDeleteFactoryNativeAt463A90(SaveFileDeleteFactoryNativeBody0Ch&);
// deleteData 仅看低 byte；可选 dat 后始终删除 bmp，DeleteFileA FALSE 不阻断后续。
void DeleteSaveFileFactorySlotNativeAt476460(
    SaveFileDeleteFactoryNativeBody0Ch&, std::int32_t slot, std::uint32_t deleteData,
    ISaveFileDeleteFactoryNativeSource&);
// 先发布 *latest=-1；槽号从 1 起，每轮最终 Free 后才重读有符号 slotCountAt08。
void DeleteSaveFileFactoryAllNativeAt5022E0(
    SaveFileDeleteFactoryNativeBody0Ch&, std::int32_t* latest, ISaveFileDeleteFactoryNativeSource&);

// output 必须是未构造 CString 槽；仅成功返回后，调用者才取得一次清理资格。
void ConstructSaveFileFactoryDataNameNativeAt47AF10(RuntimeCString& output,
    const SaveFileDeleteFactoryNativeBody0Ch&, std::int32_t slot, ISaveFileDeleteFactoryNativeSource&);
void ConstructSaveFileFactoryPreviewNameNativeAt47AFA0(RuntimeCString& output,
    const SaveFileDeleteFactoryNativeBody0Ch&, std::int32_t slot, ISaveFileDeleteFactoryNativeSource&);

const SaveFileDeleteFactoryNativeVtable1& BaseSaveFileDeleteFactoryNativeVtable() noexcept;    // 5B2A68
const SaveFileDeleteFactoryNativeVtable1& RegularSaveFileDeleteFactoryNativeVtable() noexcept; // 5B2A88
const SaveFileDeleteFactoryNativeVtable1& QuickSaveFileDeleteFactoryNativeVtable() noexcept;   // 5B2A98
const SaveFileDeleteFactoryNativeVtable1& AutoSaveFileDeleteFactoryNativeVtable() noexcept;    // 5B2ABC

// 显式内存边界供 sized-delete 测试；表 thunk 使用 DefaultEnvironmentRootMemory。
// prefix Free 与 DeleteBody 均允许首次异常外传；Free 失败不执行正文 sized delete。
void* DeleteBaseSaveFileDeleteFactoryNativeAt469960(SaveFileDeleteFactoryNativeBody0Ch&,
    std::uint32_t flags, IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
void* DeleteRegularSaveFileDeleteFactoryNativeAt469100(SaveFileDeleteFactoryNativeBody0Ch&,
    std::uint32_t flags, IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
void* DeleteQuickSaveFileDeleteFactoryNativeAt469030(SaveFileDeleteFactoryNativeBody0Ch&,
    std::uint32_t flags, IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
void* DeleteAutoSaveFileDeleteFactoryNativeAt4686D0(SaveFileDeleteFactoryNativeBody0Ch&,
    std::uint32_t flags, IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
}
