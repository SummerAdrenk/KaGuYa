#pragma once

#include "RendererReference.h"
#include <Windows.h>

namespace KaGuYa::Graphics
{
// 原CLoadLibrary的对象存在性、HMODULE和引用控制块是三个独立状态。
class BackendLibrary final
{
public:
    ~BackendLibrary() noexcept(false);
    bool Load(const char* directory, const char* fileName);
    FARPROC Resolve(const char* name) const;
    template<class T> T ResolveAs(const char* name) const
    {
        return reinterpret_cast<T>(Resolve(name));
    }
    template<class T> T Require(const char* name, const char* failure) const
    {
        const auto function = ResolveAs<T>(name);
        // 原初始化缺必需导出抛char*，由本后端Initialize自己的捕获层发布错误。
        if (!function) throw const_cast<char*>(failure);
        return function;
    }
    HMODULE Handle() const noexcept;
    bool HasObject() const noexcept { return bool(library_); }
    void Close();
    void Unload();

private:
    struct Library
    {
        virtual ~Library() noexcept(false);
        HMODULE module = nullptr;
    };
    RendererReference<Library> library_;
};
}
