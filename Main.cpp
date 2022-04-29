/*****************************************************************************
 * 禅论可视化分析系统
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
// 数学函数部分
//=============================================================================

// 顶底扫描定位函数
void Parse1(int nCount, float *pOut, float *pHigh, float *pLow)
{
  int nState = -1;
  int nHigh  = 0;
  int nLow   = 0;

  for (int i = 1; i < nCount; i++)
  {
    // 设定默认输出为零
    pOut[i] = 0;

    // 寻找高点模式
    if (nState == 1)
    {
      // 如果当前最高大于之前最高，更新位置信息
      if (pHigh[i] >= pHigh[nHigh])
      {
        pOut[nHigh] = 0;
        nHigh = i;
        pOut[nHigh] = 1;
      }

      // 确认转向（原文：当前最高小于高点最低，当前最低小于高点最低）
      if ((pHigh[i] < pHigh[nHigh]) && (pLow[i]  < pLow[nHigh]))
      {
        pOut[nHigh] = 1;

        nState = -1;
        nLow   = i;
      }
    }

    // 寻找低点模式
    else if (nState == -1)
    {
      // 如果当前最低小于之前最低，更新位置信息
      if (pLow[i] <= pLow[nLow])
      {
        pOut[nLow] = 0;
        nLow = i;
        pOut[nLow] = -1;
      }

      // 确认转向（原文：当前最高大于高点最低，当前最低大于高点最低）
      if ((pLow[i]  > pLow[nLow]) && (pHigh[i] > pHigh[nLow]))
      {
        pOut[nLow] = -1;

        nState = 1;
        nHigh  = i;
      }
    }
  }
}

// 化简函数（至少5根K线完成一笔）
void Parse2(int nCount, float *pOut, float *pHigh, float *pLow)
{
  int nSpan = 0;
  int nCurrTop = 0, nPrevTop = 0;
  int nCurrBot = 0, nPrevBot = 0;

  for (int i = 0; i < nCount; i++)
  {
    // 遇到高点，合并化简上升段（上下上）
    if (pOut[i] == 1)
    {
      // 更新位置信息
      nPrevTop = nCurrTop;
      nCurrTop = i;

      // 存在小于五根的线段，去除中间一段
      if ((pHigh[nCurrTop] >= pHigh[nPrevTop]) &&
          (pLow [nCurrBot] >  pLow [nPrevBot]))
      {
        // 检查合法性（严格按照连续五根形成一笔）
        if (((nCurrTop - nCurrBot < 4) && (nCount   - nCurrTop > 4)) ||
             (nCurrBot - nPrevTop < 4) || (nPrevTop - nPrevBot < 4))
        {
          pOut[nCurrBot] = 0;
          pOut[nPrevTop] = 0;
        }
        else if (nCount - nCurrTop > 4)
        {
          // 检查第三段（上）K线合并
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

          // 检查第二段（下）K线合并
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

          // 检查第一段（上）K线合并
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

    // 遇到低点，合并化简下降段（下上下）
    if (pOut[i] == -1)
    {
      // 更新位置信息
      nPrevBot = nCurrBot;
      nCurrBot = i;

      // 存在小于五根的线段，去除中间一段
      if ((pLow [nCurrBot] <= pLow [nPrevBot]) &&
          (pHigh[nCurrTop] <  pHigh[nPrevTop]))
      {
        // 检查合法性（严格按照连续五根形成一笔）
        if (((nCurrBot - nCurrTop < 4) && (nCount   - nCurrBot > 4)) ||
             (nCurrTop - nPrevBot < 4) || (nPrevBot - nPrevTop < 4))
        {
          pOut[nCurrTop] = 0;
          pOut[nPrevBot] = 0;
        }
        else if (nCount - nCurrBot > 4)
        {
          // 检查第三段（下）K线合并
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

          // 检查第二段（上）K线合并
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

          // 检查第一段（下）K线合并
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
// 输出函数1号：线段高低点标记信号
//=============================================================================

void Func1(int nCount, float *pOut, float *pHigh, float *pLow, float *pTime)
{
  // 搜寻所有的高低点
  Parse1(nCount, pOut, pHigh, pLow);

  // 根据设置的遍数，进行化简（第归算法）
  for (int i = 0; i < *pTime; i++)
  {
    Parse2(nCount, pOut, pHigh, pLow);
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
