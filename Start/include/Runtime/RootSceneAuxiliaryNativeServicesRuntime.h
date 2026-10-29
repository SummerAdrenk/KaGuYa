#pragma once
#include <Runtime/RootScenePublicNativeServicesRuntime.h>
#include <Runtime/RootSystemInformationDisplayNativeSource.h>
#include <Runtime/RootLogicalRectangleRuntime.h>
#include <Runtime/RootNameRegistrationNativeSource.h>
#include <Runtime/GeneralConfirmDialogNative.h>
#include <Runtime/RootSaveLoadConfirmDialogNativeSource.h>
#include <Runtime/RootSaveLoadNativeSource.h>
#include <Runtime/SaveLoadPageFactoryNative.h>
#include <Runtime/SaveLoadPageSlotNative.h>
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <Runtime/SaveLoadMessageBoxNative.h>
#include <Runtime/SavePreviewNativeCaptureRuntime.h>
#include <Runtime/ParamsResourceNameRuntime.h>
#include <Runtime/RootTransitionFrameRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 所有来源必须属于同一Process；文件依赖特指存档/journal，不替换封包资源文件API。
struct RootSceneAuxiliaryNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IGuiControlResourceLoadServices& pictures;
    ISpriteNativeResourceSource& sprites;
    IStartFileApi& files;
    const IMessageEnvironmentPathSource& messagePaths;
    IScrVmProgramRandSource& random;
    const SaveFileFormatProfile& saveProfile;
    IModalDialogRunnerServices& modal;
    RootEditWindowNativeSource& edits;
    EditWindowNativeViewportServices& viewport;
    IRootLogicalRectanglePlatformApi& rectangles;
    const RuntimeCString& confirmationMask;
    const RuntimeCString& confirmationFont;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
};

// 生产Scene外部依赖的唯一服务集合；不持原task/GUI/预览/目录或额外Ref。
// 只冷装配，原业务构造/取根/文件I/O仍留在其各个原调用点。
// 此集合先于RootSceneNativeServicesRuntime存在，并活过全部场景/GUI的末Ref和Post解绑。
class RootSceneAuxiliaryNativeServicesRuntime final
{
public:
    explicit RootSceneAuxiliaryNativeServicesRuntime(RootSceneAuxiliaryNativeDependencies)noexcept;
    RootSceneAuxiliaryNativeServicesRuntime(const RootSceneAuxiliaryNativeServicesRuntime&)=delete;
    RootSceneAuxiliaryNativeServicesRuntime& operator=(const RootSceneAuxiliaryNativeServicesRuntime&)=delete;
    RootSaveLoadNativeSource& Persistence()noexcept{return persistence_;}
    SaveLoadPageFactoryNative& Pages()noexcept{return pages_;}
    SaveLoadMessageBoxNativeServices& DeletionConfirmation()noexcept{return deletion_;}
    SaveLoadConfirmDialogNativeServices& SaveConfirmation()noexcept{return saveConfirmation_;}
    GeneralConfirmDialogNativeServices& GeneralConfirmation()noexcept{return general_.Services();}
    NameRegistrationDialogNativeServices& NameRegistration()noexcept{return names_;}
    ISavePreviewCaptureServices& Preview()noexcept{return preview_;}
    SystemInformationDisplayNativeServices& Information()noexcept{return information_;}
private:
    class Capture final:public INativeScreenCaptureServices
    {
    public:
        explicit Capture(NativeGraphicsRuntimeLibrary& graphics)noexcept:graphics_(graphics){}
        GraphicsSurfaceReference CaptureScreenNativeAt4ACDD0()override;
    private:
        NativeGraphicsRuntimeLibrary& graphics_;
    } capture_;
    RootSceneGuiNativeScreenSource screen_;
    GuiModalInputSource input_;
    RootLogicalRectangleRuntime rectangles_;
    RootSystemInformationDisplayNativeSource systemSource_;
    SystemDialogNativeServices systemDialog_;
    SystemInformationDisplayNativeServices information_;
    RootGuiModalSource modalRoot_;
    RootGeneralConfirmDialogNativeSource general_;
    RootNameRegistrationNativeSource nameSource_;
    NameRegistrationDialogNativeServices names_;
    RootSaveLoadConfirmDialogNativeSource saveConfirmationSource_;
    SaveLoadConfirmDialogNativeServices saveConfirmation_;
    ParamsResourceNameRuntime resourceNames_;
    RootSavePreviewRuntimeSource previewSource_;
    PatternPreviewNativeServices previewServices_;
    SavePreviewNativeCaptureRuntime preview_;
    RootSaveLoadNativeSource persistence_;
    EnvironmentSaveLoadPageSecondNativeSource pageSource_;
    RootSaveLoadPageSlotNativeSource slotSource_;
    RootSaveLoadSystemPageNativeSource systemPageSource_;
    RootSaveLoadPopupNativeSource popupSource_;
    SaveLoadPageSecondNativeServices commonPage_;
    SaveLoadPageSlotNativeServices slots_;
    SaveLoadPageNativeServices regularPages_;
    SaveLoadSystemPageNativeServices systemPage_;
    SaveLoadPopupNativeServices popup_;
    SaveLoadPageFactoryNative pages_;
    SaveLoadMessageBoxNativeChildFactoriesRuntime deletionChildren_;
    SaveLoadMessageBoxNativeServices deletion_;
};
}
