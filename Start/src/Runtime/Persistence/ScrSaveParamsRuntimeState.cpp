#include <Runtime/ScrSaveParamsRuntimeState.h>
#include <Runtime/RuntimeCStringSerialize.h>
#include "NativeSerialize18.Internal.h"

#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
void __fastcall SerializeSaveParams(void* body, void*, void* native18)
{
    SerializeSaveParamsNative18At48B720(*static_cast<ScrSaveParamsRuntimeState*>(body), native18);
}

void* __fastcall DeleteSaveParams(void* body, void*, std::uint32_t flags) noexcept
{
    return static_cast<ScrSaveParamsRuntimeState*>(body)->DeleteAtVslot04(flags);
}

const NativeArchiveVtable2 methods{
    reinterpret_cast<decltype(NativeArchiveVtable2::serializeAt00)>(&SerializeSaveParams),
    reinterpret_cast<decltype(NativeArchiveVtable2::deleteAt04)>(&DeleteSaveParams)};
}

const NativeArchiveVtable2& SaveParamsNativeMethods() noexcept
{
    return methods;
}

ScrSaveParamsRuntimeState::ScrSaveParamsRuntimeState(IRuntimeStringManagerSource& strings)
    : vptrAt00(&methods)
{
    // 46CE7C..46CED5 / EH 5E6604：逐串取得清理资格，初始化后先清 title 再清 description。
    struct PrefixCleanup
    {
        ScrSaveParamsRuntimeState& body;
        unsigned count = 0;
        bool active = true;
        ~PrefixCleanup() noexcept
        {
            if (!active) return;
            if (count >= 2) DestroyRuntimeCStringAt40CD40(&body.saveTitleAt08);
            if (count >= 1) DestroyRuntimeCStringAt40CD40(&body.saveDescriptionAt04);
            // EH 5E6628：state1 的 598BEF -> 40F860；包括首串构造前的失败。
            RetireNativeArchive(&body);
        }
    } cleanup{*this};
    InitializeEmptyRuntimeCString(saveDescriptionAt04, strings);
    cleanup.count = 1;
    InitializeEmptyRuntimeCString(saveTitleAt08, strings);
    cleanup.count = 2;
    SetRuntimeCStringAt437680(saveTitleAt08, "", 0);
    SetRuntimeCStringAt437680(saveDescriptionAt04, "", 0);
    cleanup.active = false;
}

ScrSaveParamsRuntimeState::~ScrSaveParamsRuntimeState() noexcept
{
    // 469270 / EH 5E6508 flags=5：Free 异常直接 terminate，不重试，也不继续释放下一串。
    DestroyRuntimeCStringAt40CD40(&saveTitleAt08);
    DestroyRuntimeCStringAt40CD40(&saveDescriptionAt04);
    // 4692CF：两个旧 chars 指针不清零，仅退化原 IArchive 首槽。
    RetireNativeArchive(this);
}

void* ScrSaveParamsRuntimeState::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    this->~ScrSaveParamsRuntimeState();
    if (flags & 1u) ::operator delete(this, sizeof(ScrSaveParamsRuntimeState));
    return this;
}

void ScrSaveParamsRuntimeState::Serialize(SerializeBuffer& archive)
{
    // 48B720 两次 +4C 调用顺序与成员地址顺序相反；第二串失败不撤回第一串。
    SerializeRuntimeCStringAt465ED0(archive, saveTitleAt08);
    SerializeRuntimeCStringAt465ED0(archive, saveDescriptionAt04);
}

void SerializeSaveParamsNative18At48B720(ScrSaveParamsRuntimeState& body, void* native18)
{
    // 48B72C/48B735 每次重读 serializer 当前表，传原 +08/+04 字段地址。
    NativeSerialize18Detail::Argument(native18, 0x4C, &body.saveTitleAt08);
    NativeSerialize18Detail::Argument(native18, 0x4C, &body.saveDescriptionAt04);
}
}
