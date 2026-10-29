#include <Runtime/DrawTextRebuildServices.h>
#include <Runtime/TextWindowNativeFormat.h>
#include <Runtime/TextWindowNativeText.h>

namespace KaGuYa::Start::Runtime
{
DrawTextNativeRebuildServices::DrawTextNativeRebuildServices(ITextWindowNativeFormatSource& s,
    TextWindowLayerNativeServices& w,IGraphicsTextApi& t,IGraphicsTextGlyphApi& g,
    ITextWindowTextServices& layout,IScrVmCommandErrorSink& e,IRuntimeStringManagerSource& stringsSource)noexcept
    :source(s),window(w),text(t),glyphs(g),errors(e),strings(stringsSource),fixedLayout_(&layout){}

DrawTextNativeRebuildServices::DrawTextNativeRebuildServices(ITextWindowNativeFormatSource& s,
    TextWindowLayerNativeServices& w,GraphicsTextNativeApi& graphics,const char* const& face,
    IScrVmCommandErrorSink& e,IRuntimeStringManagerSource& stringsSource,
    std::span<const TextWindowTextDescriptor,2> descriptors)noexcept
    :source(s),window(w),text(graphics),glyphs(graphics),errors(e),strings(stringsSource),fixedLayout_(nullptr),
     nativeLayout_(std::in_place,graphics,face,descriptors){}

bool DrawTextNativeRebuildServices::RebuildForFixedBody(TextWindowLayerNativeBodyE8h& body,
    std::uint32_t refreshUnread)
{
    // view构造只存借用，不读Root/Env/字体或正文；原4C1650核及catch边界原样保留。
    // 两种域由构造入口明确选择，不在旧layout失败后换路，也不复用可重入覆盖的缓存。
    std::optional<TextWindowNativeTextServices> native;
    auto* layout=fixedLayout_;
    if(nativeLayout_)
    {
        native.emplace(body,window.state,window.environment,nativeLayout_->graphics,
            nativeLayout_->fontFace,errors,nativeLayout_->descriptors);
        layout=&*native;
    }
    TextWindowNativeText executor(body,text,glyphs,*layout,strings);
    TextWindowNativeFormat format(body,source,window,executor,text,errors);
    return format.RebuildAt4C1650(refreshUnread);
}
}
