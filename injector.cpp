//kernel32, advapi32, shell32, user32, msvcrt.
#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <string>
#include <fstream>

static const wchar_t* CLSID_OA   = L"{57C3D834-F47B-46BA-9924-83C2873B0686}";
static const wchar_t* CLSID_AMSI = L"{4B770032-31CA-43B1-AB0D-32C5FE2F82FA}";
static const wchar_t* DLL_PATH   = L"C:\\Program Files (x86)\\Kaspersky Lab\\Kaspersky Antivirus 21.3\\office_antivirus.dll";

static bool EnsureKey(HKEY root, const std::wstring& subkey) {
    HKEY hKey = nullptr;
    if (RegCreateKeyExW(root, subkey.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return true;
    }
    return false;
}

static bool SetRegValue(HKEY root, const std::wstring& subkey, const std::wstring& valueName, const std::wstring& data) {
    HKEY hKey = nullptr;
    if (RegCreateKeyExW(root, subkey.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS)
        return false;
    DWORD cb = (DWORD)((data.size() + 1) * sizeof(wchar_t));
    LSTATUS st = RegSetValueExW(hKey, valueName.c_str(), 0, REG_SZ, (const BYTE*)data.c_str(), cb);
    RegCloseKey(hKey);
    return st == ERROR_SUCCESS;
}

static bool KeyExists(HKEY root, const std::wstring& subkey) {
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(root, subkey.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return true;
    }
    return false;
}

int main(int argc, const char** argv, const char** envp) {
    //CLSID OA_ComAntivirus
    std::wstring base = L"CLSID\\";
    base += CLSID_OA;
    EnsureKey(HKEY_CLASSES_ROOT, base);
    EnsureKey(HKEY_CLASSES_ROOT, base + L"\\Implemented Categories");
    EnsureKey(HKEY_CLASSES_ROOT, base + L"\\Implemented Categories\\{56FFCC30-D398-11D0-B2AE-00A0C908FA49}");
    EnsureKey(HKEY_CLASSES_ROOT, base + L"\\InprocServer32");

    SetRegValue(HKEY_CLASSES_ROOT, base, L"", L"OA_ComAntivirus Class");
    SetRegValue(HKEY_CLASSES_ROOT, base + L"\\InprocServer32", L"", DLL_PATH);
    SetRegValue(HKEY_CLASSES_ROOT, base + L"\\InprocServer32", L"ThreadingModel", L"Apartment");

    EnsureKey(HKEY_CLASSES_ROOT, base + L"\\Version");
    SetRegValue(HKEY_CLASSES_ROOT, base + L"\\Version", L"", L"1.0");

    std::wstring base2 = L"CLSID\\";
    base2 += CLSID_AMSI;
    EnsureKey(HKEY_CLASSES_ROOT, base2);
    EnsureKey(HKEY_CLASSES_ROOT, base2 + L"\\InprocServer32");
    SetRegValue(HKEY_CLASSES_ROOT, base2, L"", L"AMSI_ComAntivirus Class");
    SetRegValue(HKEY_CLASSES_ROOT, base2 + L"\\InprocServer32", L"", DLL_PATH);
    SetRegValue(HKEY_CLASSES_ROOT, base2 + L"\\InprocServer32", L"ThreadingModel", L"Both");

    EnsureKey(HKEY_CLASSES_ROOT, base2 + L"\\Version");
    SetRegValue(HKEY_CLASSES_ROOT, base2 + L"\\Version", L"", L"1.0");

    //AMSI provider
    std::wstring amsiProv = L"SOFTWARE\\Microsoft\\AMSI\\Providers\\";
    amsiProv += CLSID_AMSI;
    EnsureKey(HKEY_LOCAL_MACHINE, amsiProv);

    EnsureKey(HKEY_CURRENT_USER, L"SOFTWARE\\MyCheat");
    SetRegValue(HKEY_CURRENT_USER, L"SOFTWARE\\MyCheat", L"Trigger", L"0");

    std::wcout << L"Step 1 succses\n";

    std::wstring folder = L"C:\\Program Files (x86)\\Kaspersky Lab\\Kaspersky Antivirus 21.3";
    DWORD attrs = GetFileAttributesW(folder.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        SHCreateDirectoryExW(nullptr, folder.c_str(), nullptr);
    }
    std::wcout << L"Step 2 succses \n";

    std::wcout << L"Step 3 succses \n";
    std::wstring kaspRoot = L"C:\\Program Files (x86)\\Kaspersky Lab";
    attrs = GetFileAttributesW(kaspRoot.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        SetFileAttributesW(kaspRoot.c_str(), attrs | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
    }
    std::wcout << L"Step 4 succses \n";

    std::wcout << L"now name ur dll cheat.dll and place it in C: \n";
    std::cout << "\nDid you place your DLL in the folder? (y/n): ";
    char answer = 0;
    std::cin >> answer;
    if (tolower(answer) != 'y') {
        std::cout << "\n[-] DLL not placed. Exiting loader...\n";
        system("pause");
        exit(0);
    }

    if (MessageBoxW(nullptr, L"Press OK in lobby", L"Injector", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK) {
        // Arm the trigger
        HKEY hKey = nullptr;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\MyCheat", 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
            RegSetValueExW(hKey, L"Trigger", 0, REG_SZ, (const BYTE*)L"1", 4);
            RegCloseKey(hKey);
            std::wcout << L"Succsesfully injected \n\n";
        }
    }

    system("pause");
    return 0;
}
