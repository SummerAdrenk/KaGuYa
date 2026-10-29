#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/ParamsRuntimeReadCursor.h>
#include <Runtime/ParamsGameSystemRuntimeReader.h>
#include <Runtime/ParamsPatternNativeReader.h>
#include <Runtime/ParamsPatternNativeAccess.h>
#include <Runtime/ParamsHeaderGlobals.h>
#include <Runtime/ParamsHeaderRuntime.h>
#include <Runtime/ParamsSerializeInput.h>

#include <array>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
namespace
{
void RecordHeaderBranch(const ParamsRuntimeRoot& root, const std::string_view header) noexcept
{
    if (auto* diagnostic = ParamsRuntimeRootDiagnostic::Find(root)) diagnostic->header = header;
}
}

void ParamsRuntimeRoot::SerializeAt434940(SerializeBuffer& input)
{
    ParamsSerializeBufferInput adapter(input);
    SerializeAt434940(adapter, ProcessParamsHeaderSource());
}

void ParamsRuntimeRoot::SerializeNativeAt434940(void* native18)
{
    ParamsNativeSerialize18Input adapter(native18);
    SerializeAt434940(adapter, ProcessParamsHeaderSource());
}

void ParamsRuntimeRoot::SerializeAt434940(IParamsSerializeInput& input, const IParamsHeaderSource& headers)
{
    // 434977 在 mode 检查前置 -1；写模式不动三个子对象。
    versionAt28 = -1.0;
    auto* const diagnostic = ParamsRuntimeRootDiagnostic::Find(*this);
    if (diagnostic) diagnostic->BeginRead();
    if (input.ReadModeAt04()) return;
    std::uint32_t consumed = 0;
    const auto matches = [&](ParamsHeaderId id)
    {
        // 434990/993先取输入对象，再读取对应全局并Clone。不能依赖实参求值顺序。
        const auto bytes = input.CaptureInputAt14();
        const auto& header = headers.Borrow(id);
        return MatchParamsHeaderAt413B30(header, bytes, consumed);
    };
    constexpr std::array<std::string_view, 4> baseHeaders{
        "[SCR-PARAMS]v02", "[SCR-PARAMS]v03", "[SCR-PARAMS]v04", "[SCR-PARAMS]v05"};
    constexpr std::array<ParamsHeaderId, 4> baseIds{
        ParamsHeaderId::V02, ParamsHeaderId::V03, ParamsHeaderId::V04, ParamsHeaderId::V05};
    std::size_t matchedBase = baseHeaders.size();
    for (std::size_t i = 0; i != baseIds.size(); ++i)
    {
        if (matches(baseIds[i]))
        {
            versionAt28 = static_cast<double>(i + 2);
            matchedBase = i;
            RecordHeaderBranch(*this, baseHeaders[i]);
            break;
        }
    }
    // 分支由匹配结果决定，不能被getter/Clone/Free对version的回调写入改变。
    if (matchedBase == baseIds.size()) return;
    if (matchedBase == 3)
    {
        const auto baseConsumed = consumed;
        constexpr std::array<std::string_view, 8> minorHeaders{
            "[SCR-PARAMS]v05.1", "[SCR-PARAMS]v05.2", "[SCR-PARAMS]v05.3", "[SCR-PARAMS]v05.4",
            "[SCR-PARAMS]v05.5", "[SCR-PARAMS]v05.6", "[SCR-PARAMS]v05.7", "[SCR-PARAMS]v05.8"};
        constexpr std::array<ParamsHeaderId, 8> minorIds{
            ParamsHeaderId::V051, ParamsHeaderId::V052, ParamsHeaderId::V053, ParamsHeaderId::V054,
            ParamsHeaderId::V055, ParamsHeaderId::V056, ParamsHeaderId::V057, ParamsHeaderId::V058};
        constexpr std::array<double, 8> versions{5.1, 5.2, 5.3, 5.4, 5.5, 5.6, 5.7, 5.8};
        bool minorMatched = false;
        for (std::size_t i = 0; i != minorIds.size(); ++i)
        {
            if (matches(minorIds[i]))
            {
                versionAt28 = versions[i];
                minorMatched = true;
                RecordHeaderBranch(*this, minorHeaders[i]);
                break;
            }
        }
        // 434C5A仅恢复base匹配计数，不覆盖回调已改写的version。
        if (!minorMatched) consumed = baseConsumed;
    }

    // 外部借用安全代次先失效；不持有旧GS，也不把诊断值写入11Ch正文或Ref控制块。
    if (auto* current = ParamsRuntimeRootDiagnostic::Find(*this)) current->InvalidateGameSystem();
    RebuildParamsGameSystemAt434C60(gameSystemAt04.Native());
    // 434D1D在发布后重新解析当前槽；不能拿factory先前缓存的正文替代。
    auto* const gameSystem = gameSystemAt04.get();
    const double gameVersion = versionAt28;
    const auto gameOffset = consumed; // 434D46/50 precedes getter14.
    {
        const auto bytes = input.CaptureInputAt14();
        if (!gameSystem)
            throw std::logic_error("434D59: Params GameSystem resolves to null (safe boundary)");
        ParamsRuntimeReadCursor cursor(bytes);
        cursor.CommitOffsetAfter419EB0(gameOffset);
        const auto* currentDiagnostic = ParamsRuntimeRootDiagnostic::Find(*this);
        // 诊断不跨reader回调持有绑定；换根后不能把旧解析的元数据写入新根记录。
        struct CommitMetadata final
        {
            const ParamsRuntimeRoot& root;
            std::uint32_t value;
            ~CommitMetadata() noexcept
            {
                if (auto* current = ParamsRuntimeRootDiagnostic::Find(root))
                    current->rawTripleCountMetadata = value;
            }
        } metadata{*this, currentDiagnostic ? currentDiagnostic->rawTripleCountMetadata : 0};
        ReadParamsGameSystemAt41ADE0(cursor, *gameSystem, gameVersion, &metadata.value);
        consumed = static_cast<std::uint32_t>(cursor.Offset());
    }
    RebuildParamsPatternAt434D61(patternAt10.Native());
    // 434E57..434E93在factory发布后重新解析当前根槽，不拿创建时正文代替。
    auto* const pattern = patternAt10.get();
    const double patternVersion = versionAt28;
    const auto patternOffset = consumed;
    {
        const auto bytes = input.CaptureInputAt14();
        if (!pattern)
            throw std::logic_error("434E93: Params Pattern resolves to null (safe boundary)");
        ParamsRuntimeReadCursor cursor(bytes);
        cursor.CommitOffsetAfter419EB0(patternOffset);
        ReadParamsPatternNativeAt41DFD0(cursor, *pattern, patternVersion);
        consumed = static_cast<std::uint32_t>(cursor.Offset());
    }
    RebuildParamsSceneLabelAt434ECE(sceneLabelsAt1C.Native());
    auto* const labels = sceneLabelsAt1C.get();
    const double labelsVersion = versionAt28;
    const auto labelsOffset = consumed;
    {
        const auto bytes = input.CaptureInputAt14();
        if (!labels)
            throw std::logic_error("434F8D: Params SceneLabel resolves to null (safe boundary)");
        ParamsRuntimeReadCursor cursor(bytes);
        cursor.CommitOffsetAfter419EB0(labelsOffset);
        ReadParamsSceneLabelsNativeAt41F8A0(cursor, *labels, labelsVersion);
    }
    // 原版不要求 EOF，也不提交局部消费量到 serializer.Cursor()。
    if (auto* current = ParamsRuntimeRootDiagnostic::Find(*this)) current->readCompleted = true;
}

ParamsRuntimeProfile ParamsRuntimeRoot::Snapshot() const
{
    // 显式离线深快照，不驻留于生产根，也不允许拿半读状态冒充成功。
    const auto* const diagnostic = ParamsRuntimeRootDiagnostic::Find(*this);
    if (!diagnostic || !diagnostic->readCompleted || !diagnostic->versionWritten ||
        !gameSystemAt04 || !patternAt10 || !sceneLabelsAt1C)
        throw std::logic_error("params snapshot requested before successful runtime read");
    return {std::string(diagnostic->header), versionAt28, *gameSystemAt04, ExportParamsPatternSnapshot(*patternAt10),
        ExportParamsSceneLabelSnapshot(*sceneLabelsAt1C), diagnostic->rawTripleCountMetadata};
}
}
