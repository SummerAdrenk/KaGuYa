#include <Runtime/ScriptNamedValueNative.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/GameBuildProfile.h>

#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
ScriptNamedValueNativeTables GameScriptNamedValueTables() noexcept
{
    return CompiledGameProfile().scriptNames();
}
std::int32_t ResolveScriptNamedValueNativeAt498020(const char* name,
    ScriptNamedValueNativeTables tables, IRuntimeStringManagerSource& strings)
{
    // 40B530必须早于源测长和名称表读取；构造成功才取得59C080展开资格。
    auto query = ArchiveResourceNameAt40B530(name, strings);
    std::int32_t result = -1;
    const auto commonCount = tables.common.size();
    const auto count = commonCount + tables.game.size();
    for (std::size_t index = 0; index != count; ++index)
    {
        const char* candidate = index < commonCount ? tables.common[index] : tables.game[index - commonCount];
        if (!candidate) AtlThrow(E_FAIL);
        if (_mbsicmp(reinterpret_cast<const unsigned char*>(query.Value().GetString()),
                reinterpret_cast<const unsigned char*>(candidate)) == 0)
        {
            result = static_cast<std::int32_t>(index);
            break;
        }
    }
    // 4980A7先固定当前header、撤EH资格再减持；Free失败外传，不重试析构。
    query.ReleaseNormalCached(query.Value().GetString());
    return result;
}
}
