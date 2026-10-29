#pragma once

#include <Windows.h>
#include <Platform/InputViews.h>
#include <Platform/WindowHookNativeProtocol.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>
#include <memory_resource>
#include <span>
#include <vector>

namespace KaGuYa::Start::Platform
{
class IWindowMessageHook
{
public:
    virtual ~IWindowMessageHook() = default;
    [[nodiscard]] virtual bool Dispatch(
        unsigned int messageMapId,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT& result) = 0;
};

class WindowHookDispatcher final:public IWindowHookMessageSource
{
public:
    using NativeCallback = bool (*)(void* object, void* context, unsigned int messageMapId,
        UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result);
    WindowHookDispatcher();
    ~WindowHookDispatcher();

    WindowHookDispatcher(const WindowHookDispatcher&) = delete;
    WindowHookDispatcher& operator=(const WindowHookDispatcher&) = delete;

    void Add(IWindowMessageHook* hook);
    void Remove(IWindowMessageHook* hook);
    void AddNative(void* object, NativeCallback callback, void* context);
    void RemoveNative(void* object);
    void Clear();
    [[nodiscard]] bool Dispatch(
        unsigned int messageMapId,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT& result);
    [[nodiscard]] std::size_t Count() const noexcept { return hooks_.size(); }
    std::int32_t DispatchNative(HWND,UINT message,WPARAM w,LPARAM l,LRESULT* result,std::uint32_t id) override
    {return Dispatch(id,message,w,l,*result);}

private:
    CRITICAL_SECTION lock_{};
    struct HookEntry final
    {
        enum class Kind { Legacy, Native } kind;
        IWindowMessageHook* object;
        void* nativeObject;
        NativeCallback nativeCallback;
        void* context;
    };
    std::list<HookEntry> hooks_;
};

struct ViewportMapping
{
    bool enabled = false;
    int viewportX = 0;
    int viewportY = 0;
    int viewportWidth = 0;
    int viewportHeight = 0;
    int logicalWidth = 0;
    int logicalHeight = 0;
};

class IMousePositionApi
{
public:
    virtual ~IMousePositionApi() = default;
    virtual void ReadScreenPosition(POINT& position) const = 0;
    virtual void ConvertToClient(HWND window, POINT& position) const = 0;
    [[nodiscard]] virtual bool IsWindowMinimized(HWND window) const = 0;
};

class Win32MousePositionApi final : public IMousePositionApi
{
public:
    void ReadScreenPosition(POINT& position) const override;
    void ConvertToClient(HWND window, POINT& position) const override;
    [[nodiscard]] bool IsWindowMinimized(HWND window) const override;
};

class MousePositionState final
{
public:
    void Update(
        HWND window,
        const ViewportMapping& mapping,
        const IMousePositionApi& api);

    [[nodiscard]] POINT LogicalPosition() const noexcept { return logicalPosition_; }
    [[nodiscard]] POINT ClientPosition() const noexcept { return clientPosition_; }
    [[nodiscard]] bool LogicalPositionChanged() const noexcept { return logicalPositionChanged_; }

private:
    friend class MouseManagerState;
    void UpdateFromScreenPosition(
        HWND window,
        const ViewportMapping& mapping,
        const IMousePositionApi& api,
        POINT screenPosition,
        POINT previousLogicalPosition);
    POINT logicalPosition_{};
    POINT clientPosition_{};
    bool logicalPositionChanged_ = false;
};

struct MouseRawSnapshot
{
    std::array<std::uint8_t, 3> pressed{};
    std::array<std::uint8_t, 3> downLatched{};
    std::int16_t wheelSteps = 0;
};

class MouseRawState final : public IWindowMessageHook
{
public:
    MouseRawState() = default;
    explicit MouseRawState(WindowHookDispatcher& dispatcher);
    ~MouseRawState() override;

    MouseRawState(const MouseRawState&) = delete;
    MouseRawState& operator=(const MouseRawState&) = delete;

    [[nodiscard]] bool Dispatch(
        unsigned int messageMapId,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT& result);

    [[nodiscard]] MouseRawSnapshot ConsumeFrameSnapshot() noexcept;
    [[nodiscard]] std::uint8_t Pressed(std::size_t button) const noexcept;
    [[nodiscard]] std::uint8_t DownLatched(std::size_t button) const noexcept;
    [[nodiscard]] std::int16_t PendingWheelSteps() const noexcept { return wheelSteps_; }
    [[nodiscard]] std::int16_t WheelRemainder() const noexcept { return wheelRemainder_; }

private:
    WindowHookDispatcher* dispatcher_ = nullptr;
    std::array<std::uint8_t, 3> pressed_{};
    std::array<std::uint8_t, 3> downLatched_{};
    std::int16_t wheelSteps_ = 0;
    std::int16_t wheelRemainder_ = 0;
};

class MouseManagerState final
{
public:
    MouseManagerState(
        MouseRawState* rawState,
        bool initialForceClearLeft) noexcept;

    void Update(
        HWND window,
        const ViewportMapping& mapping,
        const IMousePositionApi& positionApi);
    void SetReleaseSuppressionCountdown(std::uint32_t value) noexcept
    {
        releaseSuppressionCountdown_ = value;
    }
    void SetSecondaryCountdown(std::uint32_t value) noexcept { secondaryCountdown_ = value; }
    void ForceClearLeftOnNextUpdate() noexcept { forceClearLeft_ = true; }

    [[nodiscard]] std::uint8_t Pressed(std::size_t button) const noexcept;
    [[nodiscard]] std::uint8_t PreviousPressed(std::size_t button) const noexcept;
    [[nodiscard]] std::uint8_t DownLatched(std::size_t button) const noexcept;
    [[nodiscard]] std::int16_t WheelSteps() const noexcept { return wheelSteps_; }
    // 46D5BD/46D5E6仅消费manager+13/+14的锁存，不改raw输入或当前按住状态。
    void SetDownLatched(std::size_t button, std::uint8_t value) noexcept { downLatched_[button] = value; }
    [[nodiscard]] std::uint32_t ReleaseSuppressionCountdown() const noexcept
    {
        return releaseSuppressionCountdown_;
    }
    [[nodiscard]] std::uint32_t SecondaryCountdown() const noexcept { return secondaryCountdown_; }
    [[nodiscard]] const MousePositionState& Position() const noexcept { return position_; }

private:
    MouseRawState* rawState_ = nullptr;
    std::array<std::uint8_t, 3> pressed_{};
    std::array<std::uint8_t, 3> downLatched_{};
    std::array<std::uint8_t, 3> previousPressed_{};
    MousePositionState position_;
    std::uint32_t releaseSuppressionCountdown_ = 0;
    std::uint32_t secondaryCountdown_ = 0;
    std::int16_t wheelSteps_ = 0;
    bool forceClearLeft_ = false;
};

class ICursorApi
{
public:
    virtual ~ICursorApi() = default;
    [[nodiscard]] virtual HCURSOR Load(HINSTANCE instance, LPCSTR resource) const = 0;
    virtual void Set(HCURSOR cursor) const = 0;
    [[nodiscard]] virtual HCURSOR Get() const = 0;
    virtual void ReadScreenPosition(POINT& position) const = 0;
    virtual void ConvertToClient(HWND window, POINT& position) const = 0;
    virtual void ReadClientRectangle(HWND window, RECT& rectangle) const = 0;
    [[nodiscard]] virtual bool Contains(const RECT& rectangle, POINT position) const = 0;
};

class Win32CursorApi final : public ICursorApi
{
public:
    [[nodiscard]] HCURSOR Load(HINSTANCE instance, LPCSTR resource) const override;
    void Set(HCURSOR cursor) const override;
    [[nodiscard]] HCURSOR Get() const override;
    void ReadScreenPosition(POINT& position) const override;
    void ConvertToClient(HWND window, POINT& position) const override;
    void ReadClientRectangle(HWND window, RECT& rectangle) const override;
    [[nodiscard]] bool Contains(const RECT& rectangle, POINT position) const override;
};

class CursorState final : public IWindowMessageHook
{
public:
    CursorState(HWND window, HINSTANCE moduleInstance, const ICursorApi& api);
    CursorState(
        HWND window,
        HINSTANCE moduleInstance,
        const ICursorApi& api,
        WindowHookDispatcher& dispatcher);
    ~CursorState() override;

    CursorState(const CursorState&) = delete;
    CursorState& operator=(const CursorState&) = delete;

    [[nodiscard]] bool Dispatch(
        unsigned int messageMapId,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT& result);
    [[nodiscard]] bool LoadAndApply(
        LPCSTR resource,
        bool systemResource,
        bool force);
    [[nodiscard]] bool RestoreSystemArrow();
    [[nodiscard]] HCURSOR CurrentCursor() const noexcept { return currentCursor_; }

private:
    HWND window_ = nullptr;
    HINSTANCE moduleInstance_ = nullptr;
    const ICursorApi& api_;
    WindowHookDispatcher* dispatcher_ = nullptr;
    HCURSOR currentCursor_ = nullptr;
    HCURSOR previousCursor_ = nullptr;
};

class KeyboardState final : public IWindowMessageHook
{
public:
    KeyboardState() = default;
    explicit KeyboardState(WindowHookDispatcher& dispatcher);
    ~KeyboardState() override;

    KeyboardState(const KeyboardState&) = delete;
    KeyboardState& operator=(const KeyboardState&) = delete;

    [[nodiscard]] bool Dispatch(
        unsigned int messageMapId,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT& result);

    [[nodiscard]] std::uint8_t KeyState(std::size_t virtualKey) const noexcept;

private:
    void HandleFocusLoss();

    WindowHookDispatcher* dispatcher_ = nullptr;
    std::array<std::uint8_t, 256> keyStates_{};
};

struct KeyboardBinding
{
    std::uint8_t actionIndex = 0;
    std::uint8_t virtualKey = 0;
};

class KeyboardFrameState final
{
public:
    explicit KeyboardFrameState(std::size_t actionCapacity);
    // 显式分配来源供窄边界验证；借用的 memory 必须长于本对象。
    KeyboardFrameState(std::size_t actionCapacity, std::pmr::memory_resource& memory);
    KeyboardFrameState(
        std::size_t actionCapacity,
        std::span<const KeyboardBinding> bindings);

    void AddBindings(std::span<const KeyboardBinding> bindings);
    void Update(const KeyboardState& keyboard);
    [[nodiscard]] std::size_t ActionCount() const noexcept { return actionCount_; }
    [[nodiscard]] std::size_t ActionCapacity() const noexcept { return actions_.size(); }
    [[nodiscard]] std::size_t BindingCount(std::size_t actionIndex) const noexcept;
    [[nodiscard]] std::uint8_t VirtualKey(
        std::size_t actionIndex,
        std::size_t bindingIndex) const noexcept;
    [[nodiscard]] std::uint8_t CurrentState(
        std::size_t actionIndex,
        std::size_t bindingIndex) const noexcept;
    [[nodiscard]] std::uint8_t PreviousState(
        std::size_t actionIndex,
        std::size_t bindingIndex) const noexcept;

private:
    struct Mapping
    {
        std::uint8_t virtualKey = 0;
        std::array<std::uint8_t, 2> states{};
    };

    std::pmr::vector<std::pmr::vector<Mapping>> actions_;
    std::size_t actionCount_ = 0;
    std::size_t currentBuffer_ = 0;
};

class InputAggregate final : public IInputRuntime
{
public:
    MouseView BorrowMouse() const override{return MouseView(mouseManager_.get());}
    KeyboardView BorrowKeyboard() const override{return KeyboardView(keyboardFrames_.get());}
    CursorView BorrowCursor() const override{return CursorView(publishedCursor_);}
    void UpdateWithSource(Runtime::IInputFrameNativeSource&,const IMousePositionApi&) override;
    InputAggregate() noexcept = default;
    InputAggregate(
        HWND window,
        HINSTANCE moduleInstance,
        WindowHookDispatcher& dispatcher,
        const ICursorApi& cursorApi,
        std::size_t keyboardActionCapacity,
        std::span<const KeyboardBinding> keyboardBindings,
        bool initialForceClearLeft);
    ~InputAggregate();

    InputAggregate(const InputAggregate&) = delete;
    InputAggregate& operator=(const InputAggregate&) = delete;

    void InitializeAt4507E0(HWND window, HINSTANCE moduleInstance,
        WindowHookDispatcher& dispatcher, const ICursorApi& cursorApi,
        std::size_t keyboardActionCapacity, std::span<const KeyboardBinding> keyboardBindings,
        bool initialForceClearLeft,
        std::pmr::memory_resource& keyboardMemory = *std::pmr::get_default_resource());
    void ReleaseAt451150() noexcept;
    // 仅供现代装配诊断；原4507E0不以返回值表示构造是否成功。
    [[nodiscard]] bool HasMouseAndKeyboard() const noexcept
        { return mouseManager_ && keyboardFrames_ && keyboardRaw_; }

    void Update(
        HWND window,
        const ViewportMapping& mapping,
        const IMousePositionApi& mousePositionApi);

    [[nodiscard]] MouseManagerState& Mouse() noexcept { return *mouseManager_; }
    [[nodiscard]] const MouseManagerState& Mouse() const noexcept { return *mouseManager_; }
    [[nodiscard]] MouseManagerState* MouseOrNull() noexcept { return mouseManager_.get(); }
    [[nodiscard]] const MouseManagerState* MouseOrNull() const noexcept { return mouseManager_.get(); }
    [[nodiscard]] CursorState* Cursor() noexcept { return publishedCursor_; }
    [[nodiscard]] const CursorState* Cursor() const noexcept { return publishedCursor_; }
    [[nodiscard]] const KeyboardFrameState* KeyboardOrNull() const noexcept { return keyboardFrames_.get(); }
    [[nodiscard]] KeyboardFrameState& Keyboard() noexcept { return *keyboardFrames_; }
    [[nodiscard]] const KeyboardFrameState& Keyboard() const noexcept
    {
        return *keyboardFrames_;
    }

private:
    void Release() noexcept;

    std::unique_ptr<MouseRawState> mouseRaw_;
    std::unique_ptr<MouseManagerState> mouseManager_;
    std::unique_ptr<CursorState> cursor_;
    CursorState* publishedCursor_ = nullptr;
    std::unique_ptr<KeyboardState> keyboardRaw_;
    std::unique_ptr<KeyboardFrameState> keyboardFrames_;
};
}
