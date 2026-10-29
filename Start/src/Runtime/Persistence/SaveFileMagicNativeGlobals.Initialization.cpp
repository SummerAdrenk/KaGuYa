#include <Runtime/SaveFileMagicNativeGlobals.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 5B5B14原始CP932 bytes；不经源码代码页转换，Set的20h不含末尾NUL。
constexpr char MagicAt5B5B14[] =
    "\x83\x66\x81\x5B\x83\x5E\x82\xF0\x89\xFC\x91\xA2\x82\xB7\x82\xE9"
    "\x82\xC6\x8C\xEB\x93\xAE\x8D\xEC\x82\xB5\x82\xDC\x82\xB7\x81\x42";
static_assert(sizeof(MagicAt5B5B14) == 0x20 + 1);
}

void InitializeSaveFileMagicNativeAt4055C0(RuntimeCString& slot, SaveFileMagicNativeExit callback,
    SaveFileMagicNativeExitRegistrar registrar, IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(slot, strings);
    // 4055FF / FuncInfo 5F12A0只有state0 -> -1，59F200清当时的61A5C4。
    // Capture/GetNil抛出尚无资格；登记不检查EAX，不能增加现代成功标志或回滚。
    struct Unwind final
    {
        RuntimeCString& slot;
        bool active = true;
        ~Unwind() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&slot); }
    } unwind{slot};
    // 原源VA=5B5B14，高16位非零，405610必进显式Set32而非资源ID分支。
    SetRuntimeCStringAt437680(slot, MagicAt5B5B14, 0x20);
    static_cast<void>(registrar(callback));
    unwind.active = false;
}

void DestroySaveFileMagicNativeAt5ABE90(RuntimeCString& slot)
{ DestroyRuntimeCStringAt40CD40(&slot); }
}
