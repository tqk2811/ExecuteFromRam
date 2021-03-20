#pragma once
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cassert>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <assert.h>

#include <metahost.h>
#include <mscoree.h>

#define CheckHr(hr)assert(hr >= 0);

//#pragma comment(lib, "alink.lib")
//#pragma comment(lib, "corguids.lib")
//#pragma comment(lib, "format.lib")
//#pragma comment(lib, "mscorsn.lib")
//#pragma comment(lib, "tlbref.lib")
#pragma comment(lib, "mscoree.lib")
#import "mscorlib.tlb" raw_interfaces_only\
            high_property_prefixes("_get","_put","_putref")\
            rename("ReportEvent", "InteropServices_ReportEvent")
#import "MyAppDomainManager.tlb"
using namespace mscorlib;
using namespace MyAppDomainManager;
#include "MinimalHostControl.h"

