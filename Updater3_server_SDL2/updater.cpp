#include "updater.h"

using namespace std;

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



DWORD __stdcall ClientProc(LPVOID param) {

    struct addrinfo hints = { 0 }, * result, * ptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo("127.0.0.1", "9001", &hints, &result) != 0) {
        return ~0;
    }

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
        std::cout << "Couldn't create client socket" << std::endl;
        return ~1;
    }

    long long rc = RecvFile(client, "D:\\Updater_test_program2.exe");
    if (rc < 0) {
        std::cout << "Failed to recv file: " << rc << std::endl;
    }

    closesocket(client);

    return 0;
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
    return folder;
}

int mi_ez_int(string adr){
    int k=0; bool error = false;
    if (adr[adr.length()-1]=='.')
        return 0;
    for (int i=0; i<adr.length(); i++){
        if (adr[i]=='.'){
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
    return 2;
}

int filesize(string filename){
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int fajlkereses_rekurziv(string folder, int& filecount, vector<string>& fajlok_helye, long int& all_bytes){
    int ret = 0;
    DIR *dir;
    struct dirent *ent;
    cout<<folder<<endl;
    if ((dir = opendir (folder.c_str())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        string hm = ent->d_name;
        int k = mi_ez_int(folder+hm);
        if (k==1){
            filecount++;
            ret++;

            all_bytes+=filesize(folder+hm);
            fajlok_helye.push_back(folder+hm);
        }
        if (k==2){
            ret += fajlkereses_rekurziv(folder+hm+"\\",filecount,fajlok_helye, all_bytes);
        }
      }
      closedir (dir);
    } else {
      /* could not open directory */
      perror ("");
      //return EXIT_FAILURE;
    }
    return ret;
}

void kuldendo(vector<string>& fajlok_helye, vector<string>& fajlok_helye_kuldendo, int filecount){
    int startdirlength = ExePath().length();
    for (int i=0; i<filecount; i++){
        string uj_fajl = "";
        for (int j=0; j<fajlok_helye[i].length()-startdirlength; j++){  /// ki van kommentezve !!!
            uj_fajl= uj_fajl + fajlok_helye[i][j+startdirlength];       /// ki van kommentezve !!!
        }
        fajlok_helye_kuldendo.push_back(uj_fajl);
    }
}

void Find_all_files(int& filecount, vector<string>& fajlok_helye, vector<string>& fajlok_helye_kuldendo, long int& all_bytes){
    string folder = "content\\";                                                                  /// ki van kommentezve !!!
                                                                /// ki van kommentezve !!!
    filecount = fajlkereses_rekurziv(ExePath()+folder,filecount,fajlok_helye, all_bytes);       /// ki van kommentezve !!!
    kuldendo(fajlok_helye,fajlok_helye_kuldendo, filecount);

}

int create_server_process(string port)
{
    int ord=0;
    if (port[3]=='2')
        ord=0;
    if (port[3]=='3')
        ord=1;
    if (port[3]=='4')
        ord=2;

    int filecount;
    vector<string> fajlok_helye;
    vector<string> fajlok_helye_kuldendo;
    vector<int> fajlok_merete;

    long int kuldendo_bytes = 0;

    startT.lock();
    Find_all_files(filecount, fajlok_helye, fajlok_helye_kuldendo, kuldendo_bytes);
    for (int i=0; i<filecount; i++)
        fajlok_merete.push_back(GetFileSize(fajlok_helye[i]));
    startT.unlock();


    plannedB.lock();
    data.all_bytes = kuldendo_bytes;
    plannedB.unlock();

    plannedF.lock();
    data.all_files = filecount;
    plannedF.unlock();


    while (true){
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        sentB.lock();
        data.sent_bytes[ord] = 0;
        sentB.unlock();

        sentF.lock();
        data.sent_files[ord] = 0;
        sentF.unlock();

        startT.lock();
        data.starter_time[ord]=time(NULL);
        startT.unlock();
        {

            char num[30]; stringstream ss_t; ss_t<<filecount; ss_t>>num;
            char temp[30]; strcat(num," "); stringstream ss_k;
            ss_k<<kuldendo_bytes; ss_k>>temp;
            strcat(num,temp);

            struct addrinfo hints = { 0 };
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            hints.ai_flags = AI_PASSIVE;

            struct addrinfo* result = NULL;
            if (0 != getaddrinfo(NULL, port.c_str(), &hints, &result)) {
                // TODO: failed (don't just return, clean up)
            }
            SOCKET server = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
            if (server == INVALID_SOCKET) {
                // TODO: failed (don't just return, clean up)
                cout<<"Err0"<<endl;
            }

            if (bind(server, result->ai_addr, (int)result->ai_addrlen) == INVALID_SOCKET) {
                // TODO: failed (don't just return, clean up)
                cout<<"Err1"<<endl;
            }
            freeaddrinfo(result);



            if (listen(server, SOMAXCONN) == SOCKET_ERROR) {
                // TODO: failed (don't just return, clean up)
                cout<<"Err2"<<endl;
            }

            SOCKET client = accept(server, NULL, NULL);
            boolO.lock();
            data.online[ord] = true;
            boolO.unlock();

            char mindegy[200];
            recv(client,mindegy,200,0);

            send(client,num,20,0);
            for (int i=0; i<filecount;i++){
                char* filename = new char[fajlok_helye_kuldendo[i].length()+1];
                recv(client,filename,5,0);
                for(int j=0; j<fajlok_helye_kuldendo[i].length()+1; j++)
                    filename[j]=fajlok_helye_kuldendo[i][j];
                send(client,filename,fajlok_helye_kuldendo[i].length()+1,0);

                delete[] filename;
            }

            for (int i=0; i<filecount+1; i++){
                int file_num = 1;
                if (file_num>=0 && file_num<filecount){
                    const int64_t rc = SendFile(client, fajlok_helye[i]);
                    if (rc < 0) {
                        std::cout << "Failed to send file: " << rc << std::endl;
                    } else {
                        sentB.lock();
                        data.sent_bytes[ord]+=fajlok_merete[i];
                        sentB.unlock();
                        sentF.lock();
                        data.sent_files[ord]++;
                        sentF.unlock();
                    }
                } else {
                    break;
                }
            }
            closesocket(client);
            closesocket(server);

        }
        WSACleanup();
        boolO.lock();
        data.online[ord] = false;
        boolO.unlock();
    }
}


DWORD __stdcall ServerProc_1(LPVOID param){
    create_server_process("9002");
}

DWORD __stdcall ServerProc_2(LPVOID param){
    create_server_process("9003");
}

DWORD __stdcall ServerProc_3(LPVOID param){
    create_server_process("9004");
}


