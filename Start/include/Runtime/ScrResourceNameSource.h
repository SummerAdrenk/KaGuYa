#pragma once

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 原调用者先捕获Pattern裸receiver；后续回调可能换根，不能在查询时重新取根。
// 此视图不分配、不AddRef、不复制表；来源对象必须活到最后一次查询。
class ScrResourceNameReceiver final
{
public:
    using Lookup = const char* (*)(const void*, std::int32_t);

    ScrResourceNameReceiver(const void* receiver, Lookup lookup) noexcept
        : receiver_(receiver), lookup_(lookup) {}

    [[nodiscard]] const char* FindNameAt4AD1B0(std::int32_t id) const
        { return lookup_(receiver_, id); }

private:
    const void* receiver_;
    Lookup lookup_;
};

class IScrResourceNameSource
{
public:
    virtual ~IScrResourceNameSource() = default;
    [[nodiscard]] virtual ScrResourceNameReceiver CapturePatternNames() const = 0;
};

// 49CF4F..5E在取得静态Params与解出Pattern之间读取operand；两个时点不能合并。
// 仅借根地址，不预读Pattern、不AddRef；第二步必须访问这个根而非再次获取owner。
class ScrResourceNameRootReceiver final
{
public:
    using Capture = ScrResourceNameReceiver (*)(const void*);

    ScrResourceNameRootReceiver(const void* receiver, Capture capture) noexcept
        : receiver_(receiver), capture_(capture) {}

    [[nodiscard]] ScrResourceNameReceiver CapturePatternNames() const
        { return capture_(receiver_); }

private:
    const void* receiver_;
    Capture capture_;
};

class IScrResourceNameRootSource
{
public:
    virtual ~IScrResourceNameRootSource() = default;
    [[nodiscard]] virtual ScrResourceNameRootReceiver CaptureParamsForNames() const = 0;
};

// 47A020反向查找有独立的null/空表前缀；不由按ID查名接口推导。
class IScrResourceLookupSource : public IScrResourceNameSource
{
public:
    [[nodiscard]] virtual std::int32_t FindResourceIdAt47A020(const char* nameCp932) const = 0;
};
}
