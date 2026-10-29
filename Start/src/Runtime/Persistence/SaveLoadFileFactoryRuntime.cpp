#include <Runtime/SaveLoadFileFactoryRuntime.h>
#include <Runtime/SaveFileContainerRuntime.h>

#include <bit>
#include <cstdio>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::string FormatFileName(const std::string& prefix, std::int32_t slot, const char* format)
{
    // 47AF71/47B001 的 %02d 是最小宽度，不是两位截断；负数也不预先拒绝。
    const int size = std::snprintf(nullptr, 0, format, prefix.c_str(), slot);
    if (size < 0) throw std::runtime_error("save filename formatting failed");
    std::string name(static_cast<std::size_t>(size) + 1, '\0');
    if (std::snprintf(name.data(), name.size(), format, prefix.c_str(), slot) != size)
        throw std::runtime_error("save filename formatting length changed");
    name.resize(static_cast<std::size_t>(size));
    return name;
}
}

bool Win32SaveLoadDeleteFileApi::DeleteFileAt47655C(const std::string& path)
{
    return ::DeleteFileA(path.c_str()) != FALSE;
}

SaveLoadFileFactoryRuntime::SaveLoadFileFactoryRuntime(SaveFileContainerRuntime& container,
    ISaveLoadPreviewServices& preview, ISaveLoadDeleteFileApi& files, const std::string& directory,
    std::string prefix, std::int32_t slotCount, std::int32_t& latest)
    : container_(container), preview_(preview), files_(files), directory_(directory),
      prefix_(std::move(prefix)), slotCount_(slotCount), latest_(latest)
{
}

std::string SaveLoadFileFactoryRuntime::BuildDataFileNameAt47AF10(std::int32_t slot)
{
    return FormatFileName(prefix_, slot, "%s%02d.dat");
}

std::string SaveLoadFileFactoryRuntime::BuildPreviewFileNameAt47AFA0(std::int32_t slot)
{
    return FormatFileName(prefix_, slot, "%s%02d.bmp");
}

std::int32_t SaveLoadFileFactoryRuntime::LoadSlotAt480830(std::int32_t slot)
{
    try
    {
        bool loaded;
        {
            const auto name = BuildDataFileNameAt47AF10(slot);
            loaded = container_.ReadAt4C7420(name, 1);
        }
        if (!loaded) throw 1; // 480893..48091B：先释放名称，再报容器失败。
        return 0;
    }
    catch (int code) { return code; }
    catch (...) { return -1; } // FuncInfo 5E9104：.H 在先，type0 在后。
}

std::int32_t SaveLoadFileFactoryRuntime::SaveSlotAt488CF0(std::int32_t slot, bool screenshot)
{
    return SaveSlotAt488CF0(slot, screenshot ? std::int32_t{1} : std::int32_t{0});
}

std::int32_t SaveLoadFileFactoryRuntime::SaveSlotAt488CF0(std::int32_t slot, std::int32_t mode)
{
    try
    {
        bool saved;
        {
            const auto name = BuildDataFileNameAt47AF10(slot);
            saved = container_.WriteAt4C7B10(name);
        }
        if (!saved) throw 1;
        // 488D85..488DF1：无论 mode，先生成 BMP 名并拼接目录，再选择分支。
        // 此处保留路径快照，mode2 的 Capture 即使改变根目录也不重新计算。
        const auto previewPath = directory_ + BuildPreviewFileNameAt47AFA0(slot);
        if (mode == 0)
        {
            DeleteSlotAt476460(slot, false);
        }
        else
        {
            if (mode == 2) preview_.CaptureForSaveAt48DBE0();
            // 原仅区分0和2；其他非零 int 与1相同，不收窄成bool或增加范围拒绝。
            if (!preview_.SavePreviewAt489420(previewPath)) throw 3;
        }
        return 0;
    }
    catch (int code) { return code; }
    catch (...) { return -1; } // FuncInfo 5E9064；不回滚已写 DAT 或预览前缀。
}

void SaveLoadFileFactoryRuntime::DeleteSlotAt476460(std::int32_t slot, bool deleteData)
{
    std::string path;
    if (deleteData)
    {
        path = directory_ + BuildDataFileNameAt47AF10(slot);
        static_cast<void>(files_.DeleteFileAt47655C(path));
    }
    // 47655C 后重新读取目录并构造 BMP 路径。BOOL失败不阻止下一调用，异常则外传。
    // 4765C3 才替换局部路径；新路径构造失败时旧路径仍然存在。
    path = directory_ + BuildPreviewFileNameAt47AFA0(slot);
    static_cast<void>(files_.DeleteFileAt47655C(path));
}

void SaveLoadFileFactoryRuntime::DeleteAllAt5022E0()
{
    latest_ = -1; // 50230D：在容量检查、路径构造和第一个删除之前发布。
    // 502493 的 INC 按32位回绕，502497 再作有符号比较；不用 C++ signed 溢出。
    for (std::int32_t slot = 1; slot <= slotCount_;
        slot = std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(slot) + 1U))
    {
        auto path = directory_ + BuildDataFileNameAt47AF10(slot);
        static_cast<void>(files_.DeleteFileAt47655C(path));
        path = directory_ + BuildPreviewFileNameAt47AFA0(slot);
        static_cast<void>(files_.DeleteFileAt47655C(path));
    }
}
}
