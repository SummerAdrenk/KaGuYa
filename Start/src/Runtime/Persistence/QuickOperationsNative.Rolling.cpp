#include "QuickOperationsNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
std::int32_t RollingQuickSaveNativeAt4888C0(QuickOperationsNativeFactoryBody10h& factory)
{
    using namespace QuickOperationsNativeDetail;
    auto& source = RequireQuickOperationsNativeServices().source;
    const auto previous = CaptureLatestInline(); // 4888EB
    auto& incrementTarget = CaptureLatestInline(); // 48893C，不能缓存previous来源。
    incrementTarget = Increment32(incrementTarget);
    if (incrementTarget >= 8) CaptureLatestVia417990() = 0;

    TextSlot oldTitle;
    const char* oldChars = CaptureParams()->ResolveSaveParamsAt08()->saveTitleAt08.GetString();
    PublishText(oldTitle.value, CloneRuntimeCStringDataAt414540(Header(oldChars)));
    const char* const savedTitle = oldTitle.value.GetString(); // 原ESI全程缓存。
    TextLifetime oldLifetime{oldTitle.value}; // state0，Clone失败尚无清理资格。
    TextSlot newTitle;
    InitializeEmptyRuntimeCString(newTitle.value, source.Strings());
    TextLifetime newLifetime{newTitle.value}; // state1
    FormatRuntimeCStringAt417130(newTitle.value, QuickOperationsNativeRollingTitleFormat(),
        Increment32(CaptureLatestInline()));

    // 488A76先Ensure，488A80再缓存EDI，之后Root/Params回调不得改变正常释放指针。
    source.EnsureRootAt413BE0();
    const char* const formattedTitle = newTitle.value.GetString();
    void* root = source.RootAt6198C4();
    auto* params = static_cast<ScriptParamsNativeBody*>(source.ParamsAt47BDB0(root));
    SetTitleAt48CF90(*params, formattedTitle);

    auto* save = static_cast<std::byte*>(CaptureInlineArchive(1));
    std::int32_t mode = 0;
    if (std::uint8_t(save[0x74]) != 0) mode = factory.previewAt0C != 0 ? 2 : 1;
    // 488AF2为begin-first，control非空时bias先于stride；不可统一改用417990。
    const auto slot = Increment32(CaptureLatestInline(true, true));
    const auto failed = SaveFileFactoryNativeAt488CF0(factory.base, slot, mode, source) != 0;
    if (failed)
    {
        // 删除与回写各重新取Env。删除异常不恢复latest/title；FALSE仍继续。
        const auto deleteSlot = Increment32(CaptureLatestVia417990());
        DeleteSaveFileFactorySlotNativeAt476460(factory.base, deleteSlot, 1, source);
        CaptureLatestVia417990() = previous;
    }
    SetTitleAt48CF90(*CaptureParams(), savedTitle);
    // 488BB1先state0释放缓存EDI；488BCC再state-1释放缓存ESI。
    // EH则59A958/59A950重读局部TEXT；绝不在EH中补标题或latest恢复。
    newLifetime.FinishCached(formattedTitle);
    oldLifetime.FinishCached(savedTitle);
    return failed ? 1 : 0;
}
}
