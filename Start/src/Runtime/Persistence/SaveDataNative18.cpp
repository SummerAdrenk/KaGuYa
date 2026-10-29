#include <Runtime/SaveDataNative18.h>
#include <Runtime/GameBuildProfile.h>
#include "NativeSerialize18.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace NativeSerialize18Detail;

U RecordCount(B* begin, B* end) noexcept
{
    // 48B1DF..48B1E4 / 457A73..457A7D：DWORD差值后SAR 4，不是无符号除法。
    return static_cast<U>(std::bit_cast<std::int32_t>(Address(end) - Address(begin)) >> 4);
}

void ResizeRecordsAt457A60(void* records12, U count, const void* unusedValue)
{
    // 48B251..48B257：ECX=+10，栈上传count与+10；原第二栈参数未读。
    static_cast<void>(unusedValue);
    Vector records{records12};
    auto* end = records.End();
    auto* begin = records.Begin();
    auto* capacityEnd = records.Capacity();
    const U size = RecordCount(begin, end);
    const U capacity = RecordCount(begin, capacityEnd);
    if (count > capacity)
    {
        // 457A8B..457AB2：只有扩容支检查无符号上限，再按1.5倍选容量。
        constexpr U maximum = 0x0FFFFFFFu;
        if (count > maximum) throw std::length_error("vector<T> too long");
        const U half = capacity >> 1;
        const U growth = capacity > maximum - half ? count : capacity + half;
        const U nextCapacity = growth < count ? count : growth;
        const U bytes = nextCapacity << 4;
        auto& memory = RequireNativeSerialize18Services().memory;
        B* next;
        if (bytes >= 0x1000u)
        {
            const U extended = bytes + 0x23u;
            auto* raw = memory.AllocateBody(extended <= bytes ? 0xFFFFFFFFu : extended);
            // 457AE4..457AF2：大块原有NULL拒绝、32字节对齐及begin[-4] raw cookie。
            if (!raw) _invalid_parameter_noinfo_noreturn();
            next = reinterpret_cast<B*>((Address(raw) + 0x23u) & 0xFFFFFFE0u);
            WriteAt(Plus(next, 0xFFFFFFFCu), 0, raw);
        }
        else next = bytes ? static_cast<B*>(memory.AllocateBody(bytes)) : nullptr;

        // 457B08..457B34：先清新增尾段，再重读当前旧范围；零长度memmove也保留。
        // 小块NULL没有新增guard；不借用现有typed resize中的安全终止分支。
        const U added = count - size;
        if (added) std::memset(Plus(next, size << 4), 0, added << 4);
        auto* currentBegin = records.Begin();
        auto* currentEnd = records.End();
        std::memmove(next, currentBegin, Address(currentEnd) - Address(currentBegin));
        // 457B39..457B7F：再次读旧begin/capacity并释放后，依次发布begin/end/capacity。
        // Commit/Free保留1000h阈值、raw头校验与sized delete；没有新块EH回滚。
        Commit(records, next, count, nextCapacity, 0x10);
        return;
    }
    if (count > size)
    {
        // 457B89..457BAC：使用入口缓存的end；清零之后才发布新的end。
        const U added = count - size;
        std::memset(end, 0, added << 4);
        records.End(Plus(end, added << 4));
    }
    else if (count != size) records.End(Plus(begin, count << 4));
}
}

void SerializeSaveDataNative18At48B150(void* nativeSave90, void* nativeSerializer18)
{
    SerializeSaveDataNative18WithSound(nativeSave90,nativeSerializer18,nullptr,nullptr);
}

void SerializeSaveDataNative18WithSound(void* nativeSave90,void* nativeSerializer18,
    NativeSaveDataSoundSerialize soundSerialize,void* context)
{
    using namespace NativeSerialize18Detail;
    void* const save = nativeSave90;
    void* const serializer = nativeSerializer18;

    // 48B163..48B1C7：每块各读一次mode；Scalars policy=1逐byte取6C并先增cursor。
    // 写支逐byte取14，按当前end/capacity内联追加，满时走4539A0；不改走slot50。
    Scalars(serializer, Plus(save, 0x04), 0x0C, 1);

    // 48B1C9..48B2B8：mode先于count；读支count保持未初始化，交slot58填写。
    Vector records{Plus(save, 0x10)};
    const auto writingRecords = Mode(serializer);
    U count;
    if (writingRecords)
    {
        auto* end = records.End();
        count = RecordCount(records.Begin(), end);
    }
    Argument(serializer, 0x58, &count);
    if (!writingRecords) ResizeRecordsAt457A60(records.p, count, records.p);
    // 读支先resize才有符号判count；每次比较均用slot58收到的当前局部值。
    for (U index = 0, offset = 0;
        std::bit_cast<std::int32_t>(index) < std::bit_cast<std::int32_t>(count);
        ++index, offset += 0x10)
    {
        // 48B202..48B238 / 48B270..48B2A6：四次begin和四次当前vptr分别重取。
        Argument(serializer, 0x3C, Plus(records.Begin(), offset));
        Argument(serializer, 0x3C, Plus(records.Begin(), offset + 4));
        Argument(serializer, 0x3C, Plus(records.Begin(), offset + 8));
        Argument(serializer, 0x3C, Plus(records.Begin(), offset + 0x0C));
    }

    // 48B2BB..48B2DF：base为零不读control/bias；否则先读control/stride再读bias。
    // 只按原Ref解出当前receiver，不增持、不猜kind，也不调用具名Sound叶。
    void* sound = nullptr;
    const U base = ReadAt<U>(save, 0x1C);
    if (base)
    {
        auto* control = ReadAt<const void*>(save, 0x24);
        const U stride = control ? ReadAt<U>(control, 0x0C) : 0;
        const U bias = ReadAt<U>(save, 0x20);
        sound = reinterpret_cast<void*>(base + bias * stride);
    }
    using ArchiveSlot00 = void(__thiscall*)(void*, void*);
    if(soundSerialize)soundSerialize(sound,serializer,context);
    else Slot<ArchiveSlot00>(sound, 0)(sound, serializer); // 原默认slot00/NULL语义保持。

    // 48B2E1..48B351：重新读mode，+34的12byte先于+54的bool虚槽。
    Scalars(serializer, Plus(save, 0x34), 0x0C, 1);
    Argument(serializer, 0x54, Plus(save, 0x54));

    // 48B354..48B367：第二次4C的receiver及vptr来自第一次返回值，不能固定serializer。
    void* const stringReceiver = Argument(serializer, 0x4C, Plus(save, 0x58));
    Argument(stringReceiver, 0x4C, Plus(save, 0x5C));

    // 48B36A..48B37B直接调451FA0，两次都用原serializer；其核为8byte/policy=1。
    // 451FAD / 451FBC / 451FF4..45200D：各自mode，逐byte的14与先增cursor的6C。
    Scalars(serializer, Plus(save, 0x40), 8, 1);
    Scalars(serializer, Plus(save, 0x48), 8, 1);

    // 48B380..48B3F1。
    Scalars(serializer, Plus(save, 0x60), 0x0C, 1);
    // 01:45C160/02:460D00/03:460310直接进入两字节块；旧wire没有+6C，读时保留原值。
    if (CompiledGameProfile().envReadColorBytePresent)
        Argument(serializer, 0x54, Plus(save, 0x6C));
    // 48B3F4..48B46F。
    Scalars(serializer, Plus(save, 0x6D), 2, 1);
    Argument(serializer, 0x54, Plus(save, 0x6F));
    Argument(serializer, 0x58, Plus(save, 0x70));
    // 48B472..48B54F：+81先于+74/+77，不能按地址排序或合并mode读取。
    Argument(serializer, 0x54, Plus(save, 0x81));
    Scalars(serializer, Plus(save, 0x74), 3, 1);
    Scalars(serializer, Plus(save, 0x77), 0x0A, 1);
    // 48B551..48B59A：+50仍是mode/14/6C内联核，不是slot50的后增直接cursor。
    Scalars(serializer, Plus(save, 0x50), 1, 1);
    // 48B59D..48B68F：两块各自mode，最后8C块的两支都无end/bounds检查。
    Scalars(serializer, Plus(save, 0x84), 8, 1);
    Scalars(serializer, Plus(save, 0x8C), 4, 1);
    // 48B655 / 48B690：原void返回；此处无对象/Ref/CString析构或额外释放资格。
}
}
