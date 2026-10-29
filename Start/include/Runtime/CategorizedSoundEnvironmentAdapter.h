#pragma once

#include <Runtime/CategorizedSoundEnvironmentWire.h>
#include <string>

namespace KaGuYa::Start::Runtime
{
struct ParamsGameSystem;
class EnvironmentSoundParamsArchive;
class ISettingTagErrorSource;
class SoundTypeNative;

// 仅归属 env 文件 owner；不进入 44h 原生正文，也不替换 Grouped 设置快照。
// 配置重建须重建本 owner。构造时现代模型必须已由同一原配置完整构造。
class CategorizedSoundEnvironmentAdapter final
{
public:
    using Result = CategorizedSoundEnvironmentWire::Result;
    CategorizedSoundEnvironmentAdapter(const ParamsGameSystem&,
        EnvironmentSoundParamsArchive&, ISettingTagErrorSource&);
    CategorizedSoundEnvironmentAdapter(const CategorizedSoundEnvironmentAdapter&) = delete;
    CategorizedSoundEnvironmentAdapter& operator=(const CategorizedSoundEnvironmentAdapter&) = delete;

    // 绑定错误抛 invalid_argument；wire 错误保留 Result 的 cursor/location。
    // 同一次调用内不得并发替换现代对象图，失败也已回填完整标量前缀。
    [[nodiscard]] Result Read(EnvironmentSoundParamsArchive&,
        std::span<const std::byte>, std::size_t cursor);
    [[nodiscard]] std::vector<std::byte> Write(EnvironmentSoundParamsArchive&);

private:
    enum class Role { Bgm, Effect, Loop, Voice };
    struct ChildMetadata final
    {
        std::size_t sourceIndex{}; // 原 per-file children 中的出现位置，包含跳过项。
        std::size_t targetIndex{}; // effect 或 character 的实际构造序号。
        std::size_t registrationIndex = CategorizedSoundEnvironmentWire::NoIndex;
        std::string name; // 原 CP932 字节，仅作身份核验，绝不用于查找首项。
        std::uint8_t defaultIsDisp{};
    };
    struct Metadata final
    {
        Role role{};
        std::size_t sourceIndex{};
        std::size_t masterIndex{}; // vector 顺序同时就是旧顶层注册顺序。
        std::string name;
        std::vector<ChildMetadata> children;
    };
    struct Binding final
    {
        CategorizedSoundEnvironmentWire::Value* value;
        std::uint8_t* isDisp;
        SoundTypeNative* modern;
        std::uint8_t* display;
    };

    [[nodiscard]] std::vector<Binding> Bind(EnvironmentSoundParamsArchive&, bool captureIndices);
    static void Gather(std::span<const Binding>) noexcept;
    static void Scatter(std::span<const Binding>) noexcept;

    // 仅保存原注册 provenance、序号和值侧带，不长期持有任何现代元素指针。
    std::vector<Metadata> metadata_;
    std::vector<CategorizedSoundEnvironmentWire::Entry> entries_;
};
}
