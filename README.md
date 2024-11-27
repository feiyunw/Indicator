# 简介

缠论可视化交易插件

# 风险警示

本软件仅旨在实现最贴近缠师原文所的线段、中枢概念的可视化结果，软件免费分享使用，没有任何限制。

市场有风险，使用者自行承担任何由本软件而导致的买卖交易后果，作者本人不承担因为使用本软件而导致的任何直接或间接后果。

# 安装方法

目前仅支持通达信软件使用。安装时，首先将CZSC2.dll复制到通达信安装目录下的T0002\dlls目录之中，并在通达信公式管理器中将本dll加载到1号dll插件之中。

# 通达信端代码

```text
{参数RATE：单边交易费率百分数，仅K线交易用}
DLL:=TDXDLL1(1,H,L,RATE); {K线交易用11号接口替换1号接口}
HIB:=TDXDLL1(2,DLL,H,L); {中枢高点}
LOB:=TDXDLL1(3,DLL,H,L); {中枢低点}
{SIG:=TDXDLL1(4,DLL,H,L); {中枢起终点}
BSP:=TDXDLL1(5,DLL,L,H); {二三类买卖点}
{SLP:=TDXDLL1(7,DLL,H,L); {线段变化百分率}
SLP:=TDXDLL1(8,DLL,H,L); {线段斜率}
MA20:MA(CLOSE,20), COLORMAGENTA;
MA10:MA(CLOSE,10), COLORYELLOW;
MA5:MA(CLOSE,5), COLORWHITE;
DRAWLINE(DLL=-1,L,DLL=+1,H,0), COLORRED;
DRAWLINE(DLL=+1,H,DLL=-1,L,0), COLORGREEN;
IF(HIB,HIB,DRAWNULL), COLORLIRED;
IF(LOB,LOB,DRAWNULL), COLORLIMAGENTA;
DRAWNUMBER(DLL=+1,H,SLP), COLORRED, DRAWABOVE;
DRAWNUMBER(DLL=-1,L,SLP), COLORGREEN;
BUY(BSP=3,LOW);
SELL(BSP=12,HIGH);
BUYSHORT(BSP=2,LOW);
SELLSHORT(BSP=13,HIGH);
```
