/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	GDIPlusManager.cpp																	  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#include "GDIPlusManager.h"
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>

ULONG_PTR gdi_plus_manager::token_ = {};
int gdi_plus_manager::reference_count_ = {};

gdi_plus_manager::gdi_plus_manager()
{
	if (reference_count_++ == 0)
	{
		const Gdiplus::GdiplusStartupInput input;
		GdiplusStartup(&token_, &input, nullptr);
	}
}

gdi_plus_manager::~gdi_plus_manager()
{
	if (--reference_count_ == 0)
	{
		Gdiplus::GdiplusShutdown(token_);
	}
}