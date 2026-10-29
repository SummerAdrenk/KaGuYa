#include "WindowClassNative.Internal.h"
#include <cstring>

namespace KaGuYa::Start::Runtime
{
using namespace WindowClassNativeDetail;

ATOM __cdecl RegisterWindowClassNativeAt451DA0(WindowClassModuleNativeBody38h* module,
    WindowClassWinModuleNativeBody2Ch* winModule, WindowClassNativeInfo68h* info, WNDPROC* previous)
{
    return RegisterWindowClassNativeAt451DA0(module, winModule, info, previous, CurrentApi());
}

ATOM RegisterWindowClassNativeAt451DA0(WindowClassModuleNativeBody38h* module,
    WindowClassWinModuleNativeBody2Ch* winModule, WindowClassNativeInfo68h* info,
    WNDPROC* previous, IWindowClassNativeApi& api)
{
    if (!module) return 0;
    if (!winModule) return 0;
    if (!info) return 0;
    if (!previous) return 0;
    if (!Read(info->atomAt40))
    {
        RegistrationFrame6Ch frame;
        frame.savedLockAt68 = &winModule->lockAt04;
        api.EnterCriticalSection(frame.savedLockAt68);
        if (!Read(info->atomAt40))
        {
            auto& descriptor = info->descriptorAt00;
            LPCSTR className;
            const auto superclass = Read(info->superclassAt30);
            if (superclass)
            {
                frame.savedProcedureAt64 = Read(descriptor.lpfnWndProc);
                className = Read(descriptor.lpszClassName);
                auto& source = frame.superclassAt30;
                source.cbSize = 0x30;
                const auto systemResult = api.GetClassInfoExA(nullptr, superclass, &source);
                if (!systemResult)
                {
                    // 451E1B先取名称，451E1E再取模块；首查询可以改变两者。
                    const auto lateSuperclass = Read(info->superclassAt30);
                    const auto instance = Read(module->instanceAt04);
                    if (!api.GetClassInfoExA(instance, lateSuperclass, &source))
                    {
                        api.LeaveCriticalSection(frame.savedLockAt68);
                        return 0; // 451E35不走prevProc尾，即便查询/释放期间改变了记录。
                    }
                }
                std::memcpy(&descriptor, &source, 0x30);
                info->previousProcedureAt34 = Read(descriptor.lpfnWndProc);
                descriptor.lpszClassName = className;
                descriptor.lpfnWndProc = frame.savedProcedureAt64;
            }
            else
            {
                const auto instance = Read(info->systemCursorAt3C) ? nullptr : Read(module->resourceInstanceAt08);
                const auto cursorName = Read(info->cursorNameAt38);
                const auto cursor = api.LoadCursorA(instance, cursorName);
                className = Read(descriptor.lpszClassName); // 原先重读名称，后发布cursor。
                descriptor.hCursor = cursor;
            }
            auto instance = Read(module->instanceAt04);
            descriptor.style = Read(descriptor.style) & 0xFFFFBFFFu;
            descriptor.hInstance = instance;
            if (!className)
            {
                className = info->generatedNameAt42;
                static_cast<void>(api.FormatAtlName(info->generatedNameAt42, 37, info));
                instance = Read(descriptor.hInstance);
                descriptor.lpszClassName = className;
            }
            auto& existing = frame.existingAt00;
            std::memcpy(&existing, &descriptor, 0x30);
            const auto existingResult = api.GetClassInfoExA(instance, className, &existing);
            const auto existingAtom = static_cast<ATOM>(existingResult);
            info->atomAt40 = existingAtom; // AX，不是BOOL非零归一化；高16位不参与判断。
            if (!existingAtom)
            {
                const auto atom = api.RegisterClassExA(&descriptor);
                if (atom)
                {
                    static_cast<void>(AppendAtomAt451EF3(*winModule, atom, &frame.aliasMarkerAt60, api));
                }
                // 追踪失败也缓存atom；异常展开则没有此写入，也没有解锁资格。
                info->atomAt40 = atom;
            }
        }
        api.LeaveCriticalSection(&winModule->lockAt04);
    }
    // 451F74..451F7F在释放锁后才读；不返回本轮保存的atom/prevProc。
    if (Read(info->superclassAt30)) *previous = Read(info->previousProcedureAt34);
    return Read(info->atomAt40);
}
}
