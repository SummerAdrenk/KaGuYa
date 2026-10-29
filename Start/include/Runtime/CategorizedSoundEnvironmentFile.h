#pragma once

namespace KaGuYa::Start::Runtime
{
class CategorizedSoundEnvironmentAdapter;
class SaveDataEnvironmentArchive;
class SerializeBuffer;
struct NativeSerialize18Body;

// 只在env文件入口调用；不更换声音对象/vptr，也不影响设置确认使用的Grouped快照。
void SerializeCategorizedEnvironmentFile(SaveDataEnvironmentArchive&,SerializeBuffer&,
    CategorizedSoundEnvironmentAdapter&);
void SerializeCategorizedEnvironmentFileNative(SaveDataEnvironmentArchive&,NativeSerialize18Body&,
    CategorizedSoundEnvironmentAdapter&);
}
