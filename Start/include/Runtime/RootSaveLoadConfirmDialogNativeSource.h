#pragma once
#include <Runtime/SaveLoadConfirmDialogNative.h>

namespace KaGuYa::Start::Runtime
{
class NativeGraphicsRuntimeLibrary;
// 两个独立原CRT构造：fresh只在成功返回后交给caller；注册atexit由进程总装另行承担。
void ConstructSaveLoadConfirmMaskNameNativeAt406AA0(RuntimeCString& fresh,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void ConstructSaveLoadConfirmFontFaceNativeAt406B30(RuntimeCString& fresh,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
// mask/font只借原配置槽，不缓存chars，构造不Capture、不装载资源或建立另一个owner。
class RootSaveLoadConfirmDialogNativeSource final : public ISaveLoadConfirmDialogNativeSource
{
public:
    RootSaveLoadConfirmDialogNativeSource(IGuiControlResourceLoadServices& pictures,
        NativeGraphicsRuntimeLibrary& graphics, const RuntimeCString& mask, const RuntimeCString& font,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept
        : pictures_(pictures), graphics_(graphics), mask_(mask), font_(font), strings_(strings) {}
    void* CaptureLoadReceiverAt413BE0() override;
    bool LoadIntoAt4800A0(void*, GraphicsSurfaceNativeBody&, const char*) override;
    std::uint8_t LoadSurfaceAt480000(void*, YtlReferenceSlotRuntime&, const char*) override;
    const RuntimeCString& MaskNameAt61A608() override;
    const char* FontFaceAt61A60C() override;
    bool CreateTextFontAt4388B0(const GraphicsTextNativeFontParameters*) override;
    IGraphicsTextApi& TextAt4182F0() override;
    std::int32_t ReadImmutableDwordAtOriginalVa(std::uint32_t) override;
private:
    IGuiControlResourceLoadServices& pictures_;
    NativeGraphicsRuntimeLibrary& graphics_;
    const RuntimeCString& mask_;
    const RuntimeCString& font_;
    IRuntimeStringManagerSource& strings_;
};
}
