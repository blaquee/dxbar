#include <Windows.h>
#include <iostream>
#include <shlwapi.h>
#include <string.h>
#include "regkey.h"

#define DLL_NAME L"\\DxBar.dll"

//#define SPLASH_SCREEN
//#include "Splash.h"

//global vars
static wchar_t currentDir[2000];

using namespace std;

void InjectDLL_RemoteThread(DWORD pid,const wchar_t *dll) 
{
    HANDLE hProc = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_VM_WRITE|PROCESS_VM_OPERATION|PROCESS_QUERY_INFORMATION, FALSE ,pid);

    if(hProc == 0)
    {
        MessageBoxW(0, L"Failed to open process", L"Error", 0);
        return;
    }

    DWORD written=0;
    size_t pathLen = (wcslen(dll)+1)*sizeof(wchar_t);

    LPVOID remoteDll = (char*)VirtualAllocEx(hProc,0,pathLen,MEM_COMMIT,PAGE_READWRITE);

    if(remoteDll == 0)
    {
        MessageBoxW(0, L"Could not allocated memory", L"Error", 0);
        CloseHandle(hProc);
        return;
    }

    WriteProcessMemory(hProc,remoteDll,dll,pathLen,&written);
    if (written != pathLen)
    {
        MessageBoxW(0,L"Failed to inject",L"error",MB_ICONEXCLAMATION|MB_OK);
        CloseHandle(hProc);
        return;
    }

    HMODULE kernel32 = GetModuleHandle(L"kernel32.dll");

    if(kernel32 == 0)
    {
        MessageBoxW(0, L"Failed to open kernel32", L"Error", 0);
        CloseHandle(hProc);
        return;
    }

    UINT_PTR remoteLoadLib = (UINT_PTR)GetProcAddress(kernel32, "LoadLibraryW");

    wchar_t readyKeyName[20];
    swprintf(readyKeyName, 20, L"ready%d", pid);

    RegKey key(HKEY_CURRENT_USER);
    key.Open(L"Software\\DxBar");
    key[readyKeyName] = DWORD(0);
    // flush the registry manually forces it to save the change
    key.Flush();
    key.Close();

    key.Open(L"Software\\DxBar");

    DWORD th;
    HANDLE hTh = CreateRemoteThread(hProc,0,0,(LPTHREAD_START_ROUTINE)remoteLoadLib,remoteDll,CREATE_SUSPENDED,&th);

    if(hTh == 0)
    {
        MessageBoxW(0, L"Failed to create thread", L"Error", 0);
        CloseHandle(hProc);
        return;
    }
    ResumeThread(hTh);

    DWORD ret;

    bool ready = false;

    do
    {
        if(GetExitCodeProcess(hProc, &ret) && ret == STILL_ACTIVE)
        {
            RegValue rv = key[readyKeyName];
            ready = (rv == DWORD(1));
            Sleep(1);
        }
        else
        {
            MessageBoxW(0, L"Failed to properly initialize target process", L"Error", 0);
            ready = true;
        }

    }while(ready == false);

    key[readyKeyName].DeleteValue();

    VirtualFreeEx(hProc, remoteDll, 0, MEM_RELEASE);

    CloseHandle(hTh);
    CloseHandle(hProc);
}

inline string GetDirFromString(const string &str)
{
    typedef string::size_type SizeType;

    SizeType pos = str.rfind("\\");

    return pos != string::npos ? str.substr( 0,  pos+1 ) : "";
}

inline basic_string<wchar_t> GetDirFromString(const basic_string<wchar_t> &str)
{
    typedef basic_string<wchar_t>::size_type SizeType;

    SizeType pos = str.rfind(L"\\");

    return pos != basic_string<wchar_t>::npos ? str.substr( 0,  pos+1 ) : L"";
}



bool openFile(PROCESS_INFORMATION &pi)
{

    OPENFILENAMEW ofn;
    memset(&ofn,0,sizeof(OPENFILENAME));
    ofn.hInstance = GetModuleHandle(0);
    ofn.lpstrTitle = L"Select a target";
    ofn.lStructSize = sizeof(OPENFILENAME);          //ofn.
    ofn.lpstrFilter = L"App Files (*.exe, *.bat)\0*.exe;*.bat\0" L"All Files (*.*)\0*.*\0" L"\0";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrFile = new wchar_t[2000];
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 2000;
    if(GetOpenFileName(&ofn)) 
    {
        PROCESS_INFORMATION process;
        STARTUPINFO start;
        memset(&start,0,sizeof(start));
        start.cb = sizeof(start);

        basic_string<wchar_t> dir = GetDirFromString(ofn.lpstrFile);

        if(dir.empty())
        {
            CreateProcessW(0,ofn.lpstrFile,0,0,FALSE,CREATE_SUSPENDED|IDLE_PRIORITY_CLASS,0,0,&start,&process);
        }
        else
        {
            CreateProcessW(0,ofn.lpstrFile,0,0,FALSE,CREATE_SUSPENDED|IDLE_PRIORITY_CLASS,0,dir.c_str(),&start,&process);
        }
        pi = process;
        return true;
    }

    return false;
}

void getCurrentDir()
{
    GetCurrentDirectory(sizeof(currentDir)/sizeof(wchar_t), currentDir);
}

//write the current directory to registry
void writeRegistry()
{
    RegKey key(HKEY_CURRENT_USER);
    key.Create(L"Software\\DxBar");
    key[L"path"] = currentDir;
}

void showSplashScreen(HINSTANCE hInstance)
{
#if defined(SPLASH_SCREEN)
    CSplash splash;
    HBITMAP pic = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_SPLASH));
    splash.SetBitmap(pic);
    splash.ShowSplash();
    Sleep(5000);
    splash.CloseSplash();
#endif
}

inline void Tokenize(const string & str, vector<string> & tokens, const string & delimiters = " ")
{
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
}

inline string Join(const vector<string > & seq, const string & seperator)
{
    vector<string>::size_type len = seq.size(), i;

    if ( len == 0 ) 
        return "";

    if ( len == 1 ) 
        return seq[0];

    string res( seq[0] );

    for ( i = 1; i < len; ++i )
    {
        res += seperator + seq[i];
    }


    return res;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};

    vector<string> tokens;

    Tokenize(lpCmdLine, tokens, " ");

    si.cb = sizeof(STARTUPINFO);
    si.lpReserved = NULL;

    getCurrentDir();

    writeRegistry();

    showSplashScreen(hInstance);

    wchar_t dllName[] = DLL_NAME;
    wchar_t *dllPath = new wchar_t[wcslen(currentDir) + sizeof(dllName)/sizeof(wchar_t) + 2];
    wcscpy(dllPath, currentDir);
    wcscat(dllPath, dllName);

    if(tokens.empty())
    {
        if(openFile(pi))
        {
            InjectDLL_RemoteThread(pi.dwProcessId, dllPath);
            ResumeThread(pi.hThread);
        }
        else
            MessageBoxW(NULL, L"Can't open file", L"ERROR", NULL);
    }
    else
    {
        string command = Join(tokens, " ");
        string currentDir = GetDirFromString(command);
        size_t commandLen = command.length() + 1;
        size_t currentDirLen = currentDir.length() + 1;

        wchar_t *commandW = new wchar_t[commandLen];
        wchar_t *currentDirW = currentDir.empty() ? 0 : new wchar_t[currentDirLen];

        if(currentDirW)
            MultiByteToWideChar(CP_UTF8, 0, currentDir.c_str(), -1, currentDirW, (int)currentDirLen);
        
        MultiByteToWideChar(CP_UTF8, 0, command.c_str(), -1, commandW, (int)commandLen);
        
        BOOL success = CreateProcessW(0,commandW,0,0,FALSE,CREATE_SUSPENDED|IDLE_PRIORITY_CLASS,0,currentDirW,&si,&pi);
    
        delete[] commandW;
        delete[] currentDirW;

        if(success == FALSE)
        {
            MessageBoxW(0, L"Failed to create process", L"Error", 0);
        }
        else
        {
            InjectDLL_RemoteThread(pi.dwProcessId, dllPath);
            ResumeThread(pi.hThread);
        }

    }

    return 0;
}

