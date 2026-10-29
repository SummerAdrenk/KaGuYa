#pragma once

#include <Windows.h>
#include <Runtime/RuntimeStringBorrow.h>
#include <Runtime/StartupSettingsDialogNative.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

namespace KaGuYa::Start::Runtime
{
class SaveDataEnvironmentArchive;
class IStartFileApi;

class INativeStartupSettingsDialogServices
{
public:
    virtual ~INativeStartupSettingsDialogServices() = default;
    // 每次解析实际环境slot1；不缓存配置副本，不把缺槽转换成默认设置。
    [[nodiscard]] virtual SaveDataEnvironmentArchive& SaveDataAtSlot1() = 0;
    // 48388C..483900：先执行413BE0(61A524)，再借用环境路径对象+18。
    [[nodiscard]] virtual const RuntimeCString& LibraryPathAt18After413BE0() = 0;
};

// 原生叶调用可注入，便于不经过系统回调边界检查失败和嵌套消息。默认实现调用Win32。
class INativeStartupSettingsDialogApi
{
public:
    virtual ~INativeStartupSettingsDialogApi() = default;
    virtual HWND ActiveWindow() = 0;
    virtual DialogEndpointNativeServices EndpointServices() = 0;
    virtual INT_PTR DialogBoxAt489DDF(HINSTANCE instance, LPCSTR resource,
        HWND parent, DLGPROC procedure, LPARAM parameter) = 0;
    virtual LONG SetWindowLongValue(HWND window, int index, LONG value) = 0;
    virtual HWND Control(HWND dialog, int id) = 0;
    virtual LRESULT Send(HWND window, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual HWND NextWindow(HWND window, UINT command) = 0;
    virtual LONG WindowStyle(HWND window) = 0;
    virtual BOOL Show(HWND window, int command) = 0;
    virtual BOOL End(HWND dialog, INT_PTR result) = 0;
    virtual int FontDescription(HGDIOBJ font, int size, void* description) = 0;
    virtual HFONT MakeFont(const LOGFONTA& description) = 0;
    virtual BOOL ValidWindow(HWND window) = 0;
    virtual COLORREF TextColor(HDC dc, COLORREF color) = 0;
    virtual int BackgroundMode(HDC dc, int mode) = 0;
    virtual HGDIOBJ StockObject(int index) = 0;
};

struct NativeStartupSettingsDialogValues final
{
    // 489D15..489D31没有写这三项；首次WM_INITDIALOG才逐项发布。
    std::optional<bool> dontShowAgainAt20;
    std::optional<bool> fullscreenAt21;
    std::optional<std::int32_t> renderSelectionAt24;
};

// 样本18 CSettingDlg，vtable 5B3364；每轮创建一个对象，同一UI线程负责整个模态寿命。
class NativeStartupSettingsDialogRuntime final
{
public:
    NativeStartupSettingsDialogRuntime(HINSTANCE resourceInstance,
        INativeStartupSettingsDialogServices& services, IStartFileApi& files);
    NativeStartupSettingsDialogRuntime(HINSTANCE resourceInstance,
        INativeStartupSettingsDialogServices& services, IStartFileApi& files,
        INativeStartupSettingsDialogApi& api);
    ~NativeStartupSettingsDialogRuntime() noexcept;
    NativeStartupSettingsDialogRuntime(const NativeStartupSettingsDialogRuntime&) = delete;
    NativeStartupSettingsDialogRuntime& operator=(const NativeStartupSettingsDialogRuntime&) = delete;

    // 489D3C..489DE5：不检查slot1 gate、不把ID2转异常；分配返回失败置LastError14并返回-1。
    // 原始DialogBox返回值原样保留，0/-1都不是取消。不重试或回滚失败的创建记录。
    [[nodiscard]] INT_PTR RunModalAt489DDF();
    [[nodiscard]] HWND Window() const noexcept;
    [[nodiscard]] const NativeStartupSettingsDialogValues& Values() const noexcept;
    [[nodiscard]] BOOL UpdateDataAt476750(bool save, int controlId = -1);
    [[nodiscard]] BOOL ProcessWindowMessageAt485A50(HWND window, UINT message,
        WPARAM wParam, LPARAM lParam, LRESULT& result, DWORD mapId = 0);

    static INT_PTR CALLBACK InitialDialogProcedureAt4382D0(
        HWND window, UINT message, WPARAM wParam, LPARAM lParam);

private:
    struct State;
    std::unique_ptr<State> state_;
};

// 489D0F gate + 局部对话框 + 48AE7D throw int(0)。不在这里吞异常或返回初始化false，
// 外层489C90/ApplicationStagesRuntime负责catch-all；也不提前执行后续FileManager工作。
void ConfirmNativeStartupSettingsAt489C90(HINSTANCE resourceInstance,
    INativeStartupSettingsDialogServices& services, IStartFileApi& files);
void ConfirmNativeStartupSettingsAt489C90(HINSTANCE resourceInstance,
    INativeStartupSettingsDialogServices& services, IStartFileApi& files,
    INativeStartupSettingsDialogApi& api);
// 原489DD9才读资源实例；槽必须覆盖本轮模态寿命，不缓存为函数入口值。
void ConfirmNativeStartupSettingsWithResourceSlotAt489C90(const HINSTANCE&,
    INativeStartupSettingsDialogServices&, IStartFileApi&);
void ConfirmNativeStartupSettingsWithResourceSlotAt489C90(const HINSTANCE&,
    INativeStartupSettingsDialogServices&, IStartFileApi&, INativeStartupSettingsDialogApi&);
}
