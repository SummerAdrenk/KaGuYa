#pragma once

namespace KaGuYa::Start::Runtime
{
// 两个参数分别为原声音值正文地址、真实native18地址，不接受现代Buffer。
// 48C9C0 / 48BFB0：BGM共用Basic；Child仅多处理+0C开关。
void SerializeSoundBasicValueNative18(void* body, void* native18h);
void SerializeSoundChildValueNative18(void* body, void* native18h);
// 48BFF0 / 48BD00：保留原正文slot00动态分派，不能用于当前legacy表。
// 须由主线统一迁移现有正文及factory产物的slot00后再接线、验证。
void SerializeSoundEffectValueNative18(void* body, void* native18h);
void SerializeSoundKeyValueNative18(void* body, void* native18h);
}
