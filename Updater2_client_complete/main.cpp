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
#include <sys/types.h>
#include <sys/stat.h>
#include <strsafe.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

using namespace std;

#define SELF_REMOVE_STRING  TEXT("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\"")

#define DEBUG_PROGRESS true
#define DEBUG_PROGRESS_NONE true


#define IP_LOCAL "127.0.0.1"
#define IP_ADR "192.168.1.90"
#define IP_DDNS "46.107.82.104"
#define IP_ADDRESS IP_LOCAL
//"medvekilatasok.ddns.net"

void DelMe()
{
    TCHAR szModuleName[MAX_PATH];
    TCHAR szCmd[2 * MAX_PATH];
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};

    GetModuleFileName(NULL, szModuleName, MAX_PATH);

    StringCbPrintf(szCmd, 2 * MAX_PATH, SELF_REMOVE_STRING, szModuleName);

    CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}

long long GetFileSize(const std::string& fileName) {
    // long long esetén nagyobb fájlok is átmennek a rendszerrel, 2,98 GB-n próbáltam ki
    FILE* f;
    if (fopen_s(&f, fileName.c_str(), "rb") != 0) {
        return -1;
    }
    _fseeki64(f, 0, SEEK_END);
    const long long len = _ftelli64(f);
    cout<<len<<endl;
    fclose(f);
    return len;
}

///
/// Recieves data in to buffer until bufferSize value is met
///
int RecvBuffer(SOCKET s, char* buffer, int bufferSize, int chunkSize = 4 * 1024) {
    int i = 0;
    while (i < bufferSize) {
        const int l = recv(s, &buffer[i], __min(chunkSize, bufferSize - i), 0);
        if (l < 0) { return l; } // this is an error
        i += l;
    }
    return i;
}

///
/// Sends data in buffer until bufferSize value is met
///
int SendBuffer(SOCKET s, const char* buffer, int bufferSize, int chunkSize = 4 * 1024) {

    int i = 0;
    while (i < bufferSize) {
        const int l = send(s, &buffer[i], __min(chunkSize, bufferSize - i), 0);
        if (l < 0) { return l; } // this is an error
        i += l;
    }
    return i;
}

//
// Sends a file
// returns size of file if success
// returns -1 if file couldn't be opened for input
// returns -2 if couldn't send file length properly
// returns -3 if file couldn't be sent properly
//
long long SendFile(SOCKET s, const std::string& fileName, int chunkSize = 64 * 1024) {

    const long long fileSize = GetFileSize(fileName);
    cout<<fileSize<<endl;
    if (fileSize < 0) { return -1; }

    std::ifstream file(fileName, std::ifstream::binary);
    if (file.fail()) { return -1; }

    if (SendBuffer(s, reinterpret_cast<const char*>(&fileSize),
        sizeof(fileSize)) != sizeof(fileSize)) {
        return -2;
    }

    char* buffer = new char[chunkSize];
    bool errored = false;
    long long i = fileSize;
    while (i != 0) {
        const long long ssize = __min(i, (long long)chunkSize);
        if (!file.read(buffer, ssize)) { errored = true; break; }
        const int l = SendBuffer(s, buffer, (int)ssize);
        if (l < 0) { errored = true; break; }
        i -= l;
    }
    delete[] buffer;

    file.close();

    return errored ? -3 : fileSize;
}

//
// Receives a file
// returns size of file if success
// returns -1 if file couldn't be opened for output
// returns -2 if couldn't receive file length properly
// returns -3 if couldn't receive file properly
//
long long RecvFile(SOCKET s, const std::string& fileName, int chunkSize = 64 * 1024) {
    std::ofstream file(fileName, std::ofstream::binary);
    if (file.fail()) { return -1; }

    long long fileSize;
    if (RecvBuffer(s, reinterpret_cast<char*>(&fileSize),
            sizeof(fileSize)) != sizeof(fileSize)) {
        return -2;
    }

    char* buffer = new char[chunkSize];
    bool errored = false;
    long long i = fileSize;
    while (i != 0) {
        const int r = RecvBuffer(s, buffer, (int)__min(i, (long long)chunkSize));
        if ((r < 0) || !file.write(buffer, r)) { errored = true; break; }
        i -= r;
    }
    delete[] buffer;

    file.close();

    return errored ? -3 : fileSize;
}


void folder_create(string fileName){
    char* folder = new char[600];
    int base = 0;
    for (int i=0; i<fileName.length(); i++){

        if (fileName[i]=='\\'){
            int check = mkdir(folder);
            if (!check){
                //cout<<folder<<endl;
                //printf(" \r");
                    //cout<<"1";
                //printf("Directory created\n");
            }
            else {
                //cout<<folder<<endl;
                //printf(" \r");
                //printf("Unable to create directory\n");
                //cout<<folder<<endl;
                //exit(1);
            }
        }
        folder[i]=fileName[i];
    }
}





int RecvFilecount(SOCKET s, long int& planned){
    char recvb[600];
    string msg = "FILECOUNT";
    cout<<msg<<endl;
    const char* send_msg = msg.c_str();
    cout<<send_msg<<endl;
    send(s,send_msg,20,0);
    recv(s, recvb, 600, 0);
    cout<<"3 "<<recvb<<endl;
    stringstream ss;
    ss<<recvb;
    int ret;
    ss>>ret; ss>>planned;
    cout<<planned<<endl;
    return ret;
}

void RecvFilename(SOCKET s, string& fileName, int i){
    char filename[600];
    //string msg = "" + i;
    //1cout<<"4 "<<msg<<endl;
    //const char* send_msg = msg.c_str();
    char send_msg[20];
    stringstream ss; ss<<i; ss>>send_msg;
    //cout<<"5 "<<send_msg<<endl;
    send(s,send_msg,20,0);
    for (int j=0; j<600; j++)
        filename[j]=0;
    int bytes = recv(s, filename, 600, 0);
    send(s,send_msg,20,0);
    //for (int k=0; k<600; k++)
      //  printf("%c",filename[k]);
    //cout<<"9 "<<filename<<" "<<bytes<<endl;

    fileName=filename;
}

int filesize(string filename){
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

string ExePath() {
    char buffer[MAX_PATH] = { 0 };
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string folder = buffer;
    int i=0;
    for (int i=folder.length(); i>0;i--){
        if (folder[i-1]=='\\')
            break;
        folder.pop_back();
    }
    //cout<<folder<<endl;
    return folder;
}

int mi_ez_int(string adr){
    int k=0; bool error = false;
    //if (adr.length()<4)
      //  cout<<adr<<endl;
    if (adr[adr.length()-1]=='.')
        return 0;
    for (int i=0; i<adr.length(); i++){
        if (adr[i]=='.'){
            //cout<<adr<<" "<<adr[0]<<endl;
            k++;
            error=true;
        } else {
            k--;
            if(k<0)
                k=0;
        }
        if (k==2){
        return 0;
        }
    }
    if (error){
        return 1;
    }
    //if (k>0)
      //  return 1;
    return 2;
}

int fajlkereses_rekurziv(string folder){
    ///cout<<"bajos"<<endl;
    int ret = 0;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (folder.c_str())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        ret++;
        string hm = ent->d_name;
        int k = mi_ez_int(folder+hm);
        if (k==2){
            fajlkereses_rekurziv(folder+hm+"\\");
        }
      }
      closedir (dir);
    } else {
      /* could not open directory */
      perror ("");
      //return EXIT_FAILURE;
    }
    if (ret==2){
        cout<<folder<<endl;
        rmdir(folder.c_str());
    }
    return ret;
}

DWORD __stdcall ClientProc(LPVOID param) {

    cout<<filesize("content")<<endl;

    struct addrinfo hints = { 0 }, * result, * ptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(IP_ADDRESS, "9002", &hints, &result) != 0) {
        return ~0;
    }
    //cout<<"alma"<<endl;
    SOCKET client = INVALID_SOCKET;
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        client = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (client == SOCKET_ERROR) {
            // TODO: failed (don't just return, cleanup)
        }
        if (connect(client, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
            closesocket(client);
            client = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    if (client == SOCKET_ERROR) {
        std::cout << "6 Couldn't create client socket" << std::endl;
        return ~1;
    }
    int t1 = time(NULL);
    int filecount;
    long int planned_bytes, got_bytes = 0;
    filecount = RecvFilecount(client, planned_bytes);
    printf("7 Filecount: %d, planned bytes: %ld\n",filecount, planned_bytes);
    string filename;
    char filenames[filecount][200];
    int filenames_length[filecount];
    for (int i=0; i<filecount; i++){
        send(client,"alma",5,0);
        char file_adr[200];
        filenames_length[i] = recv(client,file_adr,200,0);
        string str = file_adr;
        for (int j=0; j<filenames_length[i]; j++)
            filenames[i][j]=file_adr[j];
        folder_create(filenames[i]);
    }
    int percent_bytes = 0, percent_files;
    for (int i=0; i<filecount; i++){
        percent_bytes = (uintmax_t)got_bytes*100/(uintmax_t)planned_bytes;
        percent_files = i * 100 /filecount;
        if (DEBUG_PROGRESS) printf("Progress -= %d%%: %ld/%ld Bytes, %d%%: %d/%d files=-\n%s\n",percent_bytes,got_bytes,planned_bytes,percent_files,i,filecount,filenames[i]);
        if (!DEBUG_PROGRESS_NONE) cout<<i<<" ";
        long long rc = RecvFile(client, filenames[i], filenames_length[i]);
        got_bytes += filesize(filenames[i]);
        cout<<i<<endl;
        if (DEBUG_PROGRESS) Sleep(10);
    }
    if (DEBUG_PROGRESS) printf("Progress -= %d%%: %ld/%ld Bytes, %d%%: %d/%d files=-\n",percent_bytes,got_bytes,planned_bytes,percent_files,filecount,filecount);
    cout<<"Time: "<<time(NULL)-t1<<" sec, "<<time(NULL)<<" "<<t1<<endl;
    fajlkereses_rekurziv(ExePath()+"content\\");
    cout<<"Time (javito utan): "<<time(NULL)-t1<<" sec, "<<time(NULL)<<" "<<t1<<endl;

    closesocket(client);

    return 0;
}

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);


    {
        // start a client on another thread
        HANDLE hClientThread = CreateThread(NULL, 0, ClientProc, NULL, 0, 0);
        WaitForSingleObject(hClientThread, INFINITE);
        CloseHandle(hClientThread);
    }
    WSACleanup();
    int i;
    cin>>i;
    cout<<i;
    DelMe();
    return 0;
}
