#ifndef _GXPortMonitor_GXPortMonitor_h
#define _GXPortMonitor_GXPortMonitor_h

#include <CtrlLib/CtrlLib.h>
// #include <plugin/pcre/Pcre.h>

#include <iphlpapi.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <regex>
#include <vector>

using namespace Upp;
using namespace std;

#define LAYOUTFILE <GXPortMonitor/GXPortMonitor.lay>
#include <CtrlCore/lay.h>

class GXPortMonitor : public WithGXPortMonitorLayout<TopWindow> {
public:
    typedef GXPortMonitor CLASSNAME;
    GXPortMonitor();

private:
    struct ConnectLine {
        string addr;
        DWORD pid;
    };

    void onRefresh();
    void refreshListRegex(DWORD dwScanPid);
    void refreshList(DWORD dwScanPid);

    string getIp(DWORD ip);
};

#endif

