#pragma once

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class RootCriticalSectionNativeReference;

// 5ECC30两态均转541E55 terminate，没有Leave展开动作；空槽同样Enter/Leave。
// ownerMetadata只作已构造长期owner的非拥有定位，必须在Enter返回后再读。
void RetireRootProcessNativeAt5AB3C0(RootCriticalSectionNativeReference&,
    RootRuntimeOwner* const volatile& ownerMetadata) noexcept;

// 正式冷图在5AD62C之后构造完唯一owner才绑定；不Capture、不初始化正文、不接管C++析构。
// 绑定关系必须活过全部原回调与逃逸Ref，不用于游戏中的Root正文换代。
void BindProcessRootRuntimeOwnerMetadata(RootRuntimeOwner&) noexcept;
[[nodiscard]] RootRuntimeOwner* BorrowProcessRootRuntimeOwnerMetadata() noexcept;
}
