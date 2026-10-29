#pragma once

namespace KaGuYa::Start::Runtime
{
struct GuiDialogNativeBody5Ch;
struct SettingDialogNativeRuntimeServices;
class ISettingDialogNativeConstruction
{
public:
    virtual ~ISettingDialogNativeConstruction() = default;
    virtual GuiDialogNativeBody5Ch* ConstructSettingDialogAt4F1BF0(void* storageACh) = 0;
    // 正文外元数据：隔离测试可不绑定，生产声明同一父/四页服务，不能成功兜底。
    virtual SettingDialogNativeRuntimeServices* NativeRuntimeServices() const noexcept{return nullptr;}
};
}
