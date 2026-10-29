#include <Runtime/RootModeSelectButtonNativeSource.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 源PE601658的12条40-byte记录；CP932原字节与共用字串指针均保留。
#define PREFIX "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
#define SCENE "\x83\x56\x81\x5B\x83\x93"
#define MUSIC "\x89\xB9\x8A\x79"
#define TITLE "\x83\x5E\x83\x43\x83\x67\x83\x8B"
#define ON "\x83\x49\x83\x93.vrm"
#define TARGET "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm"
#define CLICK "\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm"
const char sceneOn[]=PREFIX SCENE ON;
const char sceneTarget[]=PREFIX SCENE TARGET;
const char sceneClick[]=PREFIX SCENE CLICK;
const char musicOn[]=PREFIX MUSIC ON;
const char musicTarget[]=PREFIX MUSIC TARGET;
const char musicClick[]=PREFIX MUSIC CLICK;
const char titleOn[]=PREFIX TITLE ON;
const char titleTarget[]=PREFIX TITLE TARGET;
const char titleClick[]=PREFIX TITLE CLICK;
const char cgOn[]=PREFIX "CG" ON;
const char cgTarget[]=PREFIX "CG" TARGET;
const char cgClick[]=PREFIX "CG" CLICK;
#undef PREFIX
#undef SCENE
#undef MUSIC
#undef TITLE
#undef ON
#undef TARGET
#undef CLICK
const CgUiButtonRecordNative28h records[12]{
    {{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},{0,0},-1},
    {{sceneOn,sceneTarget,sceneClick,nullptr,nullptr,nullptr,nullptr},{1061,566},-1},
    {{musicOn,musicTarget,musicClick,nullptr,nullptr,nullptr,nullptr},{1061,621},-1},
    {{titleOn,titleTarget,titleClick,nullptr,nullptr,nullptr,nullptr},{1061,676},-1},
    {{cgOn,cgTarget,cgClick,nullptr,nullptr,nullptr,nullptr},{1061,566},-1},
    {{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},{0,0},-1},
    {{musicOn,musicTarget,musicClick,nullptr,nullptr,nullptr,nullptr},{1061,621},-1},
    {{titleOn,titleTarget,titleClick,nullptr,nullptr,nullptr,nullptr},{1061,676},-1},
    {{cgOn,cgTarget,cgClick,nullptr,nullptr,nullptr,nullptr},{1061,566},-1},
    {{sceneOn,sceneTarget,sceneClick,nullptr,nullptr,nullptr,nullptr},{1061,621},-1},
    {{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},{0,0},-1},
    {{titleOn,titleTarget,titleClick,nullptr,nullptr,nullptr,nullptr},{1061,676},-1}
};
}
std::span<const CgUiButtonRecordNative28h,12> ModeSelectButtonNativeRecordsSample18() noexcept{return records;}
std::span<const CgUiButtonRecordNative28h,12> MamatomoModeSelectButtonRecords() noexcept{return records;}
}
