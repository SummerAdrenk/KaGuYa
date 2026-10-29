#pragma once
#include <Runtime/PreWindowInitialization.h>
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
// 只借本次选定Root的业务来源；错误producer写未构造返回槽，不复制成std::string。
class IPreWindowInitializationNativeServices
{
public:
    virtual ~IPreWindowInitializationNativeServices() = default;
    virtual void RegisterEnvironmentArchivesAt47E2B0() = 0;
    virtual bool PrepareEnvironmentPersistenceAt480230() = 0;
    virtual void WriteEnvironmentPersistenceAt488740() = 0;
    virtual void InstallWindowModeAt47791D() = 0;
    virtual const SaveDataEnvironmentArchive& SaveDataAtSlot1() = 0;
    virtual ClickInfoArchive& ClickInfoAtSlot5() = 0;
    virtual void BuildInitializationErrorTextAt479EA0(RuntimeCString& outRaw) = 0;
    virtual int MessageBoxA(HWND, const char*, const char*, UINT) = 0;
};
// 原4778B0：四次原宽度复位、业务catch和state4错误尾各有独立资格。
bool RunPreWindowInitializationNativeAt4778B0(PreWindowLifecycleState&, IPreWindowInitializationNativeServices&);
}
