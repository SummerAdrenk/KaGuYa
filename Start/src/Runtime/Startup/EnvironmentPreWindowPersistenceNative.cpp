#include <Runtime/EnvironmentPreWindowPersistenceNative.h>
#include <Runtime/EnvironmentRegistryRuntime.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <Runtime/CategorizedSoundEnvironmentFile.h>
#include <Runtime/GameBuildProfile.h>
#include "../Persistence/NativeSerialize18.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
union PathStorage final
{
    RuntimeCStringConcatenation value;
    PathStorage(RuntimeStringBorrow prefix,const char* suffix,IRuntimeStringManagerSource& strings)
    {::new(&value)RuntimeCStringConcatenation(prefix,suffix,strings);}
    ~PathStorage() noexcept{}
};
struct PathLife final
{
    PathStorage& path;bool armed=true;
    ~PathLife() noexcept{if(armed)path.value.~RuntimeCStringConcatenation();}
    void Normal()
    {
        auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(path.value.GetString()))-1;
        armed=false;ReleaseRuntimeCStringHeader(header);
    }
};
struct SerializerLife final
{
    NativeSerialize18Body body;bool armed=true;
    SerializerLife() noexcept{ConstructNativeSerialize18InlineDword(&body);}
    ~SerializerLife() noexcept{if(armed)DestroyNativeSerialize18At463E60(&body);}
    void Normal(){DestroyNativeSerialize18At463E60(&body);armed=false;}
};
EnvironmentPathRuntime* CapturePaths(EnvironmentRegistryRuntime& registry)
{
    auto* current=registry.RootSource().CaptureAt4180C0();
    return ResolveEnvironmentObjectReference<EnvironmentPathRuntime>(current->pathsAt48);
}
}
bool ReadPreWindowEnvironmentAt480230(EnvironmentRegistryRuntime& registry, IStartFileApi& files,
    const FileTransactionContext* transaction)
{
    NativeSerialize18Services services{registry.Memory(),registry.Strings(),files,transaction};
    NativeSerialize18Scope scope(services);
    auto* system=static_cast<SystemDataEnvironmentArchive*>(registry.CaptureArchiveAt4180C0(0));
    auto* paths=CapturePaths(registry);
    PathStorage path(paths->saveAt10,system->stringAt28,registry.Strings());PathLife pathLife{path};
    SerializerLife serializer;
    const bool success=NativeSerialize18Detail::LoadFile(&serializer.body,nullptr,path.value.GetString())==0;
    if(success)
    {
        // 48031C先缓存表；真实机器码480321是byte mode0，cursor另写DWORD0。
        const auto* table=serializer.body.vptrAt00;
        serializer.body.writingAt10=0;serializer.body.cursorAt14=0;
        auto* selected=registry.CaptureArchiveAt4180C0(1);
        if(CompiledGameProfile().soundSettings==GameBuildProfile::SoundSettings::Categorized)
        {
            auto& save=*static_cast<SaveDataEnvironmentArchive*>(selected);
            SerializeCategorizedEnvironmentFileNative(save,serializer.body,
                registry.RootSource().RequireCategorizedSoundEnvironment(save));
        }
        else table->common.archiveAt5C(&serializer.body,selected);
    }
    // 480344仍有serializer展开资格；正常CString尾48034F先撤资格。
    serializer.Normal();pathLife.Normal();return success;
}
bool WritePreWindowEnvironmentAt488740(EnvironmentRegistryRuntime& registry, IStartFileApi& files,
    const FileTransactionContext* transaction)
{
    NativeSerialize18Services services{registry.Memory(),registry.Strings(),files,transaction};
    NativeSerialize18Scope scope(services);SerializerLife serializer;
    auto* selected=registry.CaptureArchiveAt4180C0(1);
    if(CompiledGameProfile().soundSettings==GameBuildProfile::SoundSettings::Categorized)
    {
        auto& save=*static_cast<SaveDataEnvironmentArchive*>(selected);
        SerializeCategorizedEnvironmentFileNative(save,serializer.body,
            registry.RootSource().RequireCategorizedSoundEnvironment(save));
    }
    else serializer.body.vptrAt00->common.archiveAt5C(&serializer.body,selected);
    // 4887EA/488831只在归档返回之后分别捕获SysData和路径。
    auto* system=static_cast<SystemDataEnvironmentArchive*>(registry.CaptureArchiveAt4180C0(0));
    auto* paths=CapturePaths(registry);
    PathStorage path(paths->saveAt10,system->stringAt28,registry.Strings());PathLife pathLife{path};
    const bool success=NativeSerialize18Detail::SaveFile(&serializer.body,nullptr,path.value.GetString())==0;
    pathLife.Normal();serializer.Normal();return success;
}
}
