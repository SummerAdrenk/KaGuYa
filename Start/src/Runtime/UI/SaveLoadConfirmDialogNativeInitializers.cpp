#include <Runtime/RootSaveLoadConfirmDialogNativeSource.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 与图形来源的业务方法分离，CRT/隔离退出不必拉入整套Root/Graphics依赖。
void Construct(RuntimeCString& fresh,const char* text,int length,IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(fresh,strings);
    struct Unwind final
    {
        RuntimeCString* value;
        ~Unwind()noexcept{if(value)DestroyRuntimeCStringAt40CD40(value);}
    } unwind{&fresh};
    SetRuntimeCStringAt437680(fresh,text,length);
    unwind.value=nullptr;
}
}
void ConstructSaveLoadConfirmMaskNameNativeAt406AA0(RuntimeCString& fresh,IRuntimeStringManagerSource& strings)
{
    // 旧确认框无独立遮罩；构造空槽以保持CRT生命周期，业务不装载该空串。
    const auto* name=CurrentSaveLoadResourceProfile().confirmation.mask;
    Construct(fresh,name?name:"",name?static_cast<int>(std::strlen(name)):0,strings);
}
void ConstructSaveLoadConfirmFontFaceNativeAt406B30(RuntimeCString& fresh,IRuntimeStringManagerSource& strings)
{
    const auto* face=CurrentSaveLoadResourceProfile().confirmation.text[3].face;
    Construct(fresh,face,static_cast<int>(std::strlen(face)),strings);
}
}
