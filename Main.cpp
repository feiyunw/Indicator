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

//=============================================================================
// ��ѧ��������
//=============================================================================

// ����ɨ�趨λ����
void Parse1(int nCount, float *pOut, float *pHigh, float *pLow)
{
  int nState = -1;
  int nHigh  = 0;
  int nLow   = 0;

  for (int i = 1; i < nCount; i++)
  {
    // �趨Ĭ�����Ϊ��
    pOut[i] = 0;

    // Ѱ�Ҹߵ�ģʽ
    if (nState == 1)
    {
      // �����ǰ��ߴ���֮ǰ��ߣ�����λ����Ϣ
      if (pHigh[i] >= pHigh[nHigh])
      {
        pOut[nHigh] = 0;
        nHigh = i;
        pOut[nHigh] = 1;
      }

      // ȷ��ת��ԭ�ģ���ǰ���С�ڸߵ���ͣ���ǰ���С�ڸߵ���ͣ�
      if ((pHigh[i] < pHigh[nHigh]) && (pLow[i]  < pLow[nHigh]))
      {
        pOut[nHigh] = 1;

        nState = -1;
        nLow   = i;
      }
    }

    // Ѱ�ҵ͵�ģʽ
    else if (nState == -1)
    {
      // �����ǰ���С��֮ǰ��ͣ�����λ����Ϣ
      if (pLow[i] <= pLow[nLow])
      {
        pOut[nLow] = 0;
        nLow = i;
        pOut[nLow] = -1;
      }

      // ȷ��ת��ԭ�ģ���ǰ��ߴ��ڸߵ���ͣ���ǰ��ʹ��ڸߵ���ͣ�
      if ((pLow[i]  > pLow[nLow]) && (pHigh[i] > pHigh[nLow]))
      {
        pOut[nLow] = -1;

        nState = 1;
        nHigh  = i;
      }
    }
  }
}

// ������������5��K�����һ�ʣ�
void Parse2(int nCount, float *pOut, float *pHigh, float *pLow)
{
  int nSpan = 0;
  int nCurrTop = 0, nPrevTop = 0;
  int nCurrBot = 0, nPrevBot = 0;

  for (int i = 0; i < nCount; i++)
  {
    // �����ߵ㣬�ϲ����������Σ������ϣ�
    if (pOut[i] == 1)
    {
      // ����λ����Ϣ
      nPrevTop = nCurrTop;
      nCurrTop = i;

      // ����С��������߶Σ�ȥ���м�һ��
      if ((pHigh[nCurrTop] >= pHigh[nPrevTop]) &&
          (pLow [nCurrBot] >  pLow [nPrevBot]))
      {
        // ���Ϸ��ԣ��ϸ�����������γ�һ�ʣ�
        if (((nCurrTop - nCurrBot < 4) && (nCount   - nCurrTop > 4)) ||
             (nCurrBot - nPrevTop < 4) || (nPrevTop - nPrevBot < 4))
        {
          pOut[nCurrBot] = 0;
          pOut[nPrevTop] = 0;
        }
        else if (nCount - nCurrTop > 4)
        {
          // �������Σ��ϣ�K�ߺϲ�
          nSpan = nCurrTop - nCurrBot;
          for (int j = nCurrBot; j < nCurrTop; j++)
          {
            if ((pHigh[j] >= pHigh[j+1]) && (pLow[j] <= pLow[j+1]))
            {
              nSpan--;
            }
          }
          if (nSpan < 4)
          {
            pOut[nCurrBot] = 0;
            pOut[nPrevTop] = 0;
          }

          // ���ڶ��Σ��£�K�ߺϲ�
          nSpan = nCurrBot - nPrevTop;
          for (int j = nPrevTop; j < nCurrBot; j++)
          {
            if ((pHigh[j] >= pHigh[j+1]) && (pLow[j] <= pLow[j+1]))
            {
              nSpan--;
            }
          }
          if (nSpan < 4)
          {
            pOut[nCurrBot] = 0;
            pOut[nPrevTop] = 0;
          }

          // ����һ�Σ��ϣ�K�ߺϲ�
          nSpan = nPrevTop - nPrevBot;
          for (int j = nPrevBot; j < nPrevTop; j++)
          {
            if ((pHigh[j] >= pHigh[j+1]) && (pLow[j] <= pLow[j+1]))
            {
              nSpan--;
            }
          }
          if (nSpan < 4)
          {
            pOut[nCurrBot] = 0;
            pOut[nPrevTop] = 0;
          }
        }
      }
    }

    // �����͵㣬�ϲ������½��Σ������£�
    if (pOut[i] == -1)
    {
      // ����λ����Ϣ
      nPrevBot = nCurrBot;
      nCurrBot = i;

      // ����С��������߶Σ�ȥ���м�һ��
      if ((pLow [nCurrBot] <= pLow [nPrevBot]) &&
          (pHigh[nCurrTop] <  pHigh[nPrevTop]))
      {
        // ���Ϸ��ԣ��ϸ�����������γ�һ�ʣ�
        if (((nCurrBot - nCurrTop < 4) && (nCount   - nCurrBot > 4)) ||
             (nCurrTop - nPrevBot < 4) || (nPrevBot - nPrevTop < 4))
        {
          pOut[nCurrTop] = 0;
          pOut[nPrevBot] = 0;
        }
        else if (nCount - nCurrBot > 4)
        {
          // �������Σ��£�K�ߺϲ�
          nSpan = nCurrBot - nCurrTop;
          for (int j = nCurrTop; j < nCurrBot; j++)
          {
            if ((pHigh[j] >= pHigh[j+1]) && (pLow[j] <= pLow[j+1]))
            {
              nSpan--;
            }
          }
          if (nSpan < 4)
          {
            pOut[nCurrTop] = 0;
            pOut[nPrevBot] = 0;
          }

          // ���ڶ��Σ��ϣ�K�ߺϲ�
          nSpan = nCurrTop - nPrevBot;
          for (int j = nPrevBot; j < nCurrTop; j++)
          {
            if ((pHigh[j] >= pHigh[j+1]) && (pLow[j] <= pLow[j+1]))
            {
              nSpan--;
            }
          }
          if (nSpan < 4)
          {
            pOut[nCurrTop] = 0;
            pOut[nPrevBot] = 0;
          }

          // ����һ�Σ��£�K�ߺϲ�
          nSpan = nPrevBot - nPrevTop;
          for (int j = nPrevTop; j < nPrevBot; j++)
          {
            if ((pHigh[j] >= pHigh[j+1]) && (pLow[j] <= pLow[j+1]))
            {
              nSpan--;
            }
          }
          if (nSpan < 4)
          {
            pOut[nCurrTop] = 0;
            pOut[nPrevBot] = 0;
          }
        }
      }
    }
  }
}

//=============================================================================
// �������1�ţ��߶θߵ͵����ź�
//=============================================================================

void Func1(int nCount, float *pOut, float *pHigh, float *pLow, float *pTime)
{
  // ��Ѱ���еĸߵ͵�
  Parse1(nCount, pOut, pHigh, pLow);

  // �������õı��������л��򣨵ڹ��㷨��
  for (int i = 0; i < *pTime; i++)
  {
    Parse2(nCount, pOut, pHigh, pLow);
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
