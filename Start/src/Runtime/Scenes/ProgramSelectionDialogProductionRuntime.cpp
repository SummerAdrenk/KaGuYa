#include <Runtime/ProgramSelectionDialogProductionRuntime.h>

#include <utility>

namespace KaGuYa::Start::Runtime
{
ProgramSelectionDialogProductionRuntime::
ProgramSelectionDialogProductionRuntime(
    ProgramSelectionDialogProductionDependencies dependencies)
    : inputStateLifetime_(&dependencies.inputStateLifetime),
      dialogBoundsAt74_{
          0, 0, dependencies.screenWidth, dependencies.screenHeight}
{
    try
    {
        // 0x4EAB70 先建立外层状态；两种模式只替换派生虚表和两类 child。
        dialog_ = std::make_shared<ProgramSelectionDialogRuntime>(
            dependencies.mode, dependencies.dialogServices);

        // 0x4DE262/0x4DF162：Main 总是第一个注册项。
        mainAtChild0_ = std::make_shared<
            ProgramSelectionMainDialogObjectGraphRuntime>(
                dependencies.surfaceFactory,
                dependencies.graphics,
                dependencies.buttonInputServices,
                dependencies.values,
                dependencies.mainServices,
                dependencies.mode,
                dependencies.mainDirtyRectSink);

        // 0x4DE32F/0x4DF22F：System 总是第二个注册项。
        systemAtChild1_ = std::make_shared<
            ProgramSelectionSystemDialogObjectGraphRuntime>(
                dependencies.surfaceFactory,
                dependencies.buttonInputServices,
                dependencies.systemServices,
                dependencies.mode);

        backgroundFrameAt98_ =
            dependencies.surfaceFactory.LoadStatic("bg_black.bgd");
        if (!backgroundFrameAt98_)
            throw 1;

        // 0x415010(width,height,0,1)：无 alpha，并要求创建时清空。
        compositeFrameAtA4_ = dependencies.surfaceFactory.CreateBlank(
            dependencies.screenWidth,
            dependencies.screenHeight,
            false,
            true);
        if (!compositeFrameAtA4_)
            throw 1;

        dependencies.inputStateLifetime.InstallGameInputStateAt536EA0();
        inputStateInstalled_ = true;
    }
    catch (...)
    {
        // 两个原构造函数的 EH cleanup 均跳入 0x4F8D50，而不是按成员逆序退栈。
        ReleaseAt4F8D50();
        throw;
    }
}

ProgramSelectionDialogProductionRuntime::~ProgramSelectionDialogProductionRuntime()
{
    ReleaseAt4F8D50();
}

const ProgramSelectionDialogRef&
ProgramSelectionDialogProductionRuntime::Dialog() const noexcept
{
    return dialog_;
}

const std::shared_ptr<ProgramSelectionMainDialogObjectGraphRuntime>&
ProgramSelectionDialogProductionRuntime::MainAtChild0() const noexcept
{
    return mainAtChild0_;
}

const std::shared_ptr<ProgramSelectionSystemDialogObjectGraphRuntime>&
ProgramSelectionDialogProductionRuntime::SystemAtChild1() const noexcept
{
    return systemAtChild1_;
}

const std::shared_ptr<RuntimeGraphicsSurface>&
ProgramSelectionDialogProductionRuntime::BackgroundFrameAt98() const noexcept
{
    return backgroundFrameAt98_;
}

const std::shared_ptr<RuntimeGraphicsSurface>&
ProgramSelectionDialogProductionRuntime::CompositeFrameAtA4() const noexcept
{
    return compositeFrameAtA4_;
}

const RECT&
ProgramSelectionDialogProductionRuntime::DialogBoundsAt74() const noexcept
{
    return dialogBoundsAt74_;
}

void ProgramSelectionDialogProductionRuntime::ReleaseAt4F8D50() noexcept
{
    // 0x4F8D50：+0xA4 先于 +0x98。
    compositeFrameAtA4_.reset();
    backgroundFrameAt98_.reset();

    // 0x4F6DC0 -> 0x4D8020 从 begin 向 end 释放注册项。
    mainAtChild0_.reset();
    systemAtChild1_.reset();
    dialog_.reset();

    // 0x4F7AC0 在注册 child 容器之后才释放 GuiDialog 的输入状态。
    if (inputStateInstalled_)
    {
        inputStateLifetime_->ReleaseGameInputStateAt4F7AC0();
        inputStateInstalled_ = false;
    }
}
}
