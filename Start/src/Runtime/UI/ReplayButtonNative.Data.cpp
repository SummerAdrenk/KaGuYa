#include "ReplayButtonNative.Internal.h"

namespace KaGuYa::Start::Runtime::ReplayButtonNativeDetail
{
// 原602398与5B4764..5B4814；Replay前缀为Scene，不借CG背景表替代。
#define REPLAY_SCREEN "vrm:\\\x83\x56\x81\x5B\x83\x93\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA" "_"
#define APPRECIATION "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA"
#define THUMB APPRECIATION "\x83\x54\x83\x80\x83\x6C"
#define CLICK "\x83\x4E\x83\x8A\x83\x62\x83\x4E"
#define FOCUS "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67"
#define PAGE APPRECIATION "\x83\x79\x81\x5B\x83\x57"
#define BACK "\x96\xDF\x82\xE9"
#define NEXT "\x90\x69\x82\xDE"
#define ON "\x83\x49\x83\x93"
#define PERCENT APPRECIATION "\x89\xF1\x8E\xFB\x97\xA6\x90\x94\x92\x6C"
const char* const backgrounds[10]{
    REPLAY_SCREEN "\x90\x53\x8D\xD8.bmp", nullptr,
    REPLAY_SCREEN "\x90\xE1\x88\xA4.bmp", nullptr,
    REPLAY_SCREEN "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67.bmp", nullptr,
    REPLAY_SCREEN "\x89\xC4\x91\xE3\x8E\x71.bmp", REPLAY_SCREEN "\x89\xC4\x91\xE3\x8E\x71\x96\xA2\x8A\x4A\x95\xFA.bmp",
    REPLAY_SCREEN "\x82\xBB\x82\xCC\x91\xBC.bmp", nullptr};
// 原6021DC、602468、5B4238、5B51CC/5B51E8/5B5204的CP932字节。
const char* const pageMarks[3]{
    APPRECIATION "1\x83\x79\x81\x5B\x83\x57.alp",
    APPRECIATION "2\x83\x79\x81\x5B\x83\x57.alp",
    APPRECIATION "3\x83\x79\x81\x5B\x83\x57.alp"};
const char* const stamps[3]{THUMB CLICK ".alp", THUMB FOCUS ".alp",
    THUMB "\x83\x43\x83\x8B\x83\x49\x83\x74.alp"};
const char* const percentDigits[10]{
    PERCENT "0.alp", PERCENT "1.alp", PERCENT "2.alp", PERCENT "3.alp", PERCENT "4.alp",
    PERCENT "5.alp", PERCENT "6.alp", PERCENT "7.alp", PERCENT "8.alp", PERCENT "9.alp"};
#define FORMAT "\x83\x5E\x83\x43\x83\x76%d_\x89\xE6\x91\x9C%d_"
const char* const formats[3]{FORMAT "\x83\x6D\x81\x5B\x83\x7D\x83\x8B", FORMAT CLICK,
    FORMAT "\x83\x74\x83\x48\x81\x5B\x83\x4A\x83\x58"};
// 原6023C0，完整两个28h descriptor；四个NULL、(0,0)、-1均保留。
const CgUiButtonRecordNative28h pageRecords[2]{
    {{PAGE BACK ON ".alp", PAGE BACK FOCUS ".alp", PAGE BACK CLICK ".alp", nullptr, nullptr, nullptr, nullptr}, {0, 0}, -1},
    {{PAGE NEXT ON ".alp", PAGE NEXT FOCUS ".alp", PAGE NEXT CLICK ".alp", nullptr, nullptr, nullptr, nullptr}, {0, 0}, -1}};
#undef FORMAT
#undef PERCENT
#undef ON
#undef NEXT
#undef BACK
#undef PAGE
#undef FOCUS
#undef CLICK
#undef THUMB
#undef APPRECIATION
#undef REPLAY_SCREEN
const POINT positions[9]{{112,157},{349,157},{586,157},{112,299},{349,299},{586,299},{112,441},{349,441},{586,441}};
const POINT buttonOffset{5,5}; // 602458/60245C
const POINT imageOffset{6,6}; // 602460/602464
const POINT percentPositions[3]{{216,690},{198,690},{187,690}}; // 5B4260
const int neighbors[27][4]{ // 602478..602627
    {6,3,2,1},{7,4,0,2},{8,5,1,0},{0,6,5,4},{1,7,3,5},{2,8,4,3},{3,0,8,7},{4,1,6,8},{5,2,7,6},
    {15,12,11,10},{16,13,9,11},{17,14,10,9},{9,15,14,13},{10,16,12,14},{11,17,13,12},{12,9,17,16},{13,10,15,17},{14,11,16,15},
    {24,21,20,19},{25,22,18,20},{26,23,19,18},{18,24,23,22},{19,25,21,23},{20,26,22,21},{21,18,26,25},{22,19,24,26},{23,20,25,24}};
// 原5F0314，运行中的Frame实际依此展开，不只是静态展示。
const UnwindEntry constructorUnwind[35]{
    {-1,0x59CA20},{0,0},{1,0x541E55},{1,0x59CA28},{3,0x59CA37},{1,0x59CA42},
    {5,0x59CA4A},{5,0x59CA5C},{5,0x59CA6B},{5,0x541E55},{5,0x59CA73},{5,0x541E55},
    {1,0x541E55},{1,0x59CA7E},{13,0x59CA86},{14,0x59CA8E},{15,0x59CA99},
    {16,0x59CAA1},{16,0x541E55},{16,0x59CAAC},{16,0x59CABE},{16,0x541E55},
    {16,0x59CAC6},{16,0x541E55},{16,0x59CAD1},{24,0x59CAD9},{24,0x59CAEB},
    {24,0x59CAFD},{27,0x59CB05},{27,0x59CB10},{14,0x541E55},{13,0x541E55},
    {1,0x541E55},{1,0x59CB1B},{0,0}};
const UnwindEntry percentUnwind[7]{ // 4BC950 FI 5EE7E0 -> 5EE804
    {-1,0},{0,0x59E8A0},{0,0x541E55},{0,0x59E8A8},{0,0x541E55},{0,0x59E8B0},{-1,0}};
}
