#pragma once
#include "SaveFileContainerRuntime.Internal.h"
#include <Runtime/NativeSerialize18.h>

namespace KaGuYa::Start::Runtime::SaveFileContainerDetail
{
// 原三块复用同一个18h栈地址，carrier本身没有自动析构资格。
union SaveSerializerStorage final
{
    NativeSerialize18Body body;
    SaveSerializerStorage() noexcept {}
    ~SaveSerializerStorage() noexcept {}
};
class SaveSerializerLife final
{
public:
    explicit SaveSerializerLife(SaveSerializerStorage&) noexcept;
    ~SaveSerializerLife() noexcept;
    void NormalWithUnwind();
    void NormalAfterStateDrop();
private:
    NativeSerialize18Body& body_;
    bool armed_ = true;
};
YtlReferenceControlRuntime* WriteNativeSaveBlock(ContainerFile&, WorkBuffer&,
    NativeSerialize18Body&, std::uint32_t& size, std::uint32_t seed, bool first);
void WriteNativeHistoryBlock(ContainerFile&, WorkBuffer&, NativeSerialize18Body&,
    std::uint32_t& size, std::uint32_t seed, YtlReferenceControlRuntime* secondControl);
void ReadNativeSaveBlock(ContainerFile&, WorkBuffer&, std::uint32_t size, std::uint32_t seed);
void ImportNativeSaveBlock(NativeSerialize18Body&, WorkBuffer&);
}
