#pragma once

namespace KaGuYa::Start::Runtime
{
class FileManagerRootRuntime;
// 只借原运行根的key来源，不保存某代manager、归档kind、key长度或Ref快照。
class EnvironmentArchiveNative18Scope final
{
public:
    explicit EnvironmentArchiveNative18Scope(FileManagerRootRuntime&) noexcept;
    ~EnvironmentArchiveNative18Scope() noexcept;
    EnvironmentArchiveNative18Scope(const EnvironmentArchiveNative18Scope&) = delete;
    EnvironmentArchiveNative18Scope& operator=(const EnvironmentArchiveNative18Scope&) = delete;
private:
    FileManagerRootRuntime* previous_;
};
void SerializeSecretFlagsNative18(void* archive, void* serializer18, FileManagerRootRuntime&);
void SerializeGlobalParamsNative18(void* archive, void* serializer18, FileManagerRootRuntime&);
// 真正归档slot00的入口从明确绑定的key作用域取源，不猜归档类型或使用备用全局。
void SerializeSecretFlagsNative18(void* archive, void* serializer18);
void SerializeGlobalParamsNative18(void* archive, void* serializer18);
}
