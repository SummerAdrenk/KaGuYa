#include <Runtime/LegacyTitleScene.h>
#include <Logging/Logger.h>

#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class F> struct Cleanup final
{
    F function;
    bool armed = true;
    ~Cleanup() noexcept { if (armed) function(); }
    void Run() { armed = false; function(); }
};

class MenuControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit MenuControl(LegacyTitleMenu* value) noexcept : EnvironmentObjectReferenceControl(value)
    { strideAt0C = sizeof(LegacyTitleMenu); countAt10 = 1; }
    void DestroyPayloadAtVslot00() override
    {
        auto* menu = static_cast<LegacyTitleMenu*>(payloadAt14);
        // 普通资源释放先于C++接口的noexcept析构；后者见已退休标志不重复释放。
        menu->ReleaseResources();
        delete menu;
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};

std::uint8_t __fastcall Run(TaskNativeBody4h* task, void*, std::int32_t)
{
    auto& body = *reinterpret_cast<LegacyTitleScene*>(task);
    if (body.retired) throw std::logic_error("Retired legacy title scene");
    auto& s = *body.services;
    auto* menu = ResolveEnvironmentObjectReference<LegacyTitleMenu>(body.menu);
    LegacyTitleWorkflowServices workflow{s.host, *menu, s.audio, s.names, s.quickLoad, s.surfaces, s.screen};
    UpdateLegacyTitleWorkflow(body.workflow, {body.frame, body.black}, workflow);
    return 1; // 原4C8C30：Update后空Draw，再返回AL=1；不把transition返回当完成协议。
}

TaskNativeBody4h* __fastcall Delete(TaskNativeBody4h* task, void*, std::uint32_t flags)
{
    auto& body = *reinterpret_cast<LegacyTitleScene*>(task);
    auto& memory = body.services->memory;
    ReleaseLegacyTitleScene(body);
    if (flags & 1u)
    {
        body.~LegacyTitleScene();
        memory.DeleteBody(&body, sizeof(LegacyTitleScene));
    }
    return task;
}
const TaskNativeVtable methods{
    reinterpret_cast<decltype(TaskNativeVtable::runAt00)>(&Run),
    reinterpret_cast<decltype(TaskNativeVtable::deleteAt04)>(&Delete)};
}

const TaskNativeVtable& LegacyTitleSceneMethods() noexcept { return methods; }

void ReleaseLegacyTitleScene(LegacyTitleScene& body)
{
    if (body.retired) return;
    body.retired = true;
    GraphicsSurfaceNativeScope scope(body.services->surfaces);
    // 49EFF0/51E610..51E62E：black -> frame -> menu。普通释放失败只展开后续成员。
    Cleanup menu{[&] { ReleaseYtlReference(body.menu); }};
    Cleanup frame{[&] { ReleaseYtlReference(body.frame); }};
    ReleaseYtlReference(body.black);
    frame.Run();
    menu.Run();
}

LegacyTitleScene* ConstructLegacyTitleScene(LegacyTitleScene& body, LegacyTitleSceneServices& s)
{
    body.task.vptrAt00 = &methods;
    body.services = &s;
    body.menu = {}; body.frame = {}; body.black = {};
    body.retired = false;
    body.workflow.operation = TransitOperation{};
    GraphicsSurfaceNativeScope scope(s.surfaces);
    Cleanup members{[&] { ReleaseLegacyTitleScene(body); }};

    // 49C7E8..49C815：清公共窗口，随后才加载黑图；不先提交common/root初始值。
    s.host.CaptureScenes().ClearPublicAt46DB90();
    const auto* blackName = s.host.BlackSurfaceName();
    if (!s.host.LoadBlackSurface(body.black, blackName)) throw 1;

    auto* menu = new LegacyTitleMenu(s.menuData, s.menu);
    // 4BD370：正文完成后先发布base再申请控制块；不增添原来没有的分配失败回滚资格。
    body.menu.baseAt00 = reinterpret_cast<std::uintptr_t>(menu);
    body.menu.biasAt04 = 0;
    body.menu.controlAt08 = new MenuControl(menu);
    if (!menu->Ready()) throw 1;

    auto* frame = NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings);
    AdoptGraphicsSurfaceAt407A80(body.frame, frame);
    const auto height = s.host.ScreenHeight();
    const auto width = s.host.ScreenWidth();
    if (!frame)
    {
        Logging::Write(Logging::Level::Error, "LegacyTitle: null frame allocation (Baku 49C917)");
        throw std::runtime_error("Legacy title frame allocation returned null (Baku 49C917)");
    }
    if (!frame->CreateAt415010(s.surfaces, width, height, 0, 1)) throw 1;
    menu->Compose(*frame);

    // 49C95A..49CA4C逐次取registry2；只改原写入字段，0Ch只写一个byte。
    s.host.CaptureCommon().ModeAt04() = 0;
    s.host.CaptureCommon().BytesAt24()[0] = 1;
    s.host.CaptureCommon().BytesAt24()[1] = 1;
    s.host.CaptureCommon().BytesAt24()[2] = 1;
    s.host.CaptureCommon().BytesAt24()[3] = 0;
    s.host.CaptureCommon().BytesAt08()[0] = 0;
    s.host.CaptureCommon().BytesAt08()[1] = 1;
    *reinterpret_cast<volatile std::uint8_t*>(&s.host.CaptureCommon().DwordAt0C()) = 0;
    s.host.CaptureCommon().FlagsAt18() = 0;
    s.host.CaptureCommon().FlagsAt14() = 1;
    s.host.CaptureCommon().FlagsAt1C() = 1;
    s.host.CaptureCommon().BytesAt24()[5] = 0;
    s.host.CaptureCommon().BytesAt24()[4] = 0;
    body.workflow.phase = 0;
    s.host.WriteStartupFlag(0);
    s.host.WriteStartupReadyFlag(1);
    s.host.ClearReplay();
    members.armed = false;
    return &body;
}

LegacyTitleScene* NewLegacyTitleScene(LegacyTitleSceneServices& services)
{
    auto* raw = services.memory.AllocateBody(sizeof(LegacyTitleScene));
    if (!raw) return nullptr;
    Cleanup allocation{[&] { services.memory.DeleteBody(raw, sizeof(LegacyTitleScene)); }};
    auto* result = ConstructLegacyTitleScene(*::new(raw) LegacyTitleScene, services);
    allocation.armed = false;
    return result;
}
}
