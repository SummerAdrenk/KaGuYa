#pragma once
#include <cstdint>

namespace KaGuYa::Sound
{
// 原StreamSoundPolling的五个DWORD游标；结束界限来自Open元数据，不来自读EOF的锁存。
struct SoundPollingCursor final
{
    std::uint32_t bufferBytes=0,bytesPerSecond=0,totalPcmBytes=0;
    std::uint32_t lastWrite=0,shortReadDebt=0,nextFill=0,lastPlay=0,played=0;

    void ResetForBuffer() noexcept
    {
        // 1000CD78..1000CDA0的写入顺序。
        lastPlay=0;played=0;lastWrite=0;nextFill=0;shortReadDebt=0;
    }
    bool AdvancePlay(std::uint32_t current) noexcept
    {
        played+=current>=lastPlay?current-lastPlay:bufferBytes-lastPlay+current;
        lastPlay=current;
        return played>=totalPcmBytes; // 1000DF17..1000DF27，无符号比较。
    }
    bool PlanRefill(std::uint32_t play,std::uint32_t write,std::uint32_t& requested) noexcept
    {
        // 1000E15D..1000E22B：使用第二次DS游标采样、半秒门槛和实际短读差额。
        if(write==lastWrite)return false;
        auto amount=(write>lastWrite?write-lastWrite:bufferBytes-lastWrite+write)+shortReadDebt;
        if(amount<(bytesPerSecond>>1))return false;
        shortReadDebt=0;
        const auto end=nextFill+amount;
        if(end>bufferBytes)
        {
            const auto tail=bufferBytes-nextFill;
            if(amount-tail>play)amount=tail+play;
        }
        else if(nextFill<play&&play<end)amount=play-nextFill;
        lastWrite=write;
        requested=amount&~std::uint32_t{3};
        return true;
    }
    void CommitRead(std::uint32_t requested,std::uint32_t actual) noexcept
    {
        // 1000DD82..1000DDA1：以实际读取字节推进，不把补零算成已读数据。
        nextFill=(nextFill+actual)%bufferBytes;
        if(actual<requested)shortReadDebt+=requested-actual;
    }
};
}
