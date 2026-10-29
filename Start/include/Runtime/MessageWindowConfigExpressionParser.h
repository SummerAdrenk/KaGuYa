#pragma once

#include <Runtime/MessageWindowConfigRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
enum class MessageWindowConfigExpressionErrorCode
{
    InvalidNumber,
    InvalidOperandParentheses,
    DivisionByZero,
    DivisionOverflow,
};

class MessageWindowConfigExpressionError final : public std::runtime_error
{
public:
    explicit MessageWindowConfigExpressionError(MessageWindowConfigExpressionErrorCode code);
    [[nodiscard]] MessageWindowConfigExpressionErrorCode Code() const noexcept;
private:
    MessageWindowConfigExpressionErrorCode code_;
};

// 484E40 构树 -> 48E330 左先右后求值；子项用 486CE0，数字叶用 47BAA0。
// 保持既有接口：调用者先做 45E770 的空格/Tab 删除，此类不再次规范化输入。
// 空叶为零，整串根括号不自动剥离；不能替换成通常的 C/C++ 表达式语法。
class MessageWindowConfigExpressionParser final : public IMessageWindowConfigExpressionParser
{
public:
    [[nodiscard]] std::int32_t Evaluate(std::string_view expressionCp932WithoutSpacesOrTabs) override;
};
}
