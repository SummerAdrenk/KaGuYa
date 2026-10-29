#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/ModeSelectButtonNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
enum class OptionCharPresentation { NaturalLegacy, PositionWindow, SlidingWindow };
struct OptionCharInstance final
{
    int event;
    int normalRecord;
    int unavailableRecord; // -1表示原表没有替代记录，不创造占位图片。
    int characterDisplayIndex; // -1不查询；只有18的前四实例查询IsDisp。
};
struct OptionCharResourceProfile final
{
    OptionCharPresentation presentation;
    // 两套七槽编码保持原样；旧POINT/timeline不能解释为GUI hitSlot/childId。
    std::span<const CgUiButtonRecordNative28h> nativeRecords;
    std::span<const GuiButtonDescriptorNative2Ch> records;
    std::span<const OptionCharInstance> instances;
    std::span<const GuiButtonDescriptorNative2Ch> navigationButtons;
    std::span<const POINT> positions;
    int visibleCount;
    int scanCount; // 旧数组无窗口扫描；18扫描6但只创建5个实例。
    int reportedCount;
    int previousBoundaryEvent;
    int nextBoundaryEvent;
};
const OptionCharResourceProfile& CurrentOptionCharResourceProfile();
void ValidateOptionCharResourceProfile(const OptionCharResourceProfile&);
}
