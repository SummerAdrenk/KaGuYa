#include <Runtime/LegacyDisplayDeviceOwner.h>
#include <algorithm>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
const char* Name(const std::string& name) noexcept { return name.empty() ? nullptr : name.c_str(); }
DEVMODEA EmptyMode() noexcept { DEVMODEA mode{}; mode.dmSize = sizeof(mode); return mode; }
auto FindSizeAndRate(const std::vector<DEVMODEA>& modes, LegacyDisplayMode value)
{
    return std::find_if(modes.begin(), modes.end(), [&](const auto& mode)
        { return SameLegacyDisplaySizeAndRate(DescribeLegacyDisplayMode(mode), value); });
}
}
void LegacyDisplayDeviceOwner::RequireInitialized() const
{
    if (!initialized_) throw std::logic_error("Legacy display directory is not initialized");
}
std::uint32_t LegacyDisplayDeviceOwner::SelectedIndex() const
{ RequireInitialized(); return selected_; }

LegacyDisplayDeviceOwner::Seed LegacyDisplayDeviceOwner::DefaultRecord(std::uint32_t index)
{
    // 01:468B30 / 03:46CF30：查询失败保留这四个种子值并返回false。
    Seed seed{{width_, height_, 32, 60}, false};
    const auto& device = devices_.at(index);
    auto current = EmptyMode();
    if (!api_.Enumerate(Name(device.name), ENUM_CURRENT_SETTINGS, current)) return seed;
    seed.value = DescribeLegacyDisplayMode(current);
    const auto found = FindSizeAndRate(device.modes, seed.value);
    if (found != device.modes.end())
    {
        seed.value.bitsPerPixel = found->dmBitsPerPel;
        seed.supported = true;
    }
    return seed;
}

void LegacyDisplayDeviceOwner::Initialize(std::uint32_t width, std::uint32_t height)
{
    if (initialized_ || !devices_.empty()) throw std::logic_error("Legacy display owner cannot be reinitialized");
    if (!width || !height) throw std::invalid_argument("Legacy display logical size is zero");
    width_ = width; height_ = height;
    // 01:4679DA..467A2F；启动桌面色深是所有设备模式筛选的共同输入。
    const auto desktop = ReadLegacyDisplayMode(api_, "");
    std::optional<std::uint32_t> primary;
    for (DWORD index = 0;; ++index)
    {
        DISPLAY_DEVICEA device{}; device.cb = sizeof(device);
        if (!api_.EnumerateDevice(index, device)) break;
        if (!(device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) continue;
        auto modes = EnumerateLegacyDisplayModes(api_, device.DeviceName, desktop.dmBitsPerPel, width, height);
        if (modes.empty()) continue;
        if (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
            primary = static_cast<std::uint32_t>(devices_.size());
        devices_.push_back({device.DeviceName, std::move(modes), true});
    }
    // 01:467BE9..467C98 / 03:46BFE9..46C098：仅整个目录为空才尝试NULL设备。
    if (devices_.empty())
    {
        auto modes = EnumerateLegacyDisplayModes(api_, "", desktop.dmBitsPerPel, width, height);
        if (modes.empty()) throw std::runtime_error("Legacy display directory has no eligible device");
        devices_.push_back({"", std::move(modes), true});
        primary = 0;
    }
    // 原+0C未初始化的危险域不复刻；无primary时按467CAE的默认目录0语义确定化。
    selected_ = primary.value_or(0);
    const auto count = static_cast<std::uint32_t>(devices_.size());
    if (store_.DeviceRecordCount() != count)
    {
        store_.ClearDeviceRecords();
        // 原首次重建忽略seed的bool；随后仍进入逐项校验，不提前置不可用。
        for (std::uint32_t index = 0; index < count; ++index)
            store_.AppendDeviceRecord(DefaultRecord(index).value);
    }
    if (store_.ReadScaling() > 2) store_.WriteScaling(1);
    for (std::uint32_t index = 0; index < count; ++index)
    {
        auto saved = store_.ReadDeviceMode(index);
        const auto& modes = devices_[index].modes;
        const auto found = FindSizeAndRate(modes, saved);
        if (found != modes.end())
        {
            if (saved.bitsPerPixel != found->dmBitsPerPel)
            {
                saved.bitsPerPixel = found->dmBitsPerPel;
                store_.WriteDeviceMode(index, saved);
            }
        }
        else
        {
            const auto seed = DefaultRecord(index);
            if (!seed.supported) devices_[index].usable = false;
            store_.WriteDeviceMode(index, seed.value);
        }
    }
    // 01:468468..4686ED / 03:46C868..46CAED；失败不回滚已提交的配置前缀。
    const auto checked = ReadLegacyDisplayMode(api_, "");
    if (checked.dmPelsWidth < width || checked.dmPelsHeight < height)
        throw std::runtime_error("Legacy desktop is smaller than the logical canvas");
    if (checked.dmBitsPerPel < 16) throw std::runtime_error("Legacy desktop depth is below 16 bits");
    if (std::none_of(devices_.begin(), devices_.end(), [](const auto& d) { return d.usable; }))
        throw std::runtime_error("Legacy display directory has no usable saved mode");
    initialized_ = true;
}

std::uint32_t LegacyDisplayDeviceOwner::FindUsablePrimaryOrFirst()
{
    // 01:418020 / 03:41A570，不用上次启动的primary缓存冒充当前枚举。
    for (DWORD index = 0;; ++index)
    {
        DISPLAY_DEVICEA device{}; device.cb = sizeof(device);
        if (!api_.EnumerateDevice(index, device)) break;
        if ((device.StateFlags & (DISPLAY_DEVICE_ATTACHED_TO_DESKTOP | DISPLAY_DEVICE_PRIMARY_DEVICE)) !=
            (DISPLAY_DEVICE_ATTACHED_TO_DESKTOP | DISPLAY_DEVICE_PRIMARY_DEVICE)) continue;
        for (std::uint32_t i = 0; i < devices_.size(); ++i)
            if (devices_[i].name == device.DeviceName && devices_[i].usable) return i;
        break;
    }
    for (std::uint32_t i = 0; i < devices_.size(); ++i) if (devices_[i].usable) return i;
    throw std::runtime_error("Legacy display directory lost all usable devices");
}
void LegacyDisplayDeviceOwner::SelectForFullscreen(std::optional<std::string_view> monitor)
{
    RequireInitialized();
    // 恢复失败仍拥有旧设备；不能把另一个设备的记录和这个lease混用。
    if (active_) throw std::logic_error("Release the outstanding legacy display before selecting another device");
    selected_ = 0;
    if (devices_.size() == 1) return;
    if (monitor)
        for (std::uint32_t i = 0; i < devices_.size(); ++i)
            if (devices_[i].name == *monitor) { selected_ = i; break; }
    if (!devices_[selected_].usable) selected_ = FindUsablePrimaryOrFirst();
}

LegacyDisplayOwnershipResult LegacyDisplayDeviceOwner::Acquire()
{
    if (active_) return LegacyDisplayOwnershipResult::NoChange;
    const auto& device = devices_.at(selected_);
    const auto saved = store_.ReadDeviceMode(selected_);
    const auto found = std::find_if(device.modes.begin(), device.modes.end(), [&](const auto& mode)
        { return DescribeLegacyDisplayMode(mode) == saved; });
    if (found == device.modes.end()) return LegacyDisplayOwnershipResult::MissingSavedMode;
    auto current = EmptyMode();
    if (api_.Enumerate(Name(device.name), ENUM_CURRENT_SETTINGS, current) &&
        DescribeLegacyDisplayMode(current) == saved)
    {
        active_ = selected_;
        return LegacyDisplayOwnershipResult::Acquired;
    }
    if (!ApplyLegacyDisplayMode(api_, device.name, *found)) return LegacyDisplayOwnershipResult::ApplyFailed;
    active_ = selected_;
    return LegacyDisplayOwnershipResult::Acquired;
}
LegacyDisplayOwnershipResult LegacyDisplayDeviceOwner::Release()
{
    if (!active_) return LegacyDisplayOwnershipResult::NoChange;
    const auto& name = devices_.at(*active_).name;
    auto current = EmptyMode(), registry = EmptyMode();
    // 查询失败同样必须restore-null；不能把未知状态当成已经恢复。
    const bool equal = api_.Enumerate(Name(name), ENUM_CURRENT_SETTINGS, current) &&
        api_.Enumerate(Name(name), ENUM_REGISTRY_SETTINGS, registry) &&
        DescribeLegacyDisplayMode(current) == DescribeLegacyDisplayMode(registry);
    if (!equal && api_.RestoreDesktop(Name(name)) != DISP_CHANGE_SUCCESSFUL)
        return LegacyDisplayOwnershipResult::RestoreFailed;
    active_.reset();
    return LegacyDisplayOwnershipResult::Released;
}
LegacyDisplayOwnershipResult LegacyDisplayDeviceOwner::SetExclusive(bool acquire)
{ RequireInitialized(); return acquire ? Acquire() : Release(); }
LegacyDisplayOwnershipResult LegacyDisplayDeviceOwner::Shutdown()
{
    // 显式退出入口：失败保持active，可报告/重试；不模仿旧析构失败后丢掉恢复责任。
    return Release();
}
LegacyDisplaySettingsContext LegacyDisplayDeviceOwner::CaptureDisplayContext() const
{
    RequireInitialized();
    // 原416E30无active返回空表；正常UI由Page02控制。非法直发801D明确拒绝。
    if (!active_) throw std::logic_error("Legacy settings require an acquired display device");
    const auto& device = devices_.at(*active_);
    return {device.name, *active_, device.modes};
}
DEVMODEA LegacyDisplayDeviceOwner::ReadSelectedCurrentMode()
{ RequireInitialized(); return ReadLegacyDisplayMode(api_, devices_.at(selected_).name); }
DEVMODEA LegacyDisplayDeviceOwner::ReadSelectedRegistryModeOrZero()
{
    RequireInitialized();
    auto mode = EmptyMode();
    if (!api_.Enumerate(Name(devices_.at(selected_).name), ENUM_REGISTRY_SETTINGS, mode)) return EmptyMode();
    return mode;
}
LegacyDisplayActivationAction DecideLegacyDisplayActivation(
    bool fullscreen, bool activating, bool iconic, bool minimizeGate) noexcept
{
    // 01:40BFE0 / 02:40D280 / 03:40D150：设置210/211将gate置0，失焦不恢复。
    if (!fullscreen) return LegacyDisplayActivationAction::None;
    if (activating && iconic) return LegacyDisplayActivationAction::AcquireThenRestorePlacement;
    if (!activating && minimizeGate && !iconic) return LegacyDisplayActivationAction::MinimizeThenRelease;
    return LegacyDisplayActivationAction::None;
}
}
