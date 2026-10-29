#pragma once

#include <Runtime/SoundMappingRestoreSource.h>

namespace KaGuYa::Start::Runtime
{
// Start.exe:46D62E..46DB6B。读取 CUnSaveData+0x30 的两组映射，
// 按 CSoundManager 的专用槽边界恢复播放状态。
class SoundMappingRestoreRuntime final
{
public:
    explicit SoundMappingRestoreRuntime(ISoundMappingRestoreSource& source) noexcept;

    void Restore(bool soundMappingRestoreEnabledAt38Bit20);

private:
    ISoundMappingRestoreSource& source_;
};
}
