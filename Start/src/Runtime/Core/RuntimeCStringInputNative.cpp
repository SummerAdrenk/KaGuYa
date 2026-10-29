#include <Runtime/RuntimeCStringInputNative.h>

#include <cstdint>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
class ExistingResourceSource final : public IRuntimeCStringInputResourceSource
{
public:
    bool LoadAt420510(RuntimeCString& destination, std::uint32_t id) override
    {
        return LoadRuntimeCStringResourceAt420510(destination, id);
    }
};

class ConstructionUnwind final
{
public:
    explicit ConstructionUnwind(RuntimeCString* text) noexcept : text_(text) {}
    ~ConstructionUnwind() noexcept { if (text_) DestroyRuntimeCStringAt40CD40(text_); }
    ConstructionUnwind(const ConstructionUnwind&) = delete;
    ConstructionUnwind& operator=(const ConstructionUnwind&) = delete;
    void Disarm() noexcept { text_ = nullptr; }
private:
    RuntimeCString* text_;
};
}

RuntimeCString* ConstructRuntimeCStringInputNativeAt40B530(RuntimeCString* fresh,
    const char* input, IRuntimeStringManagerSource& strings)
{
    static ExistingResourceSource resources;
    return ConstructRuntimeCStringInputNativeAt40B530(fresh, input, strings, resources);
}

RuntimeCString* ConstructRuntimeCStringInputNativeAt40B530(RuntimeCString* fresh,
    const char* input, IRuntimeStringManagerSource& strings,
    IRuntimeCStringInputResourceSource& resources)
{
    static_assert(sizeof(void*) == 4 && sizeof(RuntimeCString) == 4);
    // 40B567..40B572：raw GetNil及指针发布完成之后，才进入state0。
    InitializeEmptyRuntimeCString(*fresh, strings);
    ConstructionUnwind unwind(fresh);

    const auto address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(input));
    if (address == 0)
    {
        // 40B5A2..40B5A8：null并非直接返回nil，必须实际进入Set(0,0)。
        SetRuntimeCStringAt437680(*fresh, nullptr, 0);
    }
    else if ((address & 0xFFFF0000u) != 0)
    {
        // 40B592..40B5A8：只测首NUL，测长晚于GetNil；低WORD为0也属于chars。
        SetRuntimeCStringAt437680(*fresh, input, static_cast<int>(std::strlen(input)));
    }
    else
    {
        // 40B585..40B590：非零低地址仅是WORD资源ID，不读其字节、不消费bool。
        static_cast<void>(resources.LoadAt420510(*fresh, address & 0xFFFFu));
    }

    unwind.Disarm();
    return fresh;
}
}
