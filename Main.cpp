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
#include <cassert>

#include "Main.h"

/* ����TDXָ�깫ʽ
DLL:=TDXDLL1(1,H,L,RATE); {K�߽���ʹ��11�Žӿ��滻1�Žӿ�}
HIB:=TDXDLL1(2,DLL,H,L); {����ߵ�}
LOB:=TDXDLL1(3,DLL,H,L); {����͵�}
SIG:=TDXDLL1(4,DLL,H,L); {�������յ�}
BSP:=TDXDLL1(5,DLL,L,H); {������������}
SLP:=TDXDLL1(8,DLL,L,H); {�߶�б��}
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

// ��ȡ��������������K�ߵ�����ֵ��Ӧ��ԭʼK��������szL��szH��������������
int ReadKLines(int nReadCount, int nSkip, int nCount, float* pOut, int* szH, int* szL)
{
	assert(nReadCount >= 0);
	assert(nSkip >= -1);
	assert(nCount >= 0);
	assert(pOut);
	assert(szH);
	assert(szL);

	int nH = 0;	// szH[]����
	int nL = 0;	// szL[]����

	// ��ȡ������K��
	for (int i = nSkip + 1; (nH < nReadCount || nL < nReadCount) && i < nCount; ++i) {
		if (pOut[i] < 0.5) {
			// 0=��ȡ
		}
		else if (pOut[i] < 1.5) {
			// 1=ȡ��
			if (nH < nReadCount) {
				szH[nH++] = i;
			}
		}
		else if (pOut[i] < 2.5) {
			// 2=ȡ��
			if (nL < nReadCount) {
				szL[nL++] = i;
			}
		}
		else {
			// 3=��ȡ
			if (nH < nReadCount) {
				szH[nH++] = i;
			}
			if (nL < nReadCount) {
				szL[nL++] = i;
			}
		}
	}

	// ���pOut[]���ضϣ�nH���ܲ�����nL
	int ret = (nH < nL) ? nH : nL;

	for (int i = 0; i < ret; ++i) {
		pOut[szH[i]] = 0;
		pOut[szL[i]] = 0;
	}
	return ret;
}

// ��Ǳʵĸߵ͵㣨�ߵ�=1���͵�=-1��
void Func1(int nCount, float* pOut, float* pHigh, float* pLow, float* c)
{
	assert(nCount > 0);
	assert(pOut);
	assert(pHigh);
	assert(pLow);
	if (nCount < 5) return;

	int nH = 0;	// ��һ��K�߸ߵ�����
	int nL = 0;	// ��һ��K�ߵ͵�����
	float fH = pHigh[0];	// ��һ��K�߸ߵ�ֵ
	float fL = pLow[0];	// ��һ��K�ߵ͵�ֵ
	float fPreH = fL;	// ǰһ�����зǰ�����ϵ��K�ߵĸߵ�ֵ

	// K�ߵİ�������
	pOut[0] = 3;	// 1=ȡ�ߣ�2=ȡ�ͣ�3=��ȡ��0=��ȡ
	for (int i = 1; i < nCount; ++i) {
		if (pLow[i] <= fL && fH <= pHigh[i]) {
			// ��ǰK�߰�����һ��K��
			if (fPreH < fH) {
				// ��һ���ߵ����ǰһ�����зǰ�����ϵK�ߵĸߵ��
				// ���ϴ���ȡ���������е���ߵ�Ϊ�ߵ㣬�ε͵�Ϊ�͵�
				pOut[nH] -= 1;
				pOut[i] = 1;
				nH = i;
				fH = pHigh[nH];
			}
			else {
				// ���´���ȡ���������е���͵�Ϊ�͵㣬�θߵ�Ϊ�ߵ�
				pOut[nL] -= 2;
				pOut[i] = 2;
				nL = i;
				fL = pLow[nL];
			}
		}
		else if (fL <= pLow[i] && pHigh[i] <= fH) {
			// ��һ��K�߰�����ǰK��
			if (fPreH < fH) {
				// ��һ���ߵ����ǰһ�����зǰ�����ϵK�ߵĸߵ��
				// ���ϴ���ȡ���������е���ߵ�Ϊ�ߵ㣬�ε͵�Ϊ�͵�
				pOut[nL] -= 2;
				pOut[i] = 2;
				nL = i;
				fL = pLow[nL];
			}
			else {
				// ���´���ȡ���������е���͵�Ϊ�͵㣬�θߵ�Ϊ�ߵ�
				pOut[nH] -= 1;
				pOut[i] = 1;
				nH = i;
				fH = pHigh[nH];
			}
		}
		else {
			// û�а�����ϵ
			fPreH = fH;
			pOut[i] = 3;
			nH = i;
			fH = pHigh[nH];
			nL = i;
			fL = pLow[nL];
		}
	}

	int nBState = 0;	// �ʵ�״̬��0=δ����1=���ϣ�-1=����
	int nFState = 0;	// ����״̬��0=�����У�1=����Ϊ��
	int szH[5];	// K�߸ߵ�
	int szL[5];	// K�ߵ͵�
	int nKSpan = ReadKLines(5, -1, nCount, pOut, szH, szL);	// ������K�ߵ�����
	bool bRunning = true;

	// ��Ǳʵĸߵ͵�
	for (; bRunning; ) {
		switch (nBState) {
		case 1:	// ���ϵı�
			if (nFState) {
				// 1=����Ϊ��
				for (int i = 0; i + 1 < nKSpan; ++i) {
					if (pHigh[szH[i]] > pHigh[szH[i + 1]]) {
						// ������
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
						nFState = 0;	// �����͹�����
						break;
					}
				}
				if (nFState) {
					// ��������
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
				// 0=�����͹�����
				if (nKSpan >= 4 && pHigh[szH[1]] > pHigh[szH[2]] && pHigh[szH[2]] > pHigh[szH[3]]) {
					// 4����������K��
					if (nKSpan >= 5 && pHigh[szH[3]] > pHigh[szH[4]]) {
						// 5����������K��
						pOut[nH] = 1;	// ���±ʵ����
						nL = szL[4];
						szH[0] = szH[4];
						szL[0] = szL[4];
						int n = (szH[0] > szL[0]) ? szH[0] : szL[0];
						nKSpan = ReadKLines(4, n, nCount, pOut, szH + 1, szL + 1) + 1;
						nBState = -1;	// ���±�
						nFState = 1;	// ����Ϊ��
					}
					else if (szL[3] - szH[0] >= 4) {
						// �±ʣ������ǰ�����ϵ�����������K�ߺ͵׷������K��֮��������3��K��
						pOut[nH] = 1;	// ���±ʵ����
						int i = 3;
						nL = szL[i];
						int nRemains = nKSpan - i;
						for (int j = 0; j < nRemains; ++j) {
							szH[j] = szH[i + j];
							szL[j] = szL[i + j];
						}
						int n = (szH[nRemains - 1] > szL[nRemains - 1]) ? szH[nRemains - 1] : szL[nRemains - 1];
						nKSpan = ReadKLines(5 - nRemains, n, nCount, pOut, szH + nRemains, szL + nRemains) + nRemains;
						nBState = -1;	// ���±�
						nFState = 1;	// ����Ϊ��
					}
				}
				if (!nFState) {
					// �м̶�����
					for (int i = 1; i + 1 < nKSpan; ++i) {
						if (pLow[szL[i]] < pLow[szL[i + 1]]) {
							// �׷���
							int nRemains = nKSpan - i - 1;
							for (int j = 0; j < nRemains; ++j) {
								szH[j] = szH[i + 1 + j];
								szL[j] = szL[i + 1 + j];
							}
							int n = (szH[nRemains - 1] > szL[nRemains - 1]) ? szH[nRemains - 1] : szL[nRemains - 1];
							nKSpan = ReadKLines(5 - nRemains, n, nCount, pOut, szH + nRemains, szL + nRemains) + nRemains;
							nFState = 1;	// ���ϱʼ�������
							break;
						}
					}
					if (!nFState) {
						// ���ι�������ֹ
						pOut[nH] = 1;
						bRunning = false;
					}
				}
			}
			break;
		case -1:	// ���µı�
			if (nFState) {
				// 1=����Ϊ��
				for (int i = 0; i + 1 < nKSpan; ++i) {
					if (pLow[szL[i]] < pLow[szL[i + 1]]) {
						// �׷���
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
						nFState = 0;	// �׷��͹�����
						break;
					}
				}
				if (nFState) {
					// ��������
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
				// 0=�׷��͹�����
				if (nKSpan >= 4 && pLow[szL[1]] < pLow[szL[2]] && pLow[szL[2]] < pLow[szL[3]]) {
					// 4����������K��
					if (nKSpan >= 5 && pLow[szL[3]] < pLow[szL[4]]) {
						// 5����������K��
						pOut[nL] = -1;	// ���ϱʵ����
						nH = szH[4];
						szH[0] = szH[4];
						szL[0] = szL[4];
						int n = (szH[0] > szL[0]) ? szH[0] : szL[0];
						nKSpan = ReadKLines(4, n, nCount, pOut, szH + 1, szL + 1) + 1;
						nBState = 1;	// ���ϱ�
						nFState = 1;	// ����Ϊ��
					}
					else if (szH[3] - szL[0] >= 4) {
						// �±ʣ������ǰ�����ϵ���׷������K�ߺͶ��������K��֮��������3��K��
						pOut[nL] = -1;	// ���ϱʵ����
						int i = 3;
						nH = szH[i];
						int nRemains = nKSpan - i;
						for (int j = 0; j < nRemains; ++j) {
							szH[j] = szH[i + j];
							szL[j] = szL[i + j];
						}
						int n = (szH[nRemains - 1] > szL[nRemains - 1]) ? szH[nRemains - 1] : szL[nRemains - 1];
						nKSpan = ReadKLines(5 - nRemains, n, nCount, pOut, szH + nRemains, szL + nRemains) + nRemains;
						nBState = 1;	// ���ϱ�
						nFState = 1;	// ����Ϊ��
					}
				}
				if (!nFState) {
					// �м̵׷���
					for (int i = 1; i + 1 < nKSpan; ++i) {
						if (pHigh[szH[i]] > pHigh[szH[i + 1]]) {
							// ������
							int nRemains = nKSpan - i - 1;
							for (int j = 0; j < nRemains; ++j) {
								szH[j] = szH[i + 1 + j];
								szL[j] = szL[i + 1 + j];
							}
							int n = (szH[nRemains - 1] > szL[nRemains - 1]) ? szH[nRemains - 1] : szL[nRemains - 1];
							nKSpan = ReadKLines(5 - nRemains, n, nCount, pOut, szH + nRemains, szL + nRemains) + nRemains;
							nFState = 1;	// ���±ʼ�������
							break;
						}
					}
					if (!nFState) {
						// ���ι�������ֹ
						pOut[nL] = -1;
						bRunning = false;
					}
				}
			}
			break;
		default:	// 0=δ��
			if (nKSpan >= 5) {
				int nRemains = 1;	// �������һ��K��
				if (pHigh[szH[0]] < pHigh[szH[1]] && pHigh[szH[1]] < pHigh[szH[2]] && pHigh[szH[2]] < pHigh[szH[3]] && pHigh[szH[3]] < pHigh[szH[4]]) {
					// 5����������K��
					nH = szH[4];
					nL = szL[0];
					pOut[nL] = -1;	// ���ϱʵ����
					nBState = 1;
					nFState = 1;
				}
				else if (pLow[szL[0]] > pLow[szL[1]] && pLow[szL[1]] > pLow[szL[2]] && pLow[szL[2]] > pLow[szL[3]] && pLow[szL[3]] > pLow[szL[4]]) {
					// 5����������K��
					nH = szH[0];
					nL = szL[4];
					pOut[nH] = 1;	// ���±ʵ����
					nBState = -1;
					nFState = 1;
				}
				else {
					nRemains = nKSpan - 1;	// ȥ����һ��K��
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

//=============================================================================
// �������7�ţ��߶�ǿ�ȷ���ָ��
//=============================================================================

void Func7(int nCount, float *pOut, float *pIn, float *pHigh, float *pLow)
{
  int nPrevTop = 0, nPrevBot = 0;

  for (int i = 0; i < nCount; i++)
  {
	// �����߶θߵ�
	if (pIn[i-1] == 1)
	{
	  // ��Ǹߵ�λ��
	  nPrevTop = i - 1;
	}
	// �����߶ε͵�
	else if (pIn[i-1] == -1)
	{
	  // ��ǵ͵�λ��
	  nPrevBot = i - 1;
	}

	// �����߶μ���ģʽ
	if (pIn[i] == 1)
	{
	  // ���������߶�б��
	  pOut[i] = (pHigh[i] - pLow[nPrevBot]) / pLow[nPrevBot] * 100;
	}
	// �½��߶μ���ģʽ
	else if (pIn[i] == -1)
	{
	  // ���������߶�б��
	  pOut[i] = (pLow[i] - pHigh[nPrevTop]) / pHigh[nPrevTop] * 100;
	}
  }
}

//=============================================================================
// �������8�ţ��߶�б�ʷ���ָ��
//=============================================================================

void Func8(int nCount, float* pOut, float* pIn, float* pHigh, float* pLow)
{
	int nPrevBot = -1;
	int nPrevTop = -1;
	for (int i = 0; i < nCount; ++i) {
		pOut[i] = 0;
		if (pIn[i] > 0.5) {
			// �ߵ㣬����������б��
			if (0 <= nPrevBot) {
				pOut[i] = (pHigh[i] - pLow[nPrevBot]) / (i - nPrevBot);
			}
			nPrevTop = i;
		}
		else if (pIn[i] < -0.5) {
			// �͵㣬�����½���б��
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
