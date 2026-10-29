#include "LikabilityDialogNative.Internal.h"
#include "LikabilityDialogNative.Profile.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace LikabilityNativeDetail;
struct RefreshFrame final
{
    Vector digits{};
    Slot temp;
    void* raw = nullptr;
    int state = 0;
    ~RefreshFrame() noexcept
    {
        while (state >= 0)
        {
            const auto at = state;
            state = RefreshUnwind()[static_cast<unsigned>(at)].previous;
            switch (at)
            {
            case 0: DestroyGuiControlSurfaceReferencesAt43EAA0(digits); break;
            case 1: ReleaseYtlReference(temp); break;
            case 2: ::operator delete(raw, std::size_t{0x24}); break;
            default: std::terminate();
            }
        }
    }
};
struct LockUnwind final
{
    IEnvironmentRootLock& lock;
    bool active = true;
    ~LockUnwind() noexcept { if (active) lock.Leave(); }
};
EnvironmentRootStorage* LeaveAndRead(LikabilityDialogNativeServices& s, std::uint32_t readSite) noexcept
{
    // 原state5/10一直覆盖normal Leave及其后的6198C8重读。
    s.environmentLock.Leave();
    return s.source.EnvironmentPublicationAt6198C8(readSite);
}
EnvironmentRootStorage* InlineEnvironment(std::uint32_t checkSite, std::uint32_t publishSite, std::uint32_t readSite)
{
    auto& s = RequireLikabilityDialogNativeServices();
    s.environmentLock.Enter();
    auto* checked = s.source.EnvironmentPublicationAt6198C8(checkSite);
    LockUnwind unwind{s.environmentLock};
    if (!checked)
    {
        auto* created = CreateEnvironmentRootAt41811B(s.memory, s.strings);
        s.source.EnvironmentPublicationAt6198C8(publishSite) = created;
    }
    unwind.active = false;
    return LeaveAndRead(s, readSite);
}
std::int32_t RowValue(unsigned row, unsigned column, std::uint32_t ensureSite,
    std::uint32_t rootSite, std::uint32_t paramsSite)
{
    auto& source = RequireLikabilityDialogNativeServices().source;
    source.EnsureRootAt413BE0(ensureSite);
    auto* root = source.RootAt6198C4(rootSite);
    return NamedValue(root, VariableNames(row)[column], paramsSite);
}
void ThreeDigits(Body& b, Vector& digits, std::int32_t value, const POINT* points)
{
    const auto hundreds = value / 100;
    if (hundreds) Blit(b, digits, hundreds, points[2]);
    const auto remainder = value - hundreds * 100;
    const auto tens = remainder / 10;
    if (hundreds || tens) Blit(b, digits, tens, points[1]);
    Blit(b, digits, remainder - tens * 10, points[0]);
}
}
void RefreshLikabilityDialogNativeAt528F20(LikabilityDialogNativeBody94h& b)
{
    if (b.profileState) { RefreshLikabilityProfileGui(*b.profileState); return; }
    using namespace LikabilityNativeDetail;
    auto& s = RequireLikabilityDialogNativeServices();
    RefreshFrame f;
    Slot* end = nullptr; // 原EDI跨Load回调保留，不用vector.end重取替代快路位置。
    const auto names = LikabilityDialogNativeDigitNamesSample18();
    for (unsigned i = 0; i != 11; ++i)
    {
        f.temp = {};
        f.state = 1;
        auto* receiver = CaptureDraw(0x528F80, 0x528F85);
        const auto* name = names[i];
        f.raw = ::operator new(0x24);
        f.state = 2;
        auto* surface = f.raw ? ::new(f.raw) Surface(s.surfaces, s.strings) : nullptr;
        f.state = 1;
        AdoptGraphicsSurfaceAt407A80(f.temp, surface);
        auto* captured = Read(f.temp.controlAt08);
        if (!s.loader.LoadIntoAt4800A0(receiver, *Resolve<Surface>(f.temp), name)) throw 1;
        if (end != Read(f.digits.capacity))
        {
            end->baseAt00 = Read(f.temp.baseAt00);
            end->biasAt04 = Read(f.temp.biasAt04);
            end->controlAt08 = captured;
            if (captured) ++captured->referenceCountAt04;
            f.digits.end = ++end;
        }
        else
        {
            // 原4D83D0与4540A0同一Ref12增长kernel，详见两份ASM及PE调用目标。
            GrowGuiControlSurfaceReferencesAt4540A0(f.digits, end, f.temp);
            end = Read(f.digits.end);
            captured = Read(f.temp.controlAt08);
        }
        f.state = 3;
        ReleaseCaptured(captured);
        if (captured) { f.temp.controlAt08 = nullptr; f.temp.baseAt00 = 0; }
        f.state = 0;
    }
    for (unsigned row = 0; row != 3; ++row)
    {
        BorrowArchive0(InlineEnvironment(0x5290BC, 0x529170, 0x529187));
        const auto weight = RowValue(row, 0, 0x5291D8, 0x5291E2, 0x5291FF);
        const auto* points = Positions(row);
        // 原529211是非零就画第10项，不是负数测试，也没有abs；个位先于十位且十位0照画。
        if (weight) Blit(b, f.digits, 10, points[2]);
        const auto tens = weight / 10;
        Blit(b, f.digits, weight - tens * 10, points[0]);
        Blit(b, f.digits, tens, points[1]);
        BorrowArchive0(InlineEnvironment(0x5295FA, 0x5296AE, 0x5296CC));
        const auto intimacy = RowValue(row, 1, 0x52971D, 0x529727, 0x52973B);
        ThreeDigits(b, f.digits, intimacy, points + 3);
        BorrowArchive0(s.source.CaptureEnvironmentAt4180C0(0x529B42));
        const auto dependence = RowValue(row, 2, 0x529B8E, 0x529B98, 0x529BAC);
        ThreeDigits(b, f.digits, dependence, points + 6);
    }
    // 夏代子两次都是先固定Root再取Environment；不能复用上一代Root/Params。
    s.source.EnsureRootAt413BE0(0x529D7C);
    auto* root = s.source.RootAt6198C4(0x529D87);
    BorrowArchive0(s.source.CaptureEnvironmentAt4180C0(0x529D8F));
    if (NamedValue(root, ExtraNames()[0], 0x529DA5))
    {
        s.source.EnsureRootAt413BE0(0x529DC5);
        root = s.source.RootAt6198C4(0x529DD0);
        BorrowArchive0(s.source.CaptureEnvironmentAt4180C0(0x529DD8));
        ThreeDigits(b, f.digits, NamedValue(root, ExtraNames()[1], 0x529DEE), ExtraPositions());
    }
    DestroyGuiControlSurfaceReferencesAt43EAA0(f.digits);
    f.state = -1;
}
}
