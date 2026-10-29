#include "CgDisplayResourceNative.Internal.h"
#include <Runtime/PatternResourcePlan.h>

namespace KaGuYa::Start::Runtime::CgDisplayResourceNativeDetail
{
void ReleaseScanner(Scanner& scanner)
{
    auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(Chars(scanner.textAt04))) - 1;
    scanner.vptrAt00 = &RuntimeStringScannerNativeMethods();
    ReleaseRuntimeCStringHeader(header);
}
void DestroyScanner(Scanner& scanner) noexcept { ReleaseScanner(scanner); }
namespace
{
Scanner* __fastcall DeleteScanner(Scanner* p, void*, unsigned flags) noexcept
{ DestroyScanner(*p); if (flags & 1u) ::operator delete(p, std::size_t{8}); return p; }
const RuntimeCStringFolderNativeVtable suffixMethods{
    reinterpret_cast<decltype(RuntimeCStringFolderNativeVtable::deletingDestructorAt00)>(&DeleteScanner)};
}
void ConstructScanner(Scanner& scanner, const char* source, IRuntimeStringManagerSource& strings)
{
    scanner.vptrAt00 = &RuntimeStringScannerNativeMethods();
    InitializeEmptyRuntimeCString(scanner.textAt04, strings);
    struct Prefix final
    {
        Scanner& scanner;
        bool active = true, derived = false;
        ~Prefix() noexcept
        {
            if (!active) return;
            if (derived) DestroyScanner(scanner);
            else DestroyRuntimeCStringAt40CD40(&scanner.textAt04);
        }
    } cleanup{scanner};
    SetRuntimeCStringAt437680(scanner.textAt04, "", 0);
    scanner.vptrAt00 = &suffixMethods;
    cleanup.derived = true;
    ScanPatternResourceSuffixAt45DA40(scanner.textAt04, source, strings);
    cleanup.active = false;
}
}
