#include "BackendLibrary.h"

#include <atlstr.h>
#include <cstddef>
#include <memory>
#include <new>

namespace KaGuYa::Graphics
{
BackendLibrary::~BackendLibrary() noexcept(false) { Unload(); }

BackendLibrary::Library::~Library() noexcept(false)
{
    // 原10008120仅卸载模块，不调用业务Release。
    if (module) { FreeLibrary(module); module = nullptr; }
}

bool BackendLibrary::Load(const char* directory, const char* fileName)
{
    // 原10001050/10008160：先创建body，退旧后发布base，再分配control，最后才构造路径。
    library_.Publish(new Library, 8);
    ATL::CStringA path(directory);
    path.Append(fileName);
    // 原100010CD..100010F7：路径完成后先保存body，API返回才向该body发布句柄。
    Library* const library = library_.get();
    const HMODULE handle = LoadLibraryA(path.GetString());
    library->module = handle;
    return handle != nullptr;
}

FARPROC BackendLibrary::Resolve(const char* name) const
{
    // 原10001515构造受保护，但1000151A已撤保护，10001529的API异常不退名称串。
    // 手工存储保留构造内的基类展开，不为已完成构造的字符串增加作用域析构。
    static_assert(sizeof(ATL::CStringData) == 16);
    // 原100014CF/10001524只提前捕获body，不提前读句柄，也不额外增持Ref。
    const Library* const library = library_.get();
    alignas(ATL::CStringA) std::byte storage[sizeof(ATL::CStringA)];
    auto* symbol = ::new (storage) ATL::CStringA(name);
    const FARPROC procedure = GetProcAddress(library ? library->module : nullptr, symbol->GetString());
    symbol->~CStringT();
    return procedure;
}

HMODULE BackendLibrary::Handle() const noexcept
{
    return library_ ? library_->module : nullptr;
}

void BackendLibrary::Close()
{
    // 原100014A8判断base，即使LoadLibrary失败也要查询；Resolve返回前已经释放CString。
    if (library_)
    {
        const auto release = reinterpret_cast<void (__cdecl*)()>(Resolve("Release"));
        if (release) release();
    }
    // 原1000155A：control为空不清base；异常更不能提前卸载，供后继Close重试。
    if (library_.HasOwner()) library_.reset();
}

void BackendLibrary::Unload()
{
    library_.reset();
}
}
