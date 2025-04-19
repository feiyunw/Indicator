/*****************************************************************************
 * ���ۿ��ӻ�����ϵͳ
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
#include <algorithm>
#include <cassert>
#include <vector>

#include "Main.h"

// ���ͣ���Ϊ�ʵĶ˵㣩�Ƿ������
bool CanExtend(int f1, int f2, const std::vector<int>& vBH, const std::vector<int>& vBL, float* pHigh, float* pLow)
{
	assert(f1 * f2 > 0);
	assert(pHigh);
	assert(pLow);

	if (f1 > 0) {
		// ������
		if (pHigh[vBH[f1]] < pHigh[vBH[f2]]) {
			// ����Ķ�����ǰ��Ķ�
			return true;
		}
	}
	else {
		// �׷���
		if (pLow[vBL[-f1]] > pLow[vBL[-f2]]) {
			// ����ĵ׵���ǰ��ĵ�
			return true;
		}
	}
	return false;
}

// �Ƿ���Ч��
bool IsValidBi(int f1, int f2, const std::vector<int>& vBH, const std::vector<int>& vBL, float* pHigh)
{
	assert(f1 * f2 < 0);
	assert(pHigh);

	if (f1 > 0) {
		// ������
		if (-f2 < f1 + 3) {
			// ���׷��;�����������󣬲�������K��
			return false;
		}
		int a = std::max(vBH[f1], vBL[f1]);
		int b = std::min(vBH[-f2], vBL[-f2]);
		if (b <= a + 3) {
			// ���������K�ߺ͵׷������K��֮�䣬�����ǰ�����ϵ��������3��K��
			return false;
		}
		if (pHigh[vBH[f1]] < pHigh[vBH[-f2]]) {
			// ���������K�ߵ��������ٲ��ָ��ڵ׷������K�ߵ�����
			return false;
		}
	}
	else {
		// �׷���
		if (f2 < -f1 + 3) {
			// ���׷��;�����������󣬲�������K��
			return false;
		}
		int a = std::max(vBH[-f1], vBL[-f1]);
		int b = std::min(vBH[f2], vBL[f2]);
		if (b <= a + 3) {
			// ���������K�ߺ͵׷������K��֮�䣬�����ǰ�����ϵ��������3��K��
			return false;
		}
		if (pHigh[vBH[f2]] < pHigh[vBH[-f1]]) {
			// ���������K�ߵ��������ٲ��ָ��ڵ׷������K�ߵ�����
			return false;
		}
	}
	return true;
}

// ��Ǳʵĸߵ͵㣨�ߵ�=1���͵�=-1��
void Func1(int nCount, float* pOut, float* pHigh, float* pLow, float* c)
{
	assert(nCount > 0);
	assert(pOut);
	assert(pHigh);
	assert(pLow);

	std::vector<int> vBH;	// ��������������K�߸ߵ��Ӧ��ԭK������
	std::vector<int> vBL;	// ��������������K�ߵ͵��Ӧ��ԭK������

	// K�ߵİ�������
	vBH.reserve(nCount);
	vBL.reserve(nCount);
	vBH.push_back(0);
	vBL.push_back(0);
	for (int i = 1; i < nCount; ++i) {
		if (pLow[i] <= pLow[vBL.back()] && pHigh[vBH.back()] <= pHigh[i]) {
			// ��ǰK�߰�����һ��K��
			auto itH = ++(vBH.rbegin());
			auto itL = ++(vBL.rbegin());
			// ����ǰK�߰���֮ǰ��������K�ߣ�����ͬ���۲���ȫһ��
			for (; itH != vBH.rend() && itL != vBL.rend(); ++itH, ++itL) {
				// δ��������ǰһ��K��
				if (pHigh[*itH] > pHigh[i]) {
					// ���ڵ�ǰK�ߣ����´���
					vBL.pop_back();
					vBL.push_back(i);
					break;
				}
				if (pLow[*itL] < pLow[i]) {
					// ���ڵ�ǰK�ߣ����ϴ���
					vBH.pop_back();
					vBH.push_back(i);
					break;
				}
			}
			// ����ǰK�߰���֮ǰ����K�ߣ�����Ե�ǰK��
		}
		else if (pLow[vBL.back()] <= pLow[i] && pHigh[i] <= pHigh[vBH.back()]) {
			// ��һ��K�߰�����ǰK��
			auto itH = ++(vBH.rbegin());
			if (itH != vBH.rend()) {
				// �Ƚ���һ��K������ǰһ�����ǰ�����ϵ��K��
				if (pHigh[*itH] > pHigh[vBH.back()]) {
					// ���´���
					vBH.pop_back();
					vBH.push_back(i);
				}
				else {
					// ���ϴ���
					vBL.pop_back();
					vBL.push_back(i);
				}
			}
			else {
				// ����ǰK���ǵڶ����ұ���һ�����������õ�ǰK���滻��һ��
				vBH.pop_back();
				vBH.push_back(i);
				vBL.pop_back();
				vBL.push_back(i);
			}
		}
		else {
			// û�а�����ϵ
			vBH.push_back(i);
			vBL.push_back(i);
		}
	}

	std::vector<int> vF;	// �����б�>0=�����͵�K�߱�ţ�����������󣩣�<0=-�׷��͵�K�߱��

	// ��Ƿ���
	if (vBH.size() >= 6) {
		bool bUp = false;	// ��ǰ�ʵ�״̬��true=���ϣ�false=����
		if (pHigh[vBH[0]] < pHigh[vBH[1]]) {
			bUp = true;
		}
		for (size_t i = 1; i + 1 < vBH.size(); ++i) {
			if (bUp) {
				if (pHigh[vBH[i]] > pHigh[vBH[i + 1]]) {
					// ������
					vF.push_back(i);
					bUp = false;
				}
			}
			else {
				if (pHigh[vBH[i]] < pHigh[vBH[i + 1]]) {
					// �׷���
					vF.push_back(-static_cast<int>(i));
					bUp = true;
				}
			}
		}
	}

	std::vector<int> vFValid;	// ��Ч�ʶ�Ӧ�ķ����б����������vF������
	size_t i = 0;	// �ʵ���ʼ���ͱ��
	size_t j = 1;	// �ʵĽ������ͱ��

	// �����Ч���ͣ���Ӧ��Ч�ʣ�
	for (; j < vF.size(); ) {
		if (IsValidBi(vF[i], vF[j], vBH, vBL, pHigh)) {
			// [i, j]���ɱ�
			vFValid.push_back(i);
			vFValid.push_back(j);
			i = j;
			++j;
			break;
		}
		else {
			// [i, j]�����ɱ�
			if (j + 1 < vF.size()) {
				if (CanExtend(vF[i], vF[j + 1], vBH, vBL, pHigh, pLow)) {
					// j+1����i������i
					i = j;
					++j;
				}
				else {
					// i����j+1������j��j+1
					j += 2;
				}
			}
			else {
				break;
			}
		}
	}
	if (!vFValid.empty()) {
		for (; j < vF.size();) {
			if (IsValidBi(vF[i], vF[j], vBH, vBL, pHigh)) {
				// [i, j]���ɱ�
				i = j;
				++j;
				vFValid.push_back(i);
			}
			else {
				// [i, j]�����ɱ�
				if (j + 1 < vF.size()) {
					if (CanExtend(vF[i], vF[j + 1], vBH, vBL, pHigh, pLow)) {
						// j+1����i����j+1�滻i
						i = j + 1;
						vFValid.pop_back();
						vFValid.push_back(i);
					}
					// eles: i����j+1������j��j+1
					j += 2;
				}
				else {
					break;
				}
			}
		}
	}

	memset(pOut, 0, sizeof(*pOut) * nCount);
	for (auto it = vFValid.begin(); it != vFValid.end(); ++it) {
		if (vF[*it] > 0) {
			// ������
			pOut[vBH[vF[*it]]] = 1;
		}
		else {
			// �׷���
			pOut[vBL[-vF[*it]]] = -1;
		}
	}
}

//=============================================================================
// �������2�ţ�����ߵ�����
//=============================================================================

void Func2(int nCount, float *pOut, float *pIn, float *pHigh, float *pLow)
{
	CCentroid Centroid;

	for (int i = 0; i < nCount; ++i) {
		if (pIn[i] == 1) {
			// �����߶θߵ㣬���������㷨
			if (Centroid.PushHigh(i, pHigh[i])) {
				// �����ڸ�����õ����������
				for (int j = Centroid.GetStart(); j <= Centroid.GetEnd(); ++j) {
					pOut[j] = Centroid.GetPrevHigh();
				}
			}
		}
		else if (pIn[i] == -1) {
			// �����߶ε͵㣬���������㷨
			if (Centroid.PushLow(i, pLow[i])) {
				// �����ڸ�����õ����������
				for (int j = Centroid.GetStart(); j <= Centroid.GetEnd(); ++j) {
					pOut[j] = Centroid.GetPrevHigh();
				}
			}
		}

		// β��δ������ദ��
		if (Centroid.IsInCentre() && (Centroid.GetLines() >= 2) && (i == nCount - 1)) {
			for (int j = Centroid.GetStart(); j < nCount; ++j) {
				pOut[j] = Centroid.GetHigh();
			}
		}
	}
}

//=============================================================================
// �������3�ţ�����͵�����
//=============================================================================

void Func3(int nCount, float *pOut, float *pIn, float *pHigh, float *pLow)
{
	CCentroid Centroid;

	for (int i = 0; i < nCount; ++i) {
		if (pIn[i] == 1) {
			// �����߶θߵ㣬���������㷨
			if (Centroid.PushHigh(i, pHigh[i])) {
				// �����ڸ�����õ����������
				for (int j = Centroid.GetStart(); j <= Centroid.GetEnd(); ++j) {
					pOut[j] = Centroid.GetPrevLow();
				}
			}
		}
		else if (pIn[i] == -1) {
			// �����߶ε͵㣬���������㷨
			if (Centroid.PushLow(i, pLow[i])) {
				// �����ڸ�����õ����������
				for (int j = Centroid.GetStart(); j <= Centroid.GetEnd(); ++j) {
					pOut[j] = Centroid.GetPrevLow();
				}
			}
		}

		// β��δ������ദ��
		if (Centroid.IsInCentre() && (Centroid.GetLines() >= 2) && (i == nCount - 1)) {
			for (int j = Centroid.GetStart(); j < nCount; ++j) {
				pOut[j] = Centroid.GetLow();
			}
		}
	}
}

//=============================================================================
// �������4�ţ�������㡢�յ��ź�
//=============================================================================

void Func4(int nCount, float *pOut, float *pIn, float *pHigh, float *pLow)
{
	CCentroid Centroid;

	for (int i = 0; i < nCount; ++i) {
		if (pIn[i] == 1) {
			// �����߶θߵ㣬���������㷨
			if (Centroid.PushHigh(i, pHigh[i])) {
				// ���б��
				pOut[Centroid.GetStart()] = 1;
				pOut[Centroid.GetEnd()] = 2;
			}
		}
		else if (pIn[i] == -1) {
			// �����߶ε͵㣬���������㷨
			if (Centroid.PushLow(i, pLow[i])) {
				// ���б��
				pOut[Centroid.GetStart()] = 1;
				pOut[Centroid.GetEnd()] = 2;
			}
		}

		// β��δ������ദ��
		if (Centroid.IsInCentre() && (Centroid.GetLines() >= 2) && (i == nCount - 1)) {
			pOut[Centroid.GetStart()] = 1;
			pOut[nCount - 1] = 2;
		}
	}
}

//=============================================================================
// �������5�ţ������������ź�
//=============================================================================

void Func5(int nCount, float *pOut, float *pIn, float *pHigh, float *pLow)
{
	CCentroid Centroid;

	for (int i = 0; i < nCount; ++i) {
		if (pIn[i] == 1) {
			if (Centroid.PushHigh(i, pHigh[i])) {
				// �����������ź�
				pOut[i] = 13;
			}
			else if (Centroid.IsTop1BelowTop2()) {
				// �ڶ��������ź�
				pOut[i] = 12;
			}
			else {
				pOut[i] = 0;
			}
		}
		else if (pIn[i] == -1) {
			if (Centroid.PushLow(i, pLow[i])) {
				// ����������ź�
				pOut[i] = 3;
			}
			else if (Centroid.IsBottom1AboveBottom2()) {
				// �ڶ�������ź�
				pOut[i] = 2;
			}
			else {
				pOut[i] = 0;
			}
		}
	}
}

//=============================================================================
// �������6�ţ���̬�������ź�
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

// ��pIn��ǵĸߵ͵㴦����=1����=-1�����������һ����ǵ㵽��ǰ��ı仯�ʣ��ٷ�����
void Func7(int nCount, float* pOut, float* pIn, float* pHigh, float* pLow)
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
			pOut[i] = (pLow[i] - pHigh[x]) / pHigh[x] * 100;
			x = i;
		}
		else if (0.5 < pIn[i]) {
			pOut[i] = (pHigh[i] - pLow[x]) / pLow[x] * 100;
			x = i;
		}
		else {
			pOut[i] = 0;
		}
	}
}

// ��pIn��ǵĸߵ͵㴦����=1����=-1�����������һ����ǵ㵽��ǰ��ĸߵ�����б��
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

// �㷨������һ�Σ���ͼ��Ѱ�һ������ģ��߽��׻������
void AlgoA(int nCount, float* pOut, float* pHigh, float* pLow, float rate)
{
	// ����5������
	if (nCount < 5) {
		return;
	}

	// ��ѡ����㣺���У�����߼ۣ�����͵ģ�����ͬʱȡ����͵ģ�����ͬȡʱ�����ȵ�
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

	// ���߱���ѡ����Ϊ���ࣺ
	// ������λ�ڱ�ѡ���֮ǰ��������ѡ��㣩���ڸ�����Ӧ���㷨A
	AlgoA(buyIndex - 3, pOut, pHigh, pLow, rate);
	// ������λ�ڱ�ѡ���֮�󣨺���ѡ��㣩���ڸ�����Ӧ���㷨B
	AlgoB(nCount - buyIndex, pOut + buyIndex, pHigh + buyIndex, pLow + buyIndex, rate);
}

// �㷨B����һ�Σ���ͼ�У���֪��ѱ�ѡ�����ǵ�һ�����ߣ�Ѱ�һ������ģ��߽��׻������
void AlgoB(int nCount, float* pOut, float* pHigh, float* pLow, float rate)
{
	// ����5������
	if (nCount < 5) {
		return;
	}

	 // ��ѡ�����㣺����������ͼۣ�����ߵģ�����ͬʱȡ����ߵģ�����ͬȡʱ���ں��
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

	// �����ѡ���ײ��������ģ��㷨����
	float buy = pHigh[0];
	if (!IsGoodTrade(buy, sell, rate)) {
		return;
	}

	// ���߱��������Ϊ���ࣺ
	// ������λ����������֮�䣨�����䣩���ڸ�����Ӧ���㷨BS
	AlgoBS(sellIndex + 1, pOut, pHigh, pLow, rate);
	// ������λ������֮�󣨲������㣩���ڸ�����Ӧ���㷨A
	AlgoA(nCount - sellIndex - 4, pOut + sellIndex + 4, pHigh + sellIndex + 4, pLow + sellIndex + 4, rate);
}

// �㷨BS����һ�Σ���ͼ�У���֪��������ǵ�һ�����ߣ��������������һ�����ߣ�Ѱ�һ������ģ��߽��׻������
void AlgoBS(int nCount, float* pOut, float* pHigh, float* pLow, float rate)
{
	// ����������֮�����ټ��3������
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
	// ��ʱ���м����㣺�ڳ�ʼ���֮�������ͼۣ̾��оֲ�����ֵ�ģ����������������ģ��۸���ͬʱȡʱ�����ȵģ�
	for (int sellIndex = 4; sellIndex < lastSellIndex - 7; ++sellIndex) {
		float sell = pLow[sellIndex];
		if ((4 == sellIndex || pLow[sellIndex - 1] < sell) && sell >= pLow[sellIndex + 1] && IsGoodTrade(firstBuy, sell, rate)) {
			float buy;
			if (sellIndex + 3 < buyIndex) {
				// ������һ���ҵ��ı�ѡ���
				buy = pHigh[buyIndex];
			}
			else {
				// ��ѡ���м���㣺���м�����֮�������߼ۣ�����͵ģ�����ͬʱȡ����͵ģ�����ͬʱȡʱ�����ȵ�
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

			// �м�������ƥ��Ľ��ױ�����������
			if (IsGoodTrade(buy, sell, rate)) {
				// �ҵ����м���㡣���߱��м�����Ϊ���ࣺ
				// ������λ�����֮ǰ��������㣩���ڸ�����Ӧ���㷨B
				AlgoB(buyIndex - 3, pOut, pHigh, pLow, rate);
				// ������λ�����֮�󣨱����䣩���ڸ�����Ӧ���㷨BS
				AlgoBS(nCount - buyIndex, pOut + buyIndex, pHigh + buyIndex, pLow + buyIndex, rate);
				return;
			}
		}
		// ����Ҳ������ʵ��м���㣬������ƽ�Ѱ����һ���м�����
	}

	// �Ҳ������ʵ��м������㣬�������ͷβΪ�����㣬�㷨����
	pOut[0] = -1;
	pOut[nCount - 1] = 1;
}

void Func11(int nCount, float* pOut, float* pHigh, float* pLow, float* pRate)
{
	// *pRate�����߽��׷��ʰٷ���
	assert(pOut);
	assert(pHigh);
	assert(pLow);
	assert(pRate);
    float fRate = *pRate / 100000;

	for (int i = 0; i < nCount; ++i) {
		pOut[i] = 0;
	}
	AlgoA(nCount, pOut, pHigh, pLow, fRate);

	// �����һ������֮��Ѱ�ң�����߼ۣ�����͵ģ�ҪС�����һ��������ߵ���ͼۣ̣�������ͬʱȡ����͵ģ�����ͬʱȡʱ�����ȵ�
	int buyIndex = nCount - 1;
	if (0 <= buyIndex && -0.5 < pOut[buyIndex] && pOut[buyIndex] < 0.5) {
        --buyIndex;
        if (0 <= buyIndex && -0.5 < pOut[buyIndex] && pOut[buyIndex] < 0.5) {
            // �����ڶ������߲��������㣬��Ϊ��ѡ�������Ҹ��ͼ�
            float buy = pHigh[buyIndex];
            float low = pLow[buyIndex];
            for (int i = buyIndex - 1; i >= 0; --i) {
                if (pOut[i] > 0.5) {
                    // �������һ�����㣬��ǣ���͵�����
                    if (buy < pLow[i] && IsGoodTrade(buy, pLow[i], fRate)) {
                        pOut[buyIndex] = -1;
                    }
                    break;
                }
                // �������㣬����Ƿ��и��ͼ�
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
