#pragma once
#include <Runtime/SettingPage02Native.h>

namespace KaGuYa::Start::Runtime
{
using SettingPage01NativeBody84h = SettingPage02NativeBody84h; // 同原布局，虚表各自独立。
// The parent retains the shared body size; profile data selects its actual child IDs and controls.
class ISettingPage01Block02NativeConstruction
{
public:
    virtual ~ISettingPage01Block02NativeConstruction() = default;
    virtual GuiDialogNativeBody5Ch* ConstructBlock02At4E5510(void* storageB4h,
        GuiDialogNativeBody5Ch* parent84h, std::int32_t id) = 0;
};
const SettingPageNativeVtable39& SettingPage01NativeVtable() noexcept;
const GuiStaticDescriptorNative24h& SettingPage01FooterSample18() noexcept;
SettingPageRadioContainerNativeBody7Ch* ConstructSettingPage01Block00NativeAt4E50B0(void*, GuiDialogNativeBody5Ch*, std::int32_t);
SettingPageRadioContainerNativeBody7Ch* ConstructSettingPage01Block01NativeAt4E52E0(void*, GuiDialogNativeBody5Ch*, std::int32_t);
SettingPage01NativeBody84h* ConstructSettingPage01NativeAt4F1EE0(void*, GuiDialogNativeBody5Ch*, std::int32_t,
    ISettingPage01Block02NativeConstruction& requiredPreview);
SettingPage01NativeBody84h* DeleteSettingPage01NativeAt4FE320(SettingPage01NativeBody84h*, std::uint32_t) noexcept;
// 直接实现冻结父的两条叶；03/04只转交显式native provider，01明确需要B4依赖。
// 长寿SettingPage02NativeScope由宿主安装，不只包Construct后返回悬空服务。
class SettingPage01And02NativeConstruction final : public ISettingDialogNativePageConstruction
{
public:
    SettingPage01And02NativeConstruction(ISettingPage01Block02NativeConstruction& preview,
        ISettingDialogNativePageConstruction& later) noexcept : preview_(preview), later_(later) {}
    GuiDialogNativeBody5Ch* ConstructPage01At4F1EE0(void*, GuiDialogNativeBody5Ch*, std::int32_t) override;
    GuiDialogNativeBody5Ch* ConstructPage02At4F23B0(void*, GuiDialogNativeBody5Ch*, std::int32_t) override;
    GuiDialogNativeBody5Ch* ConstructPage03At4F2910(void* raw, GuiDialogNativeBody5Ch* p, std::int32_t id) override
    { return later_.ConstructPage03At4F2910(raw, p, id); }
    GuiDialogNativeBody5Ch* ConstructPage04At4F2FE0(void* raw, GuiDialogNativeBody5Ch* p, std::int32_t id) override
    { return later_.ConstructPage04At4F2FE0(raw, p, id); }
private:
    ISettingPage01Block02NativeConstruction& preview_;
    ISettingDialogNativePageConstruction& later_;
};
}
