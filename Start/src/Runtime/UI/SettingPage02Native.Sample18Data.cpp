#include <Runtime/SettingPage01Native.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原PE资源/POINT只读提取；所有VA只作证据，不在运行时解析或执行。
constexpr char text_5BD218[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x2E\x76\x72\x6D";
constexpr char text_5BD22C[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char text_5BD250[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
constexpr char text_5BD27C[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char text_5BD2A4[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char text_5BD2CC[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
constexpr char text_5BD2F8[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char text_5BD718[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x2E\x76\x72\x6D";
constexpr char text_5BD72C[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char text_5BD74C[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char text_5BD774[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char text_5BD798[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char text_5BD7B8[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char text_5BD7E0[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char text_5BD804[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x58\x83\x4C\x83\x62\x83\x76\x2E\x76\x72\x6D";
constexpr char text_5BD82C[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x58\x83\x4C\x83\x62\x83\x76\x2E\x76\x72\x6D";
constexpr char text_5BD85C[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x58\x83\x4C\x83\x62\x83\x76\x2E\x76\x72\x6D";
constexpr char text_5BD888[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x49\x81\x5B\x83\x67\x2E\x76\x72\x6D";
constexpr char text_5BD8B0[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x49\x81\x5B\x83\x67\x2E\x76\x72\x6D";
constexpr char text_5BD8DC[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x49\x81\x5B\x83\x67\x2E\x76\x72\x6D";
constexpr char text_5BD908[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x82\xCC\x8F\xC1\x8B\x8E\x2E\x76\x72\x6D";
constexpr char text_5BD938[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x82\xCC\x8F\xC1\x8B\x8E\x2E\x76\x72\x6D";
constexpr char text_5BD970[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x82\xCC\x8F\xC1\x8B\x8E\x2E\x76\x72\x6D";
constexpr char text_5BD9A4[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x2E\x76\x72\x6D";
constexpr char text_5BD9C4[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x2E\x76\x72\x6D";
constexpr char text_5BD9EC[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x2E\x76\x72\x6D";
constexpr char text_5BDA10[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char text_5BDA34[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char text_5BDA60[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char text_5BDA88[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xE6\x96\xCA\x90\xDD\x92\xE8\x2E\x76\x72\x6D";
constexpr POINT point_6055B0{480, 188};
constexpr POINT point_6055B8{571, 188};
constexpr POINT point_6055C0{480, 233};
constexpr POINT point_6055C8{571, 233};
constexpr POINT point_6055D0{480, 278};
constexpr POINT point_6055D8{571, 278};
constexpr POINT point_6055E0{480, 323};
constexpr POINT point_6055E8{571, 323};
constexpr POINT point_6055F0{480, 368};
constexpr POINT point_6055F8{571, 368};
constexpr POINT point_605600{480, 471};
constexpr POINT point_605608{571, 471};
constexpr POINT point_605610{480, 516};
constexpr POINT point_605618{571, 516};
constexpr POINT point_605620{480, 561};
constexpr POINT point_605628{571, 561};
constexpr POINT point_605630{480, 606};
constexpr POINT point_605638{571, 606};
constexpr POINT point_605978{327, 188};
constexpr POINT point_605980{418, 188};
constexpr POINT point_605988{327, 233};
constexpr POINT point_605990{418, 233};
constexpr POINT point_605998{327, 278};
constexpr POINT point_6059A0{418, 278};
constexpr POINT point_6059A8{327, 323};
constexpr POINT point_6059B0{418, 323};
constexpr POINT point_6059B8{327, 368};
constexpr POINT point_6059C0{418, 368};
constexpr POINT point_6059C8{327, 413};
constexpr POINT point_6059D0{418, 413};
constexpr POINT point_6059D8{327, 458};
constexpr POINT point_6059E0{418, 458};
constexpr POINT point_6059E8{327, 503};
constexpr POINT point_6059F0{418, 503};
constexpr POINT point_6059F8{327, 548};
constexpr POINT point_605A00{418, 548};
constexpr POINT point_605A08{327, 593};
constexpr POINT point_605A10{418, 593};
constexpr POINT point_605A18{928, 352};
constexpr POINT point_605A20{1019, 352};
constexpr POINT point_605A28{928, 397};
constexpr POINT point_605A30{1019, 397};
constexpr POINT point_605A38{928, 442};
constexpr POINT point_605A40{1019, 442};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p01b0_0_buttons{{
    {{50, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_6055B0}, 7, -1},
    {{51, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_6055B8}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p01b0_1_buttons{{
    {{52, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_6055C0}, 7, -1},
    {{53, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_6055C8}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p01b0_2_buttons{{
    {{54, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_6055D0}, 7, -1},
    {{55, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_6055D8}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p01b0_3_buttons{{
    {{56, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_6055E0}, 7, -1},
    {{57, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_6055E8}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p01b0_4_buttons{{
    {{58, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_6055F0}, 7, -1},
    {{59, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_6055F8}, 7, -1}}};
const std::array<SettingPageRadioGroupNative, 5> p01b0_groups{{
    {39, p01b0_0_buttons},
    {40, p01b0_1_buttons},
    {41, p01b0_2_buttons},
    {42, p01b0_3_buttons},
    {43, p01b0_4_buttons}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p01b1_0_buttons{{
    {{60, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_605600}, 7, -1},
    {{61, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_605608}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p01b1_1_buttons{{
    {{62, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_605610}, 7, -1},
    {{63, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_605618}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p01b1_2_buttons{{
    {{64, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_605620}, 7, -1},
    {{65, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_605628}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p01b1_3_buttons{{
    {{66, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_605630}, 7, -1},
    {{67, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_605638}, 7, -1}}};
const std::array<SettingPageRadioGroupNative, 4> p01b1_groups{{
    {44, p01b1_0_buttons},
    {45, p01b1_1_buttons},
    {46, p01b1_2_buttons},
    {47, p01b1_3_buttons}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b0_0_buttons{{
    {{81, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_605978}, 7, -1},
    {{82, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_605980}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b0_1_buttons{{
    {{83, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_605988}, 7, -1},
    {{84, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_605990}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b0_2_buttons{{
    {{85, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_6059A8}, 7, -1},
    {{86, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_6059B0}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b0_3_buttons{{
    {{87, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_6059B8}, 7, -1},
    {{88, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_6059C0}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b0_4_buttons{{
    {{89, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_605998}, 7, -1},
    {{90, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_6059A0}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b0_5_buttons{{
    {{91, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_605A08}, 7, -1},
    {{92, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_605A10}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b0_6_buttons{{
    {{93, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_6059F8}, 7, -1},
    {{94, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_605A00}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b0_7_buttons{{
    {{95, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_6059C8}, 7, -1},
    {{96, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_6059D0}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b0_8_buttons{{
    {{97, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_6059E8}, 7, -1},
    {{98, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_6059F0}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b0_9_buttons{{
    {{99, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_6059D8}, 7, -1},
    {{100, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_6059E0}, 7, -1}}};
const std::array<SettingPageRadioGroupNative, 10> p02b0_groups{{
    {71, p02b0_0_buttons},
    {72, p02b0_1_buttons},
    {73, p02b0_2_buttons},
    {74, p02b0_3_buttons},
    {75, p02b0_4_buttons},
    {76, p02b0_5_buttons},
    {77, p02b0_6_buttons},
    {78, p02b0_7_buttons},
    {79, p02b0_8_buttons},
    {80, p02b0_9_buttons}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b3_0_buttons{{
    {{112, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_605A18}, 7, -1},
    {{113, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_605A20}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b3_1_buttons{{
    {{114, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_605A28}, 7, -1},
    {{115, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_605A30}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b3_2_buttons{{
    {{116, {nullptr, text_5BD22C, nullptr, text_5BD250, text_5BD250, text_5BD27C, nullptr}, &point_605A38}, 7, -1},
    {{117, {nullptr, text_5BD2A4, nullptr, text_5BD2CC, text_5BD2CC, text_5BD2F8, nullptr}, &point_605A40}, 7, -1}}};
const std::array<SettingPageRadioGroupNative, 3> p02b3_groups{{
    {109, p02b3_0_buttons},
    {110, p02b3_1_buttons},
    {111, p02b3_2_buttons}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 5> p02b1{{
    {{101, {nullptr, text_5BD72C, nullptr, text_5BD74C, text_5BD74C, text_5BD774, nullptr}, nullptr}, 7, -1},
    {{102, {nullptr, text_5BD798, nullptr, text_5BD7B8, text_5BD7B8, text_5BD7E0, nullptr}, nullptr}, 7, -1},
    {{103, {nullptr, text_5BD804, nullptr, text_5BD82C, text_5BD82C, text_5BD85C, nullptr}, nullptr}, 7, -1},
    {{104, {nullptr, text_5BD888, nullptr, text_5BD8B0, text_5BD8B0, text_5BD8DC, nullptr}, nullptr}, 7, -1},
    {{105, {nullptr, text_5BD908, nullptr, text_5BD938, text_5BD938, text_5BD970, nullptr}, nullptr}, 7, -1}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 2> p02b2buttons{{
    {{106, {nullptr, text_5BD9A4, nullptr, text_5BD9C4, text_5BD9C4, text_5BD9EC, nullptr}, nullptr}, 7, -1},
    {{107, {nullptr, text_5BDA10, nullptr, text_5BDA34, text_5BDA34, text_5BDA60, nullptr}, nullptr}, 7, -1}}};
constexpr GuiStaticDescriptorNative24h p02b2static = {108, {nullptr, text_5BDA88, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr};
constexpr GuiStaticDescriptorNative24h p01footer = {27, {text_5BD218, nullptr, nullptr, text_5BD218, text_5BD218, nullptr, nullptr}, nullptr};
constexpr GuiStaticDescriptorNative24h p02footer = {28, {text_5BD718, nullptr, nullptr, text_5BD718, text_5BD718, nullptr, nullptr}, nullptr};
constexpr std::array<std::uint32_t, 39> original_p01{
    0x509ED0, 0x4FE320, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x5363C0, 0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510B90, 0x5106B0, 0x511610, 0x50C900, 0x52D450, 0x508CF0, 0x50AB50, 0x518C30, 0x525FB0, 0x524D70, 0x524D20, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80, 0x4FF130, 0x509DE0, 0x506460, 0x50A240};
constexpr std::array<std::uint32_t, 39> original_p02{
    0x509ED0, 0x4FE350, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x5363C0, 0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510B90, 0x5106B0, 0x511610, 0x50C900, 0x52D450, 0x508CF0, 0x50AB50, 0x518C30, 0x525FB0, 0x524D70, 0x524D20, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80, 0x4FF130, 0x509DE0, 0x506600, 0x50A240};
constexpr std::array<std::uint32_t, 40> original_p01b0{
    0x509ED0, 0x4FCF50, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x5363C0, 0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510B90, 0x5106B0, 0x511610, 0x50C8E0, 0x52D450, 0x508CE0, 0x50AB50, 0x518C30, 0x525FB0, 0x523480, 0x523370, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80, 0x4FF130, 0x509DE0, 0x50A1E0, 0x512680, 0x515B70};
constexpr std::array<std::uint32_t, 40> original_p01b1{
    0x509ED0, 0x4FCF90, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x5363C0, 0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510B90, 0x5106B0, 0x511610, 0x50C8E0, 0x52D450, 0x508CE0, 0x50AB50, 0x518C30, 0x525FB0, 0x523480, 0x523370, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80, 0x4FF130, 0x509DE0, 0x50A1E0, 0x512960, 0x515E60};
constexpr std::array<std::uint32_t, 40> original_p02b0{
    0x509ED0, 0x4FD130, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x5363C0, 0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510B90, 0x5106B0, 0x511610, 0x50C8E0, 0x52E370, 0x508CE0, 0x50AB50, 0x518C30, 0x525FB0, 0x523480, 0x523370, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80, 0x4FF130, 0x509DE0, 0x50A1E0, 0x512F40, 0x516280};
constexpr std::array<std::uint32_t, 38> original_p02b1{
    0x509ED0, 0x4FD170, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x536310, 0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510B90, 0x5106B0, 0x511610, 0x50BF50, 0x52F750, 0x508BD0, 0x50AB50, 0x518C30, 0x525FB0, 0x5231C0, 0x5230B0, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80, 0x50A0B0, 0x5134D0, 0x5166D0};
constexpr std::array<std::uint32_t, 35> original_p02b2{
    0x509ED0, 0x4FD210, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x536310, 0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510B90, 0x5106B0, 0x511610, 0x50D9A0, 0x52E5F0, 0x509100, 0x50AB50, 0x518C30, 0x525FB0, 0x5231C0, 0x5230B0, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80};
constexpr std::array<std::uint32_t, 40> original_p02b3{
    0x509ED0, 0x4FD2B0, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x5363C0, 0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510B90, 0x5106B0, 0x511610, 0x50C8E0, 0x52D450, 0x508CE0, 0x50AB50, 0x518C30, 0x525FB0, 0x523480, 0x523370, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80, 0x4FF130, 0x509DE0, 0x50A1E0, 0x5135B0, 0x5167A0};
constexpr std::array<std::uint32_t, 38> original_radio{
    0x509ED0, 0x4FC5A0, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x536310, 0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510B90, 0x5106B0, 0x511610, 0x50BF50, 0x52F750, 0x508BD0, 0x50AB50, 0x518C30, 0x525FB0, 0x5231C0, 0x5230B0, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80, 0x50A0B0, 0x512670, 0x515B60};
constexpr std::array<std::uint32_t, 39> original_parentbase{
    0x509ED0, 0x4FE3E0, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x5363C0, 0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510B90, 0x5106B0, 0x511610, 0x50C900, 0x52D450, 0x508CF0, 0x50AB50, 0x518C30, 0x525FB0, 0x524D70, 0x524D20, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80, 0x4FF130, 0x509DE0, 0x541C5F, 0x50A240};
constexpr std::array<std::uint32_t, 38> original_choicebase{
    0x509ED0, 0x4FBCF0, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x536310, 0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510B90, 0x5106B0, 0x511610, 0x50BF50, 0x541C5F, 0x508BD0, 0x50AB50, 0x518C30, 0x525FB0, 0x5231C0, 0x5230B0, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80, 0x50A0B0, 0x541C5F, 0x541C5F};
constexpr std::array<std::uint32_t, 28> unwind_4F1EE0{
    0xFFFFFFFF, 0x5A2D70, 0x0, 0x5A2D78, 0x0, 0x5A2D87, 0x0, 0x541E55, 0x0, 0x5A2D8F, 0x0, 0x5A2D9E, 0x0, 0x541E55, 0x0, 0x5A2DA6, 0x0, 0x5A2DB8, 0x0, 0x541E55, 0x0, 0x5A2DC0, 0xA, 0x5A2DC8, 0xA, 0x541E55, 0x0, 0x541E55};
constexpr std::array<std::uint32_t, 30> unwind_4F23B0{
    0xFFFFFFFF, 0x5A2E00, 0x0, 0x5A2E08, 0x0, 0x5A2E17, 0x0, 0x541E55, 0x0, 0x5A2E1F, 0x0, 0x5A2E31, 0x0, 0x541E55, 0x0, 0x5A2E39, 0x0, 0x5A2E48, 0x0, 0x541E55, 0x0, 0x5A2E50, 0x0, 0x5A2E5F, 0x0, 0x541E55, 0x0, 0x5A2E67, 0x0, 0x541E55};
constexpr std::array<std::uint32_t, 12> unwind_4E50B0{
    0xFFFFFFFF, 0x5A1AE0, 0x0, 0x5A1AE8, 0x0, 0x5A1AFA, 0x2, 0x5A1B02, 0x2, 0x541E55, 0x0, 0x541E55};
constexpr std::array<std::uint32_t, 12> unwind_4E52E0{
    0xFFFFFFFF, 0x5A1B30, 0x0, 0x5A1B38, 0x0, 0x5A1B4A, 0x2, 0x5A1B52, 0x2, 0x541E55, 0x0, 0x541E55};
constexpr std::array<std::uint32_t, 12> unwind_4E5E40{
    0xFFFFFFFF, 0x5A1C60, 0x0, 0x5A1C68, 0x0, 0x5A1C7A, 0x2, 0x5A1C82, 0x2, 0x541E55, 0x0, 0x541E55};
constexpr std::array<std::uint32_t, 22> unwind_4E6070{
    0xFFFFFFFF, 0x5A1CB0, 0x0, 0x5A1CBB, 0x1, 0x5A1CC3, 0x2, 0x5A1CCB, 0x2, 0x541E55, 0x2, 0x5A1CD6, 0x2, 0x541E55, 0x2, 0x5A1CDE, 0x2, 0x541E55, 0x1, 0x541E55, 0x0, 0x541E55};
constexpr std::array<std::uint32_t, 24> unwind_4E64D0{
    0xFFFFFFFF, 0x5A1D10, 0x0, 0x5A1D1B, 0x1, 0x5A1D29, 0x2, 0x5A1D31, 0x3, 0x5A1D39, 0x3, 0x541E55, 0x3, 0x5A1D44, 0x3, 0x541E55, 0x3, 0x5A1D4C, 0x3, 0x541E55, 0x2, 0x541E55, 0x1, 0x541E55};
constexpr std::array<std::uint32_t, 12> unwind_4E6930{
    0xFFFFFFFF, 0x5A1D80, 0x0, 0x5A1D88, 0x0, 0x5A1D9A, 0x2, 0x5A1DA2, 0x2, 0x541E55, 0x0, 0x541E55};
constexpr std::array<std::uint32_t, 22> unwind_4E07B0{
    0xFFFFFFFF, 0x5A13F0, 0x0, 0x5A13FB, 0x1, 0x5A1403, 0x2, 0x5A140B, 0x2, 0x541E55, 0x2, 0x5A1413, 0x2, 0x541E55, 0x2, 0x5A141B, 0x2, 0x541E55, 0x1, 0x541E55, 0x0, 0x541E55};
}
std::span<const SettingPageRadioGroupNative> SettingPageRadioGroupsSample18(SettingPageRadioContainerKind kind) noexcept
{
    switch (kind)
    {
    case SettingPageRadioContainerKind::Page01Block00: return p01b0_groups;
    case SettingPageRadioContainerKind::Page01Block01: return p01b1_groups;
    case SettingPageRadioContainerKind::Page02Block00: return p02b0_groups;
    case SettingPageRadioContainerKind::Page02Block03: return p02b3_groups;
    }
    return {};
}
std::span<const GuiButtonDescriptorNative2Ch, 5> SettingPage02Block01ButtonsSample18() noexcept { return p02b1; }
std::span<const GuiButtonDescriptorNative2Ch, 2> SettingPage02Block02ButtonsSample18() noexcept { return p02b2buttons; }
const GuiStaticDescriptorNative24h& SettingPage02Block02StaticSample18() noexcept { return p02b2static; }
const GuiStaticDescriptorNative24h& SettingPage01FooterSample18() noexcept { return p01footer; }
const GuiStaticDescriptorNative24h& SettingPage02FooterSample18() noexcept { return p02footer; }
std::span<const std::uint32_t> SettingPage02OriginalTableSample18(std::uint32_t index) noexcept
{
    switch (index)
    {
    case 0: return original_p01;
    case 1: return original_p02;
    case 2: return original_p01b0;
    case 3: return original_p01b1;
    case 4: return original_p02b0;
    case 5: return original_p02b1;
    case 6: return original_p02b2;
    case 7: return original_p02b3;
    case 8: return original_radio;
    case 9: return original_parentbase;
    case 10: return original_choicebase;
    }
    return {};
}
std::span<const std::uint32_t> SettingPage02OriginalUnwindSample18(std::uint32_t index) noexcept
{
    switch (index)
    {
    case 0: return unwind_4F1EE0;
    case 1: return unwind_4F23B0;
    case 2: return unwind_4E50B0;
    case 3: return unwind_4E52E0;
    case 4: return unwind_4E5E40;
    case 5: return unwind_4E6070;
    case 6: return unwind_4E64D0;
    case 7: return unwind_4E6930;
    case 8: return unwind_4E07B0;
    }
    return {};
}
}

