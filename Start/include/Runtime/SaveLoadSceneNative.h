#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/SaveLoadDialogNativeRuntime.h>
#include <optional>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct SaveLoadSceneNativeBody24h;
// PE 5B4DA0..5B4DAC共四槽；5B4DB0是CP932名称，不是Return槽。
// runAt00指向21字节naked RawRun；C++类型消费AL，但raw调用还保留Draw产生的EAX高位。
struct SaveLoadSceneNativeVtable4 final
{
    std::uint8_t (__thiscall* runAt00)(SaveLoadSceneNativeBody24h*, std::int32_t);
    SaveLoadSceneNativeBody24h* (__thiscall* deleteAt04)(SaveLoadSceneNativeBody24h*, std::uint32_t);
    void (__thiscall* updateAt08)(SaveLoadSceneNativeBody24h*);
    void (__thiscall* drawAt0C)(SaveLoadSceneNativeBody24h*);
};
struct SaveLoadSceneNativeBody24h final
{
    const SaveLoadSceneNativeVtable4* vptrAt00;
    std::uint32_t phaseAt04;
    union { YtlReferenceSlotRuntime dialogAt08; };
    std::uint32_t savedRegistryAt14;
    std::uint8_t modeAt18;
    std::uint8_t unwrittenAt19[8];
    std::uint8_t savedRootByteAt21;
    std::uint8_t unwrittenAt22[2];
    SaveLoadSceneNativeBody24h() noexcept {}
    ~SaveLoadSceneNativeBody24h() noexcept {}
};
static_assert(sizeof(SaveLoadSceneNativeVtable4) == 0x10);
static_assert(sizeof(SaveLoadSceneNativeBody24h) == 0x24);
static_assert(alignof(SaveLoadSceneNativeBody24h) == 4);
static_assert(std::is_standard_layout_v<SaveLoadSceneNativeBody24h>);
static_assert(offsetof(SaveLoadSceneNativeBody24h, phaseAt04) == 4);
static_assert(offsetof(SaveLoadSceneNativeBody24h, dialogAt08) == 8);
static_assert(offsetof(SaveLoadSceneNativeBody24h, savedRegistryAt14) == 0x14);
static_assert(offsetof(SaveLoadSceneNativeBody24h, modeAt18) == 0x18);
static_assert(offsetof(SaveLoadSceneNativeBody24h, unwrittenAt19) == 0x19);
static_assert(offsetof(SaveLoadSceneNativeBody24h, savedRootByteAt21) == 0x21);
static_assert(offsetof(SaveLoadSceneNativeBody24h, unwrittenAt22) == 0x22);

class ISaveLoadSceneNativeSource
{
public:
    virtual ~ISaveLoadSceneNativeSource() = default;
    virtual std::uint8_t ReadRootByteAt1AF() = 0;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t callSite) = 0;
    virtual std::uint8_t ReadRootByteAt219() = 0;
    // 先捕获当前root，再读借用byte，保持4BA778/4BA7A8的晚读取。
    virtual void WriteRootByteAt219(const std::uint8_t& value, std::uint32_t callSite) = 0;
    virtual void WriteRootSubobjectAt4AD230DwordAt10(std::uint32_t value) = 0;
    virtual void PopThenActivatePendingAt4C1760() = 0;
    virtual void ClearScenesAt4C1460() = 0;
    virtual void QueueAt47F920(const char* nameCp932) = 0;
    virtual void ResetAndQueueTitleAt47E180() = 0;
};
// 必填raw构造依赖，不是ISaveLoadDialog/shared_ptr适配器；完整184h正文尚未接通。
// 实现只负责部分成员EH；异常外传的raw delete184资格由此处外层持有。
// 原正常NULL返回保留孤儿资格，不改为现代异常或额外回收。
class ISaveLoadDialogNativeConstruction
{
public:
    virtual ~ISaveLoadDialogNativeConstruction() = default;
    virtual GuiDialogNativeBody5Ch* ConstructAt4EB950(
        void* storage184h, std::uint32_t modeByte, std::uint32_t rootByteAt1AF) = 0;
    // 正文外元数据，默认只保留旧隔离域；原生生产adapter声明完整184h服务。
    virtual SaveLoadDialogNativeRuntimeServices* NativeRuntimeServices() const noexcept{return nullptr;}
};
struct SaveLoadSceneNativeServices final
{
    ISaveLoadSceneNativeSource& source;
    ISaveLoadDialogNativeConstruction& dialog;
    IEnvironmentRootMemory& memory;
};
class SaveLoadSceneNativeScope final
{
public:
    explicit SaveLoadSceneNativeScope(SaveLoadSceneNativeServices&) noexcept;
    ~SaveLoadSceneNativeScope() noexcept;
    SaveLoadSceneNativeScope(const SaveLoadSceneNativeScope&) = delete;
    SaveLoadSceneNativeScope& operator=(const SaveLoadSceneNativeScope&) = delete;
private:
    SaveLoadSceneNativeServices* previous_;
    std::optional<SaveLoadDialogNativeRuntimeScope> dialog_;
};
const SaveLoadSceneNativeVtable4& SaveLoadSceneNativeMethods() noexcept;
SaveLoadSceneNativeBody24h* ConstructSaveLoadSceneNativeAt4A17B0(
    SaveLoadSceneNativeBody24h&, std::uint32_t modeArgument, SaveLoadSceneNativeServices&);
SaveLoadSceneNativeBody24h* NewSaveSceneNativeAt4AB3A2(SaveLoadSceneNativeServices&);
SaveLoadSceneNativeBody24h* NewLoadSceneNativeAt4AB40B(SaveLoadSceneNativeServices&);
void DestroySaveLoadSceneNativeAt4A6F90(SaveLoadSceneNativeBody24h&) noexcept;
SaveLoadSceneNativeBody24h* DeleteSaveLoadSceneNativeAt4A6F90(
    SaveLoadSceneNativeBody24h*, std::uint32_t flags) noexcept;
// 现代Services包装建立Scope后调当前table Run，只承诺uint8_t，不替代raw寄存器ABI。
std::uint8_t RunSaveLoadSceneNativeAt4C3630(
    SaveLoadSceneNativeBody24h&, std::int32_t, SaveLoadSceneNativeServices&);
void UpdateSaveLoadSceneNativeAt4BA650(SaveLoadSceneNativeBody24h&, SaveLoadSceneNativeServices&);

class RootRuntimeOwner;
class RootScriptRecoveryRuntime;
struct RootSaveLoadSceneNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    RootScriptRecoveryRuntime& recovery;
};
// 只借长期服务，各原业务叶逐点捕获当前publication，不持有Root/scene镜像。
// 默认+3C借原14h父；历史受控域由Post显式选择，不通过业务失败切换。
class RootSaveLoadSceneNativeSource final : public ISaveLoadSceneNativeSource
{
public:
    explicit RootSaveLoadSceneNativeSource(RootSaveLoadSceneNativeDependencies) noexcept;
    std::uint8_t ReadRootByteAt1AF() override;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t) override;
    std::uint8_t ReadRootByteAt219() override;
    void WriteRootByteAt219(const std::uint8_t&, std::uint32_t) override;
    void WriteRootSubobjectAt4AD230DwordAt10(std::uint32_t) override;
    void PopThenActivatePendingAt4C1760() override;
    void ClearScenesAt4C1460() override;
    void QueueAt47F920(const char*) override;
    void ResetAndQueueTitleAt47E180() override;
private:
    RootSaveLoadSceneNativeDependencies dependencies_;
};
}
