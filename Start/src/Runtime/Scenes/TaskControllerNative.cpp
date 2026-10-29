#include <Runtime/TaskControllerNative.h>
#include <bit>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=TaskControllerNativeBody18h;using Node=TaskControllerNodeNativeBody14h;using Item=TaskControllerItemNativeBody10h;
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
Item* Resolve(Node& node){return ResolveEnvironmentObjectReference<Item>(node.itemAt08);}
Item* ResolveCached(Node& node,std::uintptr_t base)
{
    if(!base)return nullptr;
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(node.itemAt08.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    return reinterpret_cast<Item*>(base+Read(node.itemAt08.biasAt04)*stride);
}
Node* Position(Body& body,Item* item)
{
    auto* const head=Read(body.headAt00);auto* node=Read(head->nextAt00);
    if(node==head)return node;
    auto priority=Read(item->priorityAt04); // 46A38E/46BF0B：首次节点base读取之前。
    while(node!=head)
    {
        const auto base=Read(node->itemAt08.baseAt00);
        auto* current=ResolveCached(*node,base);
        if(base)priority=Read(item->priorityAt04); // 46A3B0/46BF2F解析非空base后再次读取。
        if(priority<Read(current->priorityAt04))break;
        node=Read(node->nextAt00);
    }
    return node;
}
void InsertItem(Body& body,Item* item,TaskControllerNativeServices& services,bool clearLocal)
{
    auto* position=Position(body,item);YtlReferenceSlotRuntime reference;
    PublishTaskControllerItemNative(reference,item,services.memory);
    struct Unwind final
    {YtlReferenceSlotRuntime& ref;bool armed=true;~Unwind() noexcept{if(armed)ReleaseYtlReference(ref);}} unwind{reference};
    auto* const previous=Read(position->previousAt04);
    auto* node=CopyTaskListNodeNativeAt4520A0(position,previous,reference,services.memory);
    const auto count=Read(body.countAt04);
    if(0x0CCCCCCBu-count<1u)throw std::length_error("list<T> too long");
    body.countAt04=count+1u;position->previousAt04=node;previous->nextAt00=node;
    auto* control=reference.controlAt08;unwind.armed=false;
    ReleaseTaskControllerItemCachedNative(reference,control);
    // 46BFFD/46C004仅改优先级分支清local；46A451注册分支不清，bias两路均保留。
    if(clearLocal&&control){reference.controlAt08=nullptr;reference.baseAt00=0;}
}
Item* NewItem(TaskNativeBody4h* task,std::int32_t priority,TaskControllerNativeServices& services)
{
    auto* raw=services.memory.AllocateBody(0x10);
    if(!raw)return nullptr;
    auto* item=::new(raw) Item;item->taskAt00=task;item->priorityAt04=priority;
    item->requestedPriorityAt08=priority;item->removedAt0C=0;return item;
}
void DeleteBlocked(Body& body,TaskControllerNativeServices& services)
{
    auto* begin=Read(body.blockedAt0C.begin);if(!begin)return;
    const auto address=reinterpret_cast<std::uintptr_t>(begin);
    auto bytes=std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(body.blockedAt0C.capacity))-address)&~3u;
    void* raw=begin;
    if(bytes>=0x1000u)
    {
        std::memcpy(&raw,reinterpret_cast<void*>(address-4u),4);bytes+=0x23u;
        if(address-reinterpret_cast<std::uintptr_t>(raw)-4u>0x1Fu)_invalid_parameter_noinfo_noreturn();
    }
    services.memory.DeleteVector(raw,bytes);body.blockedAt0C={};
}
}
Body* ConstructTaskControllerNativeAt45B975(Body& body,TaskControllerNativeServices& services)
{
    body.headAt00=nullptr;body.countAt04=0;
    body.headAt00=NewTaskListNodeNativeAt48F550(nullptr,nullptr,services.memory);
    body.blockedAt0C={};ClearTaskControllerNativeAt4912B0(body);
    // 原WORD8=0，尾A/B不写；构造末尾仅end=begin。
    *reinterpret_cast<volatile std::uint16_t*>(reinterpret_cast<std::byte*>(&body)+8)=0;
    body.blockedAt0C.end=body.blockedAt0C.begin;return &body;
}
Body* NewTaskControllerNativeAt45B960(TaskControllerNativeServices& services)
{
    auto* raw=services.memory.AllocateBody(0x18);if(!raw)return nullptr;
    struct Unwind final
    {void* raw;IEnvironmentRootMemory& memory;bool armed=true;~Unwind() noexcept{if(armed)memory.DeleteBody(raw,0x18);}} unwind{raw,services.memory};
    auto* body=ConstructTaskControllerNativeAt45B975(*::new(raw) Body,services);unwind.armed=false;return body;
}
void RegisterTaskNativeAt46A330(Body& body,TaskNativeBody4h* task,std::int32_t priority,TaskControllerNativeServices& services)
{InsertItem(body,NewItem(task,priority,services),services,false);}
void ClearTaskControllerNativeAt4912B0(Body& body) noexcept
{
    auto* head=Read(body.headAt00);auto* node=Read(head->nextAt00);
    head->nextAt00=head;head=Read(body.headAt00);head->previousAt04=head;body.countAt04=0;
    while(node!=Read(body.headAt00))
    {
        auto* next=Read(node->nextAt00);ReleaseYtlReference(node->itemAt08);
        ::operator delete(node,std::size_t{0x14});node=next;
    }
}
void DestroyTaskControllerNativeAt4644A0(Body& body,TaskControllerNativeServices& services) noexcept
{
    DeleteBlocked(body,services);ClearTaskControllerNativeAt4912B0(body);
    ::operator delete(Read(body.headAt00),std::size_t{0x14});
}
void DeleteTaskControllerNative(Body* body,TaskControllerNativeServices& services) noexcept
{if(body){DestroyTaskControllerNativeAt4644A0(*body,services);body->~Body();::operator delete(body,std::size_t{0x18});}}
void RunTaskControllerNativeAt46BC90(Body& body,std::int32_t argument,TaskControllerNativeServices& services)
{
    std::int32_t previousPriority=-1;bool enabled=true;
    auto* head=Read(body.headAt00);auto* node=Read(head->nextAt00);
    while(node!=head)
    {
        const auto base=Read(node->itemAt08.baseAt00);
        if(!Read(ResolveCached(*node,base)->removedAt0C))
        {
            if(Read(ResolveCached(*node,base)->priorityAt04)!=previousPriority)
            {
                previousPriority=Read(ResolveCached(*node,base)->priorityAt04);
                const auto priority=Read(ResolveCached(*node,base)->priorityAt04);
                auto* end=Read(body.blockedAt0C.end);auto* at=Read(body.blockedAt0C.begin);
                while(at!=end&&Read(*at)!=priority)++at;
                enabled=at==end;
            }
            if(enabled)
            {
                auto* task=Read(Resolve(*node)->taskAt00);
                const auto result=task->vptrAt00->runAt00(task,argument);
                if(!result)Resolve(*node)->removedAt0C=1;
            }
        }
        node=Read(node->nextAt00);head=Read(body.headAt00);
    }
    if(Read(body.reprioritizeAt09))
    {
        for(;;)
        {
            head=Read(body.headAt00);node=Read(head->nextAt00);
            if(node==head){body.reprioritizeAt09=0;break;}
            const auto base=Read(node->itemAt08.baseAt00);
            // 原PE46BE27/6B短跳转都回46BE04同一节点。保留非终止域，不加skip/次数guard。
            for(;;)
            {
                if(Read(ResolveCached(*node,base)->removedAt0C))continue;
                const auto priority=Read(ResolveCached(*node,base)->priorityAt04);
                if(priority!=Read(ResolveCached(*node,base)->requestedPriorityAt08))break;
            }
            ResolveCached(*node,base)->removedAt0C=1;
            if(node==Read(body.headAt00)) {body.reprioritizeAt09=0;break;}
            const auto itemBase=Read(node->itemAt08.baseAt00);
            const auto priority=Read(ResolveCached(*node,itemBase)->requestedPriorityAt08);
            auto* task=Read(ResolveCached(*node,itemBase)->taskAt00);
            InsertItem(body,NewItem(task,priority,services),services,true);
            if(!Read(body.reprioritizeAt09))break;
        }
    }
    head=Read(body.headAt00);node=Read(head->nextAt00);
    while(node!=head)
    {
        if(Read(Resolve(*node)->removedAt0C))
        {auto* next=Read(node->nextAt00);EraseTaskControllerNodeNative(body,node);node=next;}
        else node=Read(node->nextAt00);
        head=Read(body.headAt00);
    }
}
}
