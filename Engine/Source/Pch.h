#pragma once

//--------------------------------------------------
// 
// Windows
// 
//--------------------------------------------------

#define NOMINMAX //#undef max/max

#include <SDKDDKVer.h> //最新のSDKを自動的に定義する
#include <windows.h>
#pragma comment(lib, "winmm.lib")

//Windows Runtime Library(ComPtr)
#include <wrl.h>
#pragma comment(lib, "runtimeobject.lib")

using Microsoft::WRL::ComPtr;

//Win32リソース用
#include "Resource.h"

//--------------------------------------------------
// 
// C Liblary
// 
//--------------------------------------------------
#include <cassert>

//--------------------------------------------------
// 
// C++ Liblary
// 
//--------------------------------------------------

//General
#include <chrono>
#include <memory>

//Stream IO
#include <filesystem>
#include <sstream>
#include <iostream>
#include <istream>
#include <fstream>

//Thread
#include <thread>
#include <mutex>

//task
#include <ppltasks.h>

//Numerical processing
#include <random>

//STL
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <array>
#include <string>
#include <algorithm>
#include <source_location>

//--------------------------------------------------
// 
// Engine
// 
//--------------------------------------------------
#include "Engine.h"
