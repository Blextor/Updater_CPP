#ifndef UPDATER_H_INCLUDED
#define UPDATER_H_INCLUDED

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>

#include <iostream>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <system_error>
#include <windows.h>
#include <string>
//#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <utility>
#include <winsock2.h>
#include <stdio.h>
#include <sstream>
#include <dirent.h>
#include <windows.h>
#include <fileapi.h>
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::lock

struct Updaters_data{
    long int all_bytes;
    long int sent_bytes[3];
    int all_files;
    int sent_files[3];
    int starter_time[3];
    int estimated_time[3];
    bool online[3];

    Updaters_data(){
        for (int i=0; i<3; i++){
            sent_bytes[i]=0;
            sent_files[i]=0;
            starter_time[i]=0;
            estimated_time[i]=0;
            online[i]=false;
        }
        all_files=0;
        all_bytes=0;
    }
};

extern Updaters_data data;

extern std::mutex   plannedB, /// data változóinak lock-ja
        plannedF,
        sentB,
        sentF,
        startT,
        boolO;

DWORD __stdcall ServerProc_1(LPVOID param);

DWORD __stdcall ServerProc_2(LPVOID param);

DWORD __stdcall ServerProc_3(LPVOID param);



#endif // UPDATER_H_INCLUDED
