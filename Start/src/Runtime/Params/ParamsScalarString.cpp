#include <Runtime/ParamsScalarString.h>

#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ParamsScalarString::ParamsScalarString() : ParamsScalarString(DefaultRuntimeStringManagerSource()) {}

ParamsScalarString::ParamsScalarString(IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(value_, strings);
}

ParamsScalarString::~ParamsScalarString() noexcept
{
    DestroyRuntimeCStringAt40CD40(&value_);
}

ParamsScalarString::ParamsScalarString(const ParamsScalarString& other) : ParamsScalarString()
{
    AssignSnapshot(other.View());
}

ParamsScalarString& ParamsScalarString::operator=(const ParamsScalarString& other)
{
    if (this != &other) AssignSnapshot(other.View());
    return *this;
}

ParamsScalarString::ParamsScalarString(const ParamsText& snapshot) : ParamsScalarString()
{
    AssignSnapshot(snapshot.cp932);
}

ParamsScalarString& ParamsScalarString::operator=(const ParamsText& snapshot)
{
    AssignSnapshot(snapshot.cp932);
    return *this;
}

void ParamsScalarString::AssignSnapshot(std::string_view bytes)
{
    // 工具域深写，保留嵌入NUL；不把源header共享到现代profile快照。
    if (bytes.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("Params scalar snapshot exceeds CString length");
    SetRuntimeCStringAt437680(value_, bytes.data(), static_cast<int>(bytes.size()));
}
}
