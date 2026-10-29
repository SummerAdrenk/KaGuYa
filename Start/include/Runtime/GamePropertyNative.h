#pragma once
#include <Runtime/PropertySheetNative.h>
#include <Runtime/PropertyPageTextNative.h>
#include <Runtime/PropertyPageSoundNative.h>
#include <Runtime/PropertyPageSystemNative.h>

namespace KaGuYa::Start::Runtime
{
struct GamePropertyNativeServices final
{
    PropertySheetNativeServices& sheet;
    WindowClassModuleNativeBody38h& module;
    PropertyPageTextNativeServices& text;
    PropertyPageSoundNativeServices& sound;
    PropertyPageSystemNativeServices& system;
    IRuntimeStringManagerSource& strings;
    DWORD& sheetButtonsAt6197EC;
};
class GamePropertyNativeScope final
{
public:
    explicit GamePropertyNativeScope(GamePropertyNativeServices&)noexcept;
    ~GamePropertyNativeScope()noexcept;
    GamePropertyNativeScope(const GamePropertyNativeScope&)=delete;
    GamePropertyNativeScope& operator=(const GamePropertyNativeScope&)=delete;
private:
    GamePropertyNativeServices* previous_;
    PropertySheetNativeScope sheet_;
    PropertyPageTextNativeScope text_;
    PropertyPageSoundNativeScope sound_;
    PropertyPageSystemNativeScope system_;
};
GamePropertyNativeServices& RequireGamePropertyNativeServices();
const EditWindowEndpointNativeVtable4& GamePropertyNativeVtable()noexcept;
DWORD& ProcessPropertySheetButtonsAt6197EC()noexcept;
GamePropertyNativeBody33Ch* ConstructGamePropertyNativeAt40B860(GamePropertyNativeBody33Ch&,DWORD startPage,GamePropertyNativeServices&);
// 原EHFlags5，正常析构和构造展开失败均不能把成员清理错误转换为设置取消。
void DestroyGamePropertyNativeAt40E840(GamePropertyNativeBody33Ch&,GamePropertyNativeServices&)noexcept;
PropertyPageTextNativeBody68h& GamePropertyTextNative(GamePropertyNativeBody33Ch&)noexcept;
PropertyPageMessWinNativeBody9Ch& GamePropertyMessWinNative(GamePropertyNativeBody33Ch&)noexcept;
PropertyPageSoundNativeBodyA8h& GamePropertySoundNative(GamePropertyNativeBody33Ch&)noexcept;
PropertyPageSaveLoadNativeBody5Ch& GamePropertySaveLoadNative(GamePropertyNativeBody33Ch&)noexcept;
PropertyPageSystem1NativeBody60h& GamePropertySystem1Native(GamePropertyNativeBody33Ch&)noexcept;
PropertyPageSystem2NativeBody68h& GamePropertySystem2Native(GamePropertyNativeBody33Ch&)noexcept;
}
