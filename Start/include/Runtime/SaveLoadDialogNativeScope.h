#pragma once
#include <Runtime/SaveLoadDialogNative.h>
#include <Runtime/SaveLoadMessageBoxNative.h>
#include <Runtime/SaveLoadConfirmDialogNative.h>

namespace KaGuYa::Start::Runtime
{
// 长寿元数据只借Source声明；scope覆盖创建/EH、命令及所有确认最后Ref，不拥有正文。
class SaveLoadDialogNativeConstructionScope final
{
public:
    explicit SaveLoadDialogNativeConstructionScope(SaveLoadDialogNativeConstructionServices& s) noexcept
        :delete_(s.source.NativeDeleteConfirmationServices()),slot_(s.source.NativeSlotConfirmationServices()),surfaces_(s.surfaces){}
private:
    SaveLoadMessageBoxNativeScope delete_;
    SaveLoadConfirmDialogNativeScope slot_;
    GraphicsSurfaceNativeScope surfaces_;
};
}
