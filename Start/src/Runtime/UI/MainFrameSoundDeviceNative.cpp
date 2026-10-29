#include <Runtime/MainFrameSoundDeviceNative.h>
#include <Runtime/NativeSoundRuntimeLibrary.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Logging/Logger.h>
#include <cstring>
#include <emmintrin.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
static_assert(sizeof(void*)==4&&sizeof(DWORD)==4&&sizeof(GUID)==0x10);
static_assert(sizeof(YtlReferenceSlotRuntime)==0x0C);
static_assert(offsetof(YtlReferenceSlotRuntime,baseAt00)==0&&offsetof(YtlReferenceSlotRuntime,biasAt04)==4);
static_assert(offsetof(YtlReferenceSlotRuntime,controlAt08)==8);
static_assert(sizeof(SoundManagerNativeBody50h)==0x50&&offsetof(SoundManagerNativeBody50h,deviceAt40)==0x40);
static_assert(offsetof(MainFrameNativeBody90h,windowAt00)==0);
static_assert(offsetof(EditWindowEndpointNativeBody24h,windowAt04)==4);
static_assert(offsetof(EditWindowEndpointNativeBody24h,previousAt20)==0x20);
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
struct GuidWords16 final{DWORD words[4];};
static_assert(sizeof(GuidWords16)==0x10);
__declspec(noinline) GuidWords16 Snapshot16(const GUID* guid)noexcept
{
    GuidWords16 snapshot;
    // 42417B/424208为完整16字节快照；intrinsic让编译器知道返回对象已写入。
    // 不用间接写局部的inline asm：优化后它可能省略向隐藏返回缓冲的复制。
    const auto value=_mm_loadu_si128(reinterpret_cast<const __m128i*>(guid));
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&snapshot),value);
    return snapshot;
}
SoundManagerNativeBody50h* CaptureManager(IMainFrameSoundDeviceNativeSource& source,bool* emptySlot=nullptr)
{
    source.EnsureRootAt413BE0();
    auto* root=source.RootAt6198C4();
    const auto& ref=source.ManagerRefAt24(root);
    // 42415A..424179 / 4241E7..424206：base先读；空base不读control/bias。
    const auto base=Read(ref.baseAt00);
    if(!base){if(emptySlot)*emptySlot=true;return nullptr;}
    const auto control=Read(ref.controlAt08);
    const auto stride=control?Read(*reinterpret_cast<const DWORD*>(reinterpret_cast<DWORD>(control)+0x0Cu)):0u;
    const auto bias=Read(ref.biasAt04);
    return reinterpret_cast<SoundManagerNativeBody50h*>(base+bias*stride);
}
LRESULT Previous(MainFrameNativeBody90h& frame,UINT message,WPARAM w,LPARAM l,IMainFrameNativeWindowApi& windows)
{
    // 4241AE/4241B1：到达转发尾才依次读当前HWND及上一过程，不用入口副本。
    const auto window=Read(frame.windowAt00.windowAt04);
    const auto previous=Read(frame.windowAt00.previousAt20);
    return windows.CallWindowProcA(previous,window,message,w,l);
}
}
RootMainFrameSoundDeviceNativeSource::RootMainFrameSoundDeviceNativeSource(RootRuntimeOwner& root,
    NativeSoundRuntimeLibrary& library)noexcept:root_(root),library_(library){}
const GUID* RootMainFrameSoundDeviceNativeSource::DefaultDeviceAt4240A9()
{return library_.Calls().DefaultDeviceAt45D4E2();}
void RootMainFrameSoundDeviceNativeSource::EnsureRootAt413BE0(){root_.EnsureCreatedAt413BE0();}
void* RootMainFrameSoundDeviceNativeSource::RootAt6198C4(){return root_.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootMainFrameSoundDeviceNativeSource::ManagerRefAt24(void* selectedRoot)
{
    if(!selectedRoot)return *reinterpret_cast<YtlReferenceSlotRuntime*>(0x24u);
    return static_cast<RootRuntimePayload*>(selectedRoot)->SoundManagerSlotAt24();
}
LRESULT DeviceChangeMainFrameNativeAt424070(MainFrameNativeBody90h& frame,UINT message,WPARAM w,LPARAM l,
    IMainFrameSoundDeviceNativeSource& source,IMainFrameNativeWindowApi& windows,ISoundDeviceRebuildNative& rebuilder)
{
    if(w==7)
    {
        const auto* borrowed=source.DefaultDeviceAt4240A9();
        bool emptyComparisonSlot=false;
        auto* comparisonManager=CaptureManager(source,&emptyComparisonSlot);
        if(emptyComparisonSlot)
        {
            // 2274/2275实证：启动广播可在root+24为空时到达42417B；这是安全边界，不复刻原AV。
            // 仅首次空base转发；不读borrowed/control/bias，也不把非空base的解址故障纳入此域。
            const auto lastError=::GetLastError();
            Logging::WriteFormat(Logging::Level::Warning,
                "Start_RS MainFrame 424070 safety boundary: empty comparison manager slot at root+24; "
                "original 42417B would read 0x40; forwarding current previous WndProc without GUID read/rebuild "
                "(message=0x%X wParam=0x%X lParam=0x%X).",
                message,static_cast<unsigned>(w),static_cast<unsigned>(l));
            ::SetLastError(lastError);
            return Previous(frame,message,w,l,windows);
        }
        const auto snapshot=Snapshot16(&comparisonManager->deviceAt40); // 42417B..424189，先固定manager GUID。
        const auto* words=reinterpret_cast<const DWORD*>(borrowed);
        for(unsigned i=0;i!=4;++i)
        {
            // 424190..42419F：只读到首个不同DWORD；不能memcmp或预取整个borrowed GUID。
            if(Read(words[i])!=snapshot.words[i])
            {
                auto* selected=CaptureManager(source); // 4241D7，第二次Ensure/Root/原Ref解址。
                const auto value=Snapshot16(borrowed); // 424208，仍从旧借用指针晚读全部16字节。
                GUID argument;
                std::memcpy(&argument,&value,sizeof(argument));
                rebuilder.RebuildAt432C20(selected,argument); // 424213，按值传递，不传GUID指针。
                return 0; // 424218，仅正常返回到此，不转发、不代内层提交GUID。
            }
        }
    }
    return Previous(frame,message,w,l,windows);
}
}
