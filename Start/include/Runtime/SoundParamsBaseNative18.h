#pragma once

namespace KaGuYa::Start::Runtime
{
// 48BB40：10h声音基类的原生协议；所有元素slot00须已接受native18。
void SerializeSoundParamsBaseNative18At48BB40(void* soundParams10, void* serializer18);
// 48BC77的栈临时构造不写kind+08；返回后调用者才取得464A60清理资格。
void ConstructSoundKeyValueStackNativeAt48BC77(void* storage18);
void RetireSoundKeyValueStackNativeAt464A60(void* storage18) noexcept;
}
