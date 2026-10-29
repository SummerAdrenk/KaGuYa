#include "SettingDialogNativeApply.Internal.h"

namespace KaGuYa::Start::Runtime
{
void ApplySettingDialogNativeAt51CFC0(SettingDialogNativeBodyACh& body)
{
    using namespace SettingDialogNativeApplyDetail;
    ApplySoundPrefix(body);
    unsigned next;RestoreFirstMappings(body,next);
    RestoreRemainingMappings(body,next);
    ApplyPolicyAndLayers(body);
}
}
