#ifndef _GXPortMonitor_GXPortMonitor_h
#define _GXPortMonitor_GXPortMonitor_h

#include <CtrlLib/CtrlLib.h>
#include <iphlpapi.h>

#include <string>

using namespace Upp;
using namespace std;

#define LAYOUTFILE <GXPortMonitor/GXPortMonitor.lay>
#include <CtrlCore/lay.h>

class GXPortMonitor : public WithGXPortMonitorLayout<TopWindow> {
public:
    typedef GXPortMonitor CLASSNAME;
    GXPortMonitor();

private:
    void onRefresh();
    void refreshList(DWORD dwScanPid);

    string getIp(DWORD ip);
};

#endif

