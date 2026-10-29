#pragma once

#include <cstdint>
#include <string>
#include <Runtime/RuntimeStringBorrow.h>

namespace KaGuYa::Start::Runtime
{
class IStartFileApi;
class IScrVmProgramRandSource;
class SerializeBuffer;
class SaveDataEnvironmentArchive;
struct SaveFileFormatProfile;
struct ScriptParamsNativeBody;
class ScriptDwordVectorStorage;
class IRuntimeStringManagerSource;

class ISaveFileContainerEnvironmentSource
{
public:
    virtual ~ISaveFileContainerEnvironmentSource() = default;
    virtual RuntimeStringBorrow CaptureDirectoryAt4180C0() = 0;
    virtual std::uint8_t CaptureHistoryFlagAt4C7F9B() = 0;
    virtual IRuntimeStringManagerSource& Strings() = 0;
};

// source只在入口捕获CParams；每次消费均显式传该接收者，不缓存可被重入覆盖的current。
class ISaveFileArchiveState
{
public:
    virtual ~ISaveFileArchiveState() = default;
    // 对应调用者取得原this；此处异常不属于4C7420/4C7B10内部catch的范围。
    [[nodiscard]] virtual ScriptParamsNativeBody& CaptureParamsAtEntry() = 0;

    // 4C756E..4C7576：打开及时间查询成功后、读取魔数之前即提交。
    virtual void SetLastWriteTimeAt00(ScriptParamsNativeBody& receiver, std::uint64_t value) = 0;
    // 同一接口供读写使用；读模式允许前面的字段已经提交后再抛出。
    // SerializeBuffer 原读槽不检查边界，实际 state 须明确处理非法正文，不能越界读取。
    virtual void SerializeSaveParamsAt48B720(ScriptParamsNativeBody& receiver, SerializeBuffer& archive) = 0;
    virtual void SerializeSaveParams2At48B6B0(ScriptParamsNativeBody& receiver, SerializeBuffer& archive) = 0;
    // 返回实际 +3C 向量；读取零长度第三块只 clear，不释放其容量。
    [[nodiscard]] virtual ScriptDwordVectorStorage& FormatHistoryAt3C(ScriptParamsNativeBody& receiver) = 0;
    // 4C7FCE传+60原地址，453A70在分配返回之后才读取DWORD。
    [[nodiscard]] virtual std::int32_t& CurrentFormatAt60(ScriptParamsNativeBody& receiver) = 0;
};

// 每次读写独立的栈适配器；只借用原this，不增持Ref，也不提前解析两个子正文。
class SaveFileArchiveOperation final
{
public:
    explicit SaveFileArchiveOperation(ISaveFileArchiveState& source)
        : source_(source), receiver_(source.CaptureParamsAtEntry()) {}
    void SetLastWriteTimeAt00(std::uint64_t value) { source_.SetLastWriteTimeAt00(receiver_, value); }
    void SerializeSaveParamsAt48B720(SerializeBuffer& archive)
        { source_.SerializeSaveParamsAt48B720(receiver_, archive); }
    void SerializeSaveParams2At48B6B0(SerializeBuffer& archive)
        { source_.SerializeSaveParams2At48B6B0(receiver_, archive); }
    [[nodiscard]] ScriptDwordVectorStorage& FormatHistoryAt3C()
        { return source_.FormatHistoryAt3C(receiver_); }
    [[nodiscard]] std::int32_t& CurrentFormatAt60() { return source_.CurrentFormatAt60(receiver_); }
private:
    ISaveFileArchiveState& source_;
    ScriptParamsNativeBody& receiver_;
};

class SaveFileContainerRuntime final
{
public:
    // 所有参数均借用，必须活过本对象。旧owned profile固定32；原生profile只借动态CString槽。
    // 随机源复用 VM/Transit 的同一 CRT 源，本类不 seed，也不创建私有随机序列。
    SaveFileContainerRuntime(IStartFileApi& files, ISaveFileArchiveState& state,
        const SaveDataEnvironmentArchive& save, RuntimeStringBorrow saveDirectoryCp932,
        IScrVmProgramRandSource& random, const SaveFileFormatProfile& profile);
    // 显式动态来源构造：来源自身借用，不在构造时解析Environment/目录/Save。
    SaveFileContainerRuntime(IStartFileApi& files, ISaveFileArchiveState& state,
        ISaveFileContainerEnvironmentSource& environment,
        IScrVmProgramRandSource& random, const SaveFileFormatProfile& profile);

    // 先Capture；其异常传给调用者。原两入口正文才catch-all -> false。
    // 保留已写文件/已读字段前缀，不作原子替换或回滚。
    [[nodiscard]] bool WriteAt4C7B10(const std::string& nameOrPathCp932);
    // 原factory/目录刷新直接传C字符串；不在进入容器catch前构造额外std::string实参。
    [[nodiscard]] bool WriteAt4C7B10(const char* nameOrPathCp932);
    // 保留完整DWORD；仅首块后的分支判低byte，随后同一参数槽复用为第二/三块长度。
    [[nodiscard]] bool ReadAt4C7420(const std::string& nameOrPathCp932, std::uint32_t mode);
    [[nodiscard]] bool ReadAt4C7420(const char* nameOrPathCp932, std::uint32_t mode);

private:
    IStartFileApi& files_;
    ISaveFileArchiveState& state_;
    const SaveDataEnvironmentArchive* save_;
    RuntimeStringBorrow directory_;
    IScrVmProgramRandSource& random_;
    const SaveFileFormatProfile& profile_;
    ISaveFileContainerEnvironmentSource* environment_ = nullptr;
};
}
