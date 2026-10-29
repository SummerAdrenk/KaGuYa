#pragma once
#include <Runtime/AutoSaveOperationsNative.h>

namespace KaGuYa::Start::Runtime::AutoSaveOperationsNativeDetail
{
union TextSlot
{
    TextSlot() noexcept {}
    ~TextSlot() noexcept {}
    RuntimeCString value;
};
ATL::CStringData* Header(const char*) noexcept;
void PublishText(RuntimeCString&, ATL::CStringData*) noexcept;
struct TextLifetime final
{
    RuntimeCString& text;
    bool armed = true;
    ~TextLifetime() noexcept;
    void FinishCached(const char*);
};
struct FactoryLifetime final
{
    SaveFileDeleteFactoryNativeBody0Ch& body;
    bool armed = true;
    ~FactoryLifetime() noexcept;
    void Finish();
};
std::int32_t Increment32(std::int32_t) noexcept;
void* CaptureInlineArchive(std::uint32_t index, bool beginFirst = false, bool biasFirst = false);
std::int32_t& CaptureLatestInline(bool beginFirst = false, bool biasFirst = false);
std::int32_t& CaptureLatestVia417990();
ScriptParamsNativeBody* CaptureParams();
void SetTitleAt48CF90(ScriptParamsNativeBody&, const char*);
}
