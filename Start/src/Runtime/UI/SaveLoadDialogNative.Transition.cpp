#include <Runtime/SaveLoadDialogNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)+std::uint32_t(b));}
void Reset(TransitOperation& packet) noexcept
{
    packet.destination=0;packet.previous=0;packet.source=nullptr;packet.mask=0;packet.y=0;packet.x=0;
    packet.clip=nullptr;packet.opcode=-1;packet.targetKind=0;packet.current=0;packet.target=255;packet.range=0;
}
}
void AdvanceSaveLoadDialogNativeAt51CA80(SaveLoadDialogNativeBody184h& body,
    ISaveLoadDialogNativeTransitionSource& source,IGraphicsTransitApi& graphics)
{
    auto& packet=body.operationAtAC;
    switch(body.transitionPhaseAt70)
    {
    case 0:
    {
        YtlReferenceSlotRuntime* frame;
        std::int32_t next,opcode;
        if(body.phaseAt6C==1)
        {
            // 51CBC2按值Ref增持，callee负责正常/EH消费；caller不额外减持。
            YtlReferenceSlotRuntime argument;
            CopyGraphicsSurfaceReferenceAt47D740(argument,body.canvasAt94);
            body.common.vptrAt00->compositeAt5C(&body.common,argument,nullptr);
            next=3;opcode=57;frame=&body.canvasAt94;
        }
        else{next=4;opcode=58;frame=&body.capturedAt88;}
        Reset(packet);packet.opcode=opcode;
        // 444770(index0)只按base和bias*stride解址，不查count，不取handle。
        packet.source=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(*frame);
        body.nextPhaseAt74=next;
        body.stepAtDC=source.ReadStepAt51CC28();
        body.transitionPhaseAt70=1;
        return;
    }
    case 1:
        if(packet.opcode==-1||packet.current>=packet.target)body.transitionPhaseAt70=2;
        if(source.AccelerateAt4C3430()||source.AccelerateAt4AA970())
        {packet.current=Add(packet.current,256);if(packet.current>=packet.target)packet.current=packet.target;}
        {
            // 51CB24在输入查询之后另选当前Root/transit；不固定进入时或另用现代checked调度。
            auto* selected=source.CaptureTransitAfter413BE0();
            TransitGraphicsScope scope(graphics);
            (void)selected->DispatchRawAt449DD0(packet);
        }
        // 51CB7B在Render之后才读step/current/target；回调改写包和步长必须生效。
        packet.current=Add(packet.current,body.stepAtDC);
        if(packet.current>=packet.target)packet.current=packet.target;
        source.CommitScreenAt4B02E0();
        return;
    case 2:body.phaseAt6C=body.nextPhaseAt74;return;
    default:return;
    }
}
}
