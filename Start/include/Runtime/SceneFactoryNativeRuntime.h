#pragma once
#include <Runtime/SceneManagerNative.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
enum class SceneFactoryNativeBranch : std::uint8_t
{
    Logo,Title,Music,Cg,Scene,Log,Save,Load,LoadAfter,Script,Setting,Affection,
    Movie,Ending,Text,Program,Graffiti,LegacyMenuSelection,LegacyCharacterSelection,LegacyItemAcquisition,LegacyMapSelection
};
struct SceneFactoryNativeRecord final
{
    SceneFactoryNativeBranch branch;
    const char* nameCp932;
    std::uint32_t nameAddress;
    std::uint32_t bodySize;
    // 只作原证定位；Logo/LoadAfter是内联起点，不可把此值统一转成ctor函数指针。
    std::uint32_t constructorAddress;
};
const std::array<SceneFactoryNativeRecord,17>& SceneFactoryNativeRecords() noexcept;
const SceneFactoryNativeRecord& MatchSceneFactoryNativeAt4AAF90(const char*);

class ISceneFactoryNativeConstructors
{
public:
    virtual ~ISceneFactoryNativeConstructors()=default;
    // storage是工厂唯一分配的原尺寸正文；ctor负责自己的成员展开，不二次分配task。
    virtual TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void* storage)=0;
    // 原表尺寸仅适用于对应正文；统一内部模型由拥有构造器明确选sizeof，不按游戏偏移分配。
    virtual std::size_t AllocationSize(SceneFactoryNativeBranch, std::size_t recordedSize) const noexcept
    { return recordedSize; }
    virtual SceneFactoryNativeTaskServices TaskServices() const noexcept{return {};}
};
class SceneFactoryNativeRuntime final:public ISceneFactoryNativeSource
{
public:
    SceneFactoryNativeRuntime(ISceneFactoryNativeConstructors& constructors,
        IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory()) noexcept
        :constructors_(constructors),memory_(memory){}
    TaskNativeBody4h* CreateAt4AAF90(const char*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override
    {return constructors_.TaskServices();}
private:
    ISceneFactoryNativeConstructors& constructors_;
    IEnvironmentRootMemory& memory_;
};

// 尚未迁移的分支是必填的原生构造依赖，不调用现代shared_ptr工厂补位。
class DemoSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    DemoSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,
        DemoSceneNativeServices& demo) noexcept:other_(other),demo_(demo){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Logo||b==SceneFactoryNativeBranch::Ending?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    DemoSceneNativeServices& demo_;
};
class TextSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    TextSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,
        TextDisplayNativeServices& text) noexcept:other_(other),text_(text){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Text?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    TextDisplayNativeServices& text_;
};
class GraffitiSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    GraffitiSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,
        GraffitiPageNativeServices& graffiti) noexcept:other_(other),graffiti_(graffiti){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Graffiti?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    GraffitiPageNativeServices& graffiti_;
};
class LoadAfterSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    LoadAfterSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,
        LoadAfterSelectionNativeServices& loadAfter) noexcept:other_(other),loadAfter_(loadAfter){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::LoadAfter?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    LoadAfterSelectionNativeServices& loadAfter_;
};
class ReadScriptSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    ReadScriptSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,
        ReadScriptSceneNativeServices& readScript) noexcept:other_(other),readScript_(readScript){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Script?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    ReadScriptSceneNativeServices& readScript_;
};
class TitleSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    TitleSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,
        TitleSceneNativeServices& title) noexcept:other_(other),title_(title){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Title?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    TitleSceneNativeServices& title_;
};
class SaveLoadSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    SaveLoadSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,
        SaveLoadSceneNativeServices& saveLoad) noexcept:other_(other),saveLoad_(saveLoad){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Save||b==SceneFactoryNativeBranch::Load?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    SaveLoadSceneNativeServices& saveLoad_;
};
class SettingSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    SettingSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,
        SettingSceneNativeServices& setting) noexcept:other_(other),setting_(setting){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Setting?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    SettingSceneNativeServices& setting_;
};
class LogSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    LogSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,LogSceneNativeServices& log) noexcept
        :other_(other),log_(log){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Log?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    LogSceneNativeServices& log_;
};
class MusicSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    MusicSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,MusicSceneNativeServices& music) noexcept
        :other_(other),music_(music){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Music?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    MusicSceneNativeServices& music_;
};
class CgSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    CgSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,CgSceneNativeServices& cg) noexcept
        :other_(other),cg_(cg){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Cg?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    CgSceneNativeServices& cg_;
};
class ReplaySceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    ReplaySceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,ReplaySceneNativeServices& replay)noexcept
        :other_(other),replay_(replay){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Scene?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*) override;
    SceneFactoryNativeTaskServices TaskServices()const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    ReplaySceneNativeServices& replay_;
};
class ProgramSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    ProgramSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,ProgramSceneNativeServices& program)noexcept
        :other_(other),program_(program){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Program?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*)override;
    SceneFactoryNativeTaskServices TaskServices()const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    ProgramSceneNativeServices& program_;
};
class MovieSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    MovieSceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,MovieSceneNativeServices& movie)noexcept
        :other_(other),movie_(movie){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Movie?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*)override;
    SceneFactoryNativeTaskServices TaskServices()const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    MovieSceneNativeServices& movie_;
};
class LikabilitySceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    LikabilitySceneFactoryNativeConstructorsRuntime(ISceneFactoryNativeConstructors& other,LikabilitySceneNativeServices& likability)noexcept
        :other_(other),likability_(likability){}
    std::size_t AllocationSize(SceneFactoryNativeBranch b,std::size_t n) const noexcept override
    {return b==SceneFactoryNativeBranch::Affection?n:other_.AllocationSize(b,n);}
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void*)override;
    SceneFactoryNativeTaskServices TaskServices()const noexcept override;
private:
    ISceneFactoryNativeConstructors& other_;
    LikabilitySceneNativeServices& likability_;
};
}
