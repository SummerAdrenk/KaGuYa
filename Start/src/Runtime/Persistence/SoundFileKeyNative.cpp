#include <Runtime/SoundParameterNative.h>
#include <array>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class FileKeyConstruction final
{
public:
    FileKeyConstruction(SoundFileKeyNative& body, IEnvironmentRootMemory& memory) noexcept
        : body_(body), memory_(memory), fields_{&body.keyAt04, &body.extensionAt08, &body.fileAt0C} {}
    ~FileKeyConstruction() noexcept
    {
        if (!active) return;
        // 5E7924 states48→47→46→45→3F：逐字段回退，最后5990D1 sized delete。
        while (constructed) DestroyRuntimeCStringAt40CD40(fields_[--constructed]);
        memory_.DeleteBody(&body_, 0x10);
    }
    unsigned constructed = 0;
    bool active = true;
private:
    SoundFileKeyNative& body_;
    IEnvironmentRootMemory& memory_;
    std::array<RuntimeCString*, 3> fields_;
};
void AssignNullTerminated(RuntimeCString& target, const char* source)
{
    // 410290：null表示长度0；有内嵌NUL时不会按CString头的长度共享赋值。
    SetRuntimeCStringAt437680(target, source, source ? static_cast<int>(std::strlen(source)) : 0);
}
}

SoundFileKeyNative* CreateSoundFileKeyAt47391A(const char* key, const char* extension,
    const char* file, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x10);
    if (!raw) return nullptr;
    auto* body = ::new (raw) SoundFileKeyNative(SoundNativeShellTag{});
    FileKeyConstruction unwind(*body, memory);
    InitializeEmptyRuntimeCString(body->keyAt04, strings); ++unwind.constructed;
    InitializeEmptyRuntimeCString(body->extensionAt08, strings); ++unwind.constructed;
    InitializeEmptyRuntimeCString(body->fileAt0C, strings); ++unwind.constructed;
    AssignNullTerminated(body->keyAt04, key);
    AssignNullTerminated(body->extensionAt08, extension);
    AssignNullTerminated(body->fileAt0C, file);
    unwind.active = false; // 4739C7解除正文构造资格；后续control失败不得自动删除body。
    return body;
}

void* SoundFileKeyNative::DeleteAtVslot00(std::uint32_t flags) noexcept
{
    DestroyRuntimeCStringAt40CD40(&fileAt0C);
    DestroyRuntimeCStringAt40CD40(&extensionAt08);
    DestroyRuntimeCStringAt40CD40(&keyAt04);
    // filekey没有IArchive基类；flags=0后仍保留原本的一个虚槽。
    if (flags & 1u) ::operator delete(this, std::size_t{0x10});
    return this;
}
}
