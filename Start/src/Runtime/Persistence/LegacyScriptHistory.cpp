#include <Runtime/LegacyScriptHistory.h>
#include <Runtime/FileStreamRuntime.h>
#include <Runtime/RuntimeCStringResult.h>
#include <Logging/Logger.h>

#include <ImageHlp.h>
#include <bit>
#include <cstring>
#include <mbstring.h>
#include <memory>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char warning[] =
    "\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x97\x9A\x97\xF0\x82\xCC"
    "\x8D\xC4\x8D\x5C\x92\x7A\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC"
    "\x82\xB5\x82\xBD\x81\x49\x0A\x0A\x81\xA6\x90\x56\x8B\x4B\x82\xC9"
    "\x83\x47\x83\x89\x81\x5B\x88\xC8\x8D\x7E\x82\xCC\x83\x81\x83\x62"
    "\x83\x5A\x81\x5B\x83\x57\x82\xC5\x8D\xEC\x90\xAC\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char nameFormat[] = "N \x81\x79%s\x81\x7A\r\n"; // 530FD8 / 530FE4

[[noreturn]] void InvalidContext(const char* text)
{
    Logging::Write(Logging::Level::Error, text);
    throw std::logic_error(text);
}

class TempApi final : public ILegacyScriptHistoryTempApi
{
public:
    BOOL MakeDirectoryPath(const char* path) override { return ::MakeSureDirectoryPathExists(path); }
    UINT CreateName(const char* path, const char* prefix, char* output) override
        { return ::GetTempFileNameA(path, prefix, 0, output); }
    BOOL Delete(const char* path) override { return ::DeleteFileA(path); }
};

using ContextControl = EnvironmentObjectControl<LegacyScriptHistoryContext>;
const void* Table(const void* object) noexcept
{
    const void* table;
    std::memcpy(&table, object, sizeof(table));
    return table;
}

// 41EE30 -> 421730使用_mbschr多字节查找，不换Unicode拆分或逐byte裁剪。
int FindLf(const RuntimeCString& text)
{
    if (text.IsEmpty()) return -1;
    auto* found = _mbschr(reinterpret_cast<const unsigned char*>(text.GetString()), '\n');
    return found ? static_cast<int>(reinterpret_cast<const char*>(found) - text.GetString()) : -1;
}
void Append(RuntimeCString& to, const RuntimeCString& from)
{
    // 41ED60 -> 427E40 -> 429F80: declared length limited by first NUL.
    AppendRuntimeCStringAt413550(to, from.GetString(), from.GetLength());
}
void AppendLiteral(RuntimeCString& to, const char* text)
{
    AppendRuntimeCStringAt413550(to, text, static_cast<int>(std::strlen(text)));
}

struct Selection final
{
    YtlReferenceSlotRuntime name{}, voices{}, body{};
    ~Selection() noexcept(false)
    {
        // 483320: body, voices, name. The remaining members unwind on a normal Free failure.
        struct Tail final
        {
            Selection& self;
            ~Tail() noexcept(false)
            {
                struct Name final { Selection& self; ~Name() noexcept(false) { ReleaseYtlReference(self.name); } } name{self};
                ReleaseYtlReference(self.voices);
            }
        } tail{*this};
        ReleaseYtlReference(body);
    }
    MessageFormatSelectionRawTarget Target() { return {name, voices, body}; }
};

// The shared stream's Write/Tell/Open leaves match 41BB10/41BE00/41BFE0.
// Normal close is outside its noexcept destructor so 4CD230 can catch a close exception.
class HistoryStream final
{
public:
    explicit HistoryStream(IStartFileApi& files) noexcept : files_(files) {}
    ~HistoryStream() noexcept(false)
    {
        if (armed_)
        {
            stream_.value.CloseWithApi(files_);
            DestroyFileStreamAt40ED70(&stream_.value, files_);
        }
    }
    FileStreamRuntime& Get() noexcept { return stream_.value; }
    int Open(const char* path, bool constructing)
    {
        SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
        // 41BA40(0x89): GENERIC_WRITE, no sharing, CREATE_ALWAYS, inheritable, NORMAL.
        // 0x80 is not DELETE_ON_CLOSE or a sharing bit.
        if (constructing) armed_ = false;
        const int result = stream_.value.OpenWithApi(
            {path, GENERIC_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, &security}, files_);
        armed_ = true;
        return constructing ? stream_.value.IsOpenAtVslot08() : result;
    }
private:
    IStartFileApi& files_;
    union Storage
    {
        Storage() noexcept : value() {}
        ~Storage() noexcept {}
        FileStreamRuntime value;
    } stream_;
    bool armed_ = false;
};
}

class LegacyScriptHistoryTempFile final
{
public:
    LegacyScriptHistoryTempFile(const char* directory, IStartFileApi& files,
        ILegacyScriptHistoryTempApi& api, IRuntimeStringManagerSource& strings)
        : api_(api), path_([&](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, strings); }),
          base_([&](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, strings); })
    {
        Clear(); // 4BBEE0 -> 4BBF70, even on a fresh object.
        RuntimeCStringResult dir([&](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, strings); });
        // 4CD090 ALWAYS passes a CString chars pointer. The generic null-directory
        // GetTempPath/GetModuleFileName branch of 4BBFC0 is unreachable here.
        if (!directory) InvalidContext("Baku history directory service supplied a null CString");
        SetRuntimeCStringAt437680(dir.Value(), directory, static_cast<int>(std::strlen(directory)));
        if (dir.Value().IsEmpty()) throw ATL::CAtlException(E_INVALIDARG); // 41F160(-1)
        if (dir.Value().GetString()[dir.Value().GetLength() - 1] != '\\') AppendLiteral(dir.Value(), "\\");
        if (files.GetAttributes(dir.Value().GetString()) == INVALID_FILE_ATTRIBUTES &&
            !api_.MakeDirectoryPath(dir.Value().GetString())) return;
        Clear(); // 4BC189, before GetTempFileNameA.
        PrepareRuntimeCStringWriteAt42D590(path_.Value(), MAX_PATH + 1);
        // 4BC1C3 saves the failure byte but never branches on it; consume the actual buffer.
        static_cast<void>(api_.CreateName(dir.Value().GetString(), "MES",
            const_cast<char*>(path_.Value().GetString())));
        ReleaseRuntimeCStringBufferAt486B10(path_.Value());
        if (!path_.Value().IsEmpty())
        {
            const auto* slash = _mbsrchr(reinterpret_cast<const unsigned char*>(path_.Value().GetString()), '\\');
            const char* leaf = slash ? reinterpret_cast<const char*>(slash) + 1 : path_.Value().GetString();
            if (*leaf) SetRuntimeCStringAt437680(base_.Value(), leaf, static_cast<int>(std::strlen(leaf)));
        }
    }
    ~LegacyScriptHistoryTempFile() noexcept(false) { Clear(); } // base then path members
    const char* Path() const noexcept { return path_.Value().IsEmpty() ? nullptr : path_.Value().GetString(); }
    const char* BaseName() const noexcept { return base_.Value().GetString(); }
private:
    void Clear()
    {
        if (!path_.Value().IsEmpty()) static_cast<void>(api_.Delete(path_.Value().GetString()));
        SetRuntimeCStringAt437680(path_.Value(), "", 0);
        SetRuntimeCStringAt437680(base_.Value(), "", 0);
    }
    ILegacyScriptHistoryTempApi& api_;
    RuntimeCStringResult path_, base_;
};

ILegacyScriptHistoryTempApi& DefaultLegacyScriptHistoryTempApi() noexcept
{
    static TempApi api;
    return api;
}
const char* BakuLegacyScriptHistoryWarning() noexcept { return warning; }

LegacyScriptHistoryContext::LegacyScriptHistoryContext(ScriptParamsNativeBody& owner,
    IStartFileApi& files, ILegacyScriptHistoryTempApi& temp, IRuntimeStringManagerSource& strings) noexcept
    : owner_(owner), files_(files), tempApi_(temp), strings_(strings) {}
LegacyScriptHistoryContext::~LegacyScriptHistoryContext() noexcept(false) { delete temp_; }
const char* LegacyScriptHistoryContext::Path() const noexcept { return temp_ ? temp_->Path() : nullptr; }
const char* LegacyScriptHistoryContext::BaseName() const noexcept { return temp_ ? temp_->BaseName() : nullptr; }

LegacyScriptHistoryContext* FindBakuLegacyScriptHistory(ScriptParamsNativeBody& params)
{
    auto& slot = params.referenceAt54;
    if (!slot.baseAt00 && !slot.controlAt08) return nullptr;
    const ContextControl expected(nullptr);
    if (!slot.controlAt08 || Table(slot.controlAt08) != Table(&expected) || slot.biasAt04 != 0)
        InvalidContext("Baku history: CParams owning slot is occupied by another resource");
    auto* context = ResolveEnvironmentObjectReference<LegacyScriptHistoryContext>(slot);
    if (!context || &context->owner_ != &params)
        InvalidContext("Baku history: context belongs to a different CParams");
    return context;
}

LegacyScriptHistoryContext& InstallBakuLegacyScriptHistory(ScriptParamsNativeBody& params,
    IStartFileApi& files, ILegacyScriptHistoryTempApi& temp, IRuntimeStringManagerSource& strings)
{
    if (auto* context = FindBakuLegacyScriptHistory(params))
    {
        if (&context->files_ != &files || &context->tempApi_ != &temp || &context->strings_ != &strings)
            InvalidContext("Baku history: cannot replace dependencies of an owned context");
        return *context;
    }
    // 统一模型的显式语义挂接，不将Baku旧偏移套到当前CParams。
    auto context = std::unique_ptr<LegacyScriptHistoryContext>(new LegacyScriptHistoryContext(params, files, temp, strings));
    auto control = std::make_unique<ContextControl>(context.get());
    control->strideAt0C = sizeof(LegacyScriptHistoryContext);
    control->countAt10 = 1;
    auto* result = context.release();
    params.referenceAt54 = {reinterpret_cast<std::uintptr_t>(result), 0, control.release()};
    return *result;
}

void ResetBakuLegacyScriptHistoryAt4CD090(ScriptParamsNativeBody& params, ILegacyScriptHistorySource& source)
{
    auto* context = FindBakuLegacyScriptHistory(params);
    if (!context) InvalidContext("Baku history: install the CParams-owned context before reset");
    RuntimeCStringResult directory([&](RuntimeCString* out) { source.ConstructDirectoryAt4C7340(out); });
    auto* next = new LegacyScriptHistoryTempFile(directory.Value().GetString(), context->files_,
        context->tempApi_, context->strings_);
    // 4BD210在发布前退旧拥有者；新构造失败时旧文件与索引保持不变。
    delete std::exchange(context->temp_, nullptr);
    context->temp_ = next;
    context->offsets_.clear(); // 4CD12D, only after the new owner has been published.
}

std::uint8_t WriteBakuLegacyScriptHistoryAt4CD400(ScriptParamsNativeBody& params,
    std::int32_t format, FileStreamRuntime& file, ILegacyScriptHistorySource& source) noexcept
{
    try
    {
        auto* context = FindBakuLegacyScriptHistory(params);
        if (!context) InvalidContext("Baku history: missing owned context at 4CD400");
        Selection selection;
        const std::uint32_t variant = source.NamesMatchDefaultsAt41B450() ? 0u : 1u;
        if (source.ReadFormatAt41B4C0(format, selection.Target(), variant, false) < 0) throw 1;
        if (!selection.body.baseAt00) throw 1;
        auto index = [&] { context->offsets_.push_back(static_cast<std::uint32_t>(file.TellWithApi(context->files_))); };
        auto write = [&](const RuntimeCString& text)
        {
            if (!file.WriteWithApi(text.GetString(), static_cast<DWORD>(text.GetLength()), context->files_)) throw 0;
        };
        index(); // 4CD526: commit LOW DWORD even when Tell returns FFFFFFFFFFFFFFFF.
        {
            RuntimeCStringResult voices([&](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, context->strings_); });
            SetRuntimeCStringAt437680(voices.Value(), "V ", 2);
            if (selection.voices.baseAt00)
            {
                // Borrow the array representation without incrementing its owning count.
                for (std::int32_t i = 0; ; ++i)
                {
                    // 4CD59C calls 41FC00 on EVERY iteration, after the previous CString append.
                    const auto* control = static_cast<EnvironmentObjectReferenceControl*>(selection.voices.controlAt08);
                    const auto count = control ? std::bit_cast<std::int32_t>(control->countAt10) : 0;
                    if (i >= count) break;
                    if (i) AppendLiteral(voices.Value(), ",");
                    auto slot = selection.voices;
                    slot.biasAt04 += static_cast<std::uint32_t>(i);
                    auto* voice = ResolveEnvironmentObjectReference<RuntimeCString>(slot);
                    if (!voice) InvalidContext("Baku history: null voice element");
                    Append(voices.Value(), *voice);
                }
            }
            AppendLiteral(voices.Value(), "\r\n");
            write(voices.Value());
        }
        if (selection.name.baseAt00)
        {
            index();
            RuntimeCStringResult name([&](RuntimeCString* out)
                { ::new (out) RuntimeCString(ResolveEnvironmentObjectReference<MessageTextCommand>(selection.name)->GetTextAt47D550()); });
            RuntimeCStringResult record([&](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, context->strings_); });
            if (FindLf(name.Value()) < 0)
                FormatRuntimeCStringAt417130(record.Value(), nameFormat, name.Value().GetString());
            else
            {
                RuntimeCStringResult remaining([&](RuntimeCString* out)
                    { ConstructRuntimeCStringLeftAt419D00(out, name.Value(), name.Value().GetLength(), context->strings_); });
                RuntimeCStringResult joined([&](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, context->strings_); });
                // 530F19/1A/1B are all empty strings: remove LF, retain CR, no separator.
                while (std::strcmp(remaining.Value().GetString(), "") != 0)
                {
                    const int lf = FindLf(remaining.Value());
                    if (lf < 0)
                    {
                        Append(joined.Value(), remaining.Value());
                        SetRuntimeCStringAt437680(remaining.Value(), "", 0);
                    }
                    else
                    {
                        {
                            RuntimeCStringResult part([&](RuntimeCString* out)
                                { ConstructRuntimeCStringLeftAt419D00(out, remaining.Value(), lf, context->strings_); });
                            Append(joined.Value(), part.Value());
                        }
                        RuntimeCStringResult tail([&](RuntimeCString* out)
                            { ConstructRuntimeCStringMidAt420C80(out, remaining.Value(), lf + 1,
                                remaining.Value().GetLength() - lf - 1, context->strings_); });
                        AssignRuntimeCStringAt410220(remaining.Value(), tail.Value());
                    }
                }
                FormatRuntimeCStringAt417130(record.Value(), nameFormat, joined.Value().GetString());
            }
            write(record.Value());
        }
        RuntimeCStringResult body([&](RuntimeCString* out)
            { ::new (out) RuntimeCString(ResolveEnvironmentObjectReference<MessageTextCommand>(selection.body)->GetTextAt47D550()); });
        // Original GetAt(length-1) rejects empty text AFTER V and optional N commits.
        if (body.Value().IsEmpty()) throw ATL::CAtlException(E_INVALIDARG);
        if (body.Value().GetString()[body.Value().GetLength() - 1] != '\n') AppendRuntimeCStringLineFeedAt479F48(body.Value());
        while (!body.Value().IsEmpty())
        {
            const int lf = FindLf(body.Value());
            if (lf == 0) break; // 4CD925: an empty line terminates, it is not skipped.
            index();
            RuntimeCStringResult record([&](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, context->strings_); });
            {
                RuntimeCStringResult part([&](RuntimeCString* out)
                    { ConstructRuntimeCStringLeftAt419D00(out, body.Value(), lf, context->strings_); });
                FormatRuntimeCStringAt417130(record.Value(), "M %s\r\n", part.Value().GetString());
            }
            write(record.Value());
            if (lf >= body.Value().GetLength()) break;
            RuntimeCStringResult tail([&](RuntimeCString* out)
                { ConstructRuntimeCStringMidAt420C80(out, body.Value(), lf + 1,
                    body.Value().GetLength() - lf - 1, context->strings_); });
            AssignRuntimeCStringAt410220(body.Value(), tail.Value());
        }
        return 1;
    }
    catch (...)
    {
        Logging::Write(Logging::Level::Error, "Baku history record failed at 4CD400; partial file/index retained");
        return 0;
    }
}

std::uint8_t RebuildBakuLegacyScriptHistoryAt4CD230(ScriptParamsNativeBody& params,
    ILegacyScriptHistorySource& source) noexcept
{
    try
    {
        ResetBakuLegacyScriptHistoryAt4CD090(params, source);
        auto& context = *FindBakuLegacyScriptHistory(params);
        HistoryStream stream(context.files_);
        if (!stream.Open(context.Path(), true)) throw 1;
        bool failed = false;
        // 只借唯一已序列化编号表，不用成功项重写原历史序列。
        for (std::uint32_t i = 0; i < params.historyAt3C.Size(); ++i)
        {
            const auto format = std::bit_cast<std::int32_t>(params.historyAt3C.storage.begin[i]);
            if (!WriteBakuLegacyScriptHistoryAt4CD400(params, format, stream.Get(), source))
            {
                stream.Get().CloseWithApi(context.files_);
                ResetBakuLegacyScriptHistoryAt4CD090(params, source);
                if (!stream.Open(context.Path(), false)) throw 1;
                failed = true;
            }
        }
        if (failed) source.WarnAt416B00(warning, 0x40);
        return 1; // normal stream destruction is still inside this try, including after warning
    }
    catch (...)
    {
        Logging::Write(Logging::Level::Error, "Baku history rebuild failed at 4CD230");
        return 0;
    }
}
}
