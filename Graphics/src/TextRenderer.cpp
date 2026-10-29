#include "TextRenderer.h"

#include "Surface.h"
#include "GraphicsState.h"
#include "DibBitmap.h"
#include "DibBitmapReference.h"
#include "FontWrap.h"
#include "GlyphLayout.h"
#include "TextRendererState.h"
#include "SurfaceTransfer.h"
#include <Logging/Logger.h>

#include <algorithm>
#include <bit>
#include <cstring>
#include <limits>
#include <mbctype.h>
#include <new>
#include <string>
#include <stdexcept>

namespace KaGuYa::Graphics
{
namespace
{

template<class T> void DestroyTextMember(T& value) { value.~T(); }

// 原1000F270在正常退当前槽之前撤其EH；这里只保护调用者指定的后继成员。
template<class T> class TextMemberRetirement final
{
public:
    explicit TextMemberRetirement(T& value) noexcept : value_(&value) {}
    TextMemberRetirement(const TextMemberRetirement&) = delete;
    TextMemberRetirement& operator=(const TextMemberRetirement&) = delete;
    ~TextMemberRetirement() noexcept(false) { if (value_) Retire(); }
    void Dismiss() noexcept { value_ = nullptr; }
    void Retire()
    {
        auto* value = value_;
        Dismiss();
        DestroyTextMember(*value);
    }
private:
    T* value_;
};

TextState& GetTextState()
{
    // 原1000DAA0在backend Initialize内建立CTextBlt；getter不惰性创建替代对象。
    auto* manager = GetSurfaceManager();
    auto* state = manager ? manager->text.get() : nullptr;
    if (!state)
    {
        Logging::Write(Logging::Level::Error, "文字manager尚未初始化，拒绝无效调用。");
        throw 1;
    }
    return *state;
}

bool IsDbcsLeadByte(const unsigned char value)
{
    // 原3B6BA/3BBBA使用CRT多字节表，不把它替换成Win32 ACP查询。
    return _ismbblead(value) != 0;
}

int AddCoordinate(const int left, const int right) noexcept
{ return std::bit_cast<int>(std::bit_cast<std::uint32_t>(left) + std::bit_cast<std::uint32_t>(right)); }

void ReleaseWorkDib(TextWorkDib& work)
{
    work.reference.RetireOwned();
}

bool CreateWorkDib(TextWorkDib& work, const int width, const int height)
{
    // 原94/B8/C4：先完成新body/Info构造，才退旧、发布control；Create失败保留新槽。
    work.reference.Publish(new DibBitmap);
    return work.reference->Create(width, height, 32, 1);
}

TextWorkDib& LogicalWork(TextState& state) noexcept
{ return state.options.vertical ? state.verticalWork : state.baseWork; }

bool PrepareBaseWorkDibs(TextState& state, const int width, const int height, const bool moveBase)
{
    // 1003B96B..BB78：94先发布；即使失败也先退旧A0，再尝试纵排A0。
    const bool baseReady = CreateWorkDib(state.baseWork, width, height);
    ReleaseWorkDib(state.verticalWork);
    bool verticalReady = true;
    if (state.options.vertical)
    {
        if (moveBase)
        {
            // 原空纵排会解引用空首字；保留现有明确拒绝域，不伪造首字宽度。
            if (state.glyphs.empty()) return false;
            state.baseX = AddCoordinate(state.baseX,
                std::bit_cast<int>(0u - std::bit_cast<std::uint32_t>(state.glyphs.front().extent.cx)));
        }
        verticalReady = CreateWorkDib(state.verticalWork, height, width);
    }
    return baseReady && verticalReady;
}

int PrepareEffectWorkDibs(TextState& state)
{
    const auto& work = LogicalWork(state);
    const int width = work.Width(), height = work.Height();
    // 1003B2D0后才释放AC；失败不触及后面的旧B8/C4，禁用效果也保留旧槽。
    ReleaseWorkDib(state.scaledWork);
    const auto renderMode = state.options.renderMode;
    if (renderMode == 2 &&
        !CreateWorkDib(state.scaledWork, std::bit_cast<int>(static_cast<std::uint32_t>(width) * 2u),
            std::bit_cast<int>(static_cast<std::uint32_t>(height) * 2u)))
    {
        return 3;
    }
    if (renderMode == 3 &&
        !CreateWorkDib(state.scaledWork, std::bit_cast<int>(static_cast<std::uint32_t>(width) * 4u),
            std::bit_cast<int>(static_cast<std::uint32_t>(height) * 4u)))
    {
        return 4;
    }
    if (state.options.shadowColor != CLR_INVALID &&
        !CreateWorkDib(state.shadowWork, width, height))
    {
        return 5;
    }
    if (state.options.outlineColor != CLR_INVALID)
    {
        // 原1003B4D3先发布C4，再在4D8读取半径；不能在构造/退旧之前缓存它。
        state.outlineWork.reference.Publish(new DibBitmap);
        const int radius = state.options.outlineWidth;
        if (!state.outlineWork.Native().Create(
            std::bit_cast<int>(static_cast<std::uint32_t>(width) + 2u * radius),
            std::bit_cast<int>(static_cast<std::uint32_t>(height) + 2u * radius), 32, 1))
        {
            return 5;
        }
    }
    return 0;
}

void AppendGlyph(TextState& state, HDC device, const std::uint16_t character,
    int& cursorX, int& cursorY, int& maximumWidth, int& maximumHeight)
{
    // 原3B76F先new14h，40AA0再发布局部Ref/control；control失败不补回收裸body。
    auto* body = new GlyphRecord;
    GlyphReference reference;
    reference.Publish(body);
    auto& glyph = *reference.get();
    glyph.character = character;
    glyph.x = cursorX;
    glyph.y = cursorY;
    const std::uint32_t string = character;
    if (!character || !GetTextExtentPoint32A(device, reinterpret_cast<const char*>(&string),
        (character & 0xFF00u) ? 2 : 1, &glyph.extent))
    {
        throw 3; // 1003B550：测量失败退出本次循环，已提交字形保留。
    }
    if (state.options.vertical) std::swap(glyph.extent.cx, glyph.extent.cy);
    maximumWidth = (std::max)(maximumWidth, static_cast<int>(glyph.extent.cx));
    maximumHeight = (std::max)(maximumHeight, static_cast<int>(glyph.extent.cy));
    if (state.options.vertical)
    {
        cursorY = AddCoordinate(cursorY, AddCoordinate(glyph.extent.cy, state.options.characterSpacing));
    }
    else
    {
        cursorX = AddCoordinate(cursorX, AddCoordinate(glyph.extent.cx, state.options.characterSpacing));
    }
    state.glyphs.Append(reference);
}

std::uint16_t ReadNameCharacter(const NativeTextName& name, int& index)
{
    // 原3BBA0先推进索引再校验；允许读终止NUL，每个字节重新读取CString指针/长度。
    const auto next = [&]()
    {
        const int current = index;
        index = AddCoordinate(index, 1);
        if (current < 0 || current > name.GetLength()) AtlThrow(E_INVALIDARG);
        return static_cast<unsigned char>(name.GetString()[current]);
    };
    const auto first = next();
    return static_cast<std::uint16_t>(first | (IsDbcsLeadByte(first) ? unsigned(next()) << 8 : 0u));
}
}


TextState::TextState()
{
    // 原1000F5F0..1000F66E只建空Ref/字形表，不在构造阶段分配五个工作DIB。
    ::new (&font) FontReference;
    TextMemberRetirement fontUnwind(font);
    ::new (&oversampledFont) FontReference;
    TextMemberRetirement oversampledUnwind(oversampledFont);
    ::new (&glyphs) GlyphLayout;
    TextMemberRetirement glyphsUnwind(glyphs);
    ::new (&baseWork) TextWorkDib;
    TextMemberRetirement baseUnwind(baseWork);
    ::new (&verticalWork) TextWorkDib;
    TextMemberRetirement verticalUnwind(verticalWork);
    ::new (&scaledWork) TextWorkDib;
    TextMemberRetirement scaledUnwind(scaledWork);
    ::new (&shadowWork) TextWorkDib;
    TextMemberRetirement shadowUnwind(shadowWork);
    ::new (&outlineWork) TextWorkDib;
    TextMemberRetirement outlineUnwind(outlineWork);

    // 原1000F684/694调用08080的空串分支；内层GetNil/赋空/展开全部由NativeTextName控制。
    ::new (&firstName) NativeTextName;
    TextMemberRetirement firstNameUnwind(firstName); // 1000F690：s8 -> 1004D32A。
    ::new (&secondName) NativeTextName;
    baseX = 0;
    baseY = 0;

    firstNameUnwind.Dismiss();
    outlineUnwind.Dismiss();
    shadowUnwind.Dismiss();
    scaledUnwind.Dismiss();
    verticalUnwind.Dismiss();
    baseUnwind.Dismiss();
    glyphsUnwind.Dismiss();
    oversampledUnwind.Dismiss();
    fontUnwind.Dismiss();
}

TextState::~TextState() noexcept(false)
{
    // 原10054ECC只有s0..6：字体5C/68、字形84、94/A0/AC/B8；没有C4/D0/D4。
    TextMemberRetirement fontUnwind(font);
    TextMemberRetirement oversampledUnwind(oversampledFont);
    TextMemberRetirement glyphsUnwind(glyphs);
    TextMemberRetirement baseUnwind(baseWork);
    TextMemberRetirement verticalUnwind(verticalWork);
    TextMemberRetirement scaledUnwind(scaledWork);
    TextMemberRetirement shadowUnwind(shadowWork);

    // 原1000F2A1..1000F328：任一名字或C4退役抛出，都从B8开始展开，不补退其余名字/C4。
    DestroyTextMember(secondName);
    DestroyTextMember(firstName);
    DestroyTextMember(outlineWork);
    shadowUnwind.Retire();      // 1000F328：先降到s5，再退B8。
    scaledUnwind.Retire();      // 1000F365：s4。
    verticalUnwind.Retire();    // 1000F3A2：s3。
    baseUnwind.Retire();        // 1000F3DF：s2。
    glyphsUnwind.Retire();      // 1000F41C：s1。
    oversampledUnwind.Retire(); // 1000F45D：s0。
    fontUnwind.Retire();        // 1000F48D：s-1。
}

bool ConfigureTextFont(const TextFontOptions& options)
{
    auto& state = GetTextState();
    // 10040F00比较原始quality和NUL结尾字体名，刻意不比较shadow X/Y。
    // 1003B03D只要求字体包装器存在，不要求其HFONT创建成功。
    const bool sameOptions = state.font &&
        std::memcmp(&state.options.logFont, &options.logFont, offsetof(LOGFONTA, lfFaceName)) == 0 &&
        lstrcmpA(state.options.logFont.lfFaceName, options.logFont.lfFaceName) == 0 &&
        state.options.characterSpacing == options.characterSpacing &&
        state.options.lineSpacing == options.lineSpacing &&
        state.options.renderMode == options.renderMode &&
        state.options.vertical == options.vertical &&
        state.options.textColor == options.textColor &&
        state.options.shadowColor == options.shadowColor &&
        state.options.outlineColor == options.outlineColor &&
        state.options.outlineWidth == options.outlineWidth;
    if (sameOptions)
    {
        return true;
    }
    // 1003B05E/06A先提交选项，再独立替换放大字体和基础字体；失败不回滚。
    state.options = options;
    state.options.logFont.lfQuality = NONANTIALIASED_QUALITY;
    const auto& normalized = state.options;
    state.oversampledFont.RetireOwned();
    const auto renderMode = state.options.renderMode;
    if (renderMode == 2 || renderMode == 3)
    {
        LOGFONTA oversampled = state.options.logFont;
        auto magnitude = std::bit_cast<std::uint32_t>(oversampled.lfHeight);
        if (oversampled.lfHeight < 0) magnitude = 0u - magnitude;
        auto scaled = std::bit_cast<std::int32_t>(magnitude << (renderMode - 1));
        if (scaled > 0) scaled = std::bit_cast<std::int32_t>(0u - std::bit_cast<std::uint32_t>(scaled));
        oversampled.lfHeight = scaled;
        oversampled.lfWidth = 0;
        // 原3B0F6在body分配之后读取vertical；new表达式先分配，再求构造实参。
        state.oversampledFont.Publish(new FontWrap(oversampled, RGB(255, 255, 255), state.options.vertical));
        (void)state.oversampledFont->Create(); // 原3B142忽略AL，但异常仍外传。
    }
    // 原基础字体先构造新body/face，再退旧和发布control；不缓存放大字体创建前的LOGFONT副本。
    state.font.Publish(new FontWrap(state.options.logFont, state.options.textColor, state.options.vertical));
    const bool created = state.font->Create();
    // 1003B010只更换字体/选项，不清既有glyphs或94..C4工作DIB。
    // Start 会在文字播放和页面切换中高频切换两套字体参数；成功记录只能放在 trace，
    // 否则同步日志 I/O 会直接改变原游戏的逐字与转场时序。
    std::string logFaceName;
    const std::string_view faceName(normalized.logFont.lfFaceName,
        strnlen_s(normalized.logFont.lfFaceName, LF_FACESIZE));
    if (Logging::TryCodePageToUtf8(faceName, 932, logFaceName))
    {
        Logging::WriteFormat(Logging::Level::Trace,
            "TextCreateFont: face=%s height=%ld spacing=%u/%u mode=%u vertical=%d "
            "text=%08lX shadow=%08lX offset=(%d,%d) outline=%08lX width=%u。",
            logFaceName.c_str(), normalized.logFont.lfHeight,
            static_cast<unsigned int>(normalized.characterSpacing),
            static_cast<unsigned int>(normalized.lineSpacing),
            static_cast<unsigned int>(normalized.renderMode), normalized.vertical ? 1 : 0,
            normalized.textColor, normalized.shadowColor,
            normalized.shadowOffsetX, normalized.shadowOffsetY, normalized.outlineColor,
            static_cast<unsigned int>(normalized.outlineWidth));
    }
    else
    {
        Logging::WriteFormat(Logging::Level::Error,
            "TextCreateFont: CP932 字体名转 UTF-8 失败，height=%ld mode=%u。",
            normalized.logFont.lfHeight, static_cast<unsigned int>(normalized.renderMode));
    }
    return created;
}

int SetTextBasePosition(const int x, const int y)
{
    auto& state = GetTextState();
    state.baseX = x;
    state.baseY = y;
    return static_cast<int>(reinterpret_cast<std::intptr_t>(&state));
}

const char* SetTextName(const char* name, const bool second)
{
    auto& state = GetTextState();
    auto& target = second ? state.secondName : state.firstName;
    return target.Assign(name);
}

bool InitializeTextLayout(const char* const text, const int baseX, const int baseY)
{
    auto& state = GetTextState();
    if (!state.font)
    {
        // 1003B210：字体对象不存在时抛出整数 1。
        Logging::Write(Logging::Level::Error, "TextInitialize: 字体尚未创建，抛出错误码 1。");
        throw 1;
    }
    // 原1003B267/26A在测量前提交基点；无字体的失败则不写入。
    state.baseX = baseX;
    state.baseY = baseY;
    HDC device = nullptr;
    HGDIOBJ previous = nullptr;
    int maximumWorkWidth = 0;
    int maximumWorkHeight = 0;
    bool success = true;
    try
    {
        device = CreateCompatibleDC(nullptr);
        if (device == nullptr) throw 1;
        previous = SelectObject(device, state.font->handle);
        if (previous == nullptr) throw 2;
        // 1003B5FD在SelectObject成功之后才清旧glyphs；DC/font选择失败保留旧布局。
        state.glyphs.clear();
        int cursorX = 0;
        int cursorY = 0;
        const int length = lstrlenA(text);
        int firstNameIndex = 0, secondNameIndex = 0;
        for (int index = 0; index < length;)
        {
            const int sourceIndex = index;
            const auto first = static_cast<unsigned char>(text[index]);
            index = AddCoordinate(index, 1);
            if (first == '\r') continue;
            if (first == '\n')
            {
                auto magnitude = std::bit_cast<std::uint32_t>(state.options.logFont.lfHeight);
                if (state.options.logFont.lfHeight < 0) magnitude = 0u - magnitude;
                const int lineAdvance = std::bit_cast<int>(magnitude + state.options.lineSpacing);
                if (state.options.vertical)
                {
                    cursorY = 0;
                    cursorX = AddCoordinate(cursorX, std::bit_cast<int>(0u - std::bit_cast<std::uint32_t>(lineAdvance)));
                }
                else
                {
                    cursorX = 0;
                    cursorY = AddCoordinate(cursorY, lineAdvance);
                }
                continue;
            }

            std::uint16_t character = first;
            if (IsDbcsLeadByte(first))
            {
                character |= static_cast<std::uint16_t>(static_cast<unsigned char>(text[index])) << 8;
                index = AddCoordinate(index, 1);
                if (character == 0x40F0 || character == 0x41F0)
                {
                    const auto& name = character == 0x40F0 ? state.firstName : state.secondName;
                    auto& nameIndex = character == 0x40F0 ? firstNameIndex : secondNameIndex;
                    if (nameIndex >= name.GetLength()) { nameIndex = 0; continue; }
                    character = ReadNameCharacter(name, nameIndex);
                    index = sourceIndex; // 原3B71D/765逐字重读占位符及名字，不缓存整段string_view。
                }
            }
            AppendGlyph(state, device, character, cursorX, cursorY,
                maximumWorkWidth, maximumWorkHeight);
        }
    }
    catch (...)
    {
        // 原1003B934仅置失败标志；保留已提交glyphs和当前最大尺寸，再进入DIB尾部。
        success = false;
    }
    if (device != nullptr)
    {
        if (previous != nullptr) SelectObject(device, previous);
        DeleteDC(device);
    }

    const bool baseReady = PrepareBaseWorkDibs(state, maximumWorkWidth, maximumWorkHeight, true);
    if (!success || !baseReady)
    {
        Logging::Write(Logging::Level::Error, "TextInitialize: 字形测量或基础DIB失败，内部错误码2，已提交布局保留。");
        throw 2;
    }
    const int workDibError = PrepareEffectWorkDibs(state);
    if (workDibError != 0)
    {
        Logging::WriteFormat(Logging::Level::Error,
            "TextInitialize: 创建文字工作 DIB 失败，抛出错误码 %d。", workDibError);
        throw workDibError;
    }
    Logging::WriteFormat(Logging::Level::Trace, "TextInitialize: glyphs=%zu base=(%d,%d)。",
        static_cast<std::size_t>(state.glyphs.size()), baseX, baseY);
    return true;
}

int GetTextGlyphCount()
{
    const auto size = GetTextState().glyphs.size();
    return size > static_cast<std::size_t>(std::numeric_limits<int>::max()) ?
        std::numeric_limits<int>::max() : static_cast<int>(size);
}

RECT GetTextBounds()
{
    const auto& state = GetTextState();
    RECT bounds{state.baseX, state.baseY, state.baseX, state.baseY};
    bool first = true;
    for (std::uint32_t index = 0; index < state.glyphs.size(); ++index)
    {
        const auto& glyph = state.glyphs[index];
        RECT current{state.baseX + glyph.x, state.baseY + glyph.y,
            state.baseX + glyph.x + glyph.extent.cx, state.baseY + glyph.y + glyph.extent.cy};
        if (first)
        {
            bounds = current;
            first = false;
        }
        else
        {
            UnionRect(&bounds, &bounds, &current);
        }
    }
    return bounds;
}

bool DrawTextGlyph(SurfaceInfo& destination, const unsigned int index, const bool blend, RECT* const drawnRect)
{
    auto& state = GetTextState();
    if (index >= state.glyphs.size())
    {
        return false;
    }
    if (!RenderTextGlyphWork(state, state.glyphs[index], state.options.renderMode, state.options.vertical))
    {
        return false;
    }
    // 原C25D以后才读取点；提交返回值不改变单字AL。矩形尺寸在提交后重新解析。
    const int x = AddCoordinate(state.baseX, state.glyphs[index].x);
    const int y = AddCoordinate(state.baseY, state.glyphs[index].y);
    (void)SubmitTextSurface(destination, state.baseWork.Info(), x, y, blend);
    if (drawnRect != nullptr)
    {
        const auto& glyph = state.glyphs[index];
        *drawnRect = RECT{x, y, AddCoordinate(x, glyph.extent.cx), AddCoordinate(y, glyph.extent.cy)};
    }
    return true;
}

bool DrawAllText(SurfaceInfo& destination, const bool blend, RECT* const drawnRect)
{
    auto& state = GetTextState();
    // 原1003C7E7在遍历前拒绝未知绘制模式；字体创建和测量并不因此拒绝该BYTE。
    const auto mode = state.options.renderMode;
    if (mode < 1 || mode > 3) throw 0;
    // 原C41E只分派一次mode；每字读取vertical，保持原Ref游标而非重新取begin+index。
    for (auto* slot = state.glyphs.BeginReferences(); slot != state.glyphs.EndReferences(); ++slot)
    {
        if (!RenderTextGlyphWork(state, *slot->get(), mode, state.options.vertical))
        {
            // 1003BD20/1003C3E0 在首个失败字形处立即抛出当前渲染模式。
            throw static_cast<int>(mode);
        }
        const auto& glyph = *slot->get();
        (void)SubmitTextSurface(destination, state.baseWork.Info(),
            AddCoordinate(state.baseX, glyph.x), AddCoordinate(state.baseY, glyph.y), blend);
    }
    if (drawnRect != nullptr)
    {
        *drawnRect = GetTextBounds();
    }
    return true;
}

void CreateCharacterSurfaceInto(SurfaceReference& surface, const std::uint16_t character)
{
    // 原1003CAE5已置返回对象bit；catch显式退休不撤s0，正常DC尾返回后才撤本保护。
    TextMemberRetirement outputUnwind(surface);
    auto* manager = GetSurfaceManager();
    auto* current = manager ? manager->text.get() : nullptr;
    if (!current)
    {
        Logging::Write(Logging::Level::Error, "CreateMojiSurface: 文字manager尚未初始化。");
        outputUnwind.Dismiss();
        return;
    }
    auto& state = *current;
    if (!state.font)
    {
        // 原1003CB33会空解址；保留明确拒绝域，不把无包装器与HFONT创建失败混同。
        Logging::Write(Logging::Level::Error, "CreateMojiSurface: 字体包装器尚未建立。");
        outputUnwind.Dismiss();
        return;
    }
    HDC device = nullptr;
    HGDIOBJ previous = nullptr;
    const auto closeMeasurement = [&]()
    {
        if (device != nullptr)
        {
            if (previous != nullptr) SelectObject(device, previous);
            DeleteDC(device);
        }
    };
    try
    {
        // 1003CAEE..CFAF：测量DC一直持有到创建/渲染及catch之后，不提前清理。
        device = CreateCompatibleDC(nullptr);
        if (device == nullptr) throw 1;
        previous = SelectObject(device, state.font->handle);
        if (previous == nullptr) throw 2;
        GlyphRecord glyph{};
        glyph.character = character;
        glyph.x = 0;
        glyph.y = 0; // 原1003CB63/66清单字坐标；GlyphRecord的通用构造只清WORD。
        const std::uint32_t string = character;
        if (character == 0 || !GetTextExtentPoint32A(device, reinterpret_cast<const char*>(&string),
            (character & 0xFF00u) ? 2 : 1, &glyph.extent)) throw 3;
        if (state.options.vertical) std::swap(glyph.extent.cx, glyph.extent.cy);

        // 1003CBF0/CC21：94失败立即进入catch，不能套用Initialize继续替换A0的尾部。
        if (!CreateWorkDib(state.baseWork, glyph.extent.cx, glyph.extent.cy)) throw 4;
        ReleaseWorkDib(state.verticalWork);
        if (state.options.vertical && !CreateWorkDib(state.verticalWork, glyph.extent.cy, glyph.extent.cx)) throw 4;

        // 原1003CCB8..CCDD：new只保护body构造，完成后直接向调用者Ref发布，再Create。
        // 不用AllocateSurface值返回：control抛出必须在这个输出槽留下B!=0/C=0供catch观察。
        surface.Publish(new Surface);
        // 1003CCFF先建格式12输出；1003CD1C之后才处理AC，B8/C4在本入口完全不重建。
        if (!InitializeSurfacePixels(*surface->info, glyph.extent.cx, glyph.extent.cy, 1, 1)) throw 5;
        if (state.options.renderMode == 1)
        {
            ReleaseWorkDib(state.scaledWork);
        }
        else if (state.options.renderMode == 2 || state.options.renderMode == 3)
        {
            const auto shift = state.options.renderMode - 1;
            const auto width = std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(glyph.extent.cx) << shift);
            const auto height = std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(glyph.extent.cy) << shift);
            // 1003CD92/CE7A比较旋转后的单字尺寸；同尺寸保留AC，未知mode也不清旧AC。
            // 原CD35/CE1D按base判空；尺寸变化只重建同一body的GDI，不替换Ref或补control。
            const bool newScaled = !state.scaledWork.Present();
            if (newScaled) state.scaledWork.reference.Publish(new DibBitmap);
            if ((newScaled || state.scaledWork.Width() != width || state.scaledWork.Height() != height) &&
                !state.scaledWork.Native().Create(width, height, 32, 1)) throw 5;
        }
        else throw 6;
        if (!RenderTextGlyphWork(state, glyph, state.options.renderMode, state.options.vertical)) throw 6;
        // 原CF73/CF78：仅Moji检查提交状态；失败退休输出，不退休工作DIB。
        if (SubmitTextSurface(*surface->info, state.baseWork.Info(), 0, 0, false) != 0) throw 6;
        // 原CF9F正常进入时仍为state1；首轮DC尾异常会到CF91，再从catch回到同一尾部。
        closeMeasurement();
        outputUnwind.Dismiss();
        return;
    }
    catch (...)
    {
        // 原1003CF94显式退输出，C=0不清裸B；工作94/A0/AC不回滚。
        // 若此处payload抛出，outputUnwind对应1004CF6C/73仍再退一次，不在这里撤保护。
        surface.RetireOwned();
    }
    // catch返回后的尾部不再被同一catch保护；失败外传并只展开返回Ref的s0。
    closeMeasurement();
    outputUnwind.Dismiss();
}

TextRendererOwner CreateTextRenderer()
{
    return TextRendererOwner(new TextState);
}

void TextStateDeleter::operator()(TextState* value) const
{
    // 原1000F4D6/4E2：手工析构全部正常返回后才delete raw T；抛出不得补删正文。
    NativePayloadDelete<TextState>{}(value);
}

void ResetTextRenderer()
{
    if (auto* manager = GetSurfaceManager()) manager->text.reset();
}
}
