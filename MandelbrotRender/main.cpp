#include <iostream>
#include <chrono>
#include "WindowWrapper.h"
#include "mandelbrotgenerator.h"

#include <CommCtrl.h>

#define ID_PRECISION_DEC 301
#define ID_PRECISION_INC 302

#define ID_ITERATION_DEC 303
#define ID_ITERATION_INC 304

#define ID_SAVE 305
#define ID_LOCK 306

#define ID_SAVE_DUMP 307
#define ID_LOAD_DUMP 308

#define ID_PAUSE 309


MandelbrotGenerator generator;
HWND precition_disp;
HWND iteration_disp;
HWND imageRenderProcess;
HWND renderStepProcess;
HWND renderTimeDisplay;
HWND lockSensor;
HWND paletteList;
HWND zoomDisplay;


bool isLockSensor = false;
Photo* ptrCanvas;

void renderFrameFunc(HWND hWnd, Photo* canvas, bool* workStat)
{
    ptrCanvas = canvas;

    generator.setScreenSize(2048, 2048);
    generator.setMaxPixelIterationCount(50);
    generator.setZoom(BigFloat("0.002"));
    generator.setPrecision(4);

    generator.setCenterPosition(BigFloat("0.27533764774673799358866712482462788156671406989542628591627436306743751013023030130967197535665363986058288420463735384997362663584446169657773339617717365950286959762265485804783047336923365261060963100721927003791989610861331863571141065592841226995797739723012374298589823921181693139824190379745910243872940870200527114596661654505"),
        BigFloat("0.006759649405327850670181700456194929502189750234614304846357269137106731032582471677573582008294494705826194131450773107049670717146785957633119244225710271178867840504202402362491296317894835321064971518673775630252745135294700216673815790733343134984120108524001799351076577642283751627469315124883962453013093853471898311683555782404"));

    auto gen = [&]() {
        while (*workStat)generator.renderStep();
        };

    std::thread render(gen);
    
    while (*workStat)
    {
        generator.draw(canvas);
    }

    render.join();
}


void mousePress(int32_t x, int32_t y)
{
    if (isLockSensor) return;

    if (x > 1024 || y > 1024) return;
    BigComplex cpos = generator.getCenterPosition();

    BigFloat px = x*2;
    BigFloat py = y*2;

    BigFloat halfWidth = generator.getScreenWidth() * BigFloat("-0.5");
    BigFloat halfHeght = generator.getScreenHeight() * BigFloat("-0.5");

    px = (px + halfWidth) * generator.getZoom();
    py = (py + halfHeght) * generator.getZoom();

    generator.setCenterPosition(cpos.getReal() + px, cpos.getImaginary() + py);
}


void mouseWheel(int32_t delta)
{
    if (isLockSensor) return;
    double k = exp(-double(delta)/500.0);

    BigFloat d(k);
    generator.setZoom( generator.getZoom() * d );
}


HWND createPropertysPanel(HWND parent)
{
    CreateWindowW(L"STATIC",
        L"Precision: ",
        WS_CHILD | WS_VISIBLE,
        1040, 20, 160, 25,
        parent, nullptr, GetModuleHandle(NULL), nullptr);

    precition_disp = CreateWindowW(L"EDIT",
        L"3",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DISABLED,
        1230, 20, 100, 25,
        parent, nullptr, GetModuleHandle(NULL), nullptr);

    CreateWindowW(L"BUTTON",
        L"-",
        WS_CHILD | WS_VISIBLE,
        1200, 20, 30, 25,
        parent, (HMENU) ID_PRECISION_DEC, GetModuleHandle(NULL), nullptr);

    CreateWindowW(L"BUTTON",
        L"+",
        WS_CHILD | WS_VISIBLE,
        1330, 20, 30, 25,
        parent, (HMENU)ID_PRECISION_INC, GetModuleHandle(NULL), nullptr);




    CreateWindowW(L"STATIC",
        L"Iteration count: ",
        WS_CHILD | WS_VISIBLE,
        1040, 50, 160, 25,
        parent, nullptr, GetModuleHandle(NULL), nullptr);

    iteration_disp = CreateWindowW(L"EDIT",
        L"500",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DISABLED,
        1240, 50, 100, 25,
        parent, nullptr, GetModuleHandle(NULL), nullptr);

    CreateWindowW(L"BUTTON",
        L"-10",
        WS_CHILD | WS_VISIBLE ,
        1200, 50, 40, 25,
        parent, (HMENU)ID_ITERATION_DEC, GetModuleHandle(NULL), nullptr);

    CreateWindowW(L"BUTTON",
        L"+10",
        WS_CHILD | WS_VISIBLE,
        1340, 50, 40, 25,
        parent, (HMENU)ID_ITERATION_INC, GetModuleHandle(NULL), nullptr);


     CreateWindowW(L"STATIC",
        L"Image process: ",
        WS_CHILD | WS_VISIBLE,
        1040, 100, 160, 25,
        parent, nullptr, GetModuleHandle(NULL), nullptr);

     imageRenderProcess = CreateWindowEx(0, PROGRESS_CLASS,
        L"",
        WS_CHILD | WS_VISIBLE,
        1040, 125, 500, 25,
        parent, nullptr, GetModuleHandle(NULL), nullptr);

     

     CreateWindowW(L"STATIC",
         L"Render step: ",
         WS_CHILD | WS_VISIBLE,
         1040, 160, 160, 25,
         parent, nullptr, GetModuleHandle(NULL), nullptr);

     renderStepProcess = CreateWindowEx(0, PROGRESS_CLASS,
         L"",
         WS_CHILD | WS_VISIBLE,
         1040, 185, 500, 25,
         parent, nullptr, GetModuleHandle(NULL), nullptr);



     renderTimeDisplay = CreateWindowW(L"STATIC",
         L"Render time: ",
         WS_CHILD | WS_VISIBLE,
         1040, 220, 500, 25,
         parent, nullptr, GetModuleHandle(NULL), nullptr);


     
     CreateWindowW(L"STATIC",
         L"Palette: ",
         WS_CHILD | WS_VISIBLE,
         1040, 350, 500, 25,
         parent, nullptr, GetModuleHandle(NULL), nullptr);

     paletteList = CreateWindowW(L"COMBOBOX",
         L"Palette: ",
         WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
         1040, 375, 200, 25,
         parent, 0, GetModuleHandle(NULL), nullptr);

     LPCTSTR text = L"Standart";
     SendMessage(paletteList, CB_INSERTSTRING, 0, (LPARAM)text);
     text = L"Magma";
     SendMessage(paletteList, CB_INSERTSTRING, 1, (LPARAM)text);
     text = L"Plasma";
     SendMessage(paletteList, CB_INSERTSTRING, 2, (LPARAM)text);
     text = L"Work";
     SendMessage(paletteList, CB_INSERTSTRING, 3, (LPARAM)text);
     SendMessage(paletteList, CB_SETCURSEL, 0, 0);



     
     CreateWindowW(L"BUTTON",
         L"Save screenshot",
         WS_CHILD | WS_VISIBLE,
         1040, 420, 200, 50,
         parent, (HMENU)ID_SAVE, GetModuleHandle(NULL), nullptr);

     CreateWindowW(L"BUTTON",
         L"Save dump.txt",
         WS_CHILD | WS_VISIBLE,
         1250, 420, 200, 50,
         parent, (HMENU)ID_SAVE_DUMP, GetModuleHandle(NULL), nullptr);

     CreateWindowW(L"BUTTON",
         L"Load dump.txt",
         WS_CHILD | WS_VISIBLE,
         1460, 420, 200, 50,
         parent, (HMENU)ID_LOAD_DUMP, GetModuleHandle(NULL), nullptr);



     lockSensor = CreateWindowW(L"BUTTON",
         L"LOCK",
         WS_CHILD | WS_VISIBLE,
         1040, 500, 500, 250,
         parent, (HMENU)ID_LOCK, GetModuleHandle(NULL), nullptr);

     CreateWindowW(L"BUTTON",
         L"PAUSE",
         WS_CHILD | WS_VISIBLE,
         1140, 250, 250, 30,
         parent, (HMENU)ID_PAUSE, GetModuleHandle(NULL), nullptr);


     
     CreateWindowW(L"STATIC",
         L"1px size:",
         WS_CHILD | WS_VISIBLE,
         1040, 800, 100, 25,
         parent, nullptr, GetModuleHandle(NULL), nullptr);

     zoomDisplay = CreateWindowW(L"EDIT",
         L"0",
         WS_CHILD | WS_VISIBLE | WS_BORDER,
         1040+100, 800, 150, 25,
         parent, nullptr, GetModuleHandle(NULL), nullptr);

    
     
     LPARAM lParam = MAKELPARAM(0, 500);
     SendMessage(imageRenderProcess, PBM_SETRANGE, 0, lParam);
     SendMessage(renderStepProcess, PBM_SETRANGE, 0, lParam);

     return precition_disp;
}


void buttons(int32_t id)
{
    static bool pause = false;
    switch (id)
    {
    case ID_PRECISION_DEC:
    {
        int32_t prec = generator.getPrecision();
        if(prec > 1)
            --prec;
        generator.setPrecision(prec);
        SetWindowText(precition_disp, std::to_wstring(prec).c_str());
        break;
    }
        
    case ID_PRECISION_INC:
    {
        int32_t prec = generator.getPrecision();
        ++prec;
        generator.setPrecision(prec);
        SetWindowText(precition_disp, std::to_wstring(prec).c_str());
        break;
    }

    case ID_ITERATION_DEC:
    {
        int32_t iter = generator.getPixelIterationCount();
        if(iter > 10)
            iter -= 10;
        generator.setMaxPixelIterationCount(iter);
        SetWindowText(iteration_disp, std::to_wstring(iter).c_str());
        break;
    }

    case ID_ITERATION_INC:
    {
        int32_t iter = generator.getPixelIterationCount();
        iter += 10;
        generator.setMaxPixelIterationCount(iter);
        SetWindowText(iteration_disp, std::to_wstring(iter).c_str());
        break;
    }

    case ID_SAVE:
    {
        time_t rawtime;
        struct tm timeinfo;
        char buffer[100] = "screen.bmp";

        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);

        strftime(buffer, 100, "screen_%d-%m-%Y_%H.%M.%S.bmp", &timeinfo);

        if(ptrCanvas)
            ptrCanvas->save(buffer);
        break;
    }

    case ID_SAVE_DUMP:
    {
        generator.saveDump("dump.txt");
        break;
    }

    case ID_LOAD_DUMP:
    {
        generator.loadDump("dump.txt");
        break;
    }

    case ID_LOCK:
    {
        isLockSensor = !isLockSensor;
        if(isLockSensor)
            SetWindowText(lockSensor, L"UNLOCK");
        else
            SetWindowText(lockSensor, L"LOCK");
        break;
    }

    case ID_PAUSE:
    {
        pause = !pause;
        generator.changePause(pause);
        break;
    }
    }

    if (HIWORD(id) == CBN_SELCHANGE)
    {
        generator.setPalette(SendMessage(paletteList, CB_GETCURSEL, 0, 0));
    }
}


void updateProgressBars()
{
    while (true)
    {
        SetWindowText(precition_disp, std::to_wstring(generator.getPrecision()).c_str());
        SetWindowText(iteration_disp, std::to_wstring(generator.getPixelIterationCount()).c_str());

        SendMessage(imageRenderProcess, PBM_SETPOS, (WPARAM)(500.0 * (double(generator.getCurrentImageIteration()) / double(generator.getImageIterationCount()))), 0);
        SendMessage(renderStepProcess, PBM_SETPOS, (WPARAM)(500.0 * generator.getCurrentRenderStep()) / generator.getRenderStepCount(), 0);

        std::chrono::duration<double> elapsed_seconds;
        if (generator.getCurrentRenderStep() < generator.getRenderStepCount())
        {
            elapsed_seconds = std::chrono::steady_clock::now() - generator.getStartTime();
        }
        else
        {
            elapsed_seconds = generator.getFinishTime() - generator.getStartTime();
        }
        auto time = std::chrono::duration_cast<std::chrono::seconds>(elapsed_seconds).count();
        int s = time % 60;
        time /= 60;
        int m = time % 60;
        time /= 60;
        int h = time;

        wchar_t timeDisp[200], fromDisp[200];
        wsprintf(timeDisp, L"Render time: %02d:%02d:%02d", h, m, s);
        GetWindowTextW(renderTimeDisplay, fromDisp, 200);
        if(wcscmp(timeDisp, fromDisp))
            SetWindowText(renderTimeDisplay, timeDisp);


        std::string sZoom = generator.getZoom().to_exp_str();
        std::wstring wZoom(sZoom.begin(), sZoom.end());

        GetWindowTextW(zoomDisplay, fromDisp, 200);
        if (wcscmp(wZoom.c_str(), fromDisp))
            SetWindowText(zoomDisplay, wZoom.c_str());
    }
}

int main()
{
    WindowWrapper mainWnd(L"Mandelbrot render", renderFrameFunc, mousePress, mouseWheel, buttons);
    createPropertysPanel(mainWnd.getHwnd());

    std::thread progBars(updateProgressBars);

    mainWnd.run();

    progBars.detach();
}
