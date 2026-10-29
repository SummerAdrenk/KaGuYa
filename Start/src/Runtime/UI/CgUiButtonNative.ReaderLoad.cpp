#include <Runtime/CgUiButtonReaderNative.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include "CgUiButtonNative.Internal.h"
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
using namespace CgUiButtonNativeDetail;
namespace
{
struct UnwrittenReference final
{union{YtlReferenceSlotRuntime value;};UnwrittenReference() noexcept{}};
struct RefUnwind final
{
    YtlReferenceSlotRuntime& value;bool armed=true;
    ~RefUnwind() noexcept{if(armed)ReleaseYtlReference(value);}
    void Cached(YtlReferenceControlRuntime* control) noexcept
    {
        armed=false;
        if(control&&--control->referenceCountAt04==0)
        {if(control->owningAt08)control->DestroyPayloadAtVslot00();control->DeleteControlAtVslot04(1);}
    }
    void Reload() noexcept{armed=false;ReleaseYtlReference(value);}
};
void MergeLoaded(CgUiButtonNativeBody6Ch& body,const YtlReferenceSlotRuntime& reference,std::int32_t count)
{
    auto* surface=Resolve<GraphicsSurfaceNativeBody>(reference);
    if(!count)
    {
        body.extentAt38.cx=Read(surface->widthAt08);body.extentAt38.cy=Read(surface->heightAt0C);
        surface=Resolve<GraphicsSurfaceNativeBody>(reference);
        body.naturalAt30.x=Read(surface->originXAt10);body.naturalAt30.y=Read(surface->originYAt14);
    }
    else
    {
        const auto width=Read(surface->widthAt08);const auto height=Read(surface->heightAt0C);
        surface=Resolve<GraphicsSurfaceNativeBody>(reference);
        const auto x=Read(surface->originXAt10);const auto y=Read(surface->originYAt14);
        const auto oldHeight=Read(body.extentAt38.cy);
        RECT incoming{x,y,Add(x,width),Add(y,height)};
        const auto oldY=Read(body.naturalAt30.y);const auto oldX=Read(body.naturalAt30.x);
        RECT old{oldX,oldY,Add(oldX,Read(body.extentAt38.cx)),Add(oldY,oldHeight)};
        ::UnionRect(&incoming,&incoming,&old);
        // 48CD5E..48CD6A：此入口的发布次序不同于4C1F20。
        body.naturalAt30.y=incoming.top;body.extentAt38.cx=Sub(incoming.right,incoming.left);
        body.extentAt38.cy=Sub(incoming.bottom,incoming.top);body.naturalAt30.x=incoming.left;
    }
}
struct ErrorText final
{
    union{RuntimeCString value;};bool armed=false;
    ErrorText() noexcept{}
    ~ErrorText() noexcept{if(armed)DestroyRuntimeCStringAt40CD40(&value);}
};
}
std::uint8_t LoadCgUiButtonReaderNativeAt48CAE0(CgUiButtonNativeBody6Ch& body,std::uint32_t slot,
    const ArchiveReaderReference& reader,IGraphicsSurfaceApi& surfaces,IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    // FuncInfo5E5410 catch0..6；state5/6退休叶自身terminate，不能被外层catch吞。
    try
    {
        if(slot>=7u)throw 1;
        const auto* name=reader.Get()->NameAt4179F0();
        auto identifier=ArchiveResourceNameAt40B530(name,strings);
        std::int32_t index=0;bool found=false;
        while(std::uint32_t(index)<std::uint32_t(Count(body.surfacesAt5C)))
        {
            auto* existing=ResolveCgUiButtonSurfaceNativeAt4ADED0(body,index);
            const auto* text=*reinterpret_cast<const char* const volatile*>(&existing->nameAt1C);
            if(!text)throw HRESULT{E_FAIL};
            if(!_mbsicmp(reinterpret_cast<const unsigned char*>(identifier.Value().GetString()),
                reinterpret_cast<const unsigned char*>(text))){found=true;break;}
            index=Add(index,1);
        }
        if(!found)
        {
            auto* created=NewGraphicsSurfaceAt40C2E0(surfaces,strings);
            UnwrittenReference temporary;
            ConstructGraphicsSurfaceReferenceAt451500(temporary.value,created,1);
            RefUnwind temporaryUnwind{temporary.value};
            if(!LoadGraphicsSurfaceArchiveReaderAt419D70(*Resolve<GraphicsSurfaceNativeBody>(temporary.value),reader,surfaces,0))throw 1;
            index=Count(body.surfacesAt5C);
            auto* control=Read(temporary.value.controlAt08);
            MergeLoaded(body,temporary.value,index);
            UnwrittenReference argument;
            argument.value.baseAt00=Read(temporary.value.baseAt00);
            argument.value.biasAt04=Read(temporary.value.biasAt04);
            argument.value.controlAt08=control;if(control)++control->referenceCountAt04;
            RefUnwind argumentUnwind{argument.value};
            auto* end=Read(body.surfacesAt5C.end);
            if(end==Read(body.surfacesAt5C.capacity))
            {GrowGuiControlSurfaceReferencesAt4540A0(body.surfacesAt5C,end,argument.value);control=Read(argument.value.controlAt08);}
            else AppendGuiControlSurfaceReference(body.surfacesAt5C,argument.value);
            argumentUnwind.Cached(control);temporaryUnwind.Reload();
        }
        body.slotsAt40[slot]=index;identifier.ReleaseNormal();return 1;
    }
    catch(...){return 0;}
}
std::uint8_t LoadCgUiButtonRecordNativeAt480930(CgUiButtonNativeBody6Ch* body,
    const CgUiButtonRecordNative28h* record,std::int32_t setPosition,IArchiveResourceSource& source,
    const FileTransactionContext* transaction,IStartFileApi& files,const IEnvironmentKeyTableSource& key,
    IGraphicsSurfaceApi& surfaces,ICgUiButtonResourceErrorDisplay& errors,ArchiveCodecServices codecs,
    IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    ErrorText error;InitializeEmptyRuntimeCString(error.value,strings);error.armed=true;
    SetRuntimeCStringAt437680(error.value,"syntax error",12);
    bool loaded=false;
    try
    {
        ArchiveReaderMemoryApi memory;
        ArchiveReaderFactoryRuntime factory(source,transaction,files,memory,codecs);
        for(std::uint32_t slot=0;slot<7u;++slot)
        {
            // 当前record/name仅在该槽到达时读取；源回调可修改后续资源。
            const auto* name=Read(record->namesAt00[slot]);if(!name)continue;
            if(!factory.LoadBorrowedAt483D90(name,key))
            {FormatRuntimeCStringAt417130(error.value,"%s ",Read(record->namesAt00[slot]));throw 1;}
            if(!LoadCgUiButtonReaderNativeAt48CAE0(*body,slot,factory.Reader(),surfaces,strings))
            {FormatRuntimeCStringAt417130(error.value,"%s ",Read(record->namesAt00[slot]));throw 2;}
        }
        if(setPosition)
        {const auto y=Read(record->positionAt1C.y);const auto x=Read(record->positionAt1C.x);
            body->positionAt24.x=x;body->positionAt24.y=y;body->useNaturalAt2C=0;}
        body->timelineAt68=Read(record->timelineAt24);body->stateAt04=0;body->latchAt20=0;
        loaded=true;
        // 原4638A0在error normal尾之前退factory；成功及失败按原终止资格销毁。
    }
    catch(...){loaded=false;}
    if(loaded)
    {
        auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(error.value.GetString())-16);
        error.armed=false;ReleaseRuntimeCStringHeader(header);return 1;
    }
    AppendRuntimeCStringAt413550(error.value," [ CDrawManager::LoadButton ]",0x1D);
    errors.ShowAt480A2D(error.value);
    // 480A46仍有state1清理资格：Free抛出会由展开再读本槽，而非提前撤销。
    DestroyRuntimeCStringAt40CD40(&error.value);error.armed=false;return 0;
}
}
