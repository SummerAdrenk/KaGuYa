#include "SettingsTextDisplayData.h"

namespace KaGuYa::Start::Runtime::NeeChanNoSusumeOneeChanNoItazuraSeiseikatsuSettingsTextDisplayData
{
namespace
{
// Original CP932 sample is 14 bytes (seven double-byte characters), excluding NUL.
constexpr char s0[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x2E\x76\x72\x6D";
constexpr char s1[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x2E\x76\x72\x6D";
constexpr char s2[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char s3[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
constexpr char s4[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char s5[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char s6[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
constexpr char s7[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char s8[] = "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D";
constexpr char s9[] = "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char s10[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x49\x83\x74\x92\xCA\x8F\xED\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s11[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x92\xCA\x8F\xED\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s12[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x4E\x83\x8A\x83\x62\x83\x4E\x92\xCA\x8F\xED\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s13[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x49\x83\x74\x48\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s14[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x48\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s15[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x4E\x83\x8A\x83\x62\x83\x4E\x48\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s16[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char s17[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char s18[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char s19[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char s20[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char s21[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
constexpr char s22[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x58\x83\x4C\x83\x62\x83\x76\x2E\x76\x72\x6D";
constexpr char s23[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x58\x83\x4C\x83\x62\x83\x76\x2E\x76\x72\x6D";
constexpr char s24[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x58\x83\x4C\x83\x62\x83\x76\x2E\x76\x72\x6D";
constexpr char s25[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x49\x81\x5B\x83\x67\x2E\x76\x72\x6D";
constexpr char s26[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x49\x81\x5B\x83\x67\x2E\x76\x72\x6D";
constexpr char s27[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x49\x81\x5B\x83\x67\x2E\x76\x72\x6D";
constexpr char s28[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x82\xCC\x8F\xC1\x8B\x8E\x2E\x76\x72\x6D";
constexpr char s29[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x82\xCC\x8F\xC1\x8B\x8E\x2E\x76\x72\x6D";
constexpr char s30[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x82\xCC\x8F\xC1\x8B\x8E\x2E\x76\x72\x6D";
constexpr char s31[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x2E\x76\x72\x6D";
constexpr char s32[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x2E\x76\x72\x6D";
constexpr char s33[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x2E\x76\x72\x6D";
constexpr char s34[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x49\x83\x74\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s35[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s36[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x32\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s37[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x54\x83\x93\x83\x76\x83\x8B\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x2E\x76\x72\x6D";
constexpr char s38[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x54\x83\x93\x83\x76\x83\x8B\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x32\x2E\x76\x72\x6D";
constexpr char s39[] = "\x82\x6C\x82\x72\x20\x83\x53\x83\x56\x83\x62\x83\x4E";
constexpr char s40[] = "\x83\x41\x83\x67\x83\x8A\x83\x47\x82\xA9\x82\xAE\x82\xE2";
constexpr POINT p0{480,188};
constexpr POINT p1{571,188};
constexpr POINT p2{480,233};
constexpr POINT p3{571,233};
constexpr POINT p4{480,278};
constexpr POINT p5{571,278};
constexpr POINT p6{480,323};
constexpr POINT p7{571,323};
constexpr POINT p8{480,368};
constexpr POINT p9{571,368};
constexpr POINT p10{480,471};
constexpr POINT p11{571,471};
constexpr POINT p12{480,516};
constexpr POINT p13{571,516};
constexpr POINT p14{480,561};
constexpr POINT p15{571,561};
constexpr POINT p16{480,606};
constexpr POINT p17{571,606};
constexpr POINT p18{327,188};
constexpr POINT p19{418,188};
constexpr POINT p20{327,233};
constexpr POINT p21{418,233};
constexpr POINT p22{327,323};
constexpr POINT p23{418,323};
constexpr POINT p24{327,368};
constexpr POINT p25{418,368};
constexpr POINT p26{327,278};
constexpr POINT p27{418,278};
constexpr POINT p28{327,593};
constexpr POINT p29{418,593};
constexpr POINT p30{327,548};
constexpr POINT p31{418,548};
constexpr POINT p32{327,413};
constexpr POINT p33{418,413};
constexpr POINT p34{327,503};
constexpr POINT p35{418,503};
constexpr POINT p36{327,458};
constexpr POINT p37{418,458};
constexpr POINT p38{928,352};
constexpr POINT p39{1019,352};
constexpr POINT p40{928,397};
constexpr POINT p41{1019,397};
constexpr POINT p42{928,442};
constexpr POINT p43{1019,442};
// Original descriptor 0x55C0B4, raw 0x15A8B4.
constexpr GuiStaticDescriptorNative24h d0{25,{s0,nullptr,nullptr,s0,s0,nullptr,nullptr},nullptr};
// Original descriptor 0x55C74C, raw 0x15AF4C.
constexpr GuiStaticDescriptorNative24h d1{26,{s1,nullptr,nullptr,s1,s1,nullptr,nullptr},nullptr};
// Original descriptor 0x55C258, raw 0x15AA58.
constexpr GuiButtonDescriptorNative2Ch d2{{48,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p0},7,-1};
// Original descriptor 0x55C284, raw 0x15AA84.
constexpr GuiButtonDescriptorNative2Ch d3{{49,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p1},7,-1};
// Original descriptor 0x55C2B0, raw 0x15AAB0.
constexpr GuiButtonDescriptorNative2Ch d4{{50,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p2},7,-1};
// Original descriptor 0x55C2DC, raw 0x15AADC.
constexpr GuiButtonDescriptorNative2Ch d5{{51,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p3},7,-1};
// Original descriptor 0x55C308, raw 0x15AB08.
constexpr GuiButtonDescriptorNative2Ch d6{{52,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p4},7,-1};
// Original descriptor 0x55C334, raw 0x15AB34.
constexpr GuiButtonDescriptorNative2Ch d7{{53,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p5},7,-1};
// Original descriptor 0x55C360, raw 0x15AB60.
constexpr GuiButtonDescriptorNative2Ch d8{{54,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p6},7,-1};
// Original descriptor 0x55C38C, raw 0x15AB8C.
constexpr GuiButtonDescriptorNative2Ch d9{{55,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p7},7,-1};
// Original descriptor 0x55C3B8, raw 0x15ABB8.
constexpr GuiButtonDescriptorNative2Ch d10{{56,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p8},7,-1};
// Original descriptor 0x55C3E4, raw 0x15ABE4.
constexpr GuiButtonDescriptorNative2Ch d11{{57,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p9},7,-1};
// Original descriptor 0x55C410, raw 0x15AC10.
constexpr GuiButtonDescriptorNative2Ch d12{{58,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p10},7,-1};
// Original descriptor 0x55C43C, raw 0x15AC3C.
constexpr GuiButtonDescriptorNative2Ch d13{{59,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p11},7,-1};
// Original descriptor 0x55C468, raw 0x15AC68.
constexpr GuiButtonDescriptorNative2Ch d14{{60,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p12},7,-1};
// Original descriptor 0x55C494, raw 0x15AC94.
constexpr GuiButtonDescriptorNative2Ch d15{{61,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p13},7,-1};
// Original descriptor 0x55C4C0, raw 0x15ACC0.
constexpr GuiButtonDescriptorNative2Ch d16{{62,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p14},7,-1};
// Original descriptor 0x55C4EC, raw 0x15ACEC.
constexpr GuiButtonDescriptorNative2Ch d17{{63,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p15},7,-1};
// Original descriptor 0x55C518, raw 0x15AD18.
constexpr GuiButtonDescriptorNative2Ch d18{{64,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p16},7,-1};
// Original descriptor 0x55C544, raw 0x15AD44.
constexpr GuiButtonDescriptorNative2Ch d19{{65,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p17},7,-1};
// Original descriptor 0x55C5F8, raw 0x15ADF8.
constexpr GuiSliderDescriptorNative6Ch d20{66,{s8,s9,nullptr,s8,s8,s8,nullptr},{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},nullptr,0,0,1,{994,204,1144,223},{0,0,-12,0}};
// Original descriptor 0x55C664, raw 0x15AE64.
constexpr GuiSliderDescriptorNative6Ch d21{67,{s8,s9,nullptr,s8,s8,s8,nullptr},{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},nullptr,0,0,0,{994,238,1144,257},{0,0,-12,0}};
// Original descriptor 0x55C6D0, raw 0x15AED0.
constexpr GuiSliderDescriptorNative6Ch d22{68,{s8,s9,nullptr,s8,s8,s8,nullptr},{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},nullptr,0,0,0,{750,321,1170,340},{0,0,-12,0}};
// Original descriptor 0x55C5A0, raw 0x15ADA0.
constexpr GuiButtonDescriptorNative2Ch d23{{46,{nullptr,s10,nullptr,s11,s11,s12,nullptr},nullptr},7,-1};
// Original descriptor 0x55C5CC, raw 0x15ADCC.
constexpr GuiButtonDescriptorNative2Ch d24{{47,{nullptr,s13,nullptr,s14,s14,s15,nullptr},nullptr},7,-1};
// Original descriptor 0x55C868, raw 0x15B068.
constexpr GuiButtonDescriptorNative2Ch d25{{79,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p18},7,-1};
// Original descriptor 0x55C894, raw 0x15B094.
constexpr GuiButtonDescriptorNative2Ch d26{{80,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p19},7,-1};
// Original descriptor 0x55C8C0, raw 0x15B0C0.
constexpr GuiButtonDescriptorNative2Ch d27{{81,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p20},7,-1};
// Original descriptor 0x55C8EC, raw 0x15B0EC.
constexpr GuiButtonDescriptorNative2Ch d28{{82,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p21},7,-1};
// Original descriptor 0x55C918, raw 0x15B118.
constexpr GuiButtonDescriptorNative2Ch d29{{83,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p22},7,-1};
// Original descriptor 0x55C944, raw 0x15B144.
constexpr GuiButtonDescriptorNative2Ch d30{{84,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p23},7,-1};
// Original descriptor 0x55C970, raw 0x15B170.
constexpr GuiButtonDescriptorNative2Ch d31{{85,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p24},7,-1};
// Original descriptor 0x55C99C, raw 0x15B19C.
constexpr GuiButtonDescriptorNative2Ch d32{{86,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p25},7,-1};
// Original descriptor 0x55C9C8, raw 0x15B1C8.
constexpr GuiButtonDescriptorNative2Ch d33{{87,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p26},7,-1};
// Original descriptor 0x55C9F4, raw 0x15B1F4.
constexpr GuiButtonDescriptorNative2Ch d34{{88,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p27},7,-1};
// Original descriptor 0x55CA20, raw 0x15B220.
constexpr GuiButtonDescriptorNative2Ch d35{{89,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p28},7,-1};
// Original descriptor 0x55CA4C, raw 0x15B24C.
constexpr GuiButtonDescriptorNative2Ch d36{{90,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p29},7,-1};
// Original descriptor 0x55CA78, raw 0x15B278.
constexpr GuiButtonDescriptorNative2Ch d37{{91,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p30},7,-1};
// Original descriptor 0x55CAA4, raw 0x15B2A4.
constexpr GuiButtonDescriptorNative2Ch d38{{92,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p31},7,-1};
// Original descriptor 0x55CAD0, raw 0x15B2D0.
constexpr GuiButtonDescriptorNative2Ch d39{{93,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p32},7,-1};
// Original descriptor 0x55CAFC, raw 0x15B2FC.
constexpr GuiButtonDescriptorNative2Ch d40{{94,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p33},7,-1};
// Original descriptor 0x55CB28, raw 0x15B328.
constexpr GuiButtonDescriptorNative2Ch d41{{95,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p34},7,-1};
// Original descriptor 0x55CB54, raw 0x15B354.
constexpr GuiButtonDescriptorNative2Ch d42{{96,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p35},7,-1};
// Original descriptor 0x55CB80, raw 0x15B380.
constexpr GuiButtonDescriptorNative2Ch d43{{97,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p36},7,-1};
// Original descriptor 0x55CBAC, raw 0x15B3AC.
constexpr GuiButtonDescriptorNative2Ch d44{{98,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p37},7,-1};
// Original descriptor 0x55CC50, raw 0x15B450.
constexpr GuiButtonDescriptorNative2Ch d45{{99,{nullptr,s16,nullptr,s17,s17,s18,nullptr},nullptr},7,-1};
// Original descriptor 0x55CC7C, raw 0x15B47C.
constexpr GuiButtonDescriptorNative2Ch d46{{100,{nullptr,s19,nullptr,s20,s20,s21,nullptr},nullptr},7,-1};
// Original descriptor 0x55CCA8, raw 0x15B4A8.
constexpr GuiButtonDescriptorNative2Ch d47{{101,{nullptr,s22,nullptr,s23,s23,s24,nullptr},nullptr},7,-1};
// Original descriptor 0x55CCD4, raw 0x15B4D4.
constexpr GuiButtonDescriptorNative2Ch d48{{102,{nullptr,s25,nullptr,s26,s26,s27,nullptr},nullptr},7,-1};
// Original descriptor 0x55CD00, raw 0x15B500.
constexpr GuiButtonDescriptorNative2Ch d49{{103,{nullptr,s28,nullptr,s29,s29,s30,nullptr},nullptr},7,-1};
// Original descriptor 0x55CD50, raw 0x15B550.
constexpr GuiButtonDescriptorNative2Ch d50{{104,{nullptr,s31,nullptr,s32,s32,s33,nullptr},nullptr},7,-1};
// Original descriptor 0x55CD7C, raw 0x15B57C.
constexpr GuiButtonDescriptorNative2Ch d51{{105,{nullptr,s34,nullptr,s35,s35,s36,nullptr},nullptr},7,-1};
// Original descriptor 0x55CDA8, raw 0x15B5A8.
constexpr GuiButtonDescriptorNative2Ch d52{{109,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p38},7,-1};
// Original descriptor 0x55CDD4, raw 0x15B5D4.
constexpr GuiButtonDescriptorNative2Ch d53{{110,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p39},7,-1};
// Original descriptor 0x55CE00, raw 0x15B600.
constexpr GuiButtonDescriptorNative2Ch d54{{111,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p40},7,-1};
// Original descriptor 0x55CE2C, raw 0x15B62C.
constexpr GuiButtonDescriptorNative2Ch d55{{112,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p41},7,-1};
// Original descriptor 0x55CE58, raw 0x15B658.
constexpr GuiButtonDescriptorNative2Ch d56{{113,{nullptr,s2,nullptr,s3,s3,s4,nullptr},&p42},7,-1};
// Original descriptor 0x55CE84, raw 0x15B684.
constexpr GuiButtonDescriptorNative2Ch d57{{114,{nullptr,s5,nullptr,s6,s6,s7,nullptr},&p43},7,-1};
constexpr GuiButtonDescriptorNative2Ch Page01Block00Buttons0[]{d2,d3};
constexpr GuiButtonDescriptorNative2Ch Page01Block00Buttons1[]{d4,d5};
constexpr GuiButtonDescriptorNative2Ch Page01Block00Buttons2[]{d6,d7};
constexpr GuiButtonDescriptorNative2Ch Page01Block00Buttons3[]{d8,d9};
constexpr GuiButtonDescriptorNative2Ch Page01Block00Buttons4[]{d10,d11};
constexpr SettingTextBoolRow Page01Block00Rows[]{
    {SettingTextBoolField::Shadow,37,nullptr,Page01Block00Buttons0,true,48,49,SettingTextCommit::Deferred},
    {SettingTextBoolField::Antialias,38,nullptr,Page01Block00Buttons1,true,50,51,SettingTextCommit::Deferred},
    {SettingTextBoolField::HideMessageDuringVoice,39,nullptr,Page01Block00Buttons2,true,52,53,SettingTextCommit::Deferred},
    {SettingTextBoolField::StopVoiceOnAdvance,40,nullptr,Page01Block00Buttons3,true,54,55,SettingTextCommit::Deferred},
    {SettingTextBoolField::ReadColor,41,nullptr,Page01Block00Buttons4,true,56,57,SettingTextCommit::Deferred}
};
constexpr GuiButtonDescriptorNative2Ch Page01Block01Buttons0[]{d12,d13};
constexpr GuiButtonDescriptorNative2Ch Page01Block01Buttons1[]{d14,d15};
constexpr GuiButtonDescriptorNative2Ch Page01Block01Buttons2[]{d16,d17};
constexpr GuiButtonDescriptorNative2Ch Page01Block01Buttons3[]{d18,d19};
constexpr SettingTextBoolRow Page01Block01Rows[]{
    {SettingTextBoolField::SkipUnread,42,nullptr,Page01Block01Buttons0,true,58,59,SettingTextCommit::Deferred},
    {SettingTextBoolField::ContinueSkip,43,nullptr,Page01Block01Buttons1,true,60,61,SettingTextCommit::Deferred},
    {SettingTextBoolField::SkipVoice,44,nullptr,Page01Block01Buttons2,true,62,63,SettingTextCommit::Deferred},
    {SettingTextBoolField::SkipEffectSound,45,nullptr,Page01Block01Buttons3,true,64,65,SettingTextCommit::Deferred}
};
constexpr GuiButtonDescriptorNative2Ch Page02Block00Buttons0[]{d25,d26};
constexpr GuiButtonDescriptorNative2Ch Page02Block00Buttons1[]{d27,d28};
constexpr GuiButtonDescriptorNative2Ch Page02Block00Buttons2[]{d29,d30};
constexpr GuiButtonDescriptorNative2Ch Page02Block00Buttons3[]{d31,d32};
constexpr GuiButtonDescriptorNative2Ch Page02Block00Buttons4[]{d33,d34};
constexpr GuiButtonDescriptorNative2Ch Page02Block00Buttons5[]{d35,d36};
constexpr GuiButtonDescriptorNative2Ch Page02Block00Buttons6[]{d37,d38};
constexpr GuiButtonDescriptorNative2Ch Page02Block00Buttons7[]{d39,d40};
constexpr GuiButtonDescriptorNative2Ch Page02Block00Buttons8[]{d41,d42};
constexpr GuiButtonDescriptorNative2Ch Page02Block00Buttons9[]{d43,d44};
constexpr SettingTextBoolRow Page02Block00Rows[]{
    {SettingTextBoolField::ConfirmSave,69,nullptr,Page02Block00Buttons0,true,79,80,SettingTextCommit::Deferred},
    {SettingTextBoolField::ConfirmLoad,70,nullptr,Page02Block00Buttons1,true,81,82,SettingTextCommit::Deferred},
    {SettingTextBoolField::ConfirmQuickSave,71,nullptr,Page02Block00Buttons2,true,83,84,SettingTextCommit::Deferred},
    {SettingTextBoolField::ConfirmQuickLoad,72,nullptr,Page02Block00Buttons3,true,85,86,SettingTextCommit::Deferred},
    {SettingTextBoolField::ConfirmOverwrite,73,nullptr,Page02Block00Buttons4,true,87,88,SettingTextCommit::Deferred},
    {SettingTextBoolField::ConfirmDefaults,74,nullptr,Page02Block00Buttons5,true,89,90,SettingTextCommit::Immediate},
    {SettingTextBoolField::ConfirmExit,75,nullptr,Page02Block00Buttons6,true,91,92,SettingTextCommit::Immediate},
    {SettingTextBoolField::ConfirmDeleteSave,76,nullptr,Page02Block00Buttons7,true,93,94,SettingTextCommit::Deferred},
    {SettingTextBoolField::ConfirmTitle,77,nullptr,Page02Block00Buttons8,true,95,96,SettingTextCommit::Deferred},
    {SettingTextBoolField::ConfirmDeleteAll,78,nullptr,Page02Block00Buttons9,true,97,98,SettingTextCommit::Deferred}
};
constexpr GuiButtonDescriptorNative2Ch Page02Block03Buttons0[]{d52,d53};
constexpr GuiButtonDescriptorNative2Ch Page02Block03Buttons1[]{d54,d55};
constexpr GuiButtonDescriptorNative2Ch Page02Block03Buttons2[]{d56,d57};
constexpr SettingTextBoolRow Page02Block03Rows[]{
    {SettingTextBoolField::SaveThumbnail,106,nullptr,Page02Block03Buttons0,true,109,110,SettingTextCommit::Deferred},
    {SettingTextBoolField::SaveHistory,107,nullptr,Page02Block03Buttons1,true,111,112,SettingTextCommit::Deferred},
    {SettingTextBoolField::AutoSaveChoice,108,nullptr,Page02Block03Buttons2,true,113,114,SettingTextCommit::Deferred}
};
constexpr SettingTextDisplayChild SettingPage01Children[]{{SettingTextDisplayBlock::TextOptions,5},{SettingTextDisplayBlock::SkipOptions,6},{SettingTextDisplayBlock::Preview,7}};
constexpr SettingTextDisplayChild SettingPage02Children[]{{SettingTextDisplayBlock::ConfirmOptions,8},{SettingTextDisplayBlock::RightClick,9},{SettingTextDisplayBlock::DisplayMode,10},{SettingTextDisplayBlock::SaveOptions,11}};
constexpr GuiButtonDescriptorNative2Ch Page02Block01Buttons[]{d45,d46,d47,d48,d49};
constexpr SettingTextChoiceValue Page02Block01Values[]{{99,3},{100,2},{101,4},{102,5},{103,1}};
constexpr GuiButtonDescriptorNative2Ch modeButtons[]{d50,d51};
constexpr SettingTextModeCommand modeCommands[]{{104,0x800C,true,false},{105,0x800B,true,true}};
constexpr SettingTextDisplayResourceProfile profile{
    {{SettingPage01Children,SettingPage02Children}},
    {{Page01Block00Rows,Page01Block01Rows,Page02Block00Rows,Page02Block03Rows}},
    {SettingTextChoiceField::RightClick,9,Page02Block01Buttons,Page02Block01Values,-1,false},
    {},
    {modeButtons,nullptr,modeCommands,104,105},
    {
        {{d20,d21,d22}},
        {{d23,d24}},
        {{s37,s38}},
        {0,0,0},{4,4,255},{0,0,1},
        {s39,13},{s40,14},
        -28,700,0xFFFFFF,0xFFFFFFFF,0,{1,2},0,0xFFFFFFFF,
        {1,2},{0,0},{758,531},0,0,false,false
    },
    {{&d0,&d1}}
};
}
const SettingTextDisplayResourceProfile& Profile() noexcept { return profile; }
}
