#include "GXPortMonitor.h"

GXPortMonitor::GXPortMonitor() {
    CtrlLayout(*this, "GXPortMonitor");
    array.AddColumn("Port");
    refreshBtn <<= THISBACK(onRefresh);
}

void GXPortMonitor::onRefresh() {
    DWORD dwScanPid = 0;

    try {
        dwScanPid = stoul(string(pidEdit.Get()));
        refreshList(dwScanPid);
    }
    catch (...) {
        ErrorOK("Incorrect PID! Try again...");
    }
}

void GXPortMonitor::refreshList(DWORD dwScanPid) {
    array.Clear();

    PMIB_UDPTABLE_OWNER_PID udpTablePid;
    DWORD dwActualSize = 0;

    // OTHER
    DWORD dwStatus = NO_ERROR;
    ULONG dwReserved = 0;

    dwStatus = GetExtendedUdpTable(
        udpTablePid,
        &dwActualSize,
        false,
        AF_INET,
        UDP_TABLE_OWNER_PID,
        dwReserved
    );

    if (dwStatus == NO_ERROR) {
        for (DWORD i = 0; i < udpTablePid->dwNumEntries; i++) {
            MIB_UDPROW_OWNER_PID row = udpTablePid->table[i];

            if (row.dwOwningPid == dwScanPid)
                array.Add(to_string(row.dwLocalPort).c_str());
        }
    } else if (dwStatus == ERROR_INSUFFICIENT_BUFFER) {
        udpTablePid = (PMIB_UDPTABLE_OWNER_PID)malloc(dwActualSize);

        dwStatus = GetExtendedUdpTable(
            udpTablePid,
            &dwActualSize,
            false,
            AF_INET,
            UDP_TABLE_OWNER_PID,
            dwReserved
        );

        for (DWORD i = 0; i < udpTablePid->dwNumEntries; i++) {
            MIB_UDPROW_OWNER_PID row = udpTablePid->table[i];

            if (row.dwOwningPid == dwScanPid)
                array.Add(to_string(row.dwLocalPort).c_str());
        }

        free(udpTablePid);
    }
}

string GXPortMonitor::getIp(DWORD ip) {
    string res;

    res += to_string(ip & 0xFF) + '.';
    res += to_string((ip >> 8) & 0xFF) + '.';
    res += to_string((ip >> 16) & 0xFF) + '.';
    res += to_string((ip >> 24) & 0xFF);
    return res;
}
