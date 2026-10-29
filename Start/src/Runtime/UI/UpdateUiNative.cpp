#include <Runtime/UpdateUiNative.h>

#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>
T Read(const T& value) noexcept
{
    return *static_cast<const volatile T*>(&value);
}

class CrtUpdateUiNativeMemory final : public IUpdateUiNativeMemory
{
public:
    void* NewArrayAt53FC09(std::size_t bytes) override
    {
        // 53FC09转53FBCB throwing new；不是malloc/null启发式兜底。
        return ::operator new[](bytes);
    }
    void DeleteArrayAt53FC12(void* block) override
    {
        // 原无尺寸delete[]转5406CD/54B2C0；不读cookie、不析构元素。
        ::operator delete[](block);
    }
    void FreeAt54B2C0(void* block) override
    {
        std::free(block);
    }
};
static_assert(std::is_trivially_destructible_v<CrtUpdateUiNativeMemory>);
}

IUpdateUiNativeMemory& DefaultUpdateUiNativeMemory() noexcept
{
    static CrtUpdateUiNativeMemory memory;
    return memory;
}

const std::array<UpdateUiNativeMap04h, 11>& UpdateUiNativeMapAt5AE9A0() noexcept
{
    static constexpr std::array<UpdateUiNativeMap04h, 11> map{{
        {0x800B, 1}, {0x800C, 1}, {0x800E, 1}, {0x800F, 1}, {0x8010, 1},
        {0x8011, 1}, {0x8012, 1}, {0x8018, 1}, {0x8019, 1}, {0x801A, 1},
        {0xFFFF, 0}
    }};
    return map;
}

UpdateUiNativeBody18h* InitializeUpdateUiNativeAt40B600(
    UpdateUiNativeBody18h& body, IUpdateUiNativeMemory& memory)
{
    body.elementsAt00 = nullptr;
    body.countAt04 = 0;
    body.capacityAt08 = 0;
    body.dataAt10 = nullptr;
    body.wordAt14 = 0;
    body.byteAt16 = 0;
    body.mapAt0C = UpdateUiNativeMapAt5AE9A0().data();

    // 40B670先前进再检查；固定原表含哨兵共11项，+17始终不写。
    auto* map = Read(body.mapAt0C);
    std::uint32_t count = 1;
    do
    {
        ++map;
        ++count;
    } while (Read(map->commandAt00) != 0xFFFFu);
    const auto product = std::uint64_t{count} * 8u;
    const auto bytes = product > (std::numeric_limits<std::uint32_t>::max)()
        ? (std::numeric_limits<std::uint32_t>::max)()
        : static_cast<std::uint32_t>(product);
    try
    {
        body.dataAt10 = static_cast<UpdateUiNativeData08h*>(memory.NewArrayAt53FC09(bytes));
    }
    catch (...)
    {
        // 40B6A0续接40B6AC：跳过返回值发布，不重抛、不补free/rollback。
    }
    // 分配叶外才晚读并memset；回调抛出时也不覆盖它可能改写的+10。
    if (auto* data = Read(body.dataAt10)) std::memset(data, 0, count * 8u);
    return &body;
}

void ReleaseUpdateUiNativeAt40EFB0(UpdateUiNativeBody18h& body, IUpdateUiNativeMemory& memory)
{
    auto* map = Read(body.mapAt0C);
    if (map)
    {
        auto* data = Read(body.dataAt10);
        if (data)
        {
            if (Read(map->commandAt00) != 0xFFFFu)
            {
                do
                {
                    // 两游标保留在局部；下一项map/flags/text均到原位置才读。
                    if (Read(data->flagsAt00) & 0x2000u)
                        memory.DeleteArrayAt53FC12(Read(data->textAt04));
                    ++map;
                    ++data;
                } while (Read(map->commandAt00) != 0xFFFFu);
                data = Read(body.dataAt10); // 40EFF7：遍历后重读，不释放旧游标/旧base。
            }
            // 首项就是哨兵则直接用入口data；允许NULL叶实参，不加判空。
            memory.DeleteArrayAt53FC12(data);
        }
    }
    // 40F004在data释放回调之后才读取数组base，Free成功后才清它。
    if (auto* elements = Read(body.elementsAt00))
    {
        memory.FreeAt54B2C0(elements);
        body.elementsAt00 = nullptr;
    }
    body.capacityAt08 = 0;
    body.countAt04 = 0;
}
}
