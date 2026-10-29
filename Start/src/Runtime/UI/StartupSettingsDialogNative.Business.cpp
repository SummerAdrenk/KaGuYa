#include <Runtime/StartupSettingsDialogNative.h>

#include <Runtime/FileStatusRuntime.h>
#include <Runtime/NativeStartupSettingsDialogRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/GameBuildProfile.h>

#include <bit>
#include <cstddef>
#include <cstring>
#include <initializer_list>

namespace KaGuYa::Start::Runtime
{
static_assert(sizeof(void*) == 4 && sizeof(BOOL) == 4 && sizeof(LOGFONTA) == 0x3C);
namespace
{
template<class Function>
Function ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    const std::byte* table;
    std::memcpy(&table, manager, sizeof(table));
    Function function;
    std::memcpy(&function, table + offset, sizeof(function));
    return function;
}

ATL::CStringData* Header(const char* chars) noexcept
{
    return reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(chars) - sizeof(ATL::CStringData));
}

union TextSlot
{
    TextSlot() noexcept {}
    ~TextSlot() noexcept {}
    RuntimeCString value;
};

struct PathUnwind final
{
    RuntimeCString& value;
    bool active = true;
    // 5ECC88：state0/bit1与state1均直接退到-1，只清当前槽一次；展开Free再抛则终止。
    ~PathUnwind() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&value); }
};

void MarkWritten(StartupSettingsDialogNativeServices& services, BYTE bit) noexcept
{
    // 仅发布现代只读Values资格；不读取标记来决定原业务字段是否可读。
    if (services.writtenFields) *services.writtenFields |= bit;
}

std::uint32_t ReadRenderMode(const SaveDataEnvironmentArchive& archive) noexcept
{
    std::uint32_t mode;
    std::memcpy(&mode, archive.bytesAt04.data() + 4, sizeof(mode));
    return mode;
}

void WriteRenderMode(SaveDataEnvironmentArchive& archive, std::uint32_t mode) noexcept
{
    std::memcpy(archive.bytesAt04.data() + 4, &mode, sizeof(mode));
}
}

LRESULT InitializeStartupSettingsDialogNativeAt4837E0(StartupSettingsDialogNativeBody28h& body,
    StartupSettingsDialogNativeServices& services)
{
    auto& api = services.api;
    // 保留原各指令的现场读取，包括嵌套消息改变的HWND/字段，不缓存整个body。
    const volatile auto& current = body;
    body.dontShowAt20 = 0; // 483817
    MarkWritten(services, 0x01);
    const auto mode = ReadRenderMode(services.source.SaveDataAtSlot1());
    body.renderAt24 = mode == 1u ? 0 : mode == 2u ? 2 : 1; // 483866..48388C
    MarkWritten(services, 0x04);

    bool openGl;
    {
        // 48388C..4838BC：来源执行413BE0后只借用固定的+18槽，不复制或重新捕获Root。
        const RuntimeCString& library = services.source.LibraryPathAt18After413BE0();
        using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
        auto* manager = Header(library.GetString())->pStringMgr;
        if (manager) manager = ManagerSlot<Clone>(manager, 0x10)(manager);
        if (!manager)
        {
            // 4838D8..4838E8：原全局来源后仍须Clone；不以414540复制数据冒充此虚槽。
            auto* global = services.strings.CaptureAt417360();
            manager = ManagerSlot<Clone>(global, 0x10)(global);
            if (!manager) throw ATL::CAtlException(E_FAIL); // 483B59 -> 413880
        }
        using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
        auto* nil = ManagerSlot<GetNil>(manager, 0x0C)(manager); // 4838F0，可抛原始槽
        TextSlot path;
        const auto nilChars = reinterpret_cast<std::uintptr_t>(nil) + sizeof(ATL::CStringData);
        std::memcpy(&path.value, &nilChars, sizeof(nilChars));
        PathUnwind unwind{path.value}; // GetNil成功才有state0/bit1资格。

        // 483900：Clone/GetNil之后重读同一个路径槽的chars及其header长度。
        const char* first = library.GetString();
        static constexpr char suffix[] = "Lib\\RenderGL.dll"; // 原5B33F8，显式16字节，不含NUL。
        static_assert(sizeof(suffix) - 1 == 0x10);
        ConcatenateRuntimeCStringAt4145E0(path.value, first,
            static_cast<std::uint32_t>(Header(first)->nDataLength), suffix, 0x10);
        const char* cachedPath = path.value.GetString(); // 483917，先于47D160固定ESI。
        FileStatusAt47D160 status;
        openGl = QueryFileStatusAt47D160(cachedPath, services.files, status); // state1
        // 483940先撤EH资格；正常Free用缓存header，失败不重试、不继续隐藏控件。
        unwind.active = false;
        ReleaseRuntimeCStringHeader(Header(cachedPath));
    }
    if (!openGl)
    {
        const HWND control = api.Control(current.dialogAt00.windowAt04, 1302);
        static_cast<void>(api.Show(control, SW_HIDE));
        if (current.renderAt24 == 2)
        {
            body.renderAt24 = 1; // 48397F在Show回调之后读取。
            MarkWritten(services, 0x04);
        }
    }
    body.fullscreenAt21 = services.source.SaveDataAtSlot1().bytesAt04[1] == 0;
    MarkWritten(services, 0x02);

    // 4839EA..483A33为内联DDX；各值必须先于各自GetDlgItem读取，不改调用通用DDX。
    const WPARAM dontShow = current.dontShowAt20 != 0 ? 1u : 0u;
    const HWND dontShowControl = api.Control(current.dialogAt00.windowAt04, 1200);
    static_cast<void>(api.Send(dontShowControl, BM_SETCHECK, dontShow, 0));
    if (CompiledGameProfile().startupFullscreenControl)
    {
        const WPARAM fullscreen = current.fullscreenAt21 != 0 ? 1u : 0u;
        const HWND fullscreenControl = api.Control(current.dialogAt00.windowAt04, 1201);
        static_cast<void>(api.Send(fullscreenControl, BM_SETCHECK, fullscreen, 0));
    }

    HWND radio = api.Control(current.dialogAt00.windowAt04, 1300);
    std::uint32_t index = 0;
    do
    {
        if ((api.Send(radio, WM_GETDLGCODE, 0, 0) & DLGC_RADIOBUTTON) != 0)
        {
            const WPARAM checked = std::bit_cast<std::int32_t>(index) == current.renderAt24 ? 1u : 0u;
            static_cast<void>(api.Send(radio, BM_SETCHECK, checked, 0));
            ++index;
        }
        radio = api.NextWindow(radio, GW_HWNDNEXT);
    } while (radio != nullptr && (api.WindowStyle(radio) & WS_GROUP) == 0);

    // 483AB2的this是HWND槽地址；FALSE仍继续，异常则不得继续font。
    static_cast<void>(CenterDialogWindowNativeAt413990(
        &body.dialogAt00.windowAt04, nullptr, services.endpoint.api));
    const auto font = reinterpret_cast<HGDIOBJ>(api.Send(current.dialogAt00.windowAt04, WM_GETFONT, 0, 0));
    LOGFONTA description; // 483AC7：GetObject栈缓冲未预清，不能补写原未写字段。
    constexpr int descriptionSize = sizeof(description);
    if (api.FontDescription(font, descriptionSize, &description) == descriptionSize)
    {
        description.lfWeight = 700;
        const HFONT bold = api.MakeFont(description);
        // 483AE4..483B34：不校验/保存/销毁新字体；两次均晚读body的HWND。
        for (const int id : {1600, 1602})
        {
            const HWND control = api.Control(current.dialogAt00.windowAt04, id);
            if (api.ValidWindow(control))
                static_cast<void>(api.Send(control, WM_SETFONT, reinterpret_cast<WPARAM>(bold), 1));
        }
    }
    return 1;
}

BOOL UpdateStartupSettingsDialogNativeAt476750(StartupSettingsDialogNativeBody28h& body,
    BOOL save, int control, StartupSettingsDialogNativeServices& services)
{
    auto& api = services.api;
    const volatile auto& current = body;
    if (control == -1 || control == 1200)
    {
        // 476783前置读取在save分支也发生；volatile不允许优化成只读已发布Values。
        const WPARAM previous = current.dontShowAt20 != 0 ? 1u : 0u;
        const HWND checkbox = api.Control(current.dialogAt00.windowAt04, 1200);
        if (save != 0) // 4767A1比较整个BOOL/DWORD，不截低byte。
        {
            body.dontShowAt20 = api.Send(checkbox, BM_GETCHECK, 0, 0) == BST_CHECKED;
            MarkWritten(services, 0x01);
        }
        else static_cast<void>(api.Send(checkbox, BM_SETCHECK, previous, 0));
    }
    // 01–03原111模板和DDX均没有1201；不能把NULL控件的0写回为窗口模式。
    if (CompiledGameProfile().startupFullscreenControl && (control == -1 || control == 1201))
    {
        const WPARAM previous = current.fullscreenAt21 != 0 ? 1u : 0u;
        const HWND checkbox = api.Control(current.dialogAt00.windowAt04, 1201);
        if (save != 0)
        {
            body.fullscreenAt21 = api.Send(checkbox, BM_GETCHECK, 0, 0) == BST_CHECKED;
            MarkWritten(services, 0x02);
        }
        else static_cast<void>(api.Send(checkbox, BM_SETCHECK, previous, 0));
    }
    if (control == -1 || control == 1300)
    {
        HWND radio = api.Control(current.dialogAt00.windowAt04, 1300);
        if (save != 0)
        {
            body.renderAt24 = -1; // 476852：仅在GetDlgItem正常返回后发布。
            MarkWritten(services, 0x04);
        }
        std::uint32_t index = 0;
        do
        {
            if ((api.Send(radio, WM_GETDLGCODE, 0, 0) & DLGC_RADIOBUTTON) != 0)
            {
                const auto ordinal = std::bit_cast<std::int32_t>(index);
                if (save != 0)
                {
                    // 476882非零覆盖，含indeterminate；多个选中取最后一个。
                    if (api.Send(radio, BM_GETCHECK, 0, 0) != 0)
                    {
                        body.renderAt24 = ordinal;
                        MarkWritten(services, 0x04);
                    }
                }
                else
                {
                    const WPARAM checked = ordinal == current.renderAt24 ? 1u : 0u;
                    static_cast<void>(api.Send(radio, BM_SETCHECK, checked, 0));
                }
                ++index; // 原INC按DWORD回绕；比较/发布保留signed ordinal位模式。
            }
            radio = api.NextWindow(radio, GW_HWNDNEXT);
        } while (radio != nullptr && (api.WindowStyle(radio) & WS_GROUP) == 0);
    }
    return TRUE; // 4768C4：包括未知control，异常不在此转换为成功。
}

BOOL ProcessStartupSettingsDialogNativeAt485A50(StartupSettingsDialogNativeBody28h& body,
    HWND, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result, DWORD mapId,
    StartupSettingsDialogNativeServices& services)
{
    if (mapId != 0) return FALSE;
    auto& api = services.api;
    const volatile auto& current = body;
    if (message == WM_INITDIALOG)
    {
        const auto initialized = InitializeStartupSettingsDialogNativeAt4837E0(body, services);
        *result = initialized;
        return TRUE;
    }
    if (message == WM_CTLCOLORSTATIC)
    {
        // 485AA7/485AB8/485AC6/485AD4逐次读body HWND；不使用入口HWND、不加IsWindow。
        for (const int id : {1600, 1601, 1602, 1603})
        {
            if (api.Control(current.dialogAt00.windowAt04, id) == reinterpret_cast<HWND>(lParam))
            {
                const auto dc = reinterpret_cast<HDC>(wParam);
                static_cast<void>(api.TextColor(dc, 0x000000FFu));
                static_cast<void>(api.BackgroundMode(dc, TRANSPARENT));
                *result = reinterpret_cast<LRESULT>(api.StockObject(5));
                return TRUE;
            }
        }
        *result = 0; // 485ADD：未匹配也handled；两个NULL相等仍进入上面的颜色分支。
        return TRUE;
    }
    if (message == WM_COMMAND)
    {
        // 485B2C只检查LOWORD，不筛选通知码/lParam；保留每一步已发布的副作用。
        if (LOWORD(wParam) == IDOK)
        {
            static_cast<void>(UpdateStartupSettingsDialogNativeAt476750(body, 1, -1, services));
            auto& skipTarget = services.source.SaveDataAtSlot1();
            skipTarget.bytesAt04[0] = current.dontShowAt20; // 485B90：捕获后读，原样复制BYTE。
            const auto selection = current.renderAt24; // 485B96：第二次捕获前固定EDI。
            auto& renderTarget = services.source.SaveDataAtSlot1();
            WriteRenderMode(renderTarget, selection == 0 ? 1u : selection == 2 ? 2u : 0u);
            if (CompiledGameProfile().startupFullscreenControl)
            {
                auto& fullscreenTarget = services.source.SaveDataAtSlot1();
                fullscreenTarget.bytesAt04[1] = current.fullscreenAt21 == 0 ? 1u : 0u;
            }
            static_cast<void>(api.End(current.dialogAt00.windowAt04, 1));
            *result = 0; // 485C2E：仅End正常返回后写result，忽略其BOOL。
            return TRUE;
        }
        if (LOWORD(wParam) == IDCANCEL)
        {
            static_cast<void>(api.End(current.dialogAt00.windowAt04, 2));
            *result = 0;
            return TRUE;
        }
    }
    return FALSE; // 485C5E：未处理不触碰result，无业务catch/事务回滚。
}
}
