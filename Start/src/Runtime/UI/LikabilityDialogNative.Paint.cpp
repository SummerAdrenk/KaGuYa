#include <Runtime/LikabilityResourceProfile.h>
#include <algorithm>
#include <bit>
#include <stdexcept>
#include <string>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原数组危险域只在实际访问处拒绝，不把负数/超界值改成0、绝对值或另一个角色。
template<class T> const T& At(std::span<T> values, std::int32_t index)
{
    if (index < 0 || static_cast<std::size_t>(index) >= values.size())
        throw std::out_of_range("Likability original table index outside proven extent: " + std::to_string(index));
    return values[static_cast<std::size_t>(index)];
}
std::int32_t Mul(std::int32_t a, std::int32_t b)
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b)); }
std::int32_t Add(std::int32_t a,std::int32_t b)
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a)+static_cast<std::uint32_t>(b)); }
std::int32_t Sub(std::int32_t a,std::int32_t b)
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a)-static_cast<std::uint32_t>(b)); }
POINT Plus(POINT a, POINT b) { return {a.x+b.x,a.y+b.y}; }
struct Painter final
{
    const LikabilityResourceProfile& profile;
    std::int32_t page;
    ILikabilityPaintTarget& out;
    void Natural(const char* image, POINT offset = {})
    {
        if (!image) throw std::runtime_error("Likability required image is null");
        out.Draw(image,Plus(out.Metrics(image).origin,offset));
    }
    void Digit(LikabilityNames digits, std::int32_t value, POINT point)
    { out.Draw(At(digits,value),point); }
    void Repeat(const char* image, std::int32_t value, LikabilityPoints points)
    { for (std::int32_t i=0; i<value && static_cast<std::size_t>(i)<points.size(); ++i) out.Draw(image,points[i]); }
    void Halves(const char* full, const char* half, std::int32_t value, LikabilityPoints points)
    {
        for (std::size_t i=0; value>0 && i<points.size(); ++i,value-=2)
            out.Draw(value>=2?full:half,points[i]);
    }
    void ThreeDigits(LikabilityNames digits, std::int32_t value, LikabilityPoints points,int alphabet=0,bool unitsFirst=false)
    {
        const auto hundreds=value/100, tens=(value/10)%10;
        if (unitsFirst) Digit(digits,alphabet+value%10,At(points,0));
        if (!unitsFirst && hundreds) Digit(digits,alphabet+hundreds,At(points,2));
        if (hundreds || tens) Digit(digits,alphabet+tens,At(points,1));
        if (unitsFirst && hundreds) Digit(digits,alphabet+hundreds,At(points,2));
        if (!unitsFirst) Digit(digits,alphabet+value%10,At(points,0));
    }
    void Paint(const LikabilityHighestData& d)
    {
        auto winner=0, maximum=out.Read(At(d.variables,0));
        for (int i=1;i<5;++i) { const auto value=out.Read(At(d.variables,i)); if(value>maximum){maximum=value;winner=i;} }
        for(int i=0;i<5;++i)
        {
            const auto hidden=i==4 && out.Read(d.appearance)==0;
            Natural(hidden?d.hiddenPortrait:At(d.portraits,4*i+(i!=winner)));
            auto value=out.Read(At(d.variables,i));
            const auto points=d.digitsAt.subspan(3*i,3);
            if(value>=100) { Digit(d.digits,1,points[0]); Digit(d.digits,0,points[1]); Digit(d.digits,0,points[2]); }
            else { if(value/10) Digit(d.digits,value/10,points[1]); Digit(d.digits,value%10,points[2]); }
        }
        Natural(At(d.portraits,4*winner+(out.Read(At(d.variables,winner))>=50?2:3)));
    }
    void Paint(const LikabilityRepeatedData& d)
    { for(const auto& row:d.rows) Repeat(row.image,out.Read(row.variable),row.positions); }
    void Paint(const LikabilityAnimatedData& d)
    {
        for(const auto& row:d.hearts) Repeat(row.image,out.Read(row.variable),row.positions);
        const auto obtained=out.Read(d.obtained),used=out.Read(d.used);
        if(used>obtained) return;
        // 02原式从0画used个off，再从used画到obtained；没有全局饱和。
        int i=0;
        for(;i<used;++i) out.Draw(d.off,At(d.clocks,i));
        for(;i<obtained;++i) out.Draw(d.on,At(d.clocks,i));
    }
    void Paint(const LikabilityRankData& d)
    {
        const auto value=out.Read(At(d.variables,4*page+3));
        int tier=0; while(static_cast<std::size_t>(tier)<d.thresholds.size() && value>=d.thresholds[tier]) ++tier;
        const auto rank=At(d.rankIndices,tier);
        Natural(At(d.ranks,rank),At(d.rankOffsets,0));
        if(page<3) Natural(At(d.ranks,rank+1),At(d.rankOffsets,1));
        if(page!=3) Natural(At(d.routes,out.Read(At(d.variables,4*page))),At(d.routeOffset,0));
        Repeat(At(d.marks,0),out.Read(At(d.variables,4*page+1)),d.loveAt);
        Repeat(At(d.marks,1),out.Read(At(d.variables,4*page+2)),d.lustAt);
    }
    void Paint(const LikabilityIndexedData& d)
    {
        for(int i=0;i<5;++i)
        {
            const auto value=out.Read(At(d.variables,i));
            Natural(At(d.ranks,5*i+value/5));
            Natural(At(d.remainders,6*i+value%5));
        }
        for(int i=0;i<5;++i) Natural(At(d.affection,Add(5*i,out.Read(At(d.affectionVariables,i)))));
    }
    void VariableWidth(LikabilityNames digits,LikabilityIntegers advances,int value,POINT origin,bool extra)
    {
        int width=0; for(const auto* digit:digits) width=std::max<int>(width,out.Metrics(digit).size.cx);
        auto right=(extra?2:3)*width;
        const auto put=[&](int digit){const auto* image=At(digits,digit);out.Draw(image,{origin.x+right-out.Metrics(image).size.cx,origin.y});right-=At(advances,digit);};
        put(value%10);
        const auto tens=value/10;
        if(extra) { if(tens) put(tens); }
        else if(tens && tens<100) { put(tens%10); if(tens/10) put(tens/10); }
    }
    void Paint(const LikabilityLoveHolicData& d)
    {
        if(page==2)
        {
            for(auto id:d.descriptionIds) out.Visible(id,false);
            for(const auto& item:d.items) out.Visible(item.event,out.Read(item.variable)!=0);
            return;
        }
        const auto rows=page==0?d.first:d.second;
        for(const auto& row:rows)
        {
            const auto raw=out.Read(row.variable),value=std::max(raw,0);
            const auto m=out.Metrics(d.gauge);
            RECT clip{row.barAt.x,row.barAt.y,row.barAt.x+Mul(m.size.cx,std::min(value,100))/100,row.barAt.y+m.size.cy};
            out.Draw(d.gauge,row.barAt,nullptr,&clip);
            // 08只给条形图上限100；数字保留原正值，负数单独归零。
            VariableWidth(d.digits,d.advances,value,row.numberAt,false);
        }
        if(page==1)
        {
            out.Visible(34,out.Read(d.releaseFlag)!=0);
            VariableWidth(d.smallDigits,d.smallAdvances,out.Read(d.extraValue),d.extraAt,true);
        }
    }
    void Paint(const LikabilityFamilyData& d)
    {
        const auto& row=At(d.rows,page);
        const auto affection=out.Read(row.affection),whole=affection/2;
        Repeat(At(d.icons,0),whole,row.affectionAt);
        const auto drawn=std::max(0,std::min(whole,static_cast<int>(row.affectionAt.size())));
        if(affection%2 && static_cast<std::size_t>(drawn)<row.affectionAt.size()) out.Draw(At(d.icons,1),row.affectionAt[drawn]);
        if(row.maternal) Repeat(At(d.icons,2),out.Read(row.maternal)/2,row.maternalAt);
        Repeat(At(d.icons,3),out.Read(row.third),row.thirdAt);
    }
    void Paint(const LikabilityHeartPairData& d)
    {
        const auto value=out.Read(At(d.variables,2*page));
        int i=0; for(;i<value/2 && static_cast<std::size_t>(i)<d.positions.size();++i)
        { out.Draw(At(d.icons,0),d.positions[i]); out.Draw(At(d.icons,1),d.positions[i]); }
        // 11奇数余量包含负奇数；原未检查第六个坐标，危险域由At明确拒绝。
        if(value%2) out.Draw(At(d.icons,0),At(d.positions,i));
        if(const auto* name=At(d.variables,2*page+1))
        { const auto count=out.Read(name); for(int j=0;j<count && j<10;++j) Natural(At(d.icons,2+j)); }
    }
    void Paint(const LikabilitySisterData& d)
    {
        Repeat(At(d.icons,0),out.Read(At(d.variables,3*page)),d.affectionAt);
        Repeat(At(d.icons,1),out.Read(At(d.variables,3*page+1)),d.lustAt);
        Repeat(At(d.icons,2),out.Read(At(d.variables,3*page+2)),d.haremAt);
    }
    void Paint(const LikabilityHalfData& d)
    {
        Halves(At(d.icons,0),At(d.icons,1),out.Read(At(d.variables,3*page)),d.affectionAt);
        if(page<3)
        {
            Halves(At(d.icons,2),At(d.icons,3),out.Read(At(d.variables,3*page+1)),d.lustAt);
            Repeat(At(d.icons,4),out.Read(At(d.variables,3*page+2)),d.possessionAt);
        }
    }
    void Paint(const LikabilityBadgeData& d)
    { Repeat(At(d.icons,0),out.Read(At(d.variables,2*page)),d.affectionAt); if(out.Read(At(d.variables,2*page+1))) Natural(At(d.icons,1)); }
    void Paint(const LikabilityCausalityData& d)
    {
        if(page<3)
        {
            const auto value=out.Read(At(d.variables,page));
            const auto* gauge=At(d.icons,0); const auto m=out.Metrics(gauge);
            RECT clip{m.origin.x,m.origin.y,m.origin.x+Mul(value,m.size.cx)/100,m.origin.y+m.size.cy};
            out.Draw(gauge,m.origin,nullptr,&clip);
            ThreeDigits(d.digits,value,d.digitsAt,0,true);
            if(out.Read(At(d.variables,3+page))) Natural(At(d.icons,1+page));
        }
        else if(page==3)
        {
            for(int i=0;i<3;++i) if(out.Read(At(d.variables,13+i))&2) Natural(At(d.icons,4+i));
            const auto value=out.Read(At(d.variables,6));
            for(int i=0;value!=i && i<3;++i) out.Draw(At(d.icons,28),At(d.invasionAt,i));
        }
        else
        {
            for(int i=0;i<6;++i)
            { const auto value=out.Read(At(d.variables,7+i)); for(int j=0;value!=j && j<3;++j) Natural(At(d.icons,7+3*i+j)); }
            for(int i=0;i<3;++i) if(out.Read(At(d.variables,13+i))&1) Natural(At(d.icons,25+i));
        }
    }
    void Paint(const LikabilityNurseData& d)
    {
        const auto gauge=[&](int metric,int variable,LikabilityPoints digitsAt)
        {
            const auto value=out.Read(At(d.variables,variable));
            Natural(At(d.icons,2*metric));
            const auto* filled=At(d.icons,2*metric+1);const auto m=out.Metrics(filled);const auto limit=At(d.limits,metric);
            if(value>=limit) Natural(filled);
            else
            {
                // 16原式先height/limit取整，再乘value；裁剪bottom是top+height。
                const auto top=Sub(Add(m.origin.y,m.size.cy),Mul(value,m.size.cy/limit));
                RECT clip{m.origin.x,top,m.origin.x+m.size.cx,Add(top,m.size.cy)};
                out.Draw(filled,m.origin,nullptr,&clip);
            }
            if(value>=10) { Digit(d.digits,value%10,At(digitsAt,1));Digit(d.digits,(value/10)%10,At(digitsAt,2)); }
            else Digit(d.digits,value%10,At(digitsAt,0));
            if(value>=limit) out.Draw(At(d.icons,8),At(d.upAt,metric));
        };
        if(page<3) for(int i=0;i<3;++i) gauge(i,3*page+i,d.digitsAt.subspan(3*i,3));
        else { gauge(3,9,d.specialDigitsAt);Repeat(At(d.icons,9),out.Read(At(d.variables,10)),d.extraAt); }
    }
    void Paint(const LikabilityBitData& d)
    {
        const auto bits=[&](int value,int first,int count)
        {
            for(int i=0;i<count;++i,value>>=1)
                if(const auto* image=At(d.icons,first+i+((value&1)?0:count))) Natural(image);
        };
        if(page==0)
        {
            for(const auto& r:d.numbers) ThreeDigits(d.digits,out.Read(At(d.variables,r.variable)),d.digitsAt.subspan(r.position,3),r.alphabet);
            bits(out.Read(At(d.variables,13)),1,4);
            if(out.Read(At(d.variables,12))) Natural(At(d.icons,0));
        }
        else if(page<5)
        { const auto& r=At(d.bits,page-1);bits(out.Read(At(d.variables,r.variable)),r.image,r.count); }
        else bits(out.Read(At(d.variables,14)),51,6);
    }
    void Paint(const LikabilityBaseline18Data&)
    { throw std::logic_error("Likability18 uses its retained native refresh path"); }
    void Paint(const LikabilityResidualData& d) { throw std::runtime_error(d.reason); }
};
}
void PaintLikabilityPage(const LikabilityResourceProfile& profile,std::int32_t page,ILikabilityPaintTarget& target)
{
    RequireUsableLikabilityProfile(profile);
    if(page<0 || page>=profile.flow.pages) throw std::out_of_range("Likability page is outside profile");
    Painter painter{profile,page,target};
    if(const auto* pair=std::get_if<LikabilityHeartPairData>(&profile.paint))
        painter.Natural(At(profile.backgrounds,At(pair->variables,2*page+1)==nullptr?1:0));
    else if(!profile.backgrounds.empty()) painter.Natural(profile.backgrounds[0]);
    if(!profile.pages.empty())
    {
        int background=page;
        if(const auto* bits=std::get_if<LikabilityBitData>(&profile.paint);bits && page==5)
            background=target.Read(bits->unlock)!=0?5:6;
        painter.Natural(At(profile.pages,background));
    }
    std::visit([&](const auto& data){painter.Paint(data);},profile.paint);
}
}
