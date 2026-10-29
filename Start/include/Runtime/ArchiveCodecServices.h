#pragma once

namespace KaGuYa::Start::Runtime
{
class IArchiveCodecManagerSource;
class IArchiveHuffmanErrorSource;

// 现代装配的借用依赖，绝不驻留在原30h/14h/1010h/1020h正文中。
// 无默认值：生产必须显式提供同一manager来源与真实环境错误报告链。
struct ArchiveCodecServices final
{
    IArchiveCodecManagerSource& managers;
    IArchiveHuffmanErrorSource& errors;
};
}
