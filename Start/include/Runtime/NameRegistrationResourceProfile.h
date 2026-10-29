#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
// 11–18的编辑区、字节限长和提交协议相同；本合同只承接已证不同的图与命中槽。
struct NameRegistrationResourceProfile final
{
    std::array<GuiStaticDescriptorNative24h,4> statics;
    GuiStaticDescriptorNative24h panel;
    std::array<GuiButtonDescriptorNative2Ch,2> radios;
    std::array<GuiButtonDescriptorNative2Ch,3> buttons;
};
const NameRegistrationResourceProfile& CurrentNameRegistrationResourceProfile();
}
