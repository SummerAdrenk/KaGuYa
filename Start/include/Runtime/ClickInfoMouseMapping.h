#pragma once

namespace KaGuYa::Start::Runtime
{
class SaveDataEnvironmentArchive;
class ClickInfoArchive;

// 477AB3、42A2B8、51DF6F：环境槽 1 Save+8C -> 槽 5 ClickInfo。
// 这是三处内联转换的现代共享入口，不代表原程序存在此二参 ABI。
void ApplyClickInfoMouseMapping(
    const SaveDataEnvironmentArchive& save, ClickInfoArchive& click) noexcept;
}
