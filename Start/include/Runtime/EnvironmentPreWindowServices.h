#pragma once

#include <string_view>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentInitializationDisplay
{
public:
    virtual ~IEnvironmentInitializationDisplay() = default;
    virtual void Show(std::string_view text, std::string_view caption, unsigned type) = 0;
};

class Win32EnvironmentInitializationDisplay final : public IEnvironmentInitializationDisplay
{
public:
    void Show(std::string_view text, std::string_view caption, unsigned type) override;
};

}
