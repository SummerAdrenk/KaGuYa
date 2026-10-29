#pragma once

#include <Runtime/SaveFileDeleteFactoryNative.h>
#include <Runtime/ScriptParamsNativeBody.h>

namespace KaGuYa::Start::Runtime
{
class ISaveFileCatalogItemNativeSource : public ISaveFileDeleteFactoryNativeSource
{
public:
    // 必须在传入的局部70h CParams上执行原读容器叶；mode=1，返回仅消费AL。
    // 主线可直接委托ReadSaveFileContainerNativeAt4C7420；不重新捕获Root/Params。
    // 此来源仍须宿主装配，不能以metadataLoader/标题解析替代完整读容器。
    virtual std::uint32_t ReadContainerAt4C7420(
        ScriptParamsNativeBody& fixedReceiver, const char* fullPath, std::uint32_t mode) = 0;
};

// 原48E5F0为cdecl(page,item)，不消费ECX；source/memory是显式来源边界。
// -1 quick、-2 auto、>=0 regular；其它负page仅构造/清理局部对象。
// 失败保留旧条目/count；quick/auto不写latest，普通成功才调用共享48D290。
// 下标按原32位寻址，不限8、不新增范围检查；调用方保证有效地址与生命周期。
void RefreshSaveFileCatalogItemNativeAt48E5F0(std::int32_t page, std::int32_t item,
    ISaveFileCatalogItemNativeSource& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 原48EE00: push item; push page; call48E5F0; add esp,8; ret8。
// 原stdcall wrapper不消费ECX，仅有两业务实参；此处普通C++函数不伪造this参数。
void RefreshSaveFileCatalogItemNativeAt48EE00(std::int32_t page, std::int32_t item,
    ISaveFileCatalogItemNativeSource& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
