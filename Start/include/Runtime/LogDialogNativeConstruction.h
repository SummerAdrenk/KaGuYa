#pragma once

namespace KaGuYa::Start::Runtime
{
struct GuiDialogNativeBody5Ch;
struct LogDialogNativeServices;
// 只接受传入DC原存储上的真实GUI，不接受shared_ptr或现代MessageLogDialog桥。
class ILogDialogNativeConstruction
{
public:
    virtual ~ILogDialogNativeConstruction()=default;
    virtual GuiDialogNativeBody5Ch* ConstructLogDialogAt4E23D0(void* rawDCh)=0;
    virtual LogDialogNativeServices* NativeDialogServices() const noexcept{return nullptr;}
};
}
