#include <Runtime/AuxiliaryArchiveNative18.h>
#include "NativeSerialize18.Internal.h"

namespace KaGuYa::Start::Runtime
{
void SerializeClickInfoNative18At48AFB0(void* archive, void* serializer)
{
    using namespace NativeSerialize18Detail;
    // 48AFB0三次直接452020，各块独立mode，读byte先增cursor再取buffer。
    Scalars(serializer, Plus(archive, 4), 8, 1);
    Scalars(serializer, Plus(archive, 0x0C), 8, 1);
    Scalars(serializer, Plus(archive, 0x14), 8, 1);
}
void SerializePatternFilterNative18At48AFF0(void* archive, void* serializer)
{
    using namespace NativeSerialize18Detail;
    Argument(serializer, 0x58, Plus(archive, 4));
    // 原48B00D交给当前slot34，不能内联为现代count/vector协议后绕过这个虚槽。
    Argument(serializer, 0x34, Plus(archive, 8));
}
void SerializeScriptStackFrameNative18At4D5760(void* archive, void* serializer)
{
    using namespace NativeSerialize18Detail;
    Argument(serializer, 0x3C, Plus(archive, 4));
    Argument(serializer, 0x3C, Plus(archive, 8));
}
}
