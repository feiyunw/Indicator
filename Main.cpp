/*****************************************************************************
 * 缠论可视化分析系统
 * Copyright (C) 2016, Martin Tang

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "pch.h"
#include <cassert>

#include "Main.h"

/* 缠论TDX指标公式
DLL:=TDXDLL1(1,H,L,RATE); {K线交易使用11号接口替换1号接口}
HIB:=TDXDLL1(2,DLL,H,L); {中枢高点}
LOB:=TDXDLL1(3,DLL,H,L); {中枢低点}
SIG:=TDXDLL1(4,DLL,H,L); {中枢起终点}
BSP:=TDXDLL1(5,DLL,L,H); {二三类买卖点}
SLP:=TDXDLL1(8,DLL,L,H); {线段斜率}
MA89:MA(CLOSE,89), COLORGRAY;
MA55:MA(CLOSE,55), COLORGREEN;
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
*/

// 获取经过包含处理后的K线的区间值对应的原始K线索引到szL和szH，返回索引数量
int ReadKLines(int nReadCount, int nSkip, int nCount, float* pOut, int* szH, int* szL)
{
	assert(nReadCount >= 0);
	assert(nSkip >= -1);
	assert(nCount >= 0);
	assert(pOut);
	assert(szH);
	assert(szL);

	int nH = 0;	// szH[]索引
	int nL = 0;	// szL[]索引

	// 获取处理后的K线
	for (int i = nSkip + 1; (nH < nReadCount || nL < nReadCount) && i < nCount; ++i) {
		if (pOut[i] < 0.5) {
			// 0=不取
		}
		else if (pOut[i] < 1.5) {
			// 1=取高
			if (nH < nReadCount) {
				szH[nH++] = i;
			}
		}
		else if (pOut[i] < 2.5) {
			// 2=取低
			if (nL < nReadCount) {
				szL[nL++] = i;
			}
		}
		else {
			// 3=都取
			if (nH < nReadCount) {
				szH[nH++] = i;
			}
			if (nL < nReadCount) {
				szL[nL++] = i;
			}
		}
	}

	// 如果pOut[]被截断，nH可能不等于nL
	int ret = (nH < nL) ? nH : nL;

	for (int i = 0; i < ret; ++i) {
		pOut[szH[i]] = 0;
		pOut[szL[i]] = 0;
	}
	return ret;
}

// 标记笔的高低点（高点=1，低点=-1）
void Func1(int nCount, float* pOut, float* pHigh, float* pLow, float* c)
{
	assert(nCount > 0);
	assert(pOut);
	assert(pHigh);
	assert(pLow);
	if (nCount < 5) return;

	int nH = 0;	// 上一根K线高点索引
	int nL = 0;	// 上一根K线低点索引
	float fH = pHigh[0];	// 上一根K线高点值
	float fL = pLow[0];	// 上一根K线低点值
	float fPreH = fL;	// 前一根具有非包含关系的K线的高点值

	// K线的包含处理
	pOut[0] = 3;	// 1=取高，2=取低，3=都取，0=不取
	for (int i = 1; i < nCount; ++i) {
		if (pLow[i] <= fL && fH <= pHigh[i]) {
			// 当前K线包含上一根K线
			if (fPreH < fH) {
				// 第一根高点比其前一根具有非包含关系K线的高点高
				// 向上处理，取两根Ｋ线中的最高点为高点，次低点为低点
				pOut[nH] -= 1;
				pOut[i] = 1;
				nH = i;
				fH = pHigh[nH];
			}
			else {
				// 向下处理，取两根Ｋ线中的最低点为低点，次高点为高点
				pOut[nL] -= 2;
				pOut[i] = 2;
				nL = i;
				fL = pLow[nL];
			}
		}
		else if (fL <= pLow[i] && pHigh[i] <= fH) {
			// 上一根K线包含当前K线
			if (fPreH < fH) {
				// 第一根高点比其前一根具有非包含关系K线的高点高
				// 向上处理，取两根Ｋ线中的最高点为高点，次低点为低点
				pOut[nL] -= 2;
				pOut[i] = 2;
				nL = i;
				fL = pLow[nL];
			}
			else {
				// 向下处理，取两根Ｋ线中的最低点为低点，次高点为高点
				pOut[nH] -= 1;
				pOut[i] = 1;
				nH = i;
				fH = pHigh[nH];
			}
		}
		else {
			// 没有包含关系
			fPreH = fH;
			pOut[i] = 3;
			nH = i;
			fH = pHigh[nH];
			nL = i;
			fL = pLow[nL];
		}
	}

	int nBState = 0;	// 笔的状态，0=未定，1=向上，-1=向下
	int nFState = 0;	// 分形状态，0=构造中，1=延伸为笔
	int szH[5];	// K线高点
	int szL[5];	// K线低点
	int nKSpan = ReadKLines(5, -1, nCount, pOut, szH, szL);	// 待处理K线的数量
	bool bRunning = true;

	// 标记笔的高低点
	for (; bRunning; ) {
		switch (nBState) {
		case 1:	// 向上的笔
			if (nFState) {
				// 1=延伸为笔
				for (int i = 0; i + 1 < nKSpan; ++i) {
					if (pHigh[szH[i]] > pHigh[szH[i + 1]]) {
						// 顶分型
						if (pHigh[szH[i]] > pHigh[nH]) {
							nH = szH[i];
						}
						int nRemains = nKSpan - i;
						if (i > 0) {
							for (int j = 0; j < nRemains; ++j) {
								szH[j] = szH[i + j];
								szL[j] = szL[i + j];
							}
						}
						int n = (szH[nRemains - 1] > szL[nRemains - 1]) ? szH[nRemains - 1] : szL[nRemains - 1];
						nKSpan = ReadKLines(5 - nRemains, n, nCount, pOut, szH + nRemains, szL + nRemains) + nRemains;
						nFState = 0;	// 顶分型构造中
						break;
					}
				}
				if (nFState) {
					// 继续延伸
					int i = nKSpan - 1;
					if (pHigh[szH[i]] > pHigh[nH]) {
						nH = szH[i];
					}
					szH[0] = szH[i];
					szL[0] = szL[i];
					int n = (szH[0] > szL[0]) ? szH[0] : szL[0];
					nKSpan = ReadKLines(4, n, nCount, pOut, szH + 1, szL + 1) + 1;
					if (nKSpan <= 1) {
						pOut[nH] = 1;
						bRunning = false;
					}
				}
			}
			else {
				// 0=顶分型构造中
				if (nKSpan >= 4 && pHigh[szH[1]] > pHigh[szH[2]] && pHigh[szH[2]] > pHigh[szH[3]]) {
					// 4根连续向下K线
					if (nKSpan >= 5 && pHigh[szH[3]] > pHigh[szH[4]]) {
						// 5根连续向下K线
						pOut[nH] = 1;	// 向下笔的起点
						nL = szL[4];
						szH[0] = szH[4];
						szL[0] = szL[4];
						int n = (szH[0] > szL[0]) ? szH[0] : szL[0];
						nKSpan = ReadKLines(4, n, nCount, pOut, szH + 1, szL + 1) + 1;
						nBState = -1;	// 向下笔
						nFState = 1;	// 延伸为笔
					}
					else if (szL[3] - szH[0] >= 4) {
						// 新笔：不考虑包含关系，顶分型最高K线和底分型最低K线之间至少有3根K线
						pOut[nH] = 1;	// 向下笔的起点
						int i = 3;
						nL = szL[i];
						int nRemains = nKSpan - i;
						for (int j = 0; j < nRemains; ++j) {
							szH[j] = szH[i + j];
							szL[j] = szL[i + j];
						}
						int n = (szH[nRemains - 1] > szL[nRemains - 1]) ? szH[nRemains - 1] : szL[nRemains - 1];
						nKSpan = ReadKLines(5 - nRemains, n, nCount, pOut, szH + nRemains, szL + nRemains) + nRemains;
						nBState = -1;	// 向下笔
						nFState = 1;	// 延伸为笔
					}
				}
				if (!nFState) {
					// 中继顶分型
					for (int i = 1; i + 1 < nKSpan; ++i) {
						if (pLow[szL[i]] < pLow[szL[i + 1]]) {
							// 底分型
							int nRemains = nKSpan - i - 1;
							for (int j = 0; j < nRemains; ++j) {
								szH[j] = szH[i + 1 + j];
								szL[j] = szL[i + 1 + j];
							}
							int n = (szH[nRemains - 1] > szL[nRemains - 1]) ? szH[nRemains - 1] : szL[nRemains - 1];
							nKSpan = ReadKLines(5 - nRemains, n, nCount, pOut, szH + nRemains, szL + nRemains) + nRemains;
							nFState = 1;	// 向上笔继续延伸
							break;
						}
					}
					if (!nFState) {
						// 分形构造中终止
						pOut[nH] = 1;
						bRunning = false;
					}
				}
			}
			break;
		case -1:	// 向下的笔
			if (nFState) {
				// 1=延伸为笔
				for (int i = 0; i + 1 < nKSpan; ++i) {
					if (pLow[szL[i]] < pLow[szL[i + 1]]) {
						// 底分型
						if (pLow[szL[i]] < pLow[nL]) {
							nL = szL[i];
						}
						int nRemains = nKSpan - i;
						if (i > 0) {
							for (int j = 0; j < nRemains; ++j) {
								szH[j] = szH[i + j];
								szL[j] = szL[i + j];
							}
						}
						int n = (szH[nRemains - 1] > szL[nRemains - 1]) ? szH[nRemains - 1] : szL[nRemains - 1];
						nKSpan = ReadKLines(5 - nRemains, n, nCount, pOut, szH + nRemains, szL + nRemains) + nRemains;
						nFState = 0;	// 底分型构造中
						break;
					}
				}
				if (nFState) {
					// 继续延伸
					int i = nKSpan - 1;
					if (pLow[szL[i]] < pLow[nL]) {
						nL = szL[i];
					}
					szH[0] = szH[i];
					szL[0] = szL[i];
					int n = (szH[0] > szL[0]) ? szH[0] : szL[0];
					nKSpan = ReadKLines(4, n, nCount, pOut, szH + 1, szL + 1) + 1;
					if (nKSpan <= 1) {
						pOut[nL] = -1;
						bRunning = false;
					}
				}
			}
			else {
				// 0=底分型构造中
				if (nKSpan >= 4 && pLow[szL[1]] < pLow[szL[2]] && pLow[szL[2]] < pLow[szL[3]]) {
					// 4根连续向上K线
					if (nKSpan >= 5 && pLow[szL[3]] < pLow[szL[4]]) {
						// 5根连续向上K线
						pOut[nL] = -1;	// 向上笔的起点
						nH = szH[4];
						szH[0] = szH[4];
						szL[0] = szL[4];
						int n = (szH[0] > szL[0]) ? szH[0] : szL[0];
						nKSpan = ReadKLines(4, n, nCount, pOut, szH + 1, szL + 1) + 1;
						nBState = 1;	// 向上笔
						nFState = 1;	// 延伸为笔
					}
					else if (szH[3] - szL[0] >= 4) {
						// 新笔：不考虑包含关系，底分型最低K线和顶分型最高K线之间至少有3根K线
						pOut[nL] = -1;	// 向上笔的起点
						int i = 3;
						nH = szH[i];
						int nRemains = nKSpan - i;
						for (int j = 0; j < nRemains; ++j) {
							szH[j] = szH[i + j];
							szL[j] = szL[i + j];
						}
						int n = (szH[nRemains - 1] > szL[nRemains - 1]) ? szH[nRemains - 1] : szL[nRemains - 1];
						nKSpan = ReadKLines(5 - nRemains, n, nCount, pOut, szH + nRemains, szL + nRemains) + nRemains;
						nBState = 1;	// 向上笔
						nFState = 1;	// 延伸为笔
					}
				}
				if (!nFState) {
					// 中继底分型
					for (int i = 1; i + 1 < nKSpan; ++i) {
						if (pHigh[szH[i]] > pHigh[szH[i + 1]]) {
							// 顶分型
							int nRemains = nKSpan - i - 1;
							for (int j = 0; j < nRemains; ++j) {
								szH[j] = szH[i + 1 + j];
								szL[j] = szL[i + 1 + j];
							}
							int n = (szH[nRemains - 1] > szL[nRemains - 1]) ? szH[nRemains - 1] : szL[nRemains - 1];
							nKSpan = ReadKLines(5 - nRemains, n, nCount, pOut, szH + nRemains, szL + nRemains) + nRemains;
							nFState = 1;	// 向下笔继续延伸
							break;
						}
					}
					if (!nFState) {
						// 分形构造中终止
						pOut[nL] = -1;
						bRunning = false;
					}
				}
			}
			break;
		default:	// 0=未定
			if (nKSpan >= 5) {
				int nRemains = 1;	// 保留最后一根K线
				if (pHigh[szH[0]] < pHigh[szH[1]] && pHigh[szH[1]] < pHigh[szH[2]] && pHigh[szH[2]] < pHigh[szH[3]] && pHigh[szH[3]] < pHigh[szH[4]]) {
					// 5根连续向上K线
					nH = szH[4];
					nL = szL[0];
					pOut[nL] = -1;	// 向上笔的起点
					nBState = 1;
					nFState = 1;
				}
				else if (pLow[szL[0]] > pLow[szL[1]] && pLow[szL[1]] > pLow[szL[2]] && pLow[szL[2]] > pLow[szL[3]] && pLow[szL[3]] > pLow[szL[4]]) {
					// 5根连续向下K线
					nH = szH[0];
					nL = szL[4];
					pOut[nH] = 1;	// 向下笔的起点
					nBState = -1;
					nFState = 1;
				}
				else {
					nRemains = nKSpan - 1;	// 去掉第一根K线
				}
				for (int j = 0; j < nRemains; ++j) {
					szH[j] = szH[j + nKSpan - nRemains];
					szL[j] = szL[j + nKSpan - nRemains];
				}
				int n = (szH[nRemains - 1] > szL[nRemains - 1]) ? szH[nRemains - 1] : szL[nRemains - 1];
				nKSpan = ReadKLines(5 - nRemains, n, nCount, pOut, szH + nRemains, szL + nRemains) + nRemains;
			}
			else {
				bRunning = false;
			}
			break;
		}
	}
}

//=============================================================================
// 输出函数2号：中枢高点数据
//=============================================================================

void Func2(int nCount, float *pOut, float *pIn, float *pHigh, float *pLow)
{
	CCentroid Centroid;

	for (int i = 0; i < nCount; ++i) {
		if (pIn[i] == 1) {
			// 遇到线段高点，推入中枢算法
			if (Centroid.PushHigh(i, pHigh[i])) {
				// 区段内更新算得的中枢高数据
				for (int j = Centroid.GetStart(); j <= Centroid.GetEnd(); ++j) {
					pOut[j] = Centroid.GetPrevHigh();
				}
			}
		}
		else if (pIn[i] == -1) {
			// 遇到线段低点，推入中枢算法
			if (Centroid.PushLow(i, pLow[i])) {
				// 区段内更新算得的中枢低数据
				for (int j = Centroid.GetStart(); j <= Centroid.GetEnd(); ++j) {
					pOut[j] = Centroid.GetPrevHigh();
				}
			}
		}

		// 尾部未完成中枢处理
		if (Centroid.IsInCentre() && (Centroid.GetLines() >= 2) && (i == nCount - 1)) {
			for (int j = Centroid.GetStart(); j < nCount; ++j) {
				pOut[j] = Centroid.GetHigh();
			}
		}
	}
}

//=============================================================================
// 输出函数3号：中枢低点数据
//=============================================================================

void Func3(int nCount, float *pOut, float *pIn, float *pHigh, float *pLow)
{
	CCentroid Centroid;

	for (int i = 0; i < nCount; ++i) {
		if (pIn[i] == 1) {
			// 遇到线段高点，推入中枢算法
			if (Centroid.PushHigh(i, pHigh[i])) {
				// 区段内更新算得的中枢高数据
				for (int j = Centroid.GetStart(); j <= Centroid.GetEnd(); ++j) {
					pOut[j] = Centroid.GetPrevLow();
				}
			}
		}
		else if (pIn[i] == -1) {
			// 遇到线段低点，推入中枢算法
			if (Centroid.PushLow(i, pLow[i])) {
				// 区段内更新算得的中枢低数据
				for (int j = Centroid.GetStart(); j <= Centroid.GetEnd(); ++j) {
					pOut[j] = Centroid.GetPrevLow();
				}
			}
		}

		// 尾部未完成中枢处理
		if (Centroid.IsInCentre() && (Centroid.GetLines() >= 2) && (i == nCount - 1)) {
			for (int j = Centroid.GetStart(); j < nCount; ++j) {
				pOut[j] = Centroid.GetLow();
			}
		}
	}
}

//=============================================================================
// 输出函数4号：中枢起点、终点信号
//=============================================================================

void Func4(int nCount, float *pOut, float *pIn, float *pHigh, float *pLow)
{
	CCentroid Centroid;

	for (int i = 0; i < nCount; ++i) {
		if (pIn[i] == 1) {
			// 遇到线段高点，推入中枢算法
			if (Centroid.PushHigh(i, pHigh[i])) {
				// 进行标记
				pOut[Centroid.GetStart()] = 1;
				pOut[Centroid.GetEnd()] = 2;
			}
		}
		else if (pIn[i] == -1) {
			// 遇到线段低点，推入中枢算法
			if (Centroid.PushLow(i, pLow[i])) {
				// 进行标记
				pOut[Centroid.GetStart()] = 1;
				pOut[Centroid.GetEnd()] = 2;
			}
		}

		// 尾部未完成中枢处理
		if (Centroid.IsInCentre() && (Centroid.GetLines() >= 2) && (i == nCount - 1)) {
			pOut[Centroid.GetStart()] = 1;
			pOut[nCount - 1] = 2;
		}
	}
}

//=============================================================================
// 输出函数5号：三类买卖点信号
//=============================================================================

void Func5(int nCount, float *pOut, float *pIn, float *pHigh, float *pLow)
{
	CCentroid Centroid;

	for (int i = 0; i < nCount; ++i) {
		if (pIn[i] == 1) {
			if (Centroid.PushHigh(i, pHigh[i])) {
				// 第三类卖点信号
				pOut[i] = 13;
			}
			else if (Centroid.IsTop1BelowTop2()) {
				// 第二类卖点信号
				pOut[i] = 12;
			}
			else {
				pOut[i] = 0;
			}
		}
		else if (pIn[i] == -1) {
			if (Centroid.PushLow(i, pLow[i])) {
				// 第三类买点信号
				pOut[i] = 3;
			}
			else if (Centroid.IsBottom1AboveBottom2()) {
				// 第二类买点信号
				pOut[i] = 2;
			}
			else {
				pOut[i] = 0;
			}
		}
	}
}

//=============================================================================
// 输出函数6号：形态买卖点信号
//=============================================================================

void Func6(int nCount, float *pOut, float *pIn, float *pHigh, float *pLow)
{
  float fTop1 = 0, fTop2 = 0, fTop3 = 0, fTop4 = 0;
  float fBot1 = 0, fBot2 = 0, fBot3 = 0, fBot4 = 0;

  for (int i = 0; i < nCount; i++)
  {
	if (pIn[i] == 1)
	{
	  fTop4 = fTop3;
	  fTop3 = fTop2;
	  fTop2 = fTop1;
	  fTop1 = pHigh[i];

	  if (((fBot1 - fTop2)/fTop2 > (fBot2 - fTop3)/fTop3) &&
		  ((fBot2 - fTop3)/fTop3 > (fBot3 - fTop4)/fTop4))
	  {
		if ((fBot1 < fBot2) && (fTop2 < fTop3) &&
			(fBot2 < fBot3) && (fTop3 < fTop4))
		{
		  pOut[i] = 1;
		  continue;
		}
		if ((fBot1 > fBot2) && (fTop2 > fTop3) && (fBot2 < fBot3) &&
			(fTop3 < fTop4) && (fBot1 < fTop3))
		{
		  pOut[i] = 2;
		  continue;
		}
		if ((fBot1 > fTop3) && (fBot2 > fBot3) && (fTop3 > fTop4))
		{
		  pOut[i] = 3;
		  continue;
		}
	  }
	}
	else if (pIn[i] == -1)
	{
	  fBot4 = fBot3;
	  fBot3 = fBot2;
	  fBot2 = fBot1;
	  fBot1 = pLow[i];

	  if (((fBot1 - fTop1)/fTop1 > (fBot2 - fTop2)/fTop2) &&
		  ((fBot2 - fTop2)/fTop2 > (fBot3 - fTop3)/fTop3))
	  {
		if ((fBot1 < fBot2) && (fTop1 < fTop2) &&
			(fBot2 < fBot3) && (fTop2 < fTop3))
		{
		  pOut[i] = 1;
		  continue;
		}
		if ((fBot1 > fBot2) && (fTop1 > fTop2) && (fBot2 < fBot3) &&
			(fTop2 < fTop3) && (fBot1 < fTop2))
		{
		  pOut[i] = 2;
		  continue;
		}
		if ((fBot1 > fTop2) && (fBot2 > fBot3) && (fTop2 > fTop3))
		{
		  pOut[i] = 3;
		  continue;
		}
	  }
	}
	else
	{
	  pOut[i] = 0;
	}
  }
}

//=============================================================================
// 输出函数7号：线段强度分析指标
//=============================================================================

void Func7(int nCount, float *pOut, float *pIn, float *pHigh, float *pLow)
{
  int nPrevTop = 0, nPrevBot = 0;

  for (int i = 0; i < nCount; i++)
  {
	// 遇到线段高点
	if (pIn[i-1] == 1)
	{
	  // 标记高点位置
	  nPrevTop = i - 1;
	}
	// 遇到线段低点
	else if (pIn[i-1] == -1)
	{
	  // 标记低点位置
	  nPrevBot = i - 1;
	}

	// 上升线段计算模式
	if (pIn[i] == 1)
	{
	  // 计算上升线段斜率
	  pOut[i] = (pHigh[i] - pLow[nPrevBot]) / pLow[nPrevBot] * 100;
	}
	// 下降线段计算模式
	else if (pIn[i] == -1)
	{
	  // 计算上升线段斜率
	  pOut[i] = (pLow[i] - pHigh[nPrevTop]) / pHigh[nPrevTop] * 100;
	}
  }
}

//=============================================================================
// 输出函数8号：线段斜率分析指标
//=============================================================================

void Func8(int nCount, float* pOut, float* pIn, float* pHigh, float* pLow)
{
	int nPrevBot = -1;
	int nPrevTop = -1;
	for (int i = 0; i < nCount; ++i) {
		pOut[i] = 0;
		if (pIn[i] > 0.5) {
			// 高点，计算上升段斜率
			if (0 <= nPrevBot) {
				pOut[i] = (pHigh[i] - pLow[nPrevBot]) / (i - nPrevBot);
			}
			nPrevTop = i;
		}
		else if (pIn[i] < -0.5) {
			// 低点，计算下降段斜率
			if (0 <= nPrevTop) {
				pOut[i] = (pLow[i] - pHigh[nPrevTop]) / (i - nPrevTop);
			}
			nPrevBot = i;
		}
	}
}

bool IsGoodTrade(float buy, float sell, float rate)
{
	return (sell - buy) > (sell + buy) * rate;
}

void AlgoB(int nCount, float* pOut, float* pHigh, float* pLow, float rate);
void AlgoBS(int nCount, float* pOut, float* pHigh, float* pLow, float rate);

// 算法Ａ：在一段Ｋ线图中寻找获利最大的Ｋ线交易机会组合
void AlgoA(int nCount, float* pOut, float* pHigh, float* pLow, float rate)
{
	// 至少5条Ｋ线
	if (nCount < 5) {
		return;
	}

	// 备选的买点：所有Ｋ线最高价Ｈ中最低的，Ｈ相同时取Ｌ最低的，都相同取时间在先的
	int buyIndex = 0;
	float buy = pHigh[buyIndex];
	float low = pLow[buyIndex];
	for (int i = 1; i < nCount; ++i) {
		float h = pHigh[i];
		if ((h < buy) || (h - buy < FLT_EPSILON && pLow[i] < low)) {
			buyIndex = i;
			buy = pHigh[buyIndex];
			low = pLow[buyIndex];
		}
	}

	// Ｋ线被备选买点分为两类：
	// （１）位于备选买点之前（不含备选买点）：在该区间应用算法A
	AlgoA(buyIndex - 3, pOut, pHigh, pLow, rate);
	// （２）位于备选买点之后（含备选买点）：在该区间应用算法B
	AlgoB(nCount - buyIndex, pOut + buyIndex, pHigh + buyIndex, pLow + buyIndex, rate);
}

// 算法B：在一段Ｋ线图中，已知最佳备选买点Ｂ是第一根Ｋ线，寻找获利最大的Ｋ线交易机会组合
void AlgoB(int nCount, float* pOut, float* pHigh, float* pLow, float rate)
{
	// 至少5条Ｋ线
	if (nCount < 5) {
		return;
	}

	 // 备选的卖点：后续Ｋ线最低价Ｌ中最高的，Ｌ相同时取Ｈ最高的，都相同取时间在后的
	int sellIndex = nCount - 1;
	float sell = pLow[sellIndex];
	float high = pHigh[sellIndex];
	for (int i = sellIndex - 1; i > 3; --i) {
		float l = pLow[i];
		if ((sell < l) || (sell - l < FLT_EPSILON && high < pHigh[i])) {
			sellIndex = i;
			sell = pLow[sellIndex];
			high = pHigh[sellIndex];
		}
	}

	// 如果备选交易不是有利的，算法结束
	float buy = pHigh[0];
	if (!IsGoodTrade(buy, sell, rate)) {
		return;
	}

	// Ｋ线被买卖点分为两类：
	// （１）位于买点和卖点之间（闭区间）：在该区间应用算法BS
	AlgoBS(sellIndex + 1, pOut, pHigh, pLow, rate);
	// （２）位于卖点之后（不含卖点）：在该区间应用算法A
	AlgoA(nCount - sellIndex - 4, pOut + sellIndex + 4, pHigh + sellIndex + 4, pLow + sellIndex + 4, rate);
}

// 算法BS：在一段Ｋ线图中，已知最佳买点Ｂ是第一根Ｋ线，最佳卖点Ｓ是最后一根Ｋ线，寻找获利最大的Ｋ线交易机会组合
void AlgoBS(int nCount, float* pOut, float* pHigh, float* pLow, float rate)
{
	// 相邻买卖点之间至少间隔3根Ｋ线
	if (nCount < 13) {
		if (nCount >= 5) {
			pOut[0] = -1;
			pOut[nCount - 1] = 1;
		}
		return;
	}

	int buyIndex = 0;
	int lastSellIndex = nCount - 1;
	float firstBuy = pHigh[0];
	// 临时的中间卖点：在初始买点之后Ｋ线最低价Ｌ具有局部极大值的（卖出必须是有利的；价格相同时取时间在先的）
	for (int sellIndex = 4; sellIndex < lastSellIndex - 7; ++sellIndex) {
		float sell = pLow[sellIndex];
		if ((4 == sellIndex || pLow[sellIndex - 1] < sell) && sell >= pLow[sellIndex + 1] && IsGoodTrade(firstBuy, sell, rate)) {
			float buy;
			if (sellIndex + 3 < buyIndex) {
				// 复用上一次找到的备选买点
				buy = pHigh[buyIndex];
			}
			else {
				// 备选的中间买点：在中间卖点之后Ｋ线最高价Ｈ中最低的，Ｈ相同时取Ｌ最低的，都相同时取时间在先的
				buyIndex = sellIndex + 4;
				buy = pHigh[buyIndex];
				float low = pLow[buyIndex];
				for (int i = buyIndex + 1; i < lastSellIndex - 3; ++i) {
					float h = pHigh[i];
					if ((h < buy) || (h - buy < FLT_EPSILON && pLow[i] < low)) {
						buyIndex = i;
						buy = pHigh[buyIndex];
						low = pLow[buyIndex];
					}
				}
			}
			assert(sellIndex < buyIndex);

			// 中间买卖点匹配的交易必须是有利的
			if (IsGoodTrade(buy, sell, rate)) {
				// 找到了中间买点。Ｋ线被中间买点分为两类：
				// （１）位于买点之前（不含买点）：在该区间应用算法B
				AlgoB(buyIndex - 3, pOut, pHigh, pLow, rate);
				// （２）位于买点之后（闭区间）：在该区间应用算法BS
				AlgoBS(nCount - buyIndex, pOut + buyIndex, pHigh + buyIndex, pLow + buyIndex, rate);
				return;
			}
		}
		// 如果找不到合适的中间买点，则继续推进寻找下一个中间卖点
	}

	// 找不到合适的中间买卖点，标记区间头尾为买卖点，算法结束
	pOut[0] = -1;
	pOut[nCount - 1] = 1;
}

void Func11(int nCount, float* pOut, float* pHigh, float* pLow, float* pRate)
{
	// *pRate：单边交易费率百分数
	assert(pOut);
	assert(pHigh);
	assert(pLow);
	assert(pRate);
    float fRate = *pRate / 100000;

	for (int i = 0; i < nCount; ++i) {
		pOut[i] = 0;
	}
	AlgoA(nCount, pOut, pHigh, pLow, fRate);

	// 在最后一个卖点之后寻找Ｋ线最高价Ｈ中最低的（要小于最后一根卖点Ｋ线的最低价Ｌ），Ｈ相同时取Ｌ最低的，都相同时取时间在先的
	int buyIndex = nCount - 1;
	if (0 <= buyIndex && -0.5 < pOut[buyIndex] && pOut[buyIndex] < 0.5) {
        --buyIndex;
        if (0 <= buyIndex && -0.5 < pOut[buyIndex] && pOut[buyIndex] < 0.5) {
            // 倒数第二根Ｋ线不是买卖点，作为备选，继续找更低价
            float buy = pHigh[buyIndex];
            float low = pLow[buyIndex];
            for (int i = buyIndex - 1; i >= 0; --i) {
                if (pOut[i] > 0.5) {
                    // 遇到最后一个卖点，标记Ｈ最低点后结束
                    if (buy < pLow[i] && IsGoodTrade(buy, pLow[i], fRate)) {
                        pOut[buyIndex] = -1;
                    }
                    break;
                }
                // 非买卖点，检查是否有更低价
                assert(pOut[i] > -0.5);
                if ((pHigh[i] < buy) || (pHigh[i] - buy < FLT_EPSILON && pLow[i] <= low)) {
                    buyIndex = i;
                    buy = pHigh[buyIndex];
                    low = pLow[buyIndex];
                }
            }
        }
	}
}

static PluginTCalcFuncInfo Info[] =
{
  {1, &Func1},
  {2, &Func2},
  {3, &Func3},
  {4, &Func4},
  {5, &Func5},
  {6, &Func6},
  {7, &Func7},
  {8, &Func8},
  {11, &Func11},
  {0, NULL},
};

BOOL RegisterTdxFunc(PluginTCalcFuncInfo **pInfo)
{
  if (*pInfo == NULL)
  {
	*pInfo = Info;

	return TRUE;
  }

  return FALSE;
}
