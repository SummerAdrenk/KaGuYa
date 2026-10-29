#include "SurfaceTransfer.h"
#include "TexturePixelConversion.h"
#include "DebugMute.h"
#include <Logging/Logger.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>

namespace KaGuYa::RenderDX
{
namespace
{
template<class T> void Retire(T*& object) noexcept
{
    if (object) { object->Release(); object = nullptr; }
}
template<class T> struct OwnedCom final
{
    T* object = nullptr;
    ~OwnedCom() { Retire(object); }
    OwnedCom() = default;
    OwnedCom(const OwnedCom&) = delete;
    OwnedCom& operator=(const OwnedCom&) = delete;
};

HRESULT InvalidMemory(const char* operation) noexcept
{
    Logging::Write(Logging::Level::Error, operation);
    return D3DERR_INVALIDCALL;
}

bool RowsAddressable(int pitch, UINT height, std::size_t rowBytes) noexcept
{
    if (pitch < 0) return false;
    if (!height || !rowBytes) return true;
    return static_cast<std::uint64_t>(height - 1u) * static_cast<UINT>(pitch) + rowBytes <=
        (std::numeric_limits<std::size_t>::max)();
}

HRESULT CopyWholeSurface(IDirect3DSurface9* source, IDirect3DSurface9* destination) noexcept
{
    D3DSURFACE_DESC description{};
    static_cast<void>(source->GetDesc(&description));
    D3DLOCKED_RECT input{}, output{};
    auto result = source->LockRect(&input, nullptr, 0);
    if (FAILED(result)) return result;
    result = destination->LockRect(&output, nullptr, 0);
    if (SUCCEEDED(result))
    {
        // 原08B43拷贝min(pitch)，包含padding；不能改成仅有效像素字节。
        const auto bytes = (std::min)(input.Pitch, output.Pitch);
        if (bytes < 0 || !RowsAddressable(input.Pitch, description.Height, static_cast<std::size_t>(bytes)) ||
            !RowsAddressable(output.Pitch, description.Height, static_cast<std::size_t>(bytes)) ||
            (bytes && description.Height && (!input.pBits || !output.pBits)))
            result = InvalidMemory("Whole-surface copy rejected invalid locked memory instead of reproducing an unsafe copy.");
        else
        {
            for (UINT row = 0; bytes && row < description.Height; ++row)
                std::memcpy(static_cast<std::uint8_t*>(output.pBits) + static_cast<std::size_t>(row) * output.Pitch,
                    static_cast<const std::uint8_t*>(input.pBits) + static_cast<std::size_t>(row) * input.Pitch,
                    static_cast<std::size_t>(bytes));
            result = D3D_OK;
        }
        static_cast<void>(destination->UnlockRect());
    }
    static_cast<void>(source->UnlockRect());
    return result;
}

struct LockedView final
{
    D3DLOCKED_RECT data{};
    D3DFORMAT format = D3DFMT_UNKNOWN;
};

// 原08B30/08C22的五个实际拥有字段，失败时的部分构造也保留给Reset处理。
class SurfaceLocker final
{
public:
    SurfaceLocker() = default;
    SurfaceLocker(const SurfaceLocker&) = delete;
    SurfaceLocker& operator=(const SurfaceLocker&) = delete;
    ~SurfaceLocker() { Reset(); }
    void RetireAfterException() noexcept
    {
        // 原new异常绕过普通回写尾部。本版安全退锁/退引用，但不能把未转换的克隆写回。
        flags_ |= 1u;
        Reset();
    }
    HRESULT Lock(IDirect3DSurface9* surface, const RECT& rectangle, bool readOnly,
        LockedView& view, bool clone = false) noexcept
    {
        Reset();
        D3DSURFACE_DESC description{};
        static_cast<void>(surface->GetDesc(&description));
        if (rectangle.left < 0 || rectangle.top < 0 || rectangle.right < rectangle.left ||
            rectangle.bottom < rectangle.top || static_cast<UINT>(rectangle.right) > description.Width ||
            static_cast<UINT>(rectangle.bottom) > description.Height)
            return D3DERR_INVALIDCALL;
        const bool whole = rectangle.left == 0 && rectangle.top == 0 &&
            static_cast<UINT>(rectangle.right) == description.Width &&
            static_cast<UINT>(rectangle.bottom) == description.Height;
        if (clone)
        {
            const auto created = CreateClone(surface, description, readOnly || !whole);
            if (FAILED(created)) return created;
        }
        auto* const target = clone ? cloneSurface_ : surface;
        D3DLOCKED_RECT locked{};
        const DWORD lockFlags = D3DLOCK_NOSYSLOCK | (readOnly ? D3DLOCK_READONLY : 0);
        if (!clone) static_cast<void>(SetDebugMute(1)); // 原090C7；clone最终锁没有此配对。
        const auto result = target->LockRect(&locked, whole ? nullptr : &rectangle, lockFlags);
        if (!clone) static_cast<void>(SetDebugMute(0)); // 保存hr后，失败重入之前。
        if (FAILED(result))
        {
            // 原90E2只在第一次直接锁失败时递归到10000h分支；克隆锁再失败不重复重试。
            if (!clone) return Lock(surface, rectangle, readOnly, view, true);
            return result;
        }
        view.data = locked;
        view.format = description.Format;
        flags_ = (readOnly ? 1u : 0u) | (clone ? 0x10000u : 0u);
        original_ = surface;
        original_->AddRef();
        return D3D_OK;
    }

private:
    HRESULT CreateClone(IDirect3DSurface9* surface, const D3DSURFACE_DESC& description,
        bool preserve) noexcept
    {
        static_cast<void>(surface->GetDevice(&device_));
        if (!device_) return InvalidMemory("Surface clone has no device; unsafe original null-device access rejected.");
        // 当前来源和目的均为已证15种非压缩格式，不进入原DXT级别偏移分支。
        auto result = device_->CreateTexture(description.Width, description.Height, 1, 0,
            description.Format, description.Pool == D3DPOOL_SCRATCH ? D3DPOOL_SCRATCH : D3DPOOL_SYSTEMMEM,
            &cloneTexture_, nullptr);
        if (SUCCEEDED(result))
        {
            if (!cloneTexture_) result = InvalidMemory("Surface clone creation returned no texture.");
            else result = cloneTexture_->GetSurfaceLevel(0, &cloneSurface_);
        }
        if (SUCCEEDED(result) && !cloneSurface_)
            result = InvalidMemory("Surface clone level query returned no surface.");
        if (SUCCEEDED(result) && preserve)
        {
            static_cast<void>(SetDebugMute(1));
            result = CopyWholeSurface(surface, cloneSurface_);
            static_cast<void>(SetDebugMute(0)); // 原08E70；RT恢复不在mute范围内。
            if (FAILED(result))
            {
                IDirect3DSurface9* renderTarget = nullptr;
                result = device_->CreateRenderTarget(description.Width, description.Height, description.Format,
                    D3DMULTISAMPLE_NONE, 0, TRUE, &renderTarget, nullptr);
                if (SUCCEEDED(result))
                {
                    if (!renderTarget) result = InvalidMemory("Surface clone recovery returned no render target.");
                    else result = device_->StretchRect(surface, nullptr, renderTarget, nullptr, D3DTEXF_NONE);
                    if (SUCCEEDED(result)) result = CopyWholeSurface(renderTarget, cloneSurface_);
                    Retire(renderTarget);
                }
                else if (renderTarget)
                {
                    // API失败却写出对象的危险域独立安全清理，不复制悬空资源。
                    Retire(renderTarget);
                }
            }
        }
        if (FAILED(result))
        {
            // 原8EED/8EFC/8F0B：建克隆/预复制失败直接释放，不先Unlock。
            Retire(cloneSurface_);
            Retire(cloneTexture_);
            Retire(device_);
        }
        return result;
    }

    void Reset() noexcept
    {
        auto* const locked = cloneSurface_ ? cloneSurface_ : original_;
        // 原8C27..8C3B不看“锁成功”标志；克隆Lock失败后也会尝试Unlock。
        if (locked) static_cast<void>(locked->UnlockRect());
        if (original_ && cloneSurface_ && device_ && !(flags_ & 1u))
        {
            static_cast<void>(SetDebugMute(1));
            const auto result = device_->UpdateSurface(cloneSurface_, nullptr, original_, nullptr);
            static_cast<void>(SetDebugMute(0));
            if (FAILED(result))
            {
                static_cast<void>(SetDebugMute(1));
                const auto copied = CopyWholeSurface(cloneSurface_, original_);
                static_cast<void>(SetDebugMute(0));
                if (FAILED(copied))
                    Logging::Write(Logging::Level::Error, "Surface clone writeback failed; preserving the original cleanup-only return semantics.");
            }
        }
        Retire(cloneSurface_);
        Retire(cloneTexture_);
        Retire(device_);
        Retire(original_);
    }

    DWORD flags_ = 0;
    IDirect3DSurface9* original_ = nullptr;
    IDirect3DSurface9* cloneSurface_ = nullptr;
    IDirect3DTexture9* cloneTexture_ = nullptr;
    IDirect3DDevice9* device_ = nullptr;
};
static_assert(sizeof(SurfaceLocker) == 20);

bool ClipTransfer(IDirect3DSurface9* source, IDirect3DSurface9* target, const RECT& dirty,
    RECT& clippedSource, RECT& clippedTarget) noexcept
{
    D3DSURFACE_DESC description{};
    if (FAILED(source->GetDesc(&description))) return false;
    if (description.Width > LONG_MAX || description.Height > LONG_MAX)
    {
        InvalidMemory("Primary source dimensions overflow the original signed rectangle domain.");
        return false;
    }
    const RECT sourceBounds{0, 0, static_cast<LONG>(description.Width), static_cast<LONG>(description.Height)};
    if (!IntersectRect(&clippedSource, &dirty, &sourceBounds)) return false;
    const auto width = static_cast<std::int64_t>(clippedSource.right) - clippedSource.left;
    const auto height = static_cast<std::int64_t>(clippedSource.bottom) - clippedSource.top;
    std::int64_t left = dirty.left, top = dirty.top;
    std::int64_t right = left + width, bottom = top + height;
    if (right < LONG_MIN || right > LONG_MAX || bottom < LONG_MIN || bottom > LONG_MAX)
    {
        InvalidMemory("Primary destination rectangle overflow rejected.");
        return false;
    }
    if (FAILED(target->GetDesc(&description))) return false;
    if (description.Width > LONG_MAX || description.Height > LONG_MAX)
    {
        InvalidMemory("Primary target dimensions overflow the original signed rectangle domain.");
        return false;
    }
    std::int64_t sourceLeft = clippedSource.left, sourceTop = clippedSource.top;
    std::int64_t sourceRight = clippedSource.right, sourceBottom = clippedSource.bottom;
    if (left < 0) { sourceLeft -= left; left = 0; }
    if (top < 0) { sourceTop -= top; top = 0; }
    if (right > description.Width) { sourceRight -= right - description.Width; right = description.Width; }
    if (bottom > description.Height) { sourceBottom -= bottom - description.Height; bottom = description.Height; }
    if (sourceLeft >= sourceRight || sourceTop >= sourceBottom || left >= right || top >= bottom) return false;
    clippedSource = {static_cast<LONG>(sourceLeft), static_cast<LONG>(sourceTop),
        static_cast<LONG>(sourceRight), static_cast<LONG>(sourceBottom)};
    clippedTarget = {static_cast<LONG>(left), static_cast<LONG>(top), static_cast<LONG>(right), static_cast<LONG>(bottom)};
    return true;
}

HRESULT CopySurfaceRegion(IDirect3DSurface9* source, IDirect3DSurface9* target,
    const RECT& sourceRectangle, const RECT& targetRectangle)
{
    D3DSURFACE_DESC targetDescription{}, sourceDescription{};
    // 原04DBA重新取描述，区别于02E10已检查HRESULT的裁剪查询。
    static_cast<void>(target->GetDesc(&targetDescription));
    static_cast<void>(source->GetDesc(&sourceDescription));
    {
        OwnedCom<IDirect3DDevice9> device;
        static_cast<void>(source->GetDevice(&device.object));
        if (!device.object) return InvalidMemory("Surface transfer has no source device.");
        static_cast<void>(SetDebugMute(1));
        HRESULT result = E_FAIL;
        if (targetDescription.Pool == D3DPOOL_DEFAULT)
        {
            if (sourceDescription.Pool == D3DPOOL_DEFAULT)
                result = device.object->StretchRect(source, &sourceRectangle, target, &targetRectangle, D3DTEXF_NONE);
            else if (sourceDescription.Pool == D3DPOOL_SYSTEMMEM)
            {
                const POINT point{targetRectangle.left, targetRectangle.top};
                result = device.object->UpdateSurface(source, &sourceRectangle, target, &point);
            }
        }
        static_cast<void>(SetDebugMute(0)); // 原04F0F，先于临时device.Release。
        if (SUCCEEDED(result)) return D3D_OK;
    }
    SurfaceLocker sourceLock;
    LockedView input;
    auto result = sourceLock.Lock(source, sourceRectangle, true, input);
    if (FAILED(result)) return result;
    // 原04833先检查来源数据，再取得目标写锁。
    if (!input.data.pBits) return InvalidMemory("Surface transfer source lock returned no storage.");
    SurfaceLocker targetLock;
    LockedView output;
    result = targetLock.Lock(target, targetRectangle, false, output);
    if (FAILED(result)) return result;
    const auto width = static_cast<std::size_t>(sourceRectangle.right - sourceRectangle.left);
    const auto height = static_cast<std::size_t>(sourceRectangle.bottom - sourceRectangle.top);
    const auto pixelBytes = TexturePixelBytes(output.format);
    if (input.format != D3DFMT_X8R8G8B8 || !pixelBytes || !input.data.pBits || !output.data.pBits ||
        input.data.Pitch < 0 || output.data.Pitch < 0 ||
        width > static_cast<std::size_t>(input.data.Pitch) / 4u ||
        width > static_cast<std::size_t>(output.data.Pitch) / pixelBytes ||
        !RowsAddressable(input.data.Pitch, static_cast<UINT>(height), width * 4u) ||
        !RowsAddressable(output.data.Pitch, static_cast<UINT>(height), width * pixelBytes))
        return InvalidMemory("Surface transfer rejected invalid locked format/pitch/storage.");
    try
    {
        if (!ConvertXrgbRegion(output.format, static_cast<const std::uint8_t*>(input.data.pBits),
            input.data.Pitch, static_cast<std::uint8_t*>(output.data.pBits), output.data.Pitch, width, height))
            return InvalidMemory("Primary conversion rejected an invalid region or unsafe address extent.");
    }
    catch (...)
    {
        targetLock.RetireAfterException();
        throw;
    }
    // 原04833先退目标锁，04DBA再退源锁；cleanup的普通HRESULT不改成功返回。
    return D3D_OK;
}
}

bool TransferPrimarySurface(IDirect3DTexture9* texture, IDirect3DSurface9* source,
    const RECT& dirty)
{
    if (!texture || !source) return false;
    OwnedCom<IDirect3DSurface9> target;
    const auto result = texture->GetSurfaceLevel(0, &target.object);
    if (FAILED(result) || !target.object) return false;
    RECT sourceRectangle{}, targetRectangle{};
    if (!ClipTransfer(source, target.object, dirty, sourceRectangle, targetRectangle)) return false;
    // 原02F90遗漏level surface借用的Release；本层明确安全退引用，不复制泄漏。
    return SUCCEEDED(CopySurfaceRegion(source, target.object, sourceRectangle, targetRectangle));
}
}
