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
#include <vector>

#include "Main.h"

// 分型（作为笔的端点）是否可延伸
bool CanExtend(int f1, int f2, const std::vector<int>& vBH, const std::vector<int>& vBL, float* pHigh, float* pLow)
{
	assert(f1 * f2 > 0);
	assert(pHigh);
	assert(pLow);

	if (f1 > 0) {
		// 顶分型
		if (pHigh[vBH[f1]] < pHigh[vBH[f2]]) {
			// 后面的顶高于前面的顶
			return true;
		}
	}
	else {
		// 底分型
		if (pLow[vBL[-f1]] > pLow[vBL[-f2]]) {
			// 后面的底低于前面的底
			return true;
		}
	}
	return false;
}

// 是否有效笔
bool IsValidBi(int f1, int f2, const std::vector<int>& vBH, const std::vector<int>& vBL, float* pHigh)
{
	assert(f1 * f2 < 0);
	assert(pHigh);

	if (f1 > 0) {
		// 顶分型
		if (-f2 - f1 < 3 || vBL[-f2] - vBH[f1] <= 3) {
			return false;
		}
		// 顶分型最高K线的区间高于底分型最低K线的区间
		if (pHigh[vBH[f1]] <= pHigh[vBL[-f2]]) {
			return false;
		}
	}
	else {
		// 底分型
		if (f2 + f1 < 3 || vBH[f2] - vBL[-f1] <= 3) {
			return false;
		}
		// 顶分型最高K线的区间高于底分型最低K线的区间
		if (pHigh[vBH[f2]] <= pHigh[vBL[-f1]]) {
			return false;
		}
	}
	return true;
}

// 标记笔的高低点（高点=1，低点=-1）
void Func1(int nCount, float* pOut, float* pHigh, float* pLow, float* c)
{
	assert(nCount > 0);
	assert(pOut);
	assert(pHigh);
	assert(pLow);

	std::vector<int> vBH;	// 经包含处理后的新K线高点对应的原K线索引
	std::vector<int> vBL;	// 经包含处理后的新K线低点对应的原K线索引

	// K线的包含处理
	vBH.reserve(nCount);
	vBL.reserve(nCount);
	vBH.push_back(0);
	vBL.push_back(0);
	pOut[0] = 0;
	for (int i = 1; i < nCount; ++i) {
		pOut[i] = 0;
		if (pLow[i] <= pLow[vBL.back()] && pHigh[vBH.back()] <= pHigh[i]) {
			// 当前K线包含上一根K线
			auto itH = vBH.rbegin();
			auto itL = vBL.rbegin();
			int process = 1;	// K线包含处理：1=向上；-1=向下
			for (; itH != vBH.rend() && itL != vBL.rend(); ++itH, ++itL) {
				// 未被包含的前一根K线
				if (pHigh[*itH] > pHigh[i]) {
					// 高于当前K线，向下处理
					process = -1;
					break;
				}
				if (pLow[*itL] < pLow[i]) {
					// 低于当前K线，向上处理
					break;
				}
			}
			itH = ++(vBH.rbegin());	// 被包含K线的上一根K线
			if (process == 1) {
				// 向上处理
				if (itH != vBH.rend() && pHigh[*itH] > pHigh[vBH.back()]) {
					// 前两根K线向下，新K线先向下再向上
					int preL = vBL.back();
					vBL.pop_back();
					vBL.push_back(i);
					vBH.push_back(i);
					vBL.push_back(preL);
				}
				else {
					// 前两根K线向上，合并被包含K线向上
					vBH.pop_back();
					vBH.push_back(i);
				}
			}
			else {
				// 向下处理
				if (itH != vBH.rend() && pHigh[*itH] > pHigh[vBH.back()]) {
					// 前两根K线向下，合并被包含K线向下
					vBL.pop_back();
					vBL.push_back(i);
				}
				else {
					// 前两根K线向上，新K线先向上再向下
					int preH = vBH.back();
					vBH.pop_back();
					vBH.push_back(i);
					vBH.push_back(preH);
					vBL.push_back(i);
				}
			}
		}
		else if (pLow[vBL.back()] <= pLow[i] && pHigh[i] <= pHigh[vBH.back()]) {
			// 上一根K线包含当前K线
			auto it = ++(vBH.rbegin());	// // 上一根K线的上一根
			if (it != vBH.rend() && pHigh[vBH.back()] < pHigh[*it]) {
				// 上一根K线高点低于其前一根的高点，向下处理
				vBH.pop_back();
				vBH.push_back(i);
			}
			else {
				// 向上处理
				vBL.pop_back();
				vBL.push_back(i);
			}
		}
		else {
			// 没有包含关系
			vBH.push_back(i);
			vBL.push_back(i);
		}
	}

	int nBState = 0;	// 笔的状态，0=未定，1=向上，-1=向下
	std::vector<int> vF;	// 分型列表，>0=顶分型的K线编号（经包含处理后），<0=-底分型的K线编号

	// 标记分型
	for (size_t i = 0; i + 1 < vBH.size(); ++i) {
		switch (nBState) {
		case 1:	// 向上
			if (pHigh[vBH[i]] > pHigh[vBH[i + 1]]) {
				// 顶分型
				vF.push_back(i);
				nBState = -1;	// 向下
			}
			break;
		case -1:	// 向下
			if (pLow[vBL[i]] < pLow[vBL[i + 1]]) {
				// 底分型
				vF.push_back(-static_cast<int>(i));
				nBState = 1;	// 向上
			}
			break;
		default:	// 0=未定
			if (i + 1 < vBH.size()) {
				if (pHigh[vBH[i]] < pHigh[vBH[i + 1]]) {
					nBState = 1;
				}
				else {
					nBState = -1;
				}
			}
			break;
		}
	}

	std::vector<int> vFValid;	// 仅有效笔对应的分型列表，>0=顶分型的K线编号（经包含处理后），<0=-底分型的K线编号
	size_t nInvalidFrom = 0;	// 无效笔开始的K线编号（经包含处理后）

	// 标记有效分型（对应有效笔）
	for (size_t nInvalidTo = nInvalidFrom; nInvalidTo + 1 < vF.size();) {
		// 从from开始查找第一个有效笔[to, to+1]，或至少连续三段无效笔[from X from+1 X from+2 X from+3]
		if (nInvalidTo <= nInvalidFrom + 2 && !IsValidBi(vF[nInvalidTo], vF[nInvalidTo + 1], vBH, vBL, pHigh)) {
			++nInvalidTo;
			continue;
		}
		switch (nInvalidTo - nInvalidFrom) {
			// 连续无效笔的数量
		case 0:
			// [back?, from(to), to+1]
			vFValid.push_back(vF[nInvalidTo]);
			nInvalidFrom = ++nInvalidTo;
			break;
		case 1:
			if (!vFValid.empty()) {
				// [back, from X to, to+1]
				if (CanExtend(vFValid.back(), vF[nInvalidTo], vBH, vBL, pHigh, pLow)) {
					// to优于back
					if (CanExtend(vF[nInvalidFrom], vF[nInvalidTo + 1], vBH, vBL, pHigh, pLow)) {
						// to+1优于from
						vFValid.pop_back();
						vFValid.push_back(vF[nInvalidTo]);
						nInvalidFrom = ++nInvalidTo;
					}
					else {
						// from优于to+1，from和to冲突
						for (auto it = vFValid.rbegin();;) {
							if (++it == vFValid.rend()) {
								// 择一保留，选择保留to
								vFValid.pop_back();
								vFValid.push_back(vF[nInvalidTo]);
								nInvalidFrom = ++nInvalidTo;
								break;
							}
							if (!CanExtend(*it, vF[nInvalidFrom], vBH, vBL, pHigh, pLow)) {
								// 删除from，保留to
								vFValid.pop_back();
								vFValid.push_back(vF[nInvalidTo]);
								nInvalidFrom = ++nInvalidTo;
								break;
							}
							if (++it == vFValid.rend()) {
								// 择一保留，选择保留to
								vFValid.pop_back();
								vFValid.push_back(vF[nInvalidTo]);
								nInvalidFrom = ++nInvalidTo;
								break;
							}
							if (!CanExtend(*it, vF[nInvalidTo], vBH, vBL, pHigh, pLow)) {
								// 保留from, 删除to
								vFValid.push_back(vF[nInvalidFrom]);
								nInvalidTo = nInvalidFrom + 3;
								nInvalidFrom = nInvalidTo;
								break;
							}
						}
					}
				}
				else {
					// back优于to
					if (CanExtend(vF[nInvalidFrom], vF[nInvalidTo + 1], vBH, vBL, pHigh, pLow)) {
						// to+1优于from
						nInvalidFrom = ++nInvalidTo;
					}
					else {
						// from优于to+1
						vFValid.push_back(vF[nInvalidFrom]);
						nInvalidTo = nInvalidFrom + 3;
						nInvalidFrom = nInvalidTo;
					}
				}
			}
			else {
				// [from X to, to+1]
				if (CanExtend(vF[nInvalidFrom], vF[nInvalidTo + 1], vBH, vBL, pHigh, pLow)) {
					// to+1优于from
					nInvalidFrom = ++nInvalidTo;
				}
				else {
					// from优于to+1
					vFValid.push_back(vF[nInvalidFrom]);
					nInvalidTo = nInvalidFrom + 3;
					nInvalidFrom = nInvalidTo;
				}
			}
			break;
		case 2:
			if (!vFValid.empty()) {
				// [back, from X from+1 X to, to+1]
				if (CanExtend(vFValid.back(), vF[nInvalidFrom + 1], vBH, vBL, pHigh, pLow)) {
					// from+1优于back
					if (CanExtend(vF[nInvalidFrom + 1], vF[nInvalidTo + 1], vBH, vBL, pHigh, pLow)) {
						// to+1优于from+1
						vFValid.pop_back();
						nInvalidFrom = ++nInvalidTo;
					}
					else {
						// from+1优于to+1
						vFValid.pop_back();
						vFValid.push_back(vF[nInvalidFrom + 1]);
						nInvalidFrom += 4;
						nInvalidTo = nInvalidFrom;
					}
				}
				else {
					// back优于from+1
					if (CanExtend(vF[nInvalidFrom], vF[nInvalidTo], vBH, vBL, pHigh, pLow)) {
						// to优于from
						vFValid.push_back(vF[nInvalidTo]);
						nInvalidFrom = ++nInvalidTo;
					}
					else {
						// from优于to
						vFValid.push_back(vF[nInvalidFrom]);
						nInvalidFrom = ++nInvalidTo;
					}
				}
			}
			else {
				// [from X from+1 X to, to+1]
				if (CanExtend(vF[nInvalidFrom + 1], vF[nInvalidTo + 1], vBH, vBL, pHigh, pLow)) {
					// to+1优于from+1
					if (CanExtend(vF[nInvalidFrom], vF[nInvalidTo], vBH, vBL, pHigh, pLow)) {
						// to优于from
						vFValid.push_back(vF[nInvalidTo]);
						nInvalidFrom = ++nInvalidTo;
					}
					else {
						// from优于to
						vFValid.push_back(vF[nInvalidFrom]);
						nInvalidFrom = ++nInvalidTo;
					}
				}
				else {
					// from+1优于to+1
					if (CanExtend(vF[nInvalidFrom], vF[nInvalidTo], vBH, vBL, pHigh, pLow)) {
						// to优于from，to和from+1冲突
						vFValid.push_back(vF[nInvalidTo]);
						nInvalidFrom = ++nInvalidTo;
					}
					else {
						// from优于to，from和from+1冲突
						vFValid.push_back(vF[nInvalidFrom]);
						nInvalidFrom = ++nInvalidTo;
					}
				}
			}
			break;
		case 3:
			if (!vFValid.empty()) {
				// [back, from X from+1 X ... X to ? to+1]
				if (CanExtend(vFValid.back(), vF[nInvalidFrom + 1], vBH, vBL, pHigh, pLow)) {
					// from+1优于back
					if (CanExtend(vF[nInvalidFrom], vF[nInvalidFrom + 2], vBH, vBL, pHigh, pLow)) {
						// from+2优于from
						vFValid.pop_back();
						nInvalidTo = ++nInvalidFrom;
					}
					else {
						// from优于from+2，但from和from+1冲突
						for (auto it = vFValid.rbegin();;) {
							if (++it == vFValid.rend()) {
								// 择一保留，选择保留from+1
								vFValid.pop_back();
								nInvalidTo = ++nInvalidFrom;
								break;
							}
							if (!CanExtend(*it, vF[nInvalidFrom], vBH, vBL, pHigh, pLow)) {
								// 保留from+1, 删除from
								vFValid.pop_back();
								nInvalidTo = ++nInvalidFrom;
								break;
							}
							if (++it == vFValid.rend()) {
								// 择一保留，选择保留from+1
								vFValid.pop_back();
								nInvalidTo = ++nInvalidFrom;
								break;
							}
							if (!CanExtend(*it, vF[nInvalidFrom + 1], vBH, vBL, pHigh, pLow)) {
								// 保留from，删除from+1
								vFValid.push_back(vF[nInvalidFrom]);
								nInvalidFrom += 3;
								nInvalidTo = nInvalidFrom;
								break;
							}
						}
					}
				}
				else {
					// back优于from+1
					if (CanExtend(vF[nInvalidFrom], vF[nInvalidFrom + 2], vBH, vBL, pHigh, pLow)) {
						// from+2优于from
						nInvalidFrom += 2;
						nInvalidTo = nInvalidFrom;
					}
					else {
						// from优于from+2
						vFValid.push_back(vF[nInvalidFrom]);
						nInvalidFrom += 3;
						nInvalidTo = nInvalidFrom;
					}
				}
			}
			else {
				// [from X from+1 X ... X to, to+1]
				if (CanExtend(vF[nInvalidFrom], vF[nInvalidFrom + 2], vBH, vBL, pHigh, pLow)) {
					// from+2优于from
					nInvalidFrom += 2;
					nInvalidTo = nInvalidFrom;
				}
				else {
					// from优于from+2
					if (CanExtend(vF[nInvalidFrom + 1], vF[nInvalidFrom + 3], vBH, vBL, pHigh, pLow)) {
						// from+3优于from+1
						if (IsValidBi(vF[nInvalidFrom], vF[nInvalidFrom + 3], vBH, vBL, pHigh)) {
							vFValid.push_back(vF[nInvalidFrom]);
							nInvalidFrom += 3;
							nInvalidTo = nInvalidFrom;
						}
						else {
							nInvalidFrom += 3;
							nInvalidTo = nInvalidFrom;
						}
					}
					else {
						// from+1优于from+3，from与from+1冲突
						nInvalidTo = ++nInvalidFrom;
						if (IsValidBi(vF[nInvalidFrom + 1], vF[nInvalidFrom + 4], vBH, vBL, pHigh)) {
							vFValid.push_back(vF[nInvalidFrom + 1]);
							nInvalidFrom += 4;
							nInvalidTo = nInvalidFrom;
						}
						else {
							nInvalidFrom += 4;
							nInvalidTo = nInvalidFrom;
						}
					}
				}
			}
			break;
		default:
			assert(0);
			break;
		}
	}
	for (size_t i = nInvalidFrom; i < vF.size(); i += 2) {
		if (i + 2 < vF.size()) {
			if (!CanExtend(vF[i], vF[i + 2], vBH, vBL, pHigh, pLow)) {
				vFValid.push_back(vF[i]);
				break;
			}
		}
		else {
			vFValid.push_back(vF[i]);
			break;
		}
	}
	for (auto it = vFValid.begin(); it != vFValid.end(); ++it) {
		if (*it > 0) {
			// 顶分型
			pOut[vBH[*it]] = 1;
		}
		else {
			pOut[vBL[-(*it)]] = -1;
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

// 在pIn标记的高低点处（高=1，低=-1），输出从上一个标记点到当前点的高低连线斜率
void Func8(int nCount, float* pOut, float* pIn, float* pHigh, float* pLow)
{
	int x = 0;
	int i = 0;

	for (; i < nCount; ++i) {
		pOut[i] = 0;
		if (pIn[i] < -0.5 || 0.5 < pIn[i]) {
			x = i;
			break;
		}
	}
	for (++i; i < nCount; ++i) {
		if (pIn[i] < -0.5) {
			pOut[i] = (pLow[i] - pHigh[x]) / (i - x);
			x = i;
		}
		else if (0.5 < pIn[i]) {
			pOut[i] = (pHigh[i] - pLow[x]) / (i - x);
			x = i;
		}
		else {
			pOut[i] = 0;
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
