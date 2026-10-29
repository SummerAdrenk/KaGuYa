#include "SoundRoots.h"

namespace KaGuYa::Sound
{
SoundSystemRoot::SoundSystemRoot() { sounds.Initialize(); }

std::uint32_t SoundParameterRoot::SetFocus(std::uint32_t value) noexcept
{ focus_.store(value, std::memory_order_relaxed); return value; }
std::uint32_t SoundParameterRoot::Focus() const noexcept
{ return focus_.load(std::memory_order_relaxed); }
std::uint32_t SoundParameterRoot::SetPan(std::uint32_t value) noexcept
{ pan_.store(value, std::memory_order_relaxed); return value; }
std::uint32_t SoundParameterRoot::Pan() const noexcept
{ return pan_.load(std::memory_order_relaxed); }
std::uint32_t SoundParameterRoot::SetFrequency(std::uint32_t value) noexcept
{ frequency_.store(value, std::memory_order_relaxed); return value; }
std::uint32_t SoundParameterRoot::Frequency() const noexcept
{ return frequency_.load(std::memory_order_relaxed); }
std::uint32_t SoundParameterRoot::SetPosition(std::uint32_t value) noexcept
{ position_.store(value, std::memory_order_relaxed); return value; }
std::uint32_t SoundParameterRoot::Position() const noexcept
{ return position_.load(std::memory_order_relaxed); }

namespace
{
void CheckOriginalThreadResult(_Thrd_result result)
{
    // 原12E5B的C错误到Cpp错误映射；不能套新std::mutex固定deadlock异常。
    switch (static_cast<int>(result))
    {
    case 1: case 2: std::_Throw_Cpp_error(std::_RESOURCE_UNAVAILABLE_TRY_AGAIN);
    case 3: std::_Throw_Cpp_error(std::_DEVICE_OR_RESOURCE_BUSY);
    case 4: std::_Throw_Cpp_error(std::_INVALID_ARGUMENT);
    default: return;
    }
}
}
SoundColdMutex::SoundColdMutex() noexcept
{
    storage_._Critical_section = {};
    storage_._Type = _Mtx_try;
    storage_._Thread_id = -1;
    storage_._Count = 0;
}
void SoundColdMutex::Lock() { CheckOriginalThreadResult(_Mtx_lock(&storage_)); }
void SoundColdMutex::Unlock() noexcept
{
    // 原7BAFC的unlock执行期若抛出，unwind进入terminate；不返回伪成功。
    CheckOriginalThreadResult(_Mtx_unlock(&storage_));
}
}
