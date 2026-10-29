#include "CgButtonNative.Internal.h"

namespace KaGuYa::Start::Runtime::CgButtonNativeDetail
{
// 5B42C0：CP932原字节；显式NULL项不能改用同分类正常图伪成功。
#define CG_SCREEN "vrm:\\CG\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA" "_"
#define APPRECIATION "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA"
#define THUMB APPRECIATION "\x83\x54\x83\x80\x83\x6C"
#define CLICK "\x83\x4E\x83\x8A\x83\x62\x83\x4E"
#define FOCUS "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67"
#define PAGE APPRECIATION "\x83\x79\x81\x5B\x83\x57"
#define BACK "\x96\xDF\x82\xE9"
#define NEXT "\x90\x69\x82\xDE"
#define ON "\x83\x49\x83\x93"
#define DIGIT APPRECIATION "\x8D\xB7\x95\xAA\x95\x5C\x8E\xA6\x90\x94"
#define PERCENT APPRECIATION "\x89\xF1\x8E\xFB\x97\xA6\x90\x94\x92\x6C"
#define DIGITS(prefix) prefix "0.alp", prefix "1.alp", prefix "2.alp", prefix "3.alp", prefix "4.alp", prefix "5.alp", prefix "6.alp", prefix "7.alp", prefix "8.alp", prefix "9.alp"
const char* const backgrounds[10]{
    CG_SCREEN "\x90\x53\x8D\xD8.bmp", nullptr,
    CG_SCREEN "\x90\xE1\x88\xA4.bmp", nullptr,
    CG_SCREEN "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67.bmp", nullptr,
    CG_SCREEN "\x89\xC4\x91\xE3\x8E\x71.bmp", CG_SCREEN "\x89\xC4\x91\xE3\x8E\x71\x96\xA2\x8A\x4A\x95\xFA.bmp",
    CG_SCREEN "\x82\xBB\x82\xCC\x91\xBC.bmp", nullptr};
const char* const pageMarks[3]{
    APPRECIATION "1\x83\x79\x81\x5B\x83\x57.alp",
    APPRECIATION "2\x83\x79\x81\x5B\x83\x57.alp",
    APPRECIATION "3\x83\x79\x81\x5B\x83\x57.alp"};
const char* const stamps[4]{THUMB CLICK ".alp", THUMB FOCUS ".alp",
    THUMB "\x83\x43\x83\x8B\x83\x49\x83\x74.alp",
    DIGIT "\x83\x58\x83\x89\x83\x62\x83\x56\x83\x85.alp"};
const char* const digits[10]{DIGITS(DIGIT)};
const char* const percentDigits[10]{DIGITS(PERCENT)};
#define FORMAT "\x83\x5E\x83\x43\x83\x76%d_\x89\xE6\x91\x9C%d_"
const char* const formats[3]{FORMAT "\x83\x6D\x81\x5B\x83\x7D\x83\x8B", FORMAT CLICK,
    FORMAT "\x83\x74\x83\x48\x81\x5B\x83\x4A\x83\x58"};
const CgUiButtonRecordNative28h pageRecords[2]{
    {{PAGE BACK ON ".alp", PAGE BACK FOCUS ".alp", PAGE BACK CLICK ".alp", nullptr, nullptr, nullptr, nullptr}, {0, 0}, -1},
    {{PAGE NEXT ON ".alp", PAGE NEXT FOCUS ".alp", PAGE NEXT CLICK ".alp", nullptr, nullptr, nullptr, nullptr}, {0, 0}, -1}};
#undef FORMAT
#undef DIGITS
#undef PERCENT
#undef DIGIT
#undef ON
#undef NEXT
#undef BACK
#undef PAGE
#undef FOCUS
#undef CLICK
#undef THUMB
#undef APPRECIATION
#undef CG_SCREEN
const POINT positions[9]{{112,157},{349,157},{586,157},{112,299},{349,299},{586,299},{112,441},{349,441},{586,441}};
const POINT slashes[9]{{269,273},{506,273},{743,273},{269,415},{506,415},{743,415},{269,557},{506,557},{743,557}};
const int oneX[9][2]{{249,286},{486,523},{723,760},{249,286},{486,523},{723,760},{249,286},{486,523},{723,760}};
const int twoX[9][4]{{255,242,293,280},{492,479,530,517},{729,716,767,754},{255,242,293,280},{492,479,530,517},{729,716,767,754},{255,242,293,280},{492,479,530,517},{729,716,767,754}};
const int threeX[9][6]{{255,242,229,306,293,280},{492,479,466,543,530,517},{729,716,703,780,767,754},{255,242,229,306,293,280},{492,479,466,543,530,517},{729,716,703,780,767,754},{255,242,229,306,293,280},{492,479,466,543,530,517},{729,716,703,780,767,754}};
const int numberY[9][2]{{273,273},{273,273},{273,273},{415,415},{415,415},{415,415},{557,557},{557,557},{557,557}};
const POINT percentPositions[3]{{216,690},{198,690},{187,690}};
const int neighbors[27][4]{
    {6,3,2,1},{7,4,0,2},{8,5,1,0},{0,6,5,4},{1,7,3,5},{2,8,4,3},{3,0,8,7},{4,1,6,8},{5,2,7,6},
    {15,12,11,10},{16,13,9,11},{17,14,10,9},{9,15,14,13},{10,16,12,14},{11,17,13,12},{12,9,17,16},{13,10,15,17},{14,11,16,15},
    {24,21,20,19},{25,22,18,20},{26,23,19,18},{18,24,23,22},{19,25,21,23},{20,26,22,21},{21,18,26,25},{22,19,24,26},{23,20,25,24}};
// 5EFEA4原38条(prev,action)；构造执行器据此决定normal/EH资格，而非只供文档展示。
const UnwindEntry constructorUnwind[38]{
    {-1,0x59C360},{0,0},{1,0x541E55},{1,0x59C368},{3,0x59C377},{1,0x59C382},
    {5,0x59C38A},{5,0x59C399},{5,0x59C3AB},{5,0x541E55},{5,0x59C3B3},{5,0x541E55},
    {1,0x541E55},{1,0x59C3BE},{13,0x59C3C9},{14,0x59C3D4},{15,0x59C3DF},{16,0x59C3EA},
    {17,0x59C3F2},{17,0x541E55},{17,0x541E55},{17,0x59C3FD},{17,0x59C40F},{17,0x541E55},
    {17,0x59C417},{17,0x541E55},{17,0x59C422},{26,0x59C42A},{26,0x59C43C},{26,0x59C44E},
    {29,0x59C456},{29,0x59C461},{15,0x541E55},{14,0x541E55},{13,0x541E55},{1,0x541E55},
    {1,0x59C46C},{0,0}};
}
