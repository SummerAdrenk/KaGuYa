#pragma once

namespace KaGuYa::Graphics
{
// 原CString正文槽只占4字节；名字、全局错误与DX局部错误共用07FA0/07F40协议。
class NativeAnsiString
{
public:
    enum class Initialization { NilOnly, EmptyAssignment };
    explicit NativeAnsiString(Initialization initialization = Initialization::NilOnly);
    NativeAnsiString(const NativeAnsiString& prefix, const char* suffix);
    ~NativeAnsiString() noexcept(false);
    NativeAnsiString(const NativeAnsiString&) = delete;
    NativeAnsiString& operator=(const NativeAnsiString&) = delete;

    const char* GetString() const noexcept { return characters_; }
    int GetLength() const noexcept;
    bool IsEmpty() const noexcept { return GetLength() == 0; }
    const char* Assign(const char* source);
    const char* Empty();
    void Append(const char* source);
    void AppendChar(char character);
    // 手工临时槽可按原EH再次调用；不结束C++对象生命周期，不清死槽指针。
    void Retire();

private:
    void PrepareWrite(int length);
    void PrepareBuffer(int length);
    void Fork(int capacity);
    void Reallocate(int capacity);
    char* characters_;
};
static_assert(sizeof(NativeAnsiString) == 4);
}
