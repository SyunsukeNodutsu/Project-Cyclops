//-----------------------------------------------------------------------------
// File: Subsystem.h
//
// エンジンのサブシステムに継承
//-----------------------------------------------------------------------------
#pragma once

//エンジンのサブシステムインターフェース
struct ISubsystem
{
	static void SetHWND(HWND hwnd) { m_hwnd = hwnd; }

protected:

	static HWND m_hwnd;

};
