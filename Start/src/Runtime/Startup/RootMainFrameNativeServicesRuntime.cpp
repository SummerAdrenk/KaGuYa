#include <Runtime/RootMainFrameNativeServicesRuntime.h>
#include <Runtime/RootApplicationNativeServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ProcessWinHookRuntime.h>
#include <Runtime/ProcessAppModuleNative.h>
#include <Runtime/PostWindowNativeDrawScope.h>

namespace KaGuYa::Start::Runtime
{
RootMainFrameNativeServicesRuntime::RootMainFrameNativeServicesRuntime(
    RootMainFrameNativeServiceDependencies d) noexcept
    : engine_(d.application), endpoint_(d.endpoint),
      legacy_(CompiledLegacyDisplaySettingsFamily(),d.root,d.environment,d.application.Graphics(),
          d.application.Graphics().PresentCalls(),legacyModes_,legacyPlatform_,DefaultMainFrameNativeWindowApi(),
          d.nativeBase.resourceInstanceAt08),
      legacyWindows_(DefaultMainFrameNativeWindowApi(),legacy_.EnabledBinding()),
      roots_(d.root, d.environment.RootSource(), d.application.Graphics().RootSource(), d.strings,
          d.application.Graphics().Win32Source(), d.application.Edits().Viewport(),
          d.application.Edits().Maps(), d.application.Edits().Thunks()),
      modules_(d.sound, d.application.Graphics().RootSource(), d.strings, d.application.Graphics().Win32Source()),
      close_(d.application.Auxiliary().GeneralConfirmation()),
      context_(d.root, d.environment.RootSource(), d.input.RootSource(), d.nativeBase),
      popup_(context_, {DefaultMainFrameChevronNativeApi(), d.nativeBase}),
      rebuild_(d.root, d.environment.RootSource(), d.params, d.root.NativeSoundServices()),
      soundDevice_(d.root, d.sound),
      commands_(soundDevice_, legacyWindows_, rebuild_, popup_),
      terminals_(roots_, legacyWindows_, modules_, close_,
          {DefaultMainFrameUiNativeApi(), d.nativeBase}, commands_),
      positionSource_(d.root, d.params, d.environment.RootSource()),
      position_{positionSource_, DefaultMainFramePopupPositionNativeApi(), endpoint_.thunks, d.memory},
      actions_(d.root, d.environment.RootSource(), d.application.Draws().ToolInteraction(),
          d.application.Scenes().ManagerServices().controller),
      recovery_(d.environment, d.root, &::MessageBoxA, d.memory, d.strings, &d.application.Draws().Layers().text),
      aboutSource_(d.root, d.environment.RootSource(), d.params),
      about_{aboutSource_, DefaultAboutDialogNativeApi(), endpoint_, d.nativeBase},
      menuSource_(d.root, d.environment.RootSource(), d.application.ScriptErrors(), d.strings),
      menuCalls_(position_, actions_, d.root.NativeSoundServices(), recovery_,
          d.application.Scenes().ManagerServices(), d.settings, about_),
      menu_{menuSource_, menuCalls_, DefaultMainFrameMenuNativeApi()},
      dispatch_{roots_, legacyWindows_, terminals_, ProcessWindowHookMessageSource(),
          ProcessAppModuleNative(), ProcessAppModulePublicationNative(), DefaultAppModuleMessageLoopsNativeServices()},
      window_{dispatch_, d.application.Idle(), DefaultMainFrameNativeCallbackApi(), DefaultEditWindowNativeMessageApi(),
          endpoint_, DefaultUpdateUiNativeMemory(), d.memory},
      classes_{endpoint_.winModule, d.nativeBase.instanceAt04, d.nativeBase.resourceInstanceAt08,
          DefaultMainFrameWindowClassNativeApi()},
      application_{window_, windowClass_, classes_, d.application.Idle(), messages_,
          DefaultWindowClassNativeApi(), DefaultMainFrameApplicationNativeApi()}
{
    // 原601208是静态CLASSINFO初值；注册仍到原4316F0点才执行，不提前取根或Win32类。
    InitializeMainFrameWindowClassNativeAt601208(windowClass_, &InitialMainFrameWindowNativeAt438340);
}
int RootMainFrameNativeServicesRuntime::RunApplicationAt4342A0(LPSTR line, int show)
{
    RootLegacyDisplaySettingsScope legacyScope(legacy_.Family(),legacy_.EnabledBinding());
    RootLegacyDisplayApplicationSource lifecycle(engine_.Lifecycle(),legacy_.EnabledBinding());
    // 原成员单槽/Popup回调经同一长期服务进入；scope不接管业务对象或补清理。
    // 4B7611 -> 47E940会直接调用全部图层的reset虚槽，不能只在DrawLayersView内绑定。
    // 借用同一长期Draw服务覆盖消息泵/场景回调；不取根、不创建图层、不改变原catch。
    auto& draws=engine_.Draws();
    PostWindowNativeDrawScope drawing(draws.Layers(),draws.LayerRuntime(),&engine_.Graphics(),&draws.Gui());
    MainFrameMenuBindingNativeScope menu(menu_, position_);
    int result;
    try { result = RunMainFrameApplicationNativeAt4342A0(line, show, lifecycle, application_); }
    catch (...)
    {
        // 原frame/scope可能已经展开；只核已完成的释放边界，禁止在此补Cancel。
        // 原异常继续传播；附加恢复失败被显式诊断并保存在长期owner中，不替换第一异常。
        legacy_.ObserveRunUnwind();
        throw;
    }
    legacy_.FinishRun();
    return result;
}
}
