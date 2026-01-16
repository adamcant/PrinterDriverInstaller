#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>

#define ID_PRINTER_NAME   1
#define ID_PRINTER_IP     2
#define ID_DRIVER_PATH    3
#define ID_GENERATE_BTN   4
#define ID_EXIT_BTN       5
#define ID_RADIO_PS       6
#define ID_RADIO_EXE      7
#define ID_BROWSE_BTN     8

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void CreateControls(HWND);
void SavePowerShellScript(const char*, const char*, const char*);
void SaveBatchScript(const char* printerName, const char* printerIP, const char* driverPath);
void CopyDriverToOutput(const char* driverPath);
void OpenFileExplorer(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    MSG messages;
    WNDCLASS wincl;
    HWND hwnd;

    wincl.hInstance = hInstance;
    wincl.lpszClassName = "PrinterSetupClass";
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClass(&wincl)) return 0;

    hwnd = CreateWindow(
        "PrinterSetupClass",
        "Printer Setup",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        600,
        400,
        HWND_DESKTOP,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}


static HWND hPrinterName, hPrinterIP, hDriverPath, hGenerateBtn, hExitBtn, hRadioPS, hRadioExe, hBrowseBtn;

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            CreateControls(hwnd);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_BROWSE_BTN:
                    OpenFileExplorer(hwnd);
                    break;

                case ID_GENERATE_BTN: {
                    char printerName[256];
                    char printerIP[16];
                    char driverPath[1024];

                    GetWindowText(hPrinterName, printerName, sizeof(printerName));
                    GetWindowText(hPrinterIP, printerIP, sizeof(printerIP));
                    GetWindowText(hDriverPath, driverPath, sizeof(driverPath));

                    if (strlen(printerName) == 0 || strlen(printerIP) == 0) {
                        MessageBox(hwnd, "Printer Name and IP Address are required.", "Error", MB_OK | MB_ICONERROR);
                    } else {
                        if (SendMessage(hRadioPS, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                            SavePowerShellScript(printerName, printerIP, driverPath);
                            MessageBox(hwnd, "PowerShell script generated successfully.", "Success", MB_OK | MB_ICONINFORMATION);
                        } else if (SendMessage(hRadioExe, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                            SaveBatchScript(printerName, printerIP, driverPath);
                            MessageBox(hwnd, "Batch script generated successfully.", "Success", MB_OK | MB_ICONINFORMATION);
                        }
                    }
                    break;
                }

                case ID_EXIT_BTN:
                    PostQuitMessage(0);
                    break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

void CreateControls(HWND hwnd) {
    CreateWindow("STATIC", "Printer Name:", WS_VISIBLE | WS_CHILD, 60, 20, 100, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("STATIC", "Printer IP:", WS_VISIBLE | WS_CHILD, 60, 60, 100, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("STATIC", "Driver Path:", WS_VISIBLE | WS_CHILD, 60, 100, 100, 20, hwnd, NULL, NULL, NULL);

    hPrinterName = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 160, 20, 400, 20, hwnd, (HMENU) ID_PRINTER_NAME, NULL, NULL);
    hPrinterIP = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 160, 60, 400, 20, hwnd, (HMENU) ID_PRINTER_IP, NULL, NULL);
    hDriverPath = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 160, 100, 300, 20, hwnd, (HMENU) ID_DRIVER_PATH, NULL, NULL);

    hBrowseBtn = CreateWindow("BUTTON", "Browse...", WS_VISIBLE | WS_CHILD, 470, 100, 100, 20, hwnd, (HMENU) ID_BROWSE_BTN, NULL, NULL);

    if (!hBrowseBtn) {
        MessageBox(hwnd, "Failed to create Browse button!", "Error", MB_OK | MB_ICONERROR);
    }

    hRadioPS = CreateWindow("BUTTON", "PowerShell Script", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 20, 140, 150, 20, hwnd, (HMENU) ID_RADIO_PS, NULL, NULL);
    hRadioExe = CreateWindow("BUTTON", "Bat Script", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 180, 140, 150, 20, hwnd, (HMENU) ID_RADIO_EXE, NULL, NULL);
    
    SendMessage(hRadioPS, BM_SETCHECK, BST_CHECKED, 0);

    CreateWindow("BUTTON", "Generate Script", WS_VISIBLE | WS_CHILD, 50, 200, 120, 30, hwnd, (HMENU) ID_GENERATE_BTN, NULL, NULL);
    CreateWindow("BUTTON", "Exit", WS_VISIBLE | WS_CHILD, 220, 200, 80, 30, hwnd, (HMENU) ID_EXIT_BTN, NULL, NULL);
}

void OpenFileExplorer(HWND hwnd) {
    OPENFILENAME ofn;
    char szFile[1024];

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Executable and MSI Files\0*.exe;*.msi\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        SetWindowText(hDriverPath, ofn.lpstrFile);
    } else {
        MessageBox(hwnd, "No file selected or operation canceled.", "Info", MB_OK | MB_ICONINFORMATION);
    }
}


void CopyDriverToOutput(const char* driverPath) {
    char destinationPath[1024];

    sprintf(destinationPath, "PrintSetup\\%s", strrchr(driverPath, '\\') + 1);

    if (GetFileAttributes(driverPath) == INVALID_FILE_ATTRIBUTES) {
        char errorMessage[512];
        sprintf(errorMessage, "Source file does not exist: %s", driverPath);
        MessageBox(NULL, errorMessage, "Error", MB_OK | MB_ICONERROR);
        return;
    }

    if (!CopyFile(driverPath, destinationPath, FALSE)) {
        DWORD error = GetLastError();
        char errorMessage[512];
        sprintf(errorMessage, "Failed to copy driver file. Error code: %lu", error);
        MessageBox(NULL, errorMessage, "Error", MB_OK | MB_ICONERROR);
    } else {
        MessageBox(NULL, "Driver file copied successfully.", "Success", MB_OK | MB_ICONINFORMATION);
    }
}

void SavePowerShellScript(const char* printerName, const char* printerIP, const char* driverPath) {
    int mkdirResult = _mkdir("PrintSetup");
    DWORD lastError = GetLastError();

    if (mkdirResult == -1 && lastError != ERROR_ALREADY_EXISTS) {
        char errorMessage[512];
        sprintf(errorMessage, "Failed to create directory. Error code: %lu", lastError);
        MessageBox(NULL, errorMessage, "Error", MB_OK | MB_ICONERROR);
        return;
    }

    if (strlen(driverPath) > 0) {
        CopyDriverToOutput(driverPath);
    }

    FILE *fp = fopen("PrintSetup\\install_printer.ps1", "w");

    if (fp == NULL) {
        DWORD error = GetLastError();
        char errorMessage[512];
        sprintf(errorMessage, "Failed to create PowerShell script file. Error code: %lu", error);
        MessageBox(NULL, errorMessage, "Error", MB_OK | MB_ICONERROR);
        return;
    }

    fprintf(fp, "### PowerShell Script to Install Printer ###\n\n");

    if (strlen(driverPath) > 0) {
        fprintf(fp, "$driverPath = '%s'\n", driverPath);
        fprintf(fp, "Write-Host 'Installing printer driver from local executable...'\n");
        fprintf(fp, "Start-Process -FilePath $driverPath -ArgumentList '/quiet' -Wait\n\n");
    } else {
        fprintf(fp, "Write-Host 'Using generic built-in Windows printer driver...'\n\n");
    }

    fprintf(fp, "$printerName = '%s'\n", printerName);
    fprintf(fp, "$printerIP = '%s'\n", printerIP);
    fprintf(fp, "Write-Host 'Adding printer...'\n");

    fprintf(fp, "$portName = Add-PrinterPort -Name $printerIP -PrinterHostAddress $printerIP\n");
    fprintf(fp, "Add-Printer -Name $printerName -PortName $printerIP -DriverName 'Microsoft PS Class Driver'\n\n");

    fclose(fp);
}

void SaveBatchScript(const char* printerName, const char* printerIP, const char* driverPath) {
    int mkdirResult = _mkdir("PrintSetup");
    DWORD lastError = GetLastError();

    if (mkdirResult == -1 && lastError != ERROR_ALREADY_EXISTS) {
        char errorMessage[512];
        sprintf(errorMessage, "Failed to create directory. Error code: %lu", lastError);
        MessageBox(NULL, errorMessage, "Error", MB_OK | MB_ICONERROR);
        return;
    }

    if (strlen(driverPath) > 0) {
        CopyDriverToOutput(driverPath);
    }

    FILE *fp = fopen("PrintSetup\\install_printer.bat", "w");

    if (fp == NULL) {
        DWORD error = GetLastError();
        char errorMessage[512];
        sprintf(errorMessage, "Failed to create Batch script file. Error code: %lu", error);
        MessageBox(NULL, errorMessage, "Error", MB_OK | MB_ICONERROR);
        return;
    }

    fprintf(fp, "REM Batch Script to Install Printer\n\n");

    if (strlen(driverPath) > 0) {
        fprintf(fp, "SET driverPath=%%~dp0%s\n", strrchr(driverPath, '\\') + 1);
        fprintf(fp, "ECHO Installing printer driver from local executable...\n");
        fprintf(fp, "START /WAIT %%driverPath%% /quiet\n\n");
    } else {
        fprintf(fp, "ECHO Using generic built-in Windows printer driver...\n\n");
    }

    fprintf(fp, "SET printerName=%s\n", printerName);
    fprintf(fp, "SET printerIP=%s\n", printerIP);
    fprintf(fp, "ECHO Adding printer...\n");

    fprintf(fp, "rundll32 printui.dll,PrintUIEntry /if /b \"%%printerName%%\" /r \"%%printerIP%%\" /m \"Microsoft PS Class Driver\"\n\n");

    fclose(fp);
}