#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <new>
#include <stdexcept>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local IGraphicsSurfaceApi* activeApi = nullptr;

[[noreturn]] void MissingGraphicsScope()
{ throw std::logic_error("CSurface destruction requires its external Graphics scope"); }

GraphicsSurfaceNativeBody* __fastcall DeleteBody(GraphicsSurfaceNativeBody* self, void*, std::uint32_t flags);
const GraphicsSurfaceNativeVtable methods{
    reinterpret_cast<decltype(GraphicsSurfaceNativeVtable::deletingDestructor)>(&DeleteBody)};

void ClearFields(GraphicsSurfaceNativeBody& body)
{
    body.handleAt04 = 0;
    body.widthAt08 = 0;
    body.heightAt0C = 0;
    body.originXAt10 = 0;
    body.originYAt14 = 0;
    body.alphaAt18 = 0;
    // 432754..80：Set空串可抛；颜色必须在其后清零，padding不写。
    SetRuntimeCStringAt437680(body.nameAt1C, "", 0);
    body.clearColorAt20 = 0;
}

GraphicsSurfaceNativeBody* __fastcall DeleteBody(GraphicsSurfaceNativeBody* self, void*, std::uint32_t flags)
{
    // 444B6A..B0：array cookie在正文前4字节，逆序析构；bit0独立控制释放。
    if (flags & 2u)
    {
        auto* cookie = reinterpret_cast<std::uint32_t*>(self) - 1;
        const auto count = *cookie;
        for (auto left = count; left != 0; --left) self[left - 1].~GraphicsSurfaceNativeBody();
        if (flags & 1u) ::operator delete[](cookie, std::size_t{4u + *cookie * 0x24u});
        return reinterpret_cast<GraphicsSurfaceNativeBody*>(cookie);
    }
    self->~GraphicsSurfaceNativeBody();
    if (flags & 1u) ::operator delete(self, std::size_t{0x24});
    return self;
}

class SurfaceControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SurfaceControl(GraphicsSurfaceNativeBody* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 43C890：正文可空，非空时取当时vptr，不能静态替换其删除虚槽。
        auto* body = static_cast<GraphicsSurfaceNativeBody*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructor(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(SurfaceControl) == 0x18);
}

GraphicsSurfaceNativeBody::GraphicsSurfaceNativeBody(IGraphicsSurfaceApi& api,
    IRuntimeStringManagerSource& strings)
    : vptrAt00(&methods), handleAt04(0), widthAt08(0), heightAt0C(0), originXAt10(0), originYAt14(0)
{
    // 40C332..351：alpha/颜色尚未初始化，nil构造成功后才进入Reset。
    InitializeEmptyRuntimeCString(nameAt1C, strings);
    try { ResetAt432670(api); }
    catch (...)
    {
        struct Unwind final
        {
            RuntimeCString& name;
            ~Unwind() noexcept { DestroyRuntimeCStringAt40CD40(&name); }
        } unwind{nameAt1C};
        throw;
    }
}

GraphicsSurfaceNativeBody::~GraphicsSurfaceNativeBody() noexcept
{
    // 40EF50/444BBC均先恢复vptr；EHFlags=5，Reset或Free异常终止，不重试CString。
    vptrAt00 = &methods;
    if (handleAt04)
    {
        if (!activeApi) MissingGraphicsScope();
        activeApi->ReleaseSurface(handleAt04);
    }
    ClearFields(*this);
    DestroyRuntimeCStringAt40CD40(&nameAt1C);
}

GraphicsSurfaceNativeBody::GraphicsSurfaceNativeBody(ModernMoveTag, GraphicsSurfaceNativeBody& source) noexcept
{
    // 构造期复制representation，保证目标对象生命期成立；不新增原GetNil/Clone调用。
    std::memcpy(this, &source, sizeof(*this));
    source.handleAt04 = 0;
    source.widthAt08 = source.heightAt0C = 0;
    source.originXAt10 = source.originYAt14 = 0;
    source.alphaAt18 = 0;
    source.clearColorAt20 = 0;
    const char* empty = nullptr;
    std::memcpy(&source.nameAt1C, &empty, sizeof(empty));
}

void GraphicsSurfaceNativeBody::ResetAt432670(IGraphicsSurfaceApi& api)
{
    const auto handle = handleAt04;
    if (handle) api.ReleaseSurface(handle);
    ClearFields(*this);
}

bool GraphicsSurfaceNativeBody::CreateAt415010(IGraphicsSurfaceApi& api,
    std::int32_t width, std::int32_t height, std::int32_t alpha, std::int32_t clear)
{
    ResetAt432670(api);
    handleAt04 = api.CreateSurface(width, height, alpha, clear);
    if (!handleAt04) return false;
    // 4150DF/E5 原样传入 DWORD；415117..11A 成功后仅提交低八位。
    alphaAt18 = static_cast<std::uint8_t>(alpha);
    widthAt08 = width;
    heightAt0C = height;
    return true;
}

GraphicsSurfaceNativeScope::GraphicsSurfaceNativeScope(IGraphicsSurfaceApi& api) noexcept
    : GraphicsSurfaceNativeScope(&api) {}
GraphicsSurfaceNativeScope::GraphicsSurfaceNativeScope(IGraphicsSurfaceApi* api) noexcept
    : previous_(activeApi) { activeApi = api; }
GraphicsSurfaceNativeScope::~GraphicsSurfaceNativeScope() noexcept { activeApi = previous_; }

GraphicsSurfaceNativeBody* NewGraphicsSurfaceAt40C2E0(IGraphicsSurfaceApi& api,
    IRuntimeStringManagerSource& strings)
{
    void* raw = ::operator new(0x24);
    try { return raw ? ::new(raw) GraphicsSurfaceNativeBody(api, strings) : nullptr; }
    catch (...)
    {
        struct Unwind final
        {
            void* raw;
            ~Unwind() noexcept { ::operator delete(raw, std::size_t{0x24}); }
        } unwind{raw};
        throw;
    }
}

void AdoptGraphicsSurfaceAt407A80(YtlReferenceSlotRuntime& slot, GraphicsSurfaceNativeBody* body)
{
    ReleaseYtlReference(slot);
    if (!body) return;
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = ::operator new(0x18);
    auto* control = raw ? ::new(raw) SurfaceControl(body) : nullptr;
    slot.controlAt08 = control;
    // 原407AF4此处访问空地址；现代明确终止，不产生伪成功或回收已发布base。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x24;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

void ConstructGraphicsSurfaceReferenceAt451500(YtlReferenceSlotRuntime& out,
    GraphicsSurfaceNativeBody* body, std::uint8_t owning)
{
    out = {};
    if (!body) return;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = ::operator new(0x18);
    auto* control = raw ? ::new(raw) SurfaceControl(body) : nullptr;
    out.controlAt08 = control;
    // 原451542空control写入故障；沿用已登记的明确安全终止边界。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    out.controlAt08->owningAt08 = owning;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C = 0x24;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10 = 1;
}
}
