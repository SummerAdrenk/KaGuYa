#pragma once

#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
using SaveFileMagicNativeExit = void(__cdecl*)();
using SaveFileMagicNativeExitRegistrar = int(__cdecl*)(SaveFileMagicNativeExit);

// 原4055F5的A3立即数为61A5C4；借同一可写Process槽，不Clone、不取std::string快照。
// getter不取manager、不增持，不在退出后清槽或懒建；借用者不取得析构资格。
RuntimeCString& ProcessSaveFileMagicNative() noexcept;
const char*& ProcessSaveFileMagicNativeCharsSlot() noexcept;

// 仅接受fresh存储和有效回调。GetNil发布后才取得state0，Set/登记异常清当前槽。
// registrar普通返回值一律忽略，不补登记、不回滚；展开Free再抛时终止。
void InitializeSaveFileMagicNativeAt4055C0(RuntimeCString& slotAt61A5C4,
    SaveFileMagicNativeExit callbackAt5ABE90, SaveFileMagicNativeExitRegistrar registrarAt540012,
    IRuntimeStringManagerSource& stringsAt417360 = DefaultRuntimeStringManagerSource());

// 读当前chars后原子减持，必要时raw manager+04；不清槽、不吞异常、不附加析构。
void DestroySaveFileMagicNativeAt5ABE90(RuntimeCString& slotAt61A5C4);
}
