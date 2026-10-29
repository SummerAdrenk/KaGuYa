#pragma once

#include <Runtime/CgSceneNativeBody.h>
#include <Runtime/CgOperationDialogNative.h>
#include <Runtime/ParamsPatternNative.h>
#include <Runtime/RuntimeCStringArray.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/SpriteNative.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRoot;

// 仅原调用叶，禁止把4A84E0/4AD900或一整条Slot业务转发到现代执行器。
class ICgDisplayResourceNativeSource
{
public:
    virtual ~ICgDisplayResourceNativeSource() = default;
    virtual const ParamsRuntimeRoot* CaptureParamsAt418480(std::uint32_t callSite) = 0;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t callSite) = 0;
    // fresh是唯一原reader Ref12，进入时为空；483D90先发布后抛出时必须保留已发布Ref。
    // 可用ArchiveReaderFactoryRuntime::DetachReaderSlot/Adopt转移，不复制增持、不另存reader。
    // source须在成功及异常离开时把工厂持有转回fresh；普通/EH退役资格由本模块控制。
    virtual void OpenReaderAt483D90(ArchiveReaderReference& fresh, const char* borrowedName,
        std::uint32_t callSite) = 0;
    virtual bool LoadSpriteAt480650(SpriteNativeBodyA0h&, const ArchiveReaderReference&,
        std::uint8_t initialization, std::uint32_t argument, std::uint32_t callSite) = 0;
    virtual bool LoadSpriteMemberAt4B0840(SpriteNativeBodyA0h&, std::int32_t member,
        const ArchiveReaderReference&, std::uint8_t initialization, std::uint32_t callSite) = 0;
};
struct CgDisplayResourceNativeServices final
{
    ICgDisplayResourceNativeSource& source;
    IRuntimeStringManagerSource& strings;
    IGraphicsSurfaceApi& surfaces;
    CgOperationDialogNativeServices& operationDialog;
};

// fresh为未拥有资源的返回槽；原4AD900 ret8，index按signed比较，不补负数防护。
// 分配/写串异常在原返回槽EH资格内退役后外传；不折叠为成功空数组。
RuntimeCStringArrayReference* BuildCgDisplayNamesNativeAt4AD900(
    RuntimeCStringArrayReference& fresh, const ParamsPatternNative&, std::int32_t index,
    IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
// 原47A020首匹配/_mbsicmp，空表不检查query；不补kind或容器边界检查。
std::int32_t FindCgDisplayPatternNameNativeAt47A020(const ParamsPatternNative&, const char* query);
// 原4A84E0返回AL；只恢复原catch范围中的C++异常为false，已写Sprite/窗口不回滚。
// 直接读selectionAt18/displayWindowAt9C/operationWindowAtA8；不保存任何影子状态。
// Main须使真实GUI Input/Graphics/Screen scopes覆盖末尾operation dialog的当前+44分派。
bool ExecuteCgDisplayResourceNativeAt4A84E0(CgSceneNativeBodyB4h&, CgDisplayResourceNativeServices&);
}
