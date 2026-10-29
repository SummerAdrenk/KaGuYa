#include <Runtime/GuiDialogNative.h>

#include <bit>
#include <cstdlib>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = GuiDialogNativeBody5Ch;
using Child = GuiControlNativeBody50h;
using Slot = YtlReferenceSlotRuntime;
thread_local IGuiDialogNativeScreenSource* screenSource = nullptr;

std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
void Copy(Slot& out, const Slot& source) noexcept
{
    out.baseAt00 = source.baseAt00;
    out.biasAt04 = source.biasAt04;
    out.controlAt08 = source.controlAt08;
    if (out.controlAt08) ++out.controlAt08->referenceCountAt04;
}
void ReleaseWithoutClearing(Slot& slot)
{
    auto* control = slot.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (auto* current = slot.controlAt08) current->DeleteControlAtVslot04(1);
    }
}
struct Argument final
{
    Slot& slot;
    bool active = true;
    ~Argument() noexcept { if (active) ReleaseYtlReference(slot); }
    // 原正常尾为terminate态；仅各自明确清写的分支才清槽。
    void Finish(bool clear = false) noexcept
    {
        active = false;
        if (clear) ReleaseYtlReference(slot);
        else ReleaseWithoutClearing(slot);
    }
};
Child* ResolveChild(const Slot& slot) noexcept { return ResolveGuiControl(slot); }
GuiInputNativeBody* Input(Body* body) noexcept
{ return ResolveEnvironmentObjectReference<GuiInputNativeBody>(body->inputAt18); }
GuiNavigationNativeBody* Navigation(Body* body, std::uint32_t index) noexcept
{ return ResolveEnvironmentObjectReference<GuiNavigationNativeBody>(body->navigationAt24.begin[index]); }

// 518C30的局部map：18h head/node，保留未写padding，重复key也先分配节点。
// 4DA040/53B8A0/53B960插入平衡；4F6250/53B800按右子树、节点、左子树释放。
class StateTree final
{
    struct Node final
    {
        Node* left;
        Node* parent;
        Node* right;
        std::uint8_t black, nil;
        std::uint8_t unwritten[2];
        std::int32_t key, value;
    };
    static_assert(sizeof(Node) == 0x18);
    Node* head_ = nullptr;
    std::uint32_t count_ = 0;
    Node* Allocate(std::int32_t key, std::int32_t value)
    {
        auto* node = static_cast<Node*>(::operator new(0x18));
        node->left = head_; node->parent = head_; node->right = head_;
        node->black = 0; node->nil = 0; node->key = key; node->value = value;
        return node;
    }
    void Left(Node* old) noexcept
    {
        auto* next = old->right;
        old->right = next->left;
        if (!next->left->nil) next->left->parent = old;
        next->parent = old->parent;
        if (old == head_->parent) head_->parent = next;
        else if (old == old->parent->left) old->parent->left = next;
        else old->parent->right = next;
        next->left = old; old->parent = next;
    }
    void Right(Node* old) noexcept
    {
        auto* next = old->left;
        old->left = next->right;
        if (!next->right->nil) next->right->parent = old;
        next->parent = old->parent;
        if (old == head_->parent) head_->parent = next;
        else if (old == old->parent->right) old->parent->right = next;
        else old->parent->left = next;
        next->right = old; old->parent = next;
    }
    Node* Insert(Node* node)
    {
        auto* parent = head_;
        auto* cursor = head_->parent;
        bool left = true;
        while (!cursor->nil)
        {
            parent = cursor;
            if (node->key == cursor->key)
            {
                ::operator delete(node, std::size_t{0x18});
                return cursor;
            }
            left = node->key < cursor->key;
            cursor = left ? cursor->left : cursor->right;
        }
        if (count_ >= 0x0AAAAAA9u)
        {
            ::operator delete(node, std::size_t{0x18});
            throw std::length_error("map/set<T> too long");
        }
        ++count_;
        node->parent = parent;
        if (parent == head_) head_->parent = head_->left = head_->right = node;
        else if (left)
        {
            parent->left = node;
            if (parent == head_->left) head_->left = node;
        }
        else
        {
            parent->right = node;
            if (parent == head_->right) head_->right = node;
        }
        cursor = node;
        while (!cursor->parent->black)
        {
            parent = cursor->parent;
            auto* grand = parent->parent;
            if (parent == grand->left)
            {
                auto* uncle = grand->right;
                if (!uncle->black)
                { parent->black = uncle->black = 1; grand->black = 0; cursor = grand; }
                else
                {
                    if (cursor == parent->right) { cursor = parent; Left(cursor); }
                    cursor->parent->black = 1;
                    cursor->parent->parent->black = 0;
                    Right(cursor->parent->parent);
                }
            }
            else
            {
                auto* uncle = grand->left;
                if (!uncle->black)
                { parent->black = uncle->black = 1; grand->black = 0; cursor = grand; }
                else
                {
                    if (cursor == parent->left) { cursor = parent; Right(cursor); }
                    cursor->parent->black = 1;
                    cursor->parent->parent->black = 0;
                    Left(cursor->parent->parent);
                }
            }
        }
        head_->parent->black = 1;
        return node;
    }
    static void Erase(Node* node) noexcept
    {
        while (!node->nil)
        {
            Erase(node->right);
            auto* left = node->left;
            ::operator delete(node, std::size_t{0x18});
            node = left;
        }
    }
public:
    StateTree()
    {
        head_ = static_cast<Node*>(::operator new(0x18));
        head_->left = head_; head_->parent = head_; head_->right = head_;
        head_->black = 1; head_->nil = 1;
    }
    ~StateTree() noexcept
    {
        auto* original = head_;
        Erase(head_->parent);
        head_->parent = original; head_->left = original; head_->right = original; count_ = 0;
        ::operator delete(head_, std::size_t{0x18});
    }
    void Snapshot(std::int32_t key, std::int32_t value) { Insert(Allocate(key, value)); }
    std::int32_t LookupOrZero(std::int32_t key)
    {
        auto* cursor = head_->parent;
        while (!cursor->nil)
        {
            if (key == cursor->key) return cursor->value;
            cursor = key < cursor->key ? cursor->left : cursor->right;
        }
        return Insert(Allocate(key, 0))->value;
    }
};

GuiDirtyRectNativeBody80h* __fastcall Dirty(Body* body, void*)
{ return ResolveGuiDirtyRectReference(body->dirtyAt50); }
Body* __fastcall Delete(Body* body, void*, std::uint32_t flags) noexcept
{
    DestroyGuiDialogAt4F7AC0(body);
    if (flags & 1) ::operator delete(body, std::size_t{0x5C});
    return body;
}
void __fastcall ClearChildren(Body* body, void*) noexcept
{
    ReleaseGuiControlChildrenRangeAt4D7F00(body->childrenAt0C.begin, body->childrenAt0C.end);
    body->childrenAt0C.end = body->childrenAt0C.begin;
}
std::int32_t __fastcall Id(Body* body, void*) { return body->idAt08; }
std::int32_t __fastcall AppendChild(Body* body, void*, Slot argument)
{
    Argument cleanup{argument};
    AppendGuiControlChildReference(body->childrenAt0C, argument);
    const auto index = Add(GuiControlChildReferenceCount(body->childrenAt0C), -1);
    cleanup.Finish();
    return index;
}
Slot* __fastcall FindChild(Body* body, void*, Slot* output, std::int32_t event)
{
    auto* const end = body->childrenAt0C.end;
    for (auto* position = body->childrenAt0C.begin; position != end; ++position)
        if (ResolveChild(*position)->eventAt04 == event) { Copy(*output, *position); return output; }
    *output = {};
    return output;
}
void __fastcall SetDirty(Body* body, void*, Slot argument)
{
    Argument cleanup{argument};
    ReleaseWithoutClearing(body->dirtyAt50);
    Copy(body->dirtyAt50, argument);
    cleanup.Finish();
}
void __fastcall WriteState(Body* body, void*, std::int32_t value) { body->stateAt34 = value; }
std::int32_t __fastcall ReadState(Body* body, void*) { return body->stateAt34; }
void __fastcall WriteEnabled(Body* body, void*, std::int32_t value) { body->enabledAt38 = value; }
std::int32_t __fastcall ReadEnabled(Body* body, void*) { return body->enabledAt38; }
std::int32_t __fastcall Enabled(Body* body, void*) { return body->enabledAt38 != 0 && body->stateAt34 != 0; }
void __fastcall SetActive(Body* body, void*, Child* next)
{
    if (auto* old = body->activeAt4C)
    {
        if (old == next) return;
        old->vptrAt00->blurAt24(old);
    }
    if (next) next->vptrAt00->focusAt20(next);
    body->activeAt4C = next;
}
std::int32_t __fastcall Zero(Body*, void*) { return 0; }
void __fastcall ResetInput(Body* body, void*)
{
    body->vptrAt00->setActiveAt30(body, nullptr);
    body->inputLatchAt30 = 0;
}
[[noreturn]] std::int32_t __fastcall Pure(Body*, void*) { _purecall(); std::terminate(); }
std::int32_t __fastcall Result(Body*, void*) { return -1; }

std::int32_t __fastcall UpperInput(Body* body, void*)
{
    if (body->vptrAt00->movedAt7C(body) || body->vptrAt00->mousePressAt80(body))
    { body->vptrAt00->resetInputAt3C(body); return 0; }
    if (body->vptrAt00->priorityInputAt40(body)) return 1;
    if (body->activeAt4C && body->vptrAt00->activeInputAt38(body)) return 1;
    const auto fallback = body->inputLatchAt30 ? 2 : 0;
    std::int32_t index = 0;
    for (;; ++index)
    {
        if (index >= GuiNavigationReferenceCount(body->navigationAt24)) return fallback;
        auto* navigation = Navigation(body, static_cast<std::uint32_t>(index));
        if (body->vptrAt00->pressedAt70(body, navigation->inputAt04)) break;
    }
    if (!body->activeAt4C)
    {
        for (std::int32_t pair = 0;; ++pair)
        {
            auto* navigation = Navigation(body, static_cast<std::uint32_t>(index));
            if (pair >= GuiNavigationPairCount(*navigation)) return fallback;
            navigation = Navigation(body, static_cast<std::uint32_t>(index));
            const auto event = navigation->pairsAt08.begin[pair].sourceEventValue;
            Slot selected;
            body->vptrAt00->findChildAt14(body, &selected, event);
            Argument cleanup{selected};
            if (!selected.baseAt00) throw 1;
            auto* child = ResolveChild(selected);
            const auto state = child->vptrAt00->resourceStateAt38(child);
            if (state == 1 || state == 2) { cleanup.Finish(true); continue; }
            body->vptrAt00->setActiveAt30(body, ResolveChild(selected));
            body->inputLatchAt30 = 1;
            cleanup.Finish();
            return 3;
        }
    }
    const auto first = body->activeAt4C->eventAt04;
    auto event = first;
    for (;;)
    {
        auto* navigation = Navigation(body, static_cast<std::uint32_t>(index));
        event = navigation->vptrAt00->nextAt00(navigation, event);
        if (event < 0 || event == first) return fallback;
        Slot selected;
        body->vptrAt00->findChildAt14(body, &selected, event);
        Argument cleanup{selected};
        if (!selected.baseAt00) throw 1;
        auto* child = ResolveChild(selected);
        const auto state = child->vptrAt00->resourceStateAt38(child);
        if (state == 1 || state == 2) { cleanup.Finish(true); continue; }
        body->vptrAt00->resetChildrenAt68(body);
        body->vptrAt00->setActiveAt30(body, ResolveChild(selected));
        body->inputLatchAt30 = 1;
        cleanup.Finish();
        return 3;
    }
}
void __fastcall ProcessInput(Body* body, void*)
{
    if (!body->vptrAt00->enabledAt2C(body)) return;
    StateTree states;
    for (auto* position = body->childrenAt0C.begin; position != body->childrenAt0C.end; ++position)
    {
        auto* child = ResolveChild(*position);
        const auto state = child->vptrAt00->resourceStateAt38(child);
        const auto event = ResolveChild(*position)->eventAt04;
        states.Snapshot(event, state);
    }
    if (!body->vptrAt00->upperInputAt34(body))
    {
        for (auto* position = body->childrenAt0C.begin; position != body->childrenAt0C.end; ++position)
        {
            auto* child = ResolveChild(*position);
            if (!child->vptrAt00->keyboardAt14(child))
            {
                child = ResolveChild(*position);
                child->vptrAt00->mouseAt18(child);
            }
        }
    }
    for (auto* position = body->childrenAt0C.begin; position != body->childrenAt0C.end; ++position)
    {
        const auto previous = states.LookupOrZero(ResolveChild(*position)->eventAt04);
        auto* child = ResolveChild(*position);
        bool changed = previous != child->vptrAt00->resourceStateAt38(child);
        if (!changed && ResolveChild(*position)->typeAt08 == 5)
        {
            // type5的额外6C槽，不把它假扮成现代控件接口。
            child = ResolveChild(*position);
            const auto* table = reinterpret_cast<const GuiCheckBoxNativeVtable30*>(child->vptrAt00);
            changed = table->pressedAt6C(child) != 0;
        }
        if (changed)
        {
            child = ResolveChild(*position);
            const auto rectFunction = child->vptrAt00->rectAt3C;
            const auto* dialogTable = body->vptrAt00; // 518EBE先缓存表，rect回调之后才读64槽。
            RECT output;
            const auto* rect = rectFunction(child, &output);
            dialogTable->invalidateAt64(body, *rect);
            Slot argument;
            Copy(argument, *position);
            body->vptrAt00->childChangedAt88(body, argument);
        }
    }
}
void __fastcall FlushDirty(Body* body, void*)
{
    auto* dirty = body->vptrAt00->dirtyAt00(body);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = body->vptrAt00->dirtyAt00(body);
    const auto count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if (!count) return;
    dirty = body->vptrAt00->dirtyAt00(body);
    auto* rectangles = dirty->vptrAt00->rectsAt1C(dirty);
    for (std::int32_t index = 0; index < count; ++index)
    {
        body->vptrAt00->drawAt60(body, rectangles + index);
        RequireGuiDialogNativeScreenSource().InvalidateRectAt52603A(rectangles + index);
    }
    dirty = body->vptrAt00->dirtyAt00(body);
    dirty->vptrAt00->clearAt00(dirty);
}
void __fastcall Composite(Body* body, void*, Slot argument, const RECT* clip)
{
    Argument cleanup{argument};
    if (body->enabledAt38)
        for (auto* position = body->childrenAt0C.begin; position != body->childrenAt0C.end; ++position)
        {
            const bool normal = body->stateAt34 != 0;
            std::int32_t previous = 0;
            auto* child = ResolveChild(*position);
            if (!normal)
            {
                previous = child->vptrAt00->readStateAt48(child);
                child = ResolveChild(*position);
                child->vptrAt00->writeStateAt44(child, 0);
                child = ResolveChild(*position);
            }
            Slot copied;
            Copy(copied, argument);
            child->vptrAt00->compositeAt10(child, copied, clip);
            if (!normal)
            {
                child = ResolveChild(*position);
                child->vptrAt00->writeStateAt44(child, previous);
            }
        }
    cleanup.Finish();
}
void __fastcall Draw(Body* body, void*, const RECT* clip)
{
    if (!body->enabledAt38) return;
    for (auto* position = body->childrenAt0C.begin; position != body->childrenAt0C.end; ++position)
    {
        const bool normal = body->stateAt34 != 0;
        std::int32_t previous = 0;
        auto* child = ResolveChild(*position);
        if (!normal)
        {
            previous = child->vptrAt00->readStateAt48(child);
            child = ResolveChild(*position);
            child->vptrAt00->writeStateAt44(child, 0);
            child = ResolveChild(*position);
        }
        child->vptrAt00->drawAt0C(child, clip);
        if (!normal)
        {
            child = ResolveChild(*position);
            child->vptrAt00->writeStateAt44(child, previous);
        }
    }
}
void __fastcall Invalidate(Body* body, void*, RECT rectangle)
{
    if (body->parentAt04)
    {
        ::OffsetRect(&rectangle, body->xAt3C, body->yAt40);
        auto* parent = body->parentAt04;
        parent->vptrAt00->invalidateAt64(parent, rectangle);
    }
    else
    {
        auto* dirty = body->vptrAt00->dirtyAt00(body);
        dirty->vptrAt00->addRectAt08(dirty, &rectangle);
    }
}
void __fastcall ResetChildren(Body* body, void*)
{
    for (auto* position = body->childrenAt0C.begin; position != body->childrenAt0C.end; ++position)
    {
        auto* child = ResolveChild(*position);
        child->vptrAt00->resetAt08(child);
    }
}
std::int32_t __fastcall Held(Body* body, void*, std::int32_t action)
{
    if (auto* parent = body->parentAt04) return parent->vptrAt00->heldAt6C(parent, action);
    auto* input = Input(body); return input->vptrAt00->readHeldAt00(input, action);
}
std::int32_t __fastcall Pressed(Body* body, void*, std::int32_t action)
{
    if (auto* parent = body->parentAt04) return parent->vptrAt00->pressedAt70(parent, action);
    auto* input = Input(body); return input->vptrAt00->readPressedAt04(input, action);
}
POINT* __fastcall Pointer(Body* body, void*, POINT* output)
{
    if (auto* parent = body->parentAt04) parent->vptrAt00->pointerAt74(parent, output);
    else { auto* input = Input(body); input->vptrAt00->readPointerAt08(input, output); }
    return output; // 50A6B6/CF/F5忽略下游返回指针，始终返回本次调用者输出。
}
std::int32_t __fastcall Wheel(Body* body, void*)
{
    if (auto* parent = body->parentAt04) return parent->vptrAt00->wheelAt78(parent);
    auto* input = Input(body); return input->vptrAt00->readWheelAt0C(input);
}
std::int32_t __fastcall Moved(Body* body, void*)
{
    if (auto* parent = body->parentAt04) return parent->vptrAt00->movedAt7C(parent);
    auto* input = Input(body); return input->vptrAt00->readPointerMovedAt10(input);
}
std::int32_t __fastcall MousePress(Body* body, void*)
{
    if (auto* parent = body->parentAt04) return parent->vptrAt00->mousePressAt80(parent);
    auto* input = Input(body); return input->vptrAt00->readMousePressAt14(input);
}
std::int32_t __fastcall MouseRelease(Body* body, void*)
{
    if (auto* parent = body->parentAt04) return parent->vptrAt00->mouseReleaseAt84(parent);
    auto* input = Input(body); return input->vptrAt00->readMouseReleaseAt18(input);
}
void __fastcall ChildChanged(Body*, void*, Slot argument) noexcept { ReleaseWithoutClearing(argument); }

#define SLOT(name, function) reinterpret_cast<decltype(GuiDialogNativeVtable35::name)>(&function)
const GuiDialogNativeVtable35 table{
    SLOT(dirtyAt00, Dirty), SLOT(deletingDestructorAt04, Delete), SLOT(clearChildrenAt08, ClearChildren),
    SLOT(idAt0C, Id), SLOT(appendChildAt10, AppendChild), SLOT(findChildAt14, FindChild), SLOT(setDirtyAt18, SetDirty),
    SLOT(writeStateAt1C, WriteState), SLOT(readStateAt20, ReadState), SLOT(writeEnabledAt24, WriteEnabled),
    SLOT(readEnabledAt28, ReadEnabled), SLOT(enabledAt2C, Enabled), SLOT(setActiveAt30, SetActive),
    SLOT(upperInputAt34, UpperInput), SLOT(activeInputAt38, Zero), SLOT(resetInputAt3C, ResetInput),
    SLOT(priorityInputAt40, Zero), SLOT(beginAt44, Pure), SLOT(pollAt48, Pure), SLOT(endAt4C, Pure),
    SLOT(resultAt50, Result), SLOT(processInputAt54, ProcessInput), SLOT(flushDirtyAt58, FlushDirty),
    SLOT(compositeAt5C, Composite), SLOT(drawAt60, Draw), SLOT(invalidateAt64, Invalidate),
    SLOT(resetChildrenAt68, ResetChildren), SLOT(heldAt6C, Held), SLOT(pressedAt70, Pressed), SLOT(pointerAt74, Pointer),
    SLOT(wheelAt78, Wheel), SLOT(movedAt7C, Moved), SLOT(mousePressAt80, MousePress),
    SLOT(mouseReleaseAt84, MouseRelease), SLOT(childChangedAt88, ChildChanged)};
#undef SLOT

void DestroyChildren(Body* body) noexcept
{
    ReleaseGuiDirtyRectReferenceAt460500(body->dirtyAt50);
    DestroyGuiNavigationReferencesAt4F6E70(body->navigationAt24);
    ReleaseGuiInputReferenceAt4F6620(body->inputAt18);
    DestroyGuiControlChildrenAt4F6C60(body->childrenAt0C);
}
struct ConstructionUnwind final
{
    Body* body;
    bool active = true;
    ~ConstructionUnwind() noexcept { if (active) DestroyChildren(body); }
};
}

const GuiDialogNativeVtable35& GuiDialogBaseVtable() noexcept { return table; }
void ProcessGuiDialogInputAt518C30(GuiDialogNativeBody5Ch& body) { ProcessInput(&body, nullptr); }
Body* ConstructGuiDialogAt4E0BE0(Body* body, Body* parent, std::int32_t id)
{
    body->parentAt04 = parent;
    body->vptrAt00 = &table;
    body->idAt08 = id;
    body->childrenAt0C = {};
    body->inputAt18 = {};
    body->navigationAt24 = {};
    body->inputLatchAt30 = 0;
    body->stateAt34 = 1;
    body->enabledAt38 = 1;
    body->activeAt4C = nullptr;
    body->dirtyAt50 = {};
    ConstructionUnwind cleanup{body};
    ConstructNullGuiInputReferenceAt4E0BE0(body->inputAt18);
    CreateGuiDirtyRectReferenceAt4E0D42(body->dirtyAt50);
    // 4E0DFA..E11：四次分配成功后才写这些字段；失败保留调用方原字节。
    body->yAt40 = 0;
    body->xAt3C = 0;
    body->heightAt48 = 0;
    body->widthAt44 = 0;
    cleanup.active = false;
    return body;
}
void DestroyGuiDialogAt4F7AC0(Body* body) noexcept
{
    body->vptrAt00 = &table;
    DestroyChildren(body);
}
__declspec(noinline) GuiDialogNativeScope::GuiDialogNativeScope(IGuiDialogNativeScreenSource& source) noexcept
    : GuiDialogNativeScope(&source) {}
__declspec(noinline) GuiDialogNativeScope::GuiDialogNativeScope(IGuiDialogNativeScreenSource* source) noexcept
    : previous_(screenSource) { screenSource = source; }
__declspec(noinline) GuiDialogNativeScope::~GuiDialogNativeScope() noexcept { screenSource = previous_; }
__declspec(noinline) IGuiDialogNativeScreenSource& RequireGuiDialogNativeScreenSource()
{
    if (!screenSource) throw std::logic_error("GuiDialog screen source is not bound");
    return *screenSource;
}
}
