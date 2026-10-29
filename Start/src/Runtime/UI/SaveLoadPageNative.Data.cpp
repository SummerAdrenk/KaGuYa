#include <Runtime/SaveLoadPageNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// CP932与重复指针身份逐项取自18号权威PE，不拼接名称或读现代描述符。
constexpr char name5B9124[] = "\x83\x5A\x81\x5B\x83\x75\x8A\xEE\x96\x7B\x2E\x76\x72\x6D";
constexpr char name5B9134[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B914C[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9160[] = "\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B917C[] = "\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B9194[] = "\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B91AC[] = "\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B91C8[] = "\x83\x5E\x83\x75\x88\xDA\x93\xAE\x8D\xB6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B91E0[] = "\x83\x5E\x83\x75\x88\xDA\x93\xAE\x8D\xB6\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B91F4[] = "\x83\x5E\x83\x75\x88\xDA\x93\xAE\x8D\xB6\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9208[] = "\x83\x5E\x83\x75\x88\xDA\x93\xAE\x8D\xB6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9220[] = "\x83\x5E\x83\x75\x88\xDA\x93\xAE\x89\x45\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9238[] = "\x83\x5E\x83\x75\x88\xDA\x93\xAE\x89\x45\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B924C[] = "\x83\x5E\x83\x75\x88\xDA\x93\xAE\x89\x45\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9260[] = "\x83\x5E\x83\x75\x88\xDA\x93\xAE\x89\x45\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9278[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9290[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B92A4[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B92B8[] = "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B92D0[] = "\x83\x8D\x81\x5B\x83\x68\x8A\xEE\x96\x7B\x2E\x76\x72\x6D";
constexpr char name5B92E0[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68\x8A\xEE\x96\x7B\x2E\x76\x72\x6D";
constexpr char name5B92F8[] = "\x83\x49\x81\x5B\x83\x67\x83\x8D\x81\x5B\x83\x68\x8A\xEE\x96\x7B\x2E\x76\x72\x6D";
constexpr char name5B9310[] = "\x4C\x83\x74\x83\x40\x83\x43\x83\x8B\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9328[] = "\x4C\x83\x74\x83\x40\x83\x43\x83\x8B\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B933C[] = "\x4C\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9358[] = "\x4C\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B9370[] = "\x4C\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9388[] = "\x4C\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B93A4[] = "\x4C\x83\x5E\x83\x75\x88\xDA\x93\xAE\x8D\xB6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B93BC[] = "\x4C\x83\x5E\x83\x75\x88\xDA\x93\xAE\x8D\xB6\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B93D0[] = "\x4C\x83\x5E\x83\x75\x88\xDA\x93\xAE\x8D\xB6\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B93E4[] = "\x4C\x83\x5E\x83\x75\x88\xDA\x93\xAE\x8D\xB6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B93FC[] = "\x4C\x83\x5E\x83\x75\x88\xDA\x93\xAE\x89\x45\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B9414[] = "\x4C\x83\x5E\x83\x75\x88\xDA\x93\xAE\x89\x45\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B9428[] = "\x4C\x83\x5E\x83\x75\x88\xDA\x93\xAE\x89\x45\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B943C[] = "\x4C\x83\x5E\x83\x75\x88\xDA\x93\xAE\x89\x45\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B9454[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B946C[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B9480[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B9494[] = "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr GuiStaticDescriptorNative24h row605D14{24,{name5B9124,nullptr,nullptr,name5B9124,name5B9124,nullptr,nullptr},nullptr};
constexpr GuiStaticDescriptorNative24h row606418{24,{name5B92D0,nullptr,nullptr,name5B92D0,name5B92D0,nullptr,nullptr},nullptr};
constexpr GuiStaticDescriptorNative24h row60643C{24,{name5B92E0,nullptr,nullptr,name5B92E0,name5B92E0,nullptr,nullptr},nullptr};
constexpr GuiStaticDescriptorNative24h row606460{24,{name5B92F8,nullptr,nullptr,name5B92F8,name5B92F8,nullptr,nullptr},nullptr};
constexpr GuiButtonDescriptorNative2Ch row606368{{50,{name5B91C8,name5B91E0,nullptr,name5B91F4,name5B91F4,name5B9208,nullptr},nullptr},0,-1};
constexpr GuiButtonDescriptorNative2Ch row606394{{51,{name5B9220,name5B9238,nullptr,name5B924C,name5B924C,name5B9260,nullptr},nullptr},0,-1};
constexpr GuiButtonDescriptorNative2Ch row6063C0{{52,{name5B9278,name5B9290,nullptr,name5B92A4,name5B92A4,name5B92B8,nullptr},nullptr},0,27};
constexpr GuiButtonDescriptorNative2Ch row6064B0{{50,{name5B93A4,name5B93BC,nullptr,name5B93D0,name5B93D0,name5B93E4,nullptr},nullptr},0,-1};
constexpr GuiButtonDescriptorNative2Ch row6064DC{{51,{name5B93FC,name5B9414,nullptr,name5B9428,name5B9428,name5B943C,nullptr},nullptr},0,-1};
constexpr GuiButtonDescriptorNative2Ch row606508{{52,{name5B9454,name5B946C,nullptr,name5B9480,name5B9480,name5B9494,nullptr},nullptr},0,27};
constexpr GuiButtonDescriptorNative2Ch row606310{{25,{name5B9134,nullptr,nullptr,name5B914C,name5B914C,name5B914C,nullptr},nullptr},0,-1};
constexpr GuiButtonDescriptorNative2Ch row60633C{{41,{name5B9160,name5B917C,nullptr,name5B9194,name5B9194,name5B91AC,nullptr},nullptr},0,-1};
constexpr GuiButtonDescriptorNative2Ch row6063EC{{25,{name5B9310,nullptr,nullptr,name5B9328,name5B9328,name5B9328,nullptr},nullptr},0,-1};
constexpr GuiButtonDescriptorNative2Ch row606484{{41,{name5B933C,name5B9358,nullptr,name5B9370,name5B9370,name5B9388,nullptr},nullptr},0,-1};
constexpr std::array<GuiButtonDescriptorNative2Ch,3> saveTop{row606368,row606394,row6063C0};
constexpr std::array<GuiButtonDescriptorNative2Ch,3> loadTop{row6064B0,row6064DC,row606508};
constexpr std::array<POINT,8> positions{{{159,215},{430,215},{701,215},{972,215},{159,418},{430,418},{701,418},{972,418}}};
constexpr GuiDialogNavigationMapping nav6061B8[]{{25,28},{26,25},{27,26},{28,27},{29,32},{30,29},{31,30},{32,31},{-1,0}};
constexpr GuiDialogNavigationMapping nav606170[]{{25,26},{26,27},{27,28},{28,25},{29,30},{30,31},{31,32},{32,29},{-1,0}};
constexpr GuiDialogNavigationMapping nav606200[]{{25,29},{26,30},{27,31},{28,32},{29,25},{30,26},{31,27},{32,28},{-1,0}};
constexpr std::array<SaveLoadPageNativeNavigation,4> navigation{{{3,nav6061B8},{4,nav606170},{1,nav606200},{2,nav606200}}};
}
const GuiStaticDescriptorNative24h& SaveLoadPageNativeBackground(bool load,std::int32_t id)
{
    if(!load)return row605D14;
    switch(id){case 3:return row606418;case 4:return row60643C;case 5:return row606460;}
    // 原背景计算可越出原表；只在该读位置明确报告未编译域，不钳位到合法页面。
    throw std::out_of_range("4EE425 background descriptor outside compiled id3..5 domain");
}
std::span<const GuiButtonDescriptorNative2Ch,3> SaveLoadPageNativeTopButtons(bool load) noexcept
{return load?loadTop:saveTop;}
const GuiButtonDescriptorNative2Ch& SaveLoadPageNativeFileButton(bool load) noexcept
{return load?row6063EC:row606310;}
const GuiButtonDescriptorNative2Ch& SaveLoadPageNativeDeleteButton(bool load) noexcept
{return load?row606484:row60633C;}
std::span<const POINT,8> SaveLoadPageNativeFilePositions() noexcept{return positions;}
std::span<const SaveLoadPageNativeNavigation,4> SaveLoadPageNativeNavigationTable() noexcept{return navigation;}
}
