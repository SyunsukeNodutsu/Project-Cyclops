#pragma once

//--------------------------------------------------
// 
// Windows
// 
//--------------------------------------------------
#define NOMINMAX //#undef max/max

#include <SDKDDKVer.h> //ç≈êVÇÃSDKÇé©ìÆìIÇ…íËã`Ç∑ÇÈ
#include <windows.h>
#include <windowsx.h>
#pragma comment(lib, "winmm.lib")

//Windows Runtime Library(ComPtr)
#include <wrl.h>
#pragma comment(lib, "runtimeobject.lib")

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
