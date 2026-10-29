#include <Runtime/MessageWindowConfigExpressionParser.h>

#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace KaGuYa::Start::Runtime
{
namespace
{
using ErrorCode = MessageWindowConfigExpressionErrorCode;
constexpr auto NoNode = std::string_view::npos;

const char* ErrorMessage(ErrorCode code) noexcept
{
    switch (code)
    {
    case ErrorCode::InvalidNumber: return "CMesWin expression: invalid integer leaf";
    case ErrorCode::InvalidOperandParentheses: return "CMesWin expression: malformed operand parentheses";
    case ErrorCode::DivisionByZero: return "CMesWin expression: division by zero";
    case ErrorCode::DivisionOverflow: return "CMesWin expression: signed division overflow";
    }
    return "CMesWin expression: unknown error";
}

std::size_t SelectSplit(std::string_view text)
{
    std::size_t split = NoNode;
    std::int64_t depth = 0;
    int priority = 4;
    for (std::size_t index = 0; index < text.size(); ++index)
    {
        const auto byte = text[index];
        if (byte == '(') ++depth;
        else if (byte == ')') --depth;
        else
        {
            const int candidate = byte == '+' || byte == '-' ? 2 :
                byte == '*' || byte == '/' || byte == '%' ? 3 : 4;
            // 484EE2..484EEE：同优先级也更新，因此选最右项，得到左结合。
            // 不把开头/连续 +/- 特判成一元操作，也不在这里增加括号配平检查。
            if (depth == 0 && candidate != 4 && candidate <= priority)
            {
                priority = candidate;
                split = index;
            }
        }
    }
    return split;
}

std::string_view StripOperand(std::string_view text)
{
    // 486CE0 只对拆分后的操作数调用；长度不足三字节的 "()" 不会被剥成空串。
    while (text.size() >= 3 && text.front() == '(' && text.back() == ')')
    {
        std::int64_t depth = 1;
        for (std::size_t index = 1; index + 1 < text.size(); ++index)
        {
            if (text[index] == '(') ++depth;
            else if (text[index] == ')') --depth;
            if (depth == 0) return text;
        }
        if (depth != 1) throw MessageWindowConfigExpressionError(ErrorCode::InvalidOperandParentheses);
        text.remove_prefix(1);
        text.remove_suffix(1);
    }
    return text;
}

struct Node final
{
    std::string_view text;
    std::size_t left = NoNode;
    std::size_t right = NoNode;
    std::size_t split = NoNode;
    char operation = '\0';
    std::uint32_t value = 0;
};

struct BuildFrame final
{
    std::size_t node;
    unsigned phase = 0;
};

std::vector<Node> BuildTree(std::string_view text, std::vector<std::size_t>& postorder)
{
    std::vector<Node> nodes{{text}};
    std::vector<BuildFrame> pending{{0}};
    // 显式栈避免长表达式耗尽 C++ 调用栈；先完整构树，之后才能开始求值。
    while (!pending.empty())
    {
        const auto index = pending.back().node;
        if (pending.back().phase == 0)
        {
            const auto split = SelectSplit(nodes[index].text);
            if (split == NoNode)
            {
                postorder.push_back(index);
                pending.pop_back();
                continue;
            }
            nodes[index].split = split;
            nodes[index].operation = nodes[index].text[split];
            const auto left = StripOperand(nodes[index].text.substr(0, split));
            const auto child = nodes.size();
            nodes[index].left = child;
            nodes.push_back({left});
            pending.back().phase = 1;
            pending.push_back({child});
        }
        else if (pending.back().phase == 1)
        {
            // 原序为左子树完成后才剥离/构造右操作数，不能预先验证整个右串。
            const auto right = StripOperand(nodes[index].text.substr(nodes[index].split + 1u));
            const auto child = nodes.size();
            nodes[index].right = child;
            nodes.push_back({right});
            pending.back().phase = 2;
            pending.push_back({child});
        }
        else
        {
            postorder.push_back(index);
            pending.pop_back();
        }
    }
    return nodes;
}

unsigned char LowerAscii(unsigned char byte) noexcept
{
    return byte >= 'A' && byte <= 'Z' ? static_cast<unsigned char>(byte + ('a' - 'A')) : byte;
}

std::uint32_t ParseNumber(std::string_view text)
{
    std::size_t begin = 0;
    std::size_t end = text.size();
    std::uint32_t radix = 10;
    // 47BAA0 先 481690 转小写。合法数字只含 ASCII；任何非 ASCII 叶字节均不合法。
    // h 后缀分支先于 0x 前缀，故 "0x10h" 不是合法十六进制。
    if (end >= 2 && LowerAscii(static_cast<unsigned char>(text.back())) == 'h')
    {
        --end;
        radix = 16;
    }
    else if (end >= 3 && text[0] == '0' && LowerAscii(static_cast<unsigned char>(text[1])) == 'x')
    {
        begin = 2;
        radix = 16;
    }
    std::uint32_t value = 0;
    for (auto index = begin; index < end; ++index)
    {
        const auto byte = LowerAscii(static_cast<unsigned char>(text[index]));
        std::uint32_t digit;
        if (byte >= '0' && byte <= '9') digit = byte - '0';
        else if (radix == 16 && byte >= 'a' && byte <= 'f') digit = byte - 'a' + 10u;
        else throw MessageWindowConfigExpressionError(ErrorCode::InvalidNumber);
        value = value * radix + digit;
    }
    // 原空叶返回初始零；原 IMUL/ADD 仅保留低 32 位，不做 stoi 式范围拒绝。
    return value;
}

std::uint32_t Apply(char operation, std::uint32_t left, std::uint32_t right)
{
    if (operation == '+') return left + right;
    if (operation == '-') return left - right;
    if (operation == '*') return left * right;
    const auto dividend = std::bit_cast<std::int32_t>(left);
    const auto divisor = std::bit_cast<std::int32_t>(right);
    // 48E4DA/48E502 为 IDIV。现代实现显式抛错，绝不执行除零或 INT_MIN/-1 的 UB。
    if (divisor == 0) throw MessageWindowConfigExpressionError(ErrorCode::DivisionByZero);
    if (dividend == (std::numeric_limits<std::int32_t>::min)() && divisor == -1)
        throw MessageWindowConfigExpressionError(ErrorCode::DivisionOverflow);
    return static_cast<std::uint32_t>(operation == '/' ? dividend / divisor : dividend % divisor);
}
}

MessageWindowConfigExpressionError::MessageWindowConfigExpressionError(ErrorCode code)
    : std::runtime_error(ErrorMessage(code)), code_(code)
{
}

MessageWindowConfigExpressionErrorCode MessageWindowConfigExpressionError::Code() const noexcept
{
    return code_;
}

std::int32_t MessageWindowConfigExpressionParser::Evaluate(std::string_view expression)
{
    std::vector<std::size_t> postorder;
    auto nodes = BuildTree(expression, postorder);
    for (const auto index : postorder)
    {
        auto& node = nodes[index];
        node.value = node.operation == '\0' ? ParseNumber(node.text) :
            Apply(node.operation, nodes[node.left].value, nodes[node.right].value);
    }
    return std::bit_cast<std::int32_t>(nodes.front().value);
}
}
