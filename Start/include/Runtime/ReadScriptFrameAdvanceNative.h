#pragma once
namespace KaGuYa::Start::Runtime
{
class EnvironmentRootOwner;class InputRootOwner;
// 4B5A3A/4B5C04的slot5调用资格，随后完整4C3350；不是4C3430效果抑制。
bool SuppressReadScriptDrawAt4B5A3A(EnvironmentRootOwner&,InputRootOwner&);
}
