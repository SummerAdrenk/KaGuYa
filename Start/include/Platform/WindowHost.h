#pragma once

#include <Platform/MainWindow.h>
#include <Platform/PendingWindowCreation.h>
#include <Platform/WindowCreation.h>

#include <Windows.h>

#include <cstdint>

namespace KaGuYa::Start::Platform
{
class ExecutableWindowThunk final
{
public:
    ExecutableWindowThunk() = default;
    ~ExecutableWindowThunk();

    ExecutableWindowThunk(const ExecutableWindowThunk&) = delete;
    ExecutableWindowThunk& operator=(const ExecutableWindowThunk&) = delete;

    void Initialize(void* object, std::uintptr_t targetProcedure);
    [[nodiscard]] bool TryInitialize(void* object, std::uintptr_t targetProcedure);
    [[nodiscard]] WNDPROC Procedure() const noexcept;

private:
    void* code_ = nullptr;
};

class WindowHost final
{
public:
    explicit WindowHost(MainWindow& mainWindow);
    ~WindowHost();

    [[nodiscard]] HWND Create(
        WindowClassRegistrar& classRegistrar,
        HINSTANCE moduleInstance,
        HINSTANCE resourceInstance,
        const WindowCreationPlan& plan,
        HMENU childWindowIdentifier);

    [[nodiscard]] HWND Window() const noexcept { return window_; }
    [[nodiscard]] WNDPROC PreviousProcedure() const noexcept { return previousProcedure_; }

    static LRESULT CALLBACK InitialWindowProcedure(
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

private:
    struct MessageContext
    {
        MessageContext* previous = nullptr;
    };

    static LRESULT CALLBACK BoundWindowProcedure(
        WindowHost* host,
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

    LRESULT DispatchBound(UINT message, WPARAM wParam, LPARAM lParam);

    MainWindow& mainWindow_;
    HWND window_ = nullptr;
    WNDPROC previousProcedure_ = DefWindowProcA;
    ExecutableWindowThunk thunk_;
    PendingWindowCreateRecord createRecord_;
    MessageContext* currentMessage_ = nullptr;
    bool destroyPending_ = false;
};
}
