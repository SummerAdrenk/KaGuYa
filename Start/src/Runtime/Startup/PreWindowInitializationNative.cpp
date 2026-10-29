#include <Runtime/PreWindowInitializationNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
union ErrorText final { RuntimeCString value; ErrorText() noexcept {} ~ErrorText() noexcept {} };
// 原5B3348共20字节；IDA注释只显示ASCII前缀，末尾仍有CP932错误标题。
constexpr char errorCaption[] = "FirstInstance \x83\x47\x83\x89\x81\x5B";
static_assert(sizeof(errorCaption) == 21);
struct ErrorUnwind final
{
    RuntimeCString& value;
    bool active = true;
    ~ErrorUnwind() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&value); }
};
}
bool RunPreWindowInitializationNativeAt4778B0(PreWindowLifecycleState& state,
    IPreWindowInitializationNativeServices& services)
{
    state.Reset();
    try
    {
        services.RegisterEnvironmentArchivesAt47E2B0();
        if (!services.PrepareEnvironmentPersistenceAt480230()) services.WriteEnvironmentPersistenceAt488740();
        services.InstallWindowModeAt47791D();
        const auto& save = services.SaveDataAtSlot1();
        auto& click = services.ClickInfoAtSlot5();
        ApplyClickInfoMouseMapping(save, click);
        return true;
    }
    catch (...)
    {
        // 477BDA先退至-1；479EA0成功返回后才有state4的当前TEXT清理资格。
        ErrorText text;
        services.BuildInitializationErrorTextAt479EA0(text.value);
        ErrorUnwind unwind{text.value};
        if (!text.value.GetLength()) SetRuntimeCStringAt437680(text.value, "Syntax error", 12);
        static_cast<void>(services.MessageBoxA(nullptr, text.value.GetString(), errorCaption, MB_ICONERROR));
        // 原477C23仍在state4。普通Free若抛出，59954A会再次对当前槽减持；
        // 不能在此提前撤资格，也不能由现代CString析构只处理一次。
        DestroyRuntimeCStringAt40CD40(&text.value);
        unwind.active = false;
        return false;
    }
}
}
