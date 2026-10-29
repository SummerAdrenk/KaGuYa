#pragma once

#include <Windows.h>
#include <d3d9.h>

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace KaGuYa::RenderDX
{
class Renderer final
{
public:
    static Renderer& Instance() noexcept;

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    int Initialize(HWND window, int width, int height, int requirePostPixelBlend,
        int padTextureToPowerOfTwo);
    int Release() noexcept;

    int GetBits() const noexcept;
    int GetPitch() const noexcept;
    int GetErrorMessage() const noexcept;
    int IsFiltering() const noexcept;
    int GetMagFiltering() const noexcept;
    int GetMinFiltering() const noexcept;
    int SetMagFiltering(int mode) noexcept;
    int SetMinFiltering(int mode) noexcept;
    int UpdatePrimary(const RECT* rect);
    int RenderScreen() noexcept;
    int RenderScreenHdc() noexcept;
    const int* Resize(int width, int height, int mode) noexcept;
    int TestCooperativeLevel() const noexcept;
    int RenderReset() noexcept;

private:
    Renderer() = default;
    ~Renderer();

    struct DeviceCandidate
    {
        D3DDEVTYPE type = D3DDEVTYPE_HAL;
        D3DFORMAT depthFormat = D3DFMT_UNKNOWN;
        DWORD behaviorFlags = 0;
    };

    struct ScreenVertex
    {
        float x;
        float y;
        float z;
        float rhw;
        D3DCOLOR color;
        float u;
        float v;
    };

    // 原 CD3DDevice 的 Resize 返回 this+104，过滤 setter 返回 this。
    // 这段布局只承载导出层可观察的地址关系，不参与 D3D 对象生命周期。
    struct AbiObservableState
    {
        std::array<std::byte, 104> reserved{};
        std::array<int, 4> viewport{};
    };
    static_assert(offsetof(AbiObservableState, viewport) == 104);

    int InitializeResources(HWND window, int width, int height, int requirePostPixelBlend,
        int padTextureToPowerOfTwo);
    bool CreateDevice(int requirePostPixelBlend);
    bool BuildDeviceCandidate(D3DDEVTYPE type, int requirePostPixelBlend,
        DeviceCandidate& result, int& progressMask) noexcept;
    D3DFORMAT SelectDepthFormat(D3DDEVTYPE type) const noexcept;
    int PreferredDepthBits() const noexcept;
    bool CreatePrimaryResources(bool padTextureToPowerOfTwo);
    bool ResetDevice() noexcept;
    int PresentFrame() noexcept;
    void ComputeViewport(int width, int height, int mode) noexcept;
    void ReleasePrimaryResources() noexcept;
    void ReleaseDeviceResources() noexcept;
    enum class InitializationErrorLayer { Direct3D, Device, Texture };
    void SetInitializationError(std::wstring_view leafMessage,
        InitializationErrorLayer layer = InitializationErrorLayer::Direct3D);
    void AppendInitializationException(const char* message);
    static void LogHresult(const char* operation, HRESULT result) noexcept;

    static int ExternalFilterMode(D3DTEXTUREFILTERTYPE value) noexcept;
    D3DTEXTUREFILTERTYPE SelectFilter(int mode, bool magnification) const noexcept;
    static unsigned int NextPowerOfTwo(unsigned int value) noexcept;

    HWND window_ = nullptr;
    IDirect3D9* direct3D_ = nullptr;
    IDirect3DDevice9* device_ = nullptr;
    IDirect3DTexture9* primaryTexture_ = nullptr;
    IDirect3DSurface9* uploadSurface_ = nullptr;
    D3DLOCKED_RECT uploadLock_{};
    D3DDISPLAYMODE displayMode_{};
    D3DCAPS9 caps_{};
    D3DPRESENT_PARAMETERS presentParameters_{};
    D3DDEVTYPE deviceType_ = D3DDEVTYPE_HAL;
    int sourceWidth_ = 0;
    int sourceHeight_ = 0;
    int textureWidth_ = 0;
    int textureHeight_ = 0;
    D3DFORMAT textureFormat_ = D3DFMT_UNKNOWN;
    int targetWidth_ = 0;
    int targetHeight_ = 0;
    int resizeMode_ = -1;
    AbiObservableState abiState_{};
    bool filteringAvailable_ = false;
    DWORD textureFilterCaps_ = 0;
    D3DTEXTUREFILTERTYPE magFilter_ = D3DTEXF_POINT;
    D3DTEXTUREFILTERTYPE minFilter_ = D3DTEXF_POINT;
    std::string errorMessage_;
};
}
