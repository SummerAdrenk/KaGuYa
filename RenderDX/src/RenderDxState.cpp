#include "RenderDxState.h"
#include "TextureFormatSelection.h"
#include "TexturePixelConversion.h"
#include "SurfaceTransfer.h"

#include <Logging/Logger.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

namespace KaGuYa::RenderDX
{
namespace
{
template <typename T>
void ReleaseCom(T*& object) noexcept
{
    if (object != nullptr)
    {
        object->Release();
        object = nullptr;
    }
}

constexpr DWORD ScreenFvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

// RenderDX.dll:10002480 的候选顺序。分数先比较深度位差，再加入模板位权重。
constexpr std::array<D3DFORMAT, 6> DepthCandidates{
    D3DFMT_D16,
    D3DFMT_D15S1,
    D3DFMT_D24X8,
    D3DFMT_D24S8,
    D3DFMT_D24X4S4,
    D3DFMT_D32,
};

int DepthBits(const D3DFORMAT format) noexcept
{
    switch (format)
    {
    case D3DFMT_D16: return 16;
    case D3DFMT_D15S1: return 15;
    case D3DFMT_D24X8:
    case D3DFMT_D24S8:
    case D3DFMT_D24X4S4: return 24;
    case D3DFMT_D32: return 32;
    default: return 0;
    }
}

int StencilBits(const D3DFORMAT format) noexcept
{
    switch (format)
    {
    case D3DFMT_D15S1: return 1;
    case D3DFMT_D24X4S4: return 4;
    case D3DFMT_D24S8: return 8;
    default: return 0;
    }
}
}

Renderer& Renderer::Instance() noexcept
{
    static Renderer instance;
    return instance;
}

Renderer::~Renderer()
{
    Release();
}

int Renderer::Initialize(const HWND window, const int width, const int height,
    const int requirePostPixelBlend, const int padTextureToPowerOfTwo)
{
    Release();
    errorMessage_.clear();
    Logging::Initialize(L"RenderDX");

    // 原10001590的typed handler与100016D7 catch-all；报告本身再次抛出时
    // 不递归兜底，也不执行尚未到达的100016B7释放后缀。
    try
    {
        return InitializeResources(window, width, height,
            requirePostPixelBlend, padTextureToPowerOfTwo);
    }
    catch (char* message)
    {
        AppendInitializationException(message);
    }
    catch (...)
    {
        AppendInitializationException("syntax error : [CManager - Initialize]");
    }
    ReleaseDeviceResources();
    return 0;
}

int Renderer::InitializeResources(const HWND window, const int width, const int height,
    const int requirePostPixelBlend, const int padTextureToPowerOfTwo)
{
    window_ = window;
    sourceWidth_ = width;
    sourceHeight_ = height;
    targetWidth_ = width;
    targetHeight_ = height;
    direct3D_ = Direct3DCreate9(D3D_SDK_VERSION);
    if (direct3D_ == nullptr)
    {
        SetInitializationError(L"Direct3DCreate9に失敗！");
        ReleaseDeviceResources();
        return 0;
    }

    HRESULT result = direct3D_->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode_);
    if (FAILED(result))
    {
        SetInitializationError(L"GetAdapterDisplayModeに失敗 B10");
        LogHresult("GetAdapterDisplayMode", result);
        ReleaseDeviceResources();
        return 0;
    }

    // 10003CC0 只接受这些桌面格式，其他格式直接进入初始化错误分支。
    switch (displayMode_.Format)
    {
    case D3DFMT_R8G8B8:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A2B10G10R10:
    case D3DFMT_A2R10G10B10:
    case D3DFMT_A8B8G8R8:
        break;
    default:
        SetInitializationError(L"プライマリディスプレイの色数が足りない！");
        Logging::WriteFormat(Logging::Level::Error,
            "Initialize 不支持桌面格式：format=%d。", static_cast<int>(displayMode_.Format));
        ReleaseDeviceResources();
        return 0;
    }

    if (!CreateDevice(requirePostPixelBlend) ||
        !CreatePrimaryResources(padTextureToPowerOfTwo != 0))
    {
        ReleaseDeviceResources();
        return 0;
    }

    ComputeViewport(width, height, 0);
    // RenderDX.dll:100033FB：初始视口按逻辑尺寸建立，但尚未应用任何 Resize 模式。
    // 保持 -1 可确保首次同尺寸 Resize(..., 0) 仍执行设备 Reset 与帧提交。
    resizeMode_ = -1;
    Logging::WriteFormat(Logging::Level::Info,
        "Initialize 完成：source=%dx%d texture=%dx%d deviceType=%d pitch=%d filtering=0x%02X。",
        sourceWidth_, sourceHeight_, textureWidth_, textureHeight_, static_cast<int>(deviceType_),
        uploadLock_.Pitch, IsFiltering());
    return 1;
}

bool Renderer::CreateDevice(const int requirePostPixelBlend)
{
    int progressMask = 0;
    std::array<DeviceCandidate, 2> candidates{};
    std::size_t candidateCount = 0;
    // 原3D8A..3EE6先收集HAL/REF全部候选，再从3FC0开始创建设备。
    for (const D3DDEVTYPE type : {D3DDEVTYPE_HAL, D3DDEVTYPE_REF})
    {
        DeviceCandidate candidate{};
        if (BuildDeviceCandidate(type, requirePostPixelBlend, candidate, progressMask))
            candidates[candidateCount++] = candidate;
    }

    D3DPRESENT_PARAMETERS parameters{};
    parameters.BackBufferWidth = static_cast<UINT>(sourceWidth_);
    parameters.BackBufferHeight = static_cast<UINT>(sourceHeight_);
    parameters.BackBufferFormat = displayMode_.Format;
    parameters.BackBufferCount = 1;
    parameters.MultiSampleType = D3DMULTISAMPLE_NONE;
    parameters.MultiSampleQuality = 0;
    parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    parameters.hDeviceWindow = window_;
    parameters.Windowed = TRUE;
    parameters.FullScreen_RefreshRateInHz = 0;
    parameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    IDirect3DDevice9* device = nullptr;
    for (std::size_t index = 0; index < candidateCount; ++index)
    {
        const auto& candidate = candidates[index];
        // 原27EF/2804的零/UNKNOWN仍可入候选；每轮只覆盖原三个present字段。
        parameters.EnableAutoDepthStencil = candidate.depthFormat != D3DFMT_UNKNOWN;
        parameters.AutoDepthStencilFormat = candidate.depthFormat;
        parameters.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        const HRESULT result = direct3D_->CreateDevice(D3DADAPTER_DEFAULT, candidate.type, window_,
            candidate.behaviorFlags, &parameters, &device);
        if (FAILED(result))
        {
            Logging::WriteFormat(Logging::Level::Warning,
                "CreateDevice 候选失败：type=%d depth=%d behavior=0x%08lX hr=0x%08lX。",
                static_cast<int>(candidate.type), static_cast<int>(candidate.depthFormat),
                candidate.behaviorFlags, static_cast<unsigned long>(result));
            continue;
        }

        device_ = device;
        deviceType_ = candidate.type;
        presentParameters_ = parameters;
        filteringAvailable_ = SUCCEEDED(direct3D_->CheckDeviceFormat(D3DADAPTER_DEFAULT,
            deviceType_, displayMode_.Format, D3DUSAGE_QUERY_FILTER,
            D3DRTYPE_TEXTURE, D3DFMT_X8R8G8B8));
        // 原40F9在前；只有过滤查询成功，4125才读取本次过滤能力。
        textureFilterCaps_ = 0;
        if (filteringAvailable_)
        {
            D3DCAPS9 filterCaps{};
            if (SUCCEEDED(device_->GetDeviceCaps(&filterCaps))) textureFilterCaps_ = filterCaps.TextureFilterCaps;
        }
        magFilter_ = D3DTEXF_POINT;
        minFilter_ = D3DTEXF_POINT;
        return true;
    }

    if (candidateCount != 0)
    {
        SetInitializationError(L"デバイスの作成に失敗！");
    }
    else if ((progressMask & 4) != 0)
    {
        SetInitializationError(L"デバイスタイプがない！ (タイプ３)");
    }
    else if ((progressMask & 2) != 0)
    {
        SetInitializationError(L"デバイスタイプがない！ (タイプ２)");
    }
    else if ((progressMask & 1) != 0)
    {
        SetInitializationError(L"デバイスタイプがない！ (タイプ１)");
    }
    else
    {
        SetInitializationError(L"デバイスタイプがない！ (タイプ０)");
    }
    return false;
}

bool Renderer::BuildDeviceCandidate(const D3DDEVTYPE type, const int requirePostPixelBlend,
    DeviceCandidate& result, int& progressMask) noexcept
{
    D3DCAPS9 caps{};
    if (FAILED(direct3D_->GetDeviceCaps(D3DADAPTER_DEFAULT, type, &caps)))
    {
        return false;
    }
    progressMask |= 1;
    if (FAILED(direct3D_->CheckDeviceType(D3DADAPTER_DEFAULT, type,
        displayMode_.Format, displayMode_.Format, TRUE)))
    {
        return false;
    }
    progressMask |= 2;
    if (requirePostPixelBlend != 0 &&
        FAILED(direct3D_->CheckDeviceFormat(D3DADAPTER_DEFAULT, type, displayMode_.Format,
            D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, D3DRTYPE_TEXTURE, displayMode_.Format)))
    {
        return false;
    }
    progressMask |= 4;

    const D3DFORMAT depth = SelectDepthFormat(type);
    result.type = type;
    result.depthFormat = depth;
    result.behaviorFlags = type != D3DDEVTYPE_REF &&
        (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0
        ? D3DCREATE_HARDWARE_VERTEXPROCESSING
        : D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    return true;
}

D3DFORMAT Renderer::SelectDepthFormat(const D3DDEVTYPE type) const noexcept
{
    D3DFORMAT best = D3DFMT_UNKNOWN;
    int bestScore = (std::numeric_limits<int>::max)();
    const int preferredDepth = PreferredDepthBits();
    for (const D3DFORMAT format : DepthCandidates)
    {
        if (FAILED(direct3D_->CheckDeviceFormat(D3DADAPTER_DEFAULT, type, displayMode_.Format,
                D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, format)) ||
            FAILED(direct3D_->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, type,
                displayMode_.Format, displayMode_.Format, format)))
        {
            continue;
        }
        const int score = std::abs(DepthBits(format) - preferredDepth) + StencilBits(format);
        if (score < bestScore)
        {
            bestScore = score;
            best = format;
        }
    }
    return best;
}

int Renderer::PreferredDepthBits() const noexcept
{
    // RenderDX.dll:10002480 先把桌面像素格式映射到比较基准，再选择差值最小者。
    switch (displayMode_.Format)
    {
    case D3DFMT_R8G8B8:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_A8B8G8R8:
        return 32;
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
        return 20;
    case D3DFMT_A4R4G4B4:
    case D3DFMT_X4R4G4B4:
        return 16;
    case D3DFMT_R3G3B2:
    case D3DFMT_A8R3G3B2:
        return 8;
    case D3DFMT_A2B10G10R10:
    case D3DFMT_A2R10G10B10:
        return 40;
    case D3DFMT_A16B16G16R16:
        return 64;
    default:
        return 0;
    }
}

unsigned int Renderer::NextPowerOfTwo(const unsigned int value) noexcept
{
    unsigned int result = 1;
    while (result < value)
    {
        if (result > (std::numeric_limits<unsigned int>::max)() / 2u)
        {
            return 0;
        }
        result *= 2u;
    }
    return result;
}

bool Renderer::CreatePrimaryResources(const bool padTextureToPowerOfTwo)
{
    textureWidth_ = sourceWidth_;
    textureHeight_ = sourceHeight_;
    if (padTextureToPowerOfTwo)
    {
        textureWidth_ = static_cast<int>(NextPowerOfTwo(static_cast<unsigned int>(sourceWidth_)));
        if (textureWidth_ == 0)
        {
            SetInitializationError(L"テクスチャーの幅の計算に失敗！ : [ITexture - Create]", InitializationErrorLayer::Texture);
            return false;
        }
        textureHeight_ = static_cast<int>(NextPowerOfTwo(static_cast<unsigned int>(sourceHeight_)));
        if (textureHeight_ == 0)
        {
            SetInitializationError(L"テクスチャーの高さの計算に失敗！ : [ITexture - Create]", InitializationErrorLayer::Texture);
            return false;
        }
    }
    // 原3196重新读取纹理能力，不复用过滤阶段的caps快照；原普通HRESULT不控制此门。
    caps_ = {};
    const HRESULT capsResult = device_->GetDeviceCaps(&caps_);
    if (FAILED(capsResult)) LogHresult("Texture::GetDeviceCaps", capsResult);
    // 100030D0 在 D3DX 规范化之前先按无符号尺寸检查设备上限。
    if (static_cast<DWORD>(textureWidth_) > caps_.MaxTextureWidth ||
        static_cast<DWORD>(textureHeight_) > caps_.MaxTextureHeight)
    {
        SetInitializationError(L"テクスチャーサイズが大きすぎます！ : [ITexture - Create]", InitializationErrorLayer::Texture);
        Logging::WriteFormat(Logging::Level::Error,
            "主纹理超过设备限制：texture=%dx%d limit=%lux%lu。", textureWidth_, textureHeight_,
            caps_.MaxTextureWidth, caps_.MaxTextureHeight);
        return false;
    }
    UINT width = static_cast<UINT>(textureWidth_);
    UINT height = static_cast<UINT>(textureHeight_);
    UINT levels = 0;
    D3DFORMAT format = D3DFMT_X8R8G8B8;
    const auto requirements = PreparePrimaryTexture(device_, width, height, levels, format);
    if (FAILED(requirements))
    {
        SetInitializationError(L"テクスチャの作成に失敗しました！ : [ITexture - Create]", InitializationErrorLayer::Texture);
        LogHresult("Primary texture requirements", requirements);
        return false;
    }
    if (width > static_cast<UINT>((std::numeric_limits<int>::max)()) ||
        height > static_cast<UINT>((std::numeric_limits<int>::max)()))
    {
        SetInitializationError(L"テクスチャの作成に失敗しました！ : [ITexture - Create]", InitializationErrorLayer::Texture);
        Logging::Write(Logging::Level::Error, "Texture dimensions outside the signed upload-coordinate domain; rejected.");
        return false;
    }
    textureWidth_ = static_cast<int>(width);
    textureHeight_ = static_cast<int>(height);
    textureFormat_ = format;
    // 原4D5B使用规范化后的格式及mip数；源system-memory画布仍固定为22。
    HRESULT result = device_->CreateTexture(width, height, levels, 0, format,
        D3DPOOL_MANAGED, &primaryTexture_, nullptr);
    if (FAILED(result))
    {
        SetInitializationError(L"テクスチャの作成に失敗しました！ : [ITexture - Create]", InitializationErrorLayer::Texture);
        LogHresult("CreateTexture", result);
        return false;
    }
    D3DSURFACE_DESC textureDescription{};
    result = primaryTexture_->GetLevelDesc(0, &textureDescription);
    if (FAILED(result)) LogHresult("PrimaryTexture::GetLevelDesc", result);
    // 原31F5用实际level0宽高，而非逻辑画布尺寸。未写/非法描述属于显式安全拒绝域。
    if (textureDescription.Width == 0 || textureDescription.Height == 0 ||
        textureDescription.Width > static_cast<UINT>((std::numeric_limits<int>::max)()) ||
        textureDescription.Height > static_cast<UINT>((std::numeric_limits<int>::max)()) ||
        TexturePixelBytes(textureDescription.Format) == 0)
    {
        SetInitializationError(L"テクスチャの作成に失敗しました！ : [ITexture - Create]", InitializationErrorLayer::Texture);
        Logging::Write(Logging::Level::Error, "Primary texture returned an unwritten/invalid level-zero size; rejected.");
        return false;
    }
    textureWidth_ = static_cast<int>(textureDescription.Width);
    textureHeight_ = static_cast<int>(textureDescription.Height);
    textureFormat_ = textureDescription.Format;
    result = device_->CreateOffscreenPlainSurface(static_cast<UINT>(sourceWidth_),
        static_cast<UINT>(sourceHeight_), D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM,
        &uploadSurface_, nullptr);
    if (FAILED(result))
    {
        SetInitializationError(L"セカンダリサーフェイスの作成に失敗！ : [CD3DDevice - Initialize]", InitializationErrorLayer::Device);
        LogHresult("CreateOffscreenPlainSurface", result);
        return false;
    }

    // 100033C0 用 D3DLOCK_READONLY 取得地址和 pitch 后立即解锁，但继续向 Graphics 暴露该 system-memory 地址。
    result = uploadSurface_->LockRect(&uploadLock_, nullptr, D3DLOCK_READONLY);
    if (FAILED(result))
    {
        SetInitializationError(L"セカンダリサーフェイスのLockRectに失敗！ : [CD3DDevice - Initialize]", InitializationErrorLayer::Device);
        LogHresult("UploadSurface::LockRect", result);
        return false;
    }
    result = uploadSurface_->UnlockRect();
    if (FAILED(result))
    {
        SetInitializationError(L"セカンダリサーフェイスのUnlockRectに失敗！ : [CD3DDevice - Initialize]", InitializationErrorLayer::Device);
        LogHresult("UploadSurface::UnlockRect", result);
        return false;
    }
    return true;
}

int Renderer::Release() noexcept
{
    const bool hadRenderer = direct3D_ != nullptr || device_ != nullptr ||
        primaryTexture_ != nullptr || uploadSurface_ != nullptr;
    ReleaseDeviceResources();
    window_ = nullptr;
    sourceWidth_ = 0;
    sourceHeight_ = 0;
    textureWidth_ = 0;
    textureHeight_ = 0;
    textureFormat_ = D3DFMT_UNKNOWN;
    targetWidth_ = 0;
    targetHeight_ = 0;
    resizeMode_ = -1;
    abiState_.viewport.fill(0);
    filteringAvailable_ = false;
    textureFilterCaps_ = 0;
    magFilter_ = D3DTEXF_POINT;
    minFilter_ = D3DTEXF_POINT;
    return hadRenderer
        ? static_cast<int>(reinterpret_cast<std::intptr_t>(this))
        : 0;
}

void Renderer::ReleasePrimaryResources() noexcept
{
    uploadLock_ = {};
    ReleaseCom(uploadSurface_);
    ReleaseCom(primaryTexture_);
}

void Renderer::ReleaseDeviceResources() noexcept
{
    ReleasePrimaryResources();
    ReleaseCom(device_);
    ReleaseCom(direct3D_);
    std::memset(&presentParameters_, 0, sizeof(presentParameters_));
    std::memset(&caps_, 0, sizeof(caps_));
    std::memset(&displayMode_, 0, sizeof(displayMode_));
}

int Renderer::GetBits() const noexcept
{
    return static_cast<int>(reinterpret_cast<std::intptr_t>(uploadLock_.pBits));
}

int Renderer::GetPitch() const noexcept
{
    return uploadLock_.Pitch;
}

int Renderer::GetErrorMessage() const noexcept
{
    return static_cast<int>(reinterpret_cast<std::intptr_t>(errorMessage_.c_str()));
}

int Renderer::IsFiltering() const noexcept
{
    if (!filteringAvailable_)
    {
        return 0;
    }
    int result = 0;
    if ((textureFilterCaps_ & D3DPTFILTERCAPS_MAGFLINEAR) != 0) result |= 0x01;
    if ((textureFilterCaps_ & D3DPTFILTERCAPS_MAGFANISOTROPIC) != 0) result |= 0x02;
    if ((textureFilterCaps_ & D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD) != 0) result |= 0x04;
    if ((textureFilterCaps_ & D3DPTFILTERCAPS_MAGFGAUSSIANQUAD) != 0) result |= 0x08;
    if ((textureFilterCaps_ & D3DPTFILTERCAPS_MINFLINEAR) != 0) result |= 0x10;
    if ((textureFilterCaps_ & D3DPTFILTERCAPS_MINFANISOTROPIC) != 0) result |= 0x20;
    if ((textureFilterCaps_ & D3DPTFILTERCAPS_MINFPYRAMIDALQUAD) != 0) result |= 0x40;
    if ((textureFilterCaps_ & D3DPTFILTERCAPS_MINFGAUSSIANQUAD) != 0) result |= 0x80;
    return result;
}

int Renderer::ExternalFilterMode(const D3DTEXTUREFILTERTYPE value) noexcept
{
    switch (value)
    {
    case D3DTEXF_LINEAR: return 0;
    case D3DTEXF_ANISOTROPIC: return 1;
    case D3DTEXF_PYRAMIDALQUAD: return 2;
    case D3DTEXF_GAUSSIANQUAD: return 3;
    default: return -1;
    }
}

D3DTEXTUREFILTERTYPE Renderer::SelectFilter(const int mode, const bool magnification) const noexcept
{
    if (!filteringAvailable_)
    {
        return D3DTEXF_POINT;
    }
    switch (mode)
    {
    case 0:
        if ((textureFilterCaps_ & (magnification ? D3DPTFILTERCAPS_MAGFLINEAR : D3DPTFILTERCAPS_MINFLINEAR)) != 0)
            return D3DTEXF_LINEAR;
        break;
    case 1:
        if ((textureFilterCaps_ & (magnification ? D3DPTFILTERCAPS_MAGFANISOTROPIC : D3DPTFILTERCAPS_MINFANISOTROPIC)) != 0)
            return D3DTEXF_ANISOTROPIC;
        break;
    case 2:
        if ((textureFilterCaps_ & (magnification ? D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD : D3DPTFILTERCAPS_MINFPYRAMIDALQUAD)) != 0)
            return D3DTEXF_PYRAMIDALQUAD;
        break;
    case 3:
        if ((textureFilterCaps_ & (magnification ? D3DPTFILTERCAPS_MAGFGAUSSIANQUAD : D3DPTFILTERCAPS_MINFGAUSSIANQUAD)) != 0)
            return D3DTEXF_GAUSSIANQUAD;
        break;
    default:
        break;
    }
    return D3DTEXF_POINT;
}

int Renderer::GetMagFiltering() const noexcept
{
    return ExternalFilterMode(magFilter_);
}

int Renderer::GetMinFiltering() const noexcept
{
    return ExternalFilterMode(minFilter_);
}

int Renderer::SetMagFiltering(const int mode) noexcept
{
    magFilter_ = SelectFilter(mode, true);
    return device_ != nullptr
        ? static_cast<int>(reinterpret_cast<std::intptr_t>(&abiState_))
        : 0;
}

int Renderer::SetMinFiltering(const int mode) noexcept
{
    minFilter_ = SelectFilter(mode, false);
    return device_ != nullptr
        ? static_cast<int>(reinterpret_cast<std::intptr_t>(&abiState_))
        : 0;
}

int Renderer::UpdatePrimary(const RECT* const rect)
{
    if (rect == nullptr || primaryTexture_ == nullptr || uploadSurface_ == nullptr)
    {
        Logging::Write(Logging::Level::Error, "UpdatePrimary 收到无效矩形或未初始化资源。");
        return 0;
    }

    // 原01930 -> 02F90：逐次取表面、裁剪及锁；缓存地址只服务GetBits/GetPitch。
    return TransferPrimarySurface(primaryTexture_, uploadSurface_, *rect) ? 1 : 0;
}

int Renderer::RenderScreen() noexcept
{
    return PresentFrame();
}

int Renderer::PresentFrame() noexcept
{
    if (device_ == nullptr || primaryTexture_ == nullptr)
    {
        return D3DERR_INVALIDCALL;
    }
    HRESULT result = device_->Clear(0, nullptr, D3DCLEAR_TARGET, 0xFF000000u, 1.0f, 0);
    if (FAILED(result)) return result;
    result = device_->BeginScene();
    if (FAILED(result)) return result;

    if (filteringAvailable_)
    {
        const bool magnified = abiState_.viewport[2] > sourceWidth_ ||
            abiState_.viewport[3] > sourceHeight_;
        const bool minified = abiState_.viewport[2] < sourceWidth_ ||
            abiState_.viewport[3] < sourceHeight_;
        device_->SetSamplerState(0, D3DSAMP_MAGFILTER,
            magnified ? static_cast<DWORD>(magFilter_) : static_cast<DWORD>(D3DTEXF_POINT));
        device_->SetSamplerState(0, D3DSAMP_MINFILTER,
            minified ? static_cast<DWORD>(minFilter_) : static_cast<DWORD>(D3DTEXF_POINT));
    }

    const float left = static_cast<float>(abiState_.viewport[0]) - 0.5f;
    const float top = static_cast<float>(abiState_.viewport[1]) - 0.5f;
    // 原100021D5..100021FE：ADD取低32位，右/下端点按DWORD转float；
    // 左/上端点仍按signed转换。mode2缩小时不能用有符号溢出加法。
    const auto rightBits = static_cast<std::uint32_t>(abiState_.viewport[0]) +
        static_cast<std::uint32_t>(abiState_.viewport[2]);
    const auto bottomBits = static_cast<std::uint32_t>(abiState_.viewport[1]) +
        static_cast<std::uint32_t>(abiState_.viewport[3]);
    const float right = static_cast<float>(rightBits) - 0.5f;
    const float bottom = static_cast<float>(bottomBits) - 0.5f;
    const float maxU = static_cast<float>(sourceWidth_) / static_cast<float>(textureWidth_);
    const float maxV = static_cast<float>(sourceHeight_) / static_cast<float>(textureHeight_);
    const std::array<ScreenVertex, 4> vertices{{
        {left, top, 0.0f, 1.0f, 0xFFFFFFFFu, 0.0f, 0.0f},
        {right, top, 0.0f, 1.0f, 0xFFFFFFFFu, maxU, 0.0f},
        {left, bottom, 0.0f, 1.0f, 0xFFFFFFFFu, 0.0f, maxV},
        {right, bottom, 0.0f, 1.0f, 0xFFFFFFFFu, maxU, maxV},
    }};
    device_->SetTexture(0, primaryTexture_);
    device_->SetFVF(ScreenFvf);
    // 10002020 在 BeginScene 成功后不检查绘制和 EndScene 的返回值，
    // 导出的最终结果只来自 Present（或其 device-lost 恢复分支）。
    device_->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices.data(), sizeof(ScreenVertex));
    device_->EndScene();

    result = device_->Present(nullptr, nullptr, nullptr, nullptr);
    if (result == D3DERR_DEVICELOST)
    {
        return ResetDevice() ? 1 : 0;
    }
    return result;
}

int Renderer::RenderScreenHdc() noexcept
{
    return device_ != nullptr
        ? device_->Present(nullptr, nullptr, nullptr, nullptr)
        : D3DERR_INVALIDCALL;
}

void Renderer::ComputeViewport(const int width, const int height, const int mode) noexcept
{
    targetWidth_ = width;
    targetHeight_ = height;
    auto& viewport = abiState_.viewport;
    viewport[0] = 0;
    viewport[1] = 0;
    viewport[2] = sourceWidth_;
    viewport[3] = sourceHeight_;
    switch (mode)
    {
    case 0:
        viewport[2] = width;
        viewport[3] = height;
        break;
    case 1:
    {
        // 100022F6 使用 x86 IMUL 的低 32 位后执行无符号 DIV，溢出不扩展到 64 位。
        const std::uint32_t scaledProduct = static_cast<std::uint32_t>(height) *
            static_cast<std::uint32_t>(sourceWidth_);
        const std::uint32_t scaledHeight = scaledProduct / static_cast<std::uint32_t>(width);
        if (scaledHeight < static_cast<std::uint32_t>(sourceHeight_))
        {
            viewport[2] = static_cast<int>(scaledProduct /
                static_cast<std::uint32_t>(sourceHeight_));
            viewport[3] = height;
            viewport[0] = static_cast<int>((static_cast<std::uint32_t>(width) -
                static_cast<std::uint32_t>(viewport[2])) >> 1);
        }
        else if (scaledHeight == static_cast<std::uint32_t>(sourceHeight_))
        {
            // 1000231C -> 100022EC：商相等时保留目标高度，不做反向乘除。
            viewport[2] = width;
            viewport[3] = height;
        }
        else
        {
            viewport[2] = width;
            viewport[3] = static_cast<int>((static_cast<std::uint32_t>(width) *
                static_cast<std::uint32_t>(sourceHeight_)) /
                static_cast<std::uint32_t>(sourceWidth_));
            viewport[1] = static_cast<int>((static_cast<std::uint32_t>(height) -
                static_cast<std::uint32_t>(viewport[3])) >> 1);
        }
        break;
    }
    case 2:
        // 10002339 使用 SHR；目标小于逻辑画面时保留原版无符号下溢后的位模式。
        viewport[0] = static_cast<int>((static_cast<std::uint32_t>(width) -
            static_cast<std::uint32_t>(sourceWidth_)) >> 1);
        viewport[1] = static_cast<int>((static_cast<std::uint32_t>(height) -
            static_cast<std::uint32_t>(sourceHeight_)) >> 1);
        break;
    default:
        break;
    }
    resizeMode_ = mode;
}

const int* Renderer::Resize(const int width, const int height, const int mode) noexcept
{
    if (device_ == nullptr)
    {
        return nullptr;
    }
    if (mode == 1 && (width == 0 || sourceHeight_ == 0 || sourceWidth_ == 0))
    {
        Logging::Write(Logging::Level::Error,
            "Resize mode 1 的除数为零；原 DLL 会触发整数除零，本实现明确失败。");
        return nullptr;
    }
    if (targetWidth_ == width && targetHeight_ == height && resizeMode_ == mode)
    {
        return abiState_.viewport.data();
    }
    ComputeViewport(width, height, mode);
    presentParameters_.BackBufferWidth = static_cast<UINT>(width);
    presentParameters_.BackBufferHeight = static_cast<UINT>(height);
    const HRESULT result = device_->Reset(&presentParameters_);
    if (FAILED(result))
    {
        LogHresult("Resize::Reset", result);
    }
    PresentFrame();
    return abiState_.viewport.data();
}

int Renderer::TestCooperativeLevel() const noexcept
{
    if (device_ == nullptr) return -1;
    const HRESULT result = device_->TestCooperativeLevel();
    if (result == D3D_OK) return 0;
    // 保留驱动原始失败值，不能仅凭Start的通用设备丢失提示推断是哪一种失败。
    LogHresult("TestCooperativeLevel", result);
    if (result == D3DERR_DEVICELOST) return 1;
    if (result == D3DERR_DEVICENOTRESET) return 2;
    if (result == D3DERR_DRIVERINTERNALERROR) return 3;
    return -1;
}

bool Renderer::ResetDevice() noexcept
{
    if (device_ == nullptr) return false;
    const HRESULT cooperative = device_->TestCooperativeLevel();
    if (cooperative == D3D_OK) return true;
    if (cooperative == D3DERR_DEVICENOTRESET)
    {
        return SUCCEEDED(device_->Reset(&presentParameters_));
    }
    return false;
}

int Renderer::RenderReset() noexcept
{
    return device_ != nullptr ? device_->Reset(&presentParameters_) : D3DERR_INVALIDCALL;
}

void Renderer::SetInitializationError(const std::wstring_view leafMessage,
    const InitializationErrorLayer layer)
{
    std::wstring message(leafMessage);
    // 原3940只在已有正文时加LF；3AC0为本层追加scope，不能给早期失败强加device层。
    if (layer == InitializationErrorLayer::Texture)
        message += L"\nプライマリサーフェイスの作成に失敗！ : [CD3DDevice - Initialize]";
    if (layer != InitializationErrorLayer::Direct3D)
        message += L"\nデバイスの初期設定に失敗！ : [CDirect3D - Initialize]";
    else
        message += L" : [CDirect3D - Initialize]";
    message += L"\n初期設定に失敗！ : [CManager - Initialize]";

    BOOL usedDefaultCharacter = FALSE;
    const int required = WideCharToMultiByte(932, WC_NO_BEST_FIT_CHARS,
        message.data(), static_cast<int>(message.size()), nullptr, 0, nullptr, &usedDefaultCharacter);
    if (required <= 0 || usedDefaultCharacter != FALSE)
    {
        errorMessage_.clear();
        Logging::WriteFormat(Logging::Level::Error,
            "初始化错误串转换为 CP932 失败：Win32=%lu。", GetLastError());
        return;
    }
    errorMessage_.resize(static_cast<std::size_t>(required));
    usedDefaultCharacter = FALSE;
    const int written = WideCharToMultiByte(932, WC_NO_BEST_FIT_CHARS,
        message.data(), static_cast<int>(message.size()), errorMessage_.data(), required,
        nullptr, &usedDefaultCharacter);
    if (written != required || usedDefaultCharacter != FALSE)
    {
        errorMessage_.clear();
        Logging::WriteFormat(Logging::Level::Error,
            "初始化错误串写入 CP932 失败：Win32=%lu。", GetLastError());
        return;
    }

    std::string utf8;
    if (Logging::TryCodePageToUtf8(errorMessage_, 932, utf8))
    {
        Logging::Write(Logging::Level::Error, utf8);
    }
}

void Renderer::AppendInitializationException(const char* const message)
{
    // 原10003940先按旧正文决定追加LF，再追加异常正文；分配异常可向外传播。
    if (!errorMessage_.empty()) errorMessage_.push_back('\n');
    if (message != nullptr) errorMessage_.append(message);
    std::string utf8;
    if (Logging::TryCodePageToUtf8(errorMessage_, 932, utf8))
        Logging::Write(Logging::Level::Error, utf8);
}

void Renderer::LogHresult(const char* const operation, const HRESULT result) noexcept
{
    char buffer[256]{};
    std::snprintf(buffer, sizeof(buffer), "%s failed: HRESULT=0x%08lX",
        operation == nullptr ? "RenderDX" : operation, static_cast<unsigned long>(result));
    Logging::Write(Logging::Level::Error, buffer);
}
}
