#pragma once

namespace KaGuYa::Start::Runtime
{
// Start.exe:48B150。借用原90h正文及native ISerialize；+1C正文slot00须已接受native协议。
// 不构造/复制对象或替换vptr；不能将native参数交给现有legacy Serialize虚槽。
void SerializeSaveDataNative18At48B150(void* nativeSave90, void* nativeSerializer18);
// 仅供显式文件适配入口使用；默认slot00和设置确认快照不更换协议或vptr。
using NativeSaveDataSoundSerialize = void(*)(void* sound,void* nativeSerializer18,void* context);
void SerializeSaveDataNative18WithSound(void* nativeSave90,void* nativeSerializer18,
    NativeSaveDataSoundSerialize,void* context);
}
