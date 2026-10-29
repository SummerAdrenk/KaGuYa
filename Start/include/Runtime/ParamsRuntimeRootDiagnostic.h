#pragma once

#include <cstdint>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRoot;

// 现代适配记录，不拥有根或child，不保存double/Ref副本。须长于所有诊断调用。
// Bind/Unbind与根的Serialize/销毁由调用者串行化；查找不延长任何对象寿命。
class ParamsRuntimeRootDiagnostic final
{
public:
    ParamsRuntimeRootDiagnostic() noexcept;
    ~ParamsRuntimeRootDiagnostic() noexcept;
    ParamsRuntimeRootDiagnostic(const ParamsRuntimeRootDiagnostic&) = delete;
    ParamsRuntimeRootDiagnostic& operator=(const ParamsRuntimeRootDiagnostic&) = delete;

    // 显式适配入口；一个正文只允许一份绑定，重复绑定为现代契约错误。
    void Bind(ParamsRuntimeRoot& root) noexcept;
    void Unbind() noexcept;
    void BeginRead() noexcept;
    void InvalidateGameSystem() noexcept;
    [[nodiscard]] static ParamsRuntimeRootDiagnostic* Find(const ParamsRuntimeRoot& root) noexcept;
    static void Forget(const ParamsRuntimeRoot& root) noexcept;

    bool versionWritten = false;
    bool readCompleted = false;
    // 仅记录当前识别的静态header标签；不作为生产输入或原全局CString的镜像。
    std::string_view header;
    std::uint32_t rawTripleCountMetadata = 0;
    std::uint64_t gameSystemPublicationEpoch = 0;

private:
    const ParamsRuntimeRoot* root_ = nullptr;
    ParamsRuntimeRootDiagnostic* next_ = nullptr;
};
}
