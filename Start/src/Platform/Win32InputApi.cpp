#include <Platform/Input.h>

namespace KaGuYa::Start::Platform
{
void Win32MousePositionApi::ReadScreenPosition(POINT& position) const{GetCursorPos(&position);}
void Win32MousePositionApi::ConvertToClient(HWND window,POINT& position) const{ScreenToClient(window,&position);}
bool Win32MousePositionApi::IsWindowMinimized(HWND window) const{return IsIconic(window)!=FALSE;}
HCURSOR Win32CursorApi::Load(HINSTANCE instance,LPCSTR resource) const{return LoadCursorA(instance,resource);}
void Win32CursorApi::Set(HCURSOR cursor) const{SetCursor(cursor);}
HCURSOR Win32CursorApi::Get() const{return GetCursor();}
void Win32CursorApi::ReadScreenPosition(POINT& position) const{GetCursorPos(&position);}
void Win32CursorApi::ConvertToClient(HWND window,POINT& position) const{ScreenToClient(window,&position);}
void Win32CursorApi::ReadClientRectangle(HWND window,RECT& rectangle) const{GetClientRect(window,&rectangle);}
bool Win32CursorApi::Contains(const RECT& rectangle,POINT position) const{return PtInRect(&rectangle,position)!=FALSE;}
}
