#include <Runtime/SceneCueNativeGlobals.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原PE字节，不经源码代码页转换；长度均不含末尾NUL。
constexpr char FontAt5ADC0C[] = "\x82\x6C\x82\x72 \x83\x53\x83\x56\x83\x62\x83\x4E";
constexpr char TitleBgmAt5B506C[] = "BGM17.bgm";
constexpr char LikeAt5BB688[] =
    "sed:\\" "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xB9\x81\x79"
    "\x83\x6E\x81\x5B\x83\x67\x83\x41\x83\x62\x83\x76\x89\xB9\x81\x7A.ogg";
constexpr char WeightAt5B1BA0[] =
    "sed:\\" "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xB9\x81\x79"
    "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x89\xB9\x81\x7A.ogg";
constexpr char TrainingAt5B62F0[] =
    "sed:\\" "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xB9\x81\x79"
    "\x83\x58\x83\x65\x81\x5B\x83\x5E\x83\x58\x83\x41\x83\x62\x83\x76\x89\xB9\x81\x7A.ogg";
static_assert(sizeof(FontAt5ADC0C) == 0x0D + 1 && sizeof(TitleBgmAt5B506C) == 9 + 1);
static_assert(sizeof(LikeAt5BB688) == 0x25 + 1 && sizeof(WeightAt5B1BA0) == 0x23 + 1 &&
    sizeof(TrainingAt5B62F0) == 0x29 + 1);

void Initialize(RuntimeCString& slot, const char* bytes, int length,
    SceneCueNativeExit callback, SceneCueNativeExitRegistrar registrar,
    IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(slot, strings);
    // 五个FuncInfo均只有state0 -> -1；GetNil抛出尚无资格，Set/登记抛出读当前槽。
    struct Unwind final
    {
        RuntimeCString& slot;
        bool active = true;
        ~Unwind() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&slot); }
    } unwind{slot};
    // 五个原PE源地址高16位均非零，实际走显式长度Set，不走资源ID分支。
    SetRuntimeCStringAt437680(slot, bytes, length);
    static_cast<void>(registrar(callback));
    unwind.active = false;
}
}

void InitializeSceneFontFaceNativeAt401180(RuntimeCString& slot, SceneCueNativeExit callback,
    SceneCueNativeExitRegistrar registrar, IRuntimeStringManagerSource& strings)
{ Initialize(slot, FontAt5ADC0C, 0x0D, callback, registrar, strings); }
void InitializeTitleBgmNativeAt4068F0(RuntimeCString& slot, SceneCueNativeExit callback,
    SceneCueNativeExitRegistrar registrar, IRuntimeStringManagerSource& strings)
{ Initialize(slot, TitleBgmAt5B506C, 9, callback, registrar, strings); }
void InitializeGraffitiLikeCueNativeAt406590(RuntimeCString& slot, SceneCueNativeExit callback,
    SceneCueNativeExitRegistrar registrar, IRuntimeStringManagerSource& strings)
{ Initialize(slot, LikeAt5BB688, 0x25, callback, registrar, strings); }
void InitializeGraffitiWeightCueNativeAt406A10(RuntimeCString& slot, SceneCueNativeExit callback,
    SceneCueNativeExitRegistrar registrar, IRuntimeStringManagerSource& strings)
{ Initialize(slot, WeightAt5B1BA0, 0x23, callback, registrar, strings); }
void InitializeGraffitiTrainingCueNativeAt406500(RuntimeCString& slot, SceneCueNativeExit callback,
    SceneCueNativeExitRegistrar registrar, IRuntimeStringManagerSource& strings)
{ Initialize(slot, TrainingAt5B62F0, 0x29, callback, registrar, strings); }

void DestroySceneFontFaceNativeAt5AA250(RuntimeCString& slot) { DestroyRuntimeCStringAt40CD40(&slot); }
void DestroyTitleBgmNativeAt5AC3B0(RuntimeCString& slot) { DestroyRuntimeCStringAt40CD40(&slot); }
void DestroyGraffitiLikeCueNativeAt5AC2F0(RuntimeCString& slot) { DestroyRuntimeCStringAt40CD40(&slot); }
void DestroyGraffitiWeightCueNativeAt5AC3F0(RuntimeCString& slot) { DestroyRuntimeCStringAt40CD40(&slot); }
void DestroyGraffitiTrainingCueNativeAt5AC2D0(RuntimeCString& slot) { DestroyRuntimeCStringAt40CD40(&slot); }
}
