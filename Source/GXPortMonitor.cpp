#include "GXPortMonitor.h"

GXPortMonitor::GXPortMonitor() {
    CtrlLayout(*this, "GXPortMonitor");
    array.AddColumn("Address");
    regexBox.Set(1);
    refreshBtn <<= THISBACK(onRefresh);
}

void GXPortMonitor::onRefresh() {
    DWORD dwScanPid = 0;

    try {
        dwScanPid = stoul(string(pidEdit.Get()));
        if (!regexBox.Get())
            refreshList(dwScanPid);
        else
            refreshListRegex(dwScanPid);
    }
    catch (...) {
        ErrorOK("Incorrect PID! Try again...");
    }
}

void GXPortMonitor::refreshListRegex(DWORD dwScanPid) {
    array.Clear();

    char buffer[2048];
    string result = "";

    FILE* pipe = popen("netstat -aop udp", "r");

    if (!pipe) {
        ErrorOK("Can't execute NETSTAT.EXE! Try again...");
        return;
    }

    while (!feof(pipe)) {
        if (fgets(buffer, 2048, pipe) != NULL)
            result += buffer;
    }

    pclose(pipe);
    vector<ConnectLine> con;
    regex rgx("UDP\\s*([0-9]+.[0-9]+.[0-9]+.[0-9]+:[0-9]+)\\s*\\*:\\*\\s*([0-9]+)");
    cmatch mtc;

    while(regex_search(result.c_str(), mtc, rgx)) {
        ConnectLine line = {mtc[1], stoul(mtc[2])};
        con.push_back(line);
        result = mtc.suffix();
    }

    for (ConnectLine line : con) {
        if (line.pid == dwScanPid)
            array.Add(line.addr.c_str());
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
                array.Add((getIp(row.dwLocalAddr) + ":" + to_string(row.dwLocalPort)).c_str());
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
                array.Add((getIp(row.dwLocalAddr) + ":" + to_string(row.dwLocalPort)).c_str());
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
