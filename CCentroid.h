#pragma once
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

class CCentroid
{
private:
	int nTop1, nTop2, nBot1, nBot2;
	int nLines, nStart, nEnd;
	bool m_bInCentre;
	float fTop1, fTop2, fBot1, fBot2;
	float fHigh, fLow, fPHigh, fPLow;

public:
	CCentroid(void);
	~CCentroid();

	// const method
	int GetEnd(void) const;
	float GetHigh(void) const;
	int GetLines(void) const;
	float GetLow(void) const;
	float GetPrevHigh(void) const;
	float GetPrevLow(void) const;
	int GetStart(void) const;
	bool IsBottom1AboveBottom2(void) const;
	bool IsInCentre(void) const;
	bool IsTop1BelowTop2(void) const;

	// non-const method
	bool PushHigh(int nIndex, float fValue);
	bool PushLow(int nIndex, float fValue);
};

inline int CCentroid::GetEnd(void) const
{
    return nEnd;
}

inline float CCentroid::GetHigh(void) const
{
    return fHigh;
}

inline int CCentroid::GetLines(void) const
{
    return nLines;
}

inline float CCentroid::GetLow(void) const
{
    return fLow;
}

inline float CCentroid::GetPrevHigh(void) const
{
    return fPHigh;
}

inline float CCentroid::GetPrevLow(void) const
{
    return fPLow;
}

inline int CCentroid::GetStart(void) const
{
    return nStart;
}

inline bool CCentroid::IsBottom1AboveBottom2(void) const
{
	return fBot1 > fBot2;
}

inline bool CCentroid::IsInCentre(void) const
{
	return m_bInCentre;
}

inline bool CCentroid::IsTop1BelowTop2(void) const
{
	return fTop1 < fTop2;
}
