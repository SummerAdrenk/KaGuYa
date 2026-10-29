#pragma once
#include <Windows.h>
#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime { class IInputFrameNativeSource; }
namespace KaGuYa::Start::Platform
{
class MouseManagerState;
class KeyboardFrameState;
class CursorState;
class IMousePositionApi;
struct MouseViewMethods final
{
    std::uint8_t (*pressed)(const void*,std::size_t);
    std::uint8_t (*previous)(const void*,std::size_t);
    std::uint8_t (*latched)(const void*,std::size_t);
    std::int32_t (*wheel)(const void*);
    std::int32_t (*x)(const void*);
    std::int32_t (*y)(const void*);
    POINT (*logical)(const void*);
    POINT (*client)(const void*);
    bool (*moved)(const void*);
    std::uint32_t (*suppression)(const void*);
    std::uint32_t (*secondary)(const void*);
    void (*setLatched)(const void*,std::size_t,std::uint8_t);
    void (*setSuppression)(const void*,std::uint32_t);
    void (*setSecondary)(const void*,std::uint32_t);
    void (*clearLeft)(const void*);
};
class MousePositionView final
{
public:
    MousePositionView(const void* body,const MouseViewMethods* calls) noexcept:body_(body),calls_(calls){}
    POINT LogicalPosition() const{return calls_->logical(body_);}
    POINT ClientPosition() const{return calls_->client(body_);}
    bool LogicalPositionChanged() const{return calls_->moved(body_);}
private:
    const void* body_;const MouseViewMethods* calls_;
};
// 按值借用本次选中的正文；不拥有、拷贝状态或借共享current-body缓存。
class MouseView final
{
public:
    MouseView() noexcept=default;
    MouseView(std::nullptr_t) noexcept{}
    MouseView(const MouseManagerState&) noexcept;
    MouseView(const MouseManagerState*) noexcept;
    MouseView(const void* body,const MouseViewMethods* calls) noexcept:body_(body),calls_(calls){}
    explicit operator bool() const noexcept{return body_!=nullptr;}
    const MouseView* operator->() const noexcept{return this;}
    const MouseView& operator*() const noexcept{return *this;}
    std::uint8_t Pressed(std::size_t i) const{return calls_->pressed(body_,i);}
    std::uint8_t PreviousPressed(std::size_t i) const{return calls_->previous(body_,i);}
    std::uint8_t DownLatched(std::size_t i) const{return calls_->latched(body_,i);}
    std::int32_t WheelSteps() const{return calls_->wheel(body_);}
    std::int32_t XAt18() const{return calls_->x(body_);}
    std::int32_t YAt1C() const{return calls_->y(body_);}
    MousePositionView Position() const noexcept{return {body_,calls_};}
    std::uint32_t ReleaseSuppressionCountdown() const{return calls_->suppression(body_);}
    std::uint32_t SecondaryCountdown() const{return calls_->secondary(body_);}
    void SetDownLatched(std::size_t i,std::uint8_t value) const{calls_->setLatched(body_,i,value);}
    void SetReleaseSuppressionCountdown(std::uint32_t value) const{calls_->setSuppression(body_,value);}
    void SetSecondaryCountdown(std::uint32_t value) const{calls_->setSecondary(body_,value);}
    void ForceClearLeftOnNextUpdate() const{calls_->clearLeft(body_);}
    const void* Receiver() const noexcept{return body_;}
    friend bool operator==(const MouseView& value,std::nullptr_t) noexcept{return !value.body_;}
private:
    const void* body_=nullptr;const MouseViewMethods* calls_=nullptr;
};
struct KeyboardViewMethods final
{
    std::size_t (*checkedAction)(const void*,std::int32_t);
    std::size_t (*count)(const void*);
    std::size_t (*capacity)(const void*);
    std::size_t (*bindings)(const void*,std::size_t);
    std::uint8_t (*key)(const void*,std::size_t,std::size_t);
    std::uint8_t (*current)(const void*,std::size_t,std::size_t);
    std::uint8_t (*previous)(const void*,std::size_t,std::size_t);
};
class KeyboardView final
{
public:
    KeyboardView() noexcept=default;
    KeyboardView(std::nullptr_t) noexcept{}
    KeyboardView(const KeyboardFrameState&) noexcept;
    KeyboardView(const KeyboardFrameState*) noexcept;
    KeyboardView(const void* body,const KeyboardViewMethods* calls) noexcept:body_(body),calls_(calls){}
    explicit operator bool() const noexcept{return body_!=nullptr;}
    const KeyboardView* operator->() const noexcept{return this;}
    const KeyboardView& operator*() const noexcept{return *this;}
    std::size_t ActionCount() const{return calls_->count(body_);}
    std::size_t CheckedAction(std::int32_t action) const{return calls_->checkedAction(body_,action);}
    std::size_t ActionCapacity() const{return calls_->capacity(body_);}
    std::size_t BindingCount(std::size_t action) const{return calls_->bindings(body_,action);}
    std::uint8_t VirtualKey(std::size_t action,std::size_t binding) const{return calls_->key(body_,action,binding);}
    std::uint8_t CurrentState(std::size_t action,std::size_t binding) const{return calls_->current(body_,action,binding);}
    std::uint8_t PreviousState(std::size_t action,std::size_t binding) const{return calls_->previous(body_,action,binding);}
    const void* Receiver() const noexcept{return body_;}
    friend bool operator==(const KeyboardView& value,std::nullptr_t) noexcept{return !value.body_;}
private:
    const void* body_=nullptr;const KeyboardViewMethods* calls_=nullptr;
};
struct CursorViewMethods final
{
    bool (*load)(void*,void*,LPCSTR,bool,bool);
    bool (*restore)(void*,void*);
    HCURSOR (*current)(void*,void*);
};
class CursorView final
{
public:
    CursorView() noexcept=default;
    CursorView(std::nullptr_t) noexcept{}
    CursorView(CursorState&) noexcept;
    CursorView(CursorState*) noexcept;
    CursorView(void* body,void* context,const CursorViewMethods* calls) noexcept:body_(body),context_(context),calls_(calls){}
    explicit operator bool() const noexcept{return body_!=nullptr;}
    const CursorView* operator->() const noexcept{return this;}
    const CursorView& operator*() const noexcept{return *this;}
    bool LoadAndApply(LPCSTR n,bool system,bool force) const{return calls_->load(body_,context_,n,system,force);}
    bool RestoreSystemArrow() const{return calls_->restore(body_,context_);}
    HCURSOR CurrentCursor() const{return calls_->current(body_,context_);}
    const void* Receiver() const noexcept{return body_;}
    friend bool operator==(const CursorView& a,const CursorView& b) noexcept{return a.body_==b.body_;}
    friend bool operator==(const CursorView& a,std::nullptr_t) noexcept{return !a.body_;}
private:
    void* body_=nullptr;void* context_=nullptr;const CursorViewMethods* calls_=nullptr;
};
// 调用期获取view，具体producer负责同一Root捕获和帧更新；接口本身不拥有输入状态。
class IInputRuntime
{
public:
    virtual ~IInputRuntime()=default;
    virtual MouseView BorrowMouse() const=0;
    virtual KeyboardView BorrowKeyboard() const=0;
    virtual CursorView BorrowCursor() const=0;
    virtual void UpdateWithSource(Runtime::IInputFrameNativeSource&,const IMousePositionApi&)=0;
    MouseView Mouse() const{return BorrowMouse();}
    MouseView MouseOrNull() const{return BorrowMouse();}
    KeyboardView Keyboard() const{return BorrowKeyboard();}
    KeyboardView KeyboardOrNull() const{return BorrowKeyboard();}
    CursorView Cursor() const{return BorrowCursor();}
};
// 启动器只借用输入的生命周期；正文仍由独立singleton持有，不由PostWindow拥有。
class IInputStartupRuntime
{
public:
    virtual ~IInputStartupRuntime()=default;
    virtual IInputRuntime& Input() noexcept=0;
    virtual void InitializeAt4507E0(HWND)=0;
    virtual void ReleaseAt451150()=0;
};
}
