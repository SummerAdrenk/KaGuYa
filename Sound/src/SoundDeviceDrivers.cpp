#include "SoundDeviceDrivers.h"

#include <Logging/Logger.h>
#include <mmreg.h>
#include <dsound.h>
#include <atlstr.h>
#include <stdexcept>

namespace KaGuYa::Sound
{
struct SoundDeviceDriver final
{
    GUID id{};
    // 原108C0/10C40使用ATL宽串：nil引用、Clone/Allocate(charSize=2)、复制后提交长度。
    ATL::CSimpleStringT<wchar_t, false> description{ATL::CAtlStringMgr::GetInstance()};
    ATL::CSimpleStringT<wchar_t, false> module{ATL::CAtlStringMgr::GetInstance()};
};
static_assert(sizeof(SoundDeviceDriver) == 24);
static_assert(offsetof(SoundDeviceDriver, description) == 16);
static_assert(offsetof(SoundDeviceDriver, module) == 20);
static_assert(sizeof(std::_Ref_count_obj2<SoundDeviceDriver>) == 36);

void SoundDeviceDriverList::Initialize()
{
    head_ = new Node;
}

void SoundDeviceDriverList::Append(const std::shared_ptr<SoundDeviceDriver>& driver)
{
    auto* const previous = head_->previous;
    auto* const node = new Node(head_, previous, driver);
    // 原EF6A先分配/增持，EF71才检查容量；不把检查挪到节点创建之前。
    if (0x0FFFFFFEu - count_ < 1u)
    {
        std::destroy_at(&node->driver);
        delete node;
        Logging::Write(Logging::Level::Error, "Sound driver list capacity exceeded; unpublished node retired.");
        throw std::length_error("list<T> too long");
    }
    ++count_;
    head_->previous = node;
    previous->next = node;
}

void SoundDeviceDriverList::Clear() noexcept
{
    if (!head_) return;
    // 原F000先发布空表，再按旧next逐项退引用；driver成员倒序销毁module/description。
    auto* node = head_->next;
    head_->next = head_;
    head_->previous = head_;
    count_ = 0;
    while (node != head_)
    {
        auto* const next = node->next;
        std::destroy_at(&node->driver);
        delete node;
        node = next;
    }
}

SoundDeviceDriverList::~SoundDeviceDriverList()
{
    Clear();
    delete head_;
}

namespace
{
BOOL CALLBACK EnumerateDevice(LPGUID id, LPCWSTR description, LPCWSTR module, LPVOID context)
{
    if (!id || !description) return TRUE;
    // 原EE80的36字节实际控制块/对象；两串nil就绪后才复制GUID及API临时字符串。
    auto driver = std::make_shared<SoundDeviceDriver>();
    driver->id = *id;
    driver->description.SetString(description);
    driver->module.SetString(module);
    static_cast<SoundDeviceDriverList*>(context)->Append(driver);
    return TRUE;
}
}

SoundDefaultDeviceRoot::SoundDefaultDeviceRoot()
{
    drivers_.Initialize();
    drivers_.Clear();
    // 原34C7..34D8：任意非零枚举返回只清表，仍发布根并继续独立默认设备查询。
    if (DirectSoundEnumerateW(EnumerateDevice, &drivers_) != DS_OK) drivers_.Clear();
    // 构造异常保留失败出口，同时安全释放未发布根/节点；不复制原孤立块和遗留冷锁。
}

SoundDefaultDeviceRoot::~SoundDefaultDeviceRoot()
{
    drivers_.Clear(); // ED40的正文清表；成员析构再清空表并释放哨兵。
}

const GUID* SoundDefaultDeviceRoot::Query()
{
    // 原3505..3524：每次写临时GUID，只在hr==0时提交到同一个借用槽。
    GUID queried{};
    if (GetDeviceID(&DSDEVID_DefaultPlayback, &queried) != DS_OK) return nullptr;
    defaultDevice_ = queried;
    return &defaultDevice_;
}
}
