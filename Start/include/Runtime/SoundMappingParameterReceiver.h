#pragma once
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class EnvironmentSoundParamsArchive;
class ISoundMappingRestoreParameterSource
{
public:
    virtual ~ISoundMappingRestoreParameterSource() = default;
    virtual bool HasLoopEffectType() const = 0;
    virtual std::int32_t LoopEffectValueOrThrow() const = 0;
    virtual std::int32_t NormalEffectValueOrThrow() const = 0;
};

// 4D19D5捕获的是正文地址，不是外置适配器；此值不分配、不增持、不延长来源寿命。
class SoundMappingParameterReceiver final
{
public:
    SoundMappingParameterReceiver(const ISoundMappingRestoreParameterSource& fixture) noexcept;
    static SoundMappingParameterReceiver Native(EnvironmentSoundParamsArchive& archive) noexcept;
    static SoundMappingParameterReceiver Native(EnvironmentSoundParamsArchive* archive) noexcept;
    // 原raw虚槽调用，无现代空正文保护；显式用于已按IDA核对的调用链。
    static SoundMappingParameterReceiver RawNative(EnvironmentSoundParamsArchive* archive) noexcept;
    bool HasLoopEffectType() const { return operations_->hasLoop(context_); }
    std::int32_t LoopEffectValueOrThrow() const { return operations_->loop(context_); }
    std::int32_t NormalEffectValueOrThrow() const { return operations_->normal(context_); }
    const void* Context() const noexcept { return context_; }
private:
    struct Operations
    {
        bool (*hasLoop)(const void*);
        std::int32_t (*loop)(const void*);
        std::int32_t (*normal)(const void*);
    };
    SoundMappingParameterReceiver(const void* context, const Operations& operations) noexcept
        : context_(context), operations_(&operations) {}
    const void* context_;
    const Operations* operations_;
};
}
