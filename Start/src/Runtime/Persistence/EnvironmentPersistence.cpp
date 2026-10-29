#include <Runtime/EnvironmentPersistence.h>
#include <Runtime/CategorizedSoundEnvironmentFile.h>
#include <Runtime/GameBuildProfile.h>

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct EnvironmentFileArchive final
{
    SaveDataEnvironmentArchive& save;
    CategorizedSoundEnvironmentAdapter* categorized;
    void Serialize(SerializeBuffer& serializer)
    {
        if(CompiledGameProfile().soundSettings==GameBuildProfile::SoundSettings::Categorized)
        {
            if(!categorized)throw std::logic_error("Categorized env file has no registration metadata");
            SerializeCategorizedEnvironmentFile(save,serializer,*categorized);
        }
        else save.Serialize(serializer);
    }
};
template <typename Archive>
bool TryLoadOne(
    const std::string& path,
    Archive& archive,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& fileApi)
{
    std::vector<std::byte> bytes;
    if (ReadWholePersistenceFile(path.c_str(), bytes, transactionContext, fileApi) !=
        PersistenceIoStatus::Success)
    {
        return false;
    }

    SerializeBuffer serializer;
    serializer.MutableBytes() = std::move(bytes);
    serializer.SetWriting(false);
    archive.Serialize(serializer);
    return true;
}

template <typename Archive>
bool WriteOne(
    const std::string& path,
    Archive& archive,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& fileApi)
{
    SerializeBuffer serializer;
    archive.Serialize(serializer);
    return WriteWholePersistenceFile(
               path.c_str(), serializer.Bytes(), transactionContext, fileApi) ==
           PersistenceIoStatus::Success;
}
}

EnvironmentPersistenceCoordinator::EnvironmentPersistenceCoordinator(
    SaveDataEnvironmentArchive& saveData,
    SecretCgArchive& secretCg,
    SecretScArchive& secretSc,
    GlobalParamsArchive& globalParams,
    const IEnvironmentKeyByteSource& keyBytes,
    EnvironmentPersistencePaths paths,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& fileApi,CategorizedSoundEnvironmentAdapter* categorizedSound) noexcept
    : saveData_(saveData),
      secretCg_(secretCg),
      secretSc_(secretSc),
      globalParams_(globalParams),
      keyBytes_(keyBytes),
      paths_(std::move(paths)),
      transactionContext_(transactionContext),
      fileApi_(fileApi),categorizedSound_(categorizedSound)
{
}

bool EnvironmentPersistenceCoordinator::TryLoadEnvironment()
{
    // 480230：env.ini 只包含槽 1 的 CSaveData；失败时由调用者立即写默认值。
    EnvironmentFileArchive archive{saveData_,categorizedSound_};
    return TryLoadOne(paths_.env, archive, transactionContext_, fileApi_);
}

bool EnvironmentPersistenceCoordinator::WriteEnvironment()
{
    // 488740：正常退出无条件重新序列化槽 1。
    EnvironmentFileArchive archive{saveData_,categorizedSound_};
    return WriteOne(paths_.env, archive, transactionContext_, fileApi_);
}

bool EnvironmentPersistenceCoordinator::TryLoadSecret()
{
    std::vector<std::byte> bytes;
    if (ReadWholePersistenceFile(
            paths_.secret.c_str(), bytes, transactionContext_, fileApi_) !=
        PersistenceIoStatus::Success)
    {
        // 481220：secret.ini 不存在或读取失败时，槽 3/4 的 flag 全部清零。
        secretCg_.FillFlagsAtVslot18(0);
        secretSc_.FillFlagsAtVslot18(false);
        return false;
    }

    SerializeBuffer serializer(keyBytes_);
    serializer.MutableBytes() = std::move(bytes);
    serializer.SetWriting(false);
    secretCg_.Serialize(serializer);
    secretSc_.Serialize(serializer);
    return true;
}

bool EnvironmentPersistenceCoordinator::WriteSecret()
{
    // 489250：两个 archive 共用一个 CSerialize，顺序固定为 CG 后 Scene。
    SerializeBuffer serializer(keyBytes_);
    secretCg_.Serialize(serializer);
    secretSc_.Serialize(serializer);
    return WriteWholePersistenceFile(
               paths_.secret.c_str(), serializer.Bytes(), transactionContext_, fileApi_) ==
           PersistenceIoStatus::Success;
}

bool EnvironmentPersistenceCoordinator::TryLoadGlobal()
{
    // 489C90:48A2C6..48A31E：读取失败时保留构造阶段已设定的默认值。
    std::vector<std::byte> bytes;
    if (ReadWholePersistenceFile(paths_.global.c_str(), bytes, transactionContext_, fileApi_) !=
        PersistenceIoStatus::Success)
        return false;
    SerializeBuffer serializer(keyBytes_);
    serializer.MutableBytes() = std::move(bytes);
    serializer.SetWriting(false);
    globalParams_.Serialize(serializer);
    return true;
}

bool EnvironmentPersistenceCoordinator::WriteGlobal()
{
    // 488EE0：global.ini 只序列化槽 7 的 CGlobalParams。
    SerializeBuffer serializer(keyBytes_);
    globalParams_.Serialize(serializer);
    return WriteWholePersistenceFile(paths_.global.c_str(), serializer.Bytes(), transactionContext_, fileApi_) ==
        PersistenceIoStatus::Success;
}
}
