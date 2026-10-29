#pragma once

namespace KaGuYa::Start::Runtime
{
struct GuiDialogNativeBody5Ch;
struct MusicDialogNativeServices;

class IMusicDialogNativeConstruction
{
public:
    virtual ~IMusicDialogNativeConstruction() = default;
    virtual GuiDialogNativeBody5Ch* ConstructMusicDialogAt4E3900(void* storage84h) = 0;
    virtual MusicDialogNativeServices& NativeDialogServices() noexcept = 0;
};
}
