#pragma once
#include <Runtime/SaveLoadDialogNative.h>
#include <Runtime/SaveFileDeleteFactoryNative.h>

namespace KaGuYa::Start::Runtime
{
class ISaveLoadDialogNativeDeletionSource : public ISaveFileDeleteFactoryNativeSource
{
public:
    virtual void* CaptureInformationAfter413BE0() = 0;
    virtual void RefreshSaveSelectionAt48DBD0();
};
// 506CE0保留Env slot6、三种栈factory、原Ref及页面+8C ABI；不使用旧shared_ptr工作流。
std::int32_t ExecuteSaveLoadDeleteNativeAt506CE0(SaveLoadDialogNativeBody184h&,
    ISaveLoadDialogNativeDeletionSource&);
}
