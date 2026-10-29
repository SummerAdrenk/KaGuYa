#include "PatternPreviewNative.Internal.h"
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/PatternResourcePlan.h>
#include <Runtime/ParamsPatternNativeAccess.h>
#include <Runtime/SpriteNativeDrawing.h>
#include <bit>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot=YtlReferenceSlotRuntime;
SpriteNativeBodyA0h* Sprite(PatternLayerNativeBody80h& pattern,std::uint32_t index) noexcept
{
    const volatile auto& reference=pattern.spritesAt34;
    const auto base=reference.baseAt00;if(!base)return nullptr;
    auto* control=reference.controlAt08;
    const auto stride=control?*reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<const std::byte*>(control)+0x0C):0;
    return reinterpret_cast<SpriteNativeBodyA0h*>(base+(reference.biasAt04+index)*stride);
}
std::uint32_t Count(PatternLayerNativeBody80h& pattern) noexcept
{
    auto* control=static_cast<const volatile Slot&>(pattern.spritesAt34).controlAt08;
    return control?*reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<const std::byte*>(control)+0x10):0;
}
std::int32_t Add(std::int32_t a,std::int32_t b) noexcept{return std::bit_cast<std::int32_t>(std::uint32_t(a)+std::uint32_t(b));}
struct RefCleanup{Slot& slot;bool active=true;~RefCleanup() noexcept{if(active)ReleaseYtlReference(slot);}};
void ReplaceNameAndComposite(PatternLayerNativeBody80h& pattern,std::uint32_t index,std::int32_t offset,
    Slot& snapshot,PatternPreviewNativeServices& services)
{
    auto name=ArchiveResourceNameLeftAt419D00(Sprite(pattern,index)->logicalNameAt0C,offset,services.strings);
    AppendRuntimeCStringAt413550(name.Value(),"\x89\xB3",2);
    {
        // 46EC7A在前缀追加后重新解Sprite，420C80再读当前源和尾长度。
        const auto& logical=Sprite(pattern,index)->logicalNameAt0C;
        const auto start=Add(offset,2);
        auto tail=ArchiveResourceNameMidAt420C80(logical,start,Add(logical.GetLength(),-start),services.strings);
        // 46ECBB先缓存ESI；46ECC9追加后，46ECD1先撤tail资格，再释放该缓存chars。
        // Append失败时资格仍在，EH析构只清tail当前槽，不走正常缓存尾。
        const char* tailChars=tail.Value().GetString();
        AppendRuntimeCStringAt413550(name.Value(),tailChars,tail.Value().GetLength());
        tail.ReleaseNormalCached(tailChars);
    }
    Slot replacement;RefCleanup spriteCleanup{replacement};
    ArchiveRawResourceNameRuntime scanner("",services.strings);
    const char* chars=name.Value().GetString();
    ScanPatternResourceSuffixAt45DA40(scanner.Value(),chars,services.strings);
    auto suffix=ArchiveResourceNameAt40B530(scanner.Value().GetString(),services.strings);
    // 46ED2B先撤scanner资格；Free失败不重试它，suffix仍保有展开资格。
    scanner.ReleaseNormal();
    std::int32_t x=0,y=0;bool explicitPosition=false;
    if(!suffix.Value().GetLength())
    {
        const auto* first=services.pattern.CapturePattern();
        Slot components;RefCleanup componentsCleanup{components};YtlReferenceControlRuntime* cached=nullptr;
        const auto id=FindPatternItemByResourceName(*first,chars,PatternResourceAccess::Native);
        if(id&&ParamsPatternItemKindAt(*first,*id,PatternResourceAccess::Native)==1)
        {
            components=ParamsPatternFileGroupAt(*first,*id,PatternResourceAccess::Native).stringsAt08.RawSlot();
            cached=components.controlAt08;if(cached)++cached->referenceCountAt04;
        }
        if(components.baseAt00)
            AssignRuntimeCStringAt410220(name.Value(),*ResolveEnvironmentObjectReference<RuntimeCString>(components));
        else
        {
            ArchiveRawResourceNameRuntime explicitName(ArchiveRawResourceNameRuntime::ProducerTag{},
                [&](RuntimeCString* out){InitializeEmptyRuntimeCString(*out,services.strings);});
            const auto* second=services.pattern.CapturePattern();
            const auto id2=FindPatternItemByResourceName(*second,chars,PatternResourceAccess::Native);
            if(!id2||ParamsPatternItemKindAt(*second,*id2,PatternResourceAccess::Native)!=2)throw 2;
            AssignRuntimeCStringAt410220(explicitName.Value(),ParamsPatternExcPositionAt(*second,*id2,PatternResourceAccess::Native).subNameAt08);
            // 46EF31赋值后在同一Pattern重新解项；回调可改条目，不缓存旧position指针。
            const auto& position=ParamsPatternExcPositionAt(*second,*id2,PatternResourceAccess::Native);
            x=std::bit_cast<std::int32_t>(position.xAt0C);y=std::bit_cast<std::int32_t>(position.yAt10);
            AssignRuntimeCStringAt410220(name.Value(),explicitName.Value());explicitPosition=true;
            explicitName.ReleaseNormalCached(explicitName.Value().GetString());
        }
        componentsCleanup.active=false;ReleasePatternPreviewSavedControl(cached);
    }
    chars=name.Value().GetString();
    void* receiver=services.sprites.CaptureDrawReceiverAt413BE0();
    auto* created=NewSpriteNativeAt4813E4(services.strings);
    InstallSpriteReferenceAt48140F(replacement,created);
    if(!services.sprites.LoadAt4814F0(receiver,ResolveSpriteReference(replacement),chars,0))throw 2;
    // 46F01D在绘制及suffix Free之前保存EDI；正常退役不能晚取replacement.control。
    auto* saved=replacement.controlAt08;
    auto* sprite=ResolveSpriteReference(replacement);
    if(!explicitPosition){x=sprite->xAt2C;y=sprite->yAt30;}
    const auto adjustedX=Add(x,pattern.layer.valueAt04);
    const auto adjustedY=Add(y,pattern.layer.valueAt08);
    // 46F053在Load正常返回后重取name chars，供46F10B正常尾使用；EH仍清当前槽。
    chars=name.Value().GetString();
    sprite=ResolveSpriteReference(replacement);sprite->xAt2C=adjustedX;sprite->yAt30=adjustedY;
    (void)CompositeSpriteNativeAt46B390(*ResolveSpriteReference(replacement),
        ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(snapshot),nullptr,services.drawing);
    // 46F0BB先退suffix；其Free失败仍展开当前Sprite槽，不用保存的EDI正常尾。
    // 46F0DA撤Sprite资格后减持EDI；46F10B撤name资格后最后Free缓存chars。
    suffix.ReleaseNormalCached(suffix.Value().GetString());
    spriteCleanup.active=false;ReleasePatternPreviewSavedControl(saved);
    name.ReleaseNormalCached(chars);
}
}
void RenderPatternPreviewSpritesNative(PatternLayerNativeBody80h& pattern,Slot& snapshot,
    PatternPreviewNativeServices& services)
{
    for(std::uint32_t index=0;index<Count(pattern);++index)
    {
        if(Sprite(pattern,index)->logicalNameAt0C.GetLength()==0)continue;
        const auto& logical=Sprite(pattern,index)->logicalNameAt0C;
        const unsigned char* match=nullptr;
        if(logical.GetLength()>=0)match=_mbsstr(reinterpret_cast<const unsigned char*>(logical.GetString()),
            reinterpret_cast<const unsigned char*>("\x8D\x62"));
        const auto offset=match?static_cast<std::int32_t>(match-reinterpret_cast<const unsigned char*>(Sprite(pattern,index)->logicalNameAt0C.GetString())):-1;
        if(match&&offset>=0){ReplaceNameAndComposite(pattern,index,offset,snapshot,services);continue;}
        auto* sprite=Sprite(pattern,index);const auto x=sprite->xAt2C;const auto y=sprite->yAt30;
        const auto adjustedX=Add(x,pattern.layer.valueAt04);const auto adjustedY=Add(y,pattern.layer.valueAt08);
        // 46F193先写Y再X；绘制之前再次解Sprite，正常返回后才重解并恢复X/Y。
        sprite=Sprite(pattern,index);sprite->yAt30=adjustedY;sprite->xAt2C=adjustedX;
        (void)CompositeSpriteNativeAt46B390(*Sprite(pattern,index),
            ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(snapshot),nullptr,services.drawing);
        sprite=Sprite(pattern,index);sprite->xAt2C=x;sprite->yAt30=y;
    }
}
}
