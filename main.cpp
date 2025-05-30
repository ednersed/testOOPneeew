#include <windows.h>
#include <iostream>
#include <locale>
#include <cmath>
using namespace std;

#include "Header.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

HDC hdc;

void SetConsoleSize(int width, int height)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    COORD bufSize = { (SHORT)width, (SHORT)height };
    SetConsoleScreenBufferSize(hOut, bufSize);
    SMALL_RECT winRect = { 0, 0, (SHORT)(width - 1), (SHORT)(height - 1) };
    SetConsoleWindowInfo(hOut, TRUE, &winRect);
    HWND hwnd = GetConsoleWindow();
    if (hwnd)
        MoveWindow(hwnd, 30, 30, width * 9, height * 16, TRUE);
}

void SetupConsoleFont() {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 16;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

// Универсальная функция проверки коллизий
bool CheckCollision(Tank* tank, Point** objects, int objectCount, int objectType)
{
    if (tank->IsDestroyed()) return false;

    bool collision = false;
    for (int i = 0; i < objectCount; i++)
    {
        if (objectType == 0) // Mina
        {
            Mina* mina = static_cast<Mina*>(objects[i]);
            if (!mina->IsExploded() && mina->CheckCollision(tank))
            {
                cout << "BOOM! Tank hit a mine!" << endl;
                tank->TakeDamage(mina->GetDamage());
                mina->Explode();
                collision = true;
            }
        }
        else if (objectType == 1) // Remont
        {
            Remont* remont = static_cast<Remont*>(objects[i]);
            if (!remont->IsUsed() && remont->CheckCollision(tank))
            {
                tank->Heal(remont->GetHealAmount());
                remont->Use();
                collision = true;
            }
        }
    }
    return collision;
}

// Универсальная функция управления танком
void ControlTank(Tank* tank, Point** mineObjects, int mineCount, Point** repairObjects, int repairCount)
{
    if (tank->IsDestroyed()) return;

    int figX = tank->GetX(), figY = tank->GetY();
    while (1)
    {
        if (KEY_DOWN(VK_ESCAPE)) break;
        if (KEY_DOWN(VK_LEFT)) {
            figX -= tank->GetSpeed();
            tank->MoveTo(figX, figY);
            CheckCollision(tank, mineObjects, mineCount, 0);
            CheckCollision(tank, repairObjects, repairCount, 1);
            Sleep(200);
        }
        if (KEY_DOWN(VK_RIGHT)) {
            figX += tank->GetSpeed();
            tank->MoveTo(figX, figY);
            CheckCollision(tank, mineObjects, mineCount, 0);
            CheckCollision(tank, repairObjects, repairCount, 1);
            Sleep(200);
        }
        if (KEY_DOWN(VK_UP)) {
            figY -= tank->GetSpeed();
            tank->MoveTo(figX, figY);
            CheckCollision(tank, mineObjects, mineCount, 0);
            CheckCollision(tank, repairObjects, repairCount, 1);
            Sleep(200);
        }
        if (KEY_DOWN(VK_DOWN)) {
            figY += tank->GetSpeed();
            tank->MoveTo(figX, figY);
            CheckCollision(tank, mineObjects, mineCount, 0);
            CheckCollision(tank, repairObjects, repairCount, 1);
            Sleep(200);
        }
    }
}

int main()
{
    setlocale(LC_ALL, "English");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    SetupConsoleFont();
    system("Color 0F");
    SetConsoleSize(2000, 60);

    HWND hwnd = GetConsoleWindow();

    if (hwnd != NULL)
    {
        RECT r;
        GetWindowRect(hwnd, &r);
        hdc = GetDC(hwnd);

        if (hdc != 0)
        {
            cout << "Tank Battle Demonstration" << endl;
            cout << "========================" << endl;

            // Создаем мины и располагаем в центре по линии горизонта
            const int mineCount = 4;
            Mina* mines[mineCount];
            mines[0] = new Mina(800, 600);
            mines[1] = new Mina(1000, 600);
            mines[2] = new Mina(1200, 600);
            mines[3] = new Mina(900, 750);

            const int repairCount = 3;
            Remont* repairs[repairCount];
            repairs[0] = new Remont(850, 450);
            repairs[1] = new Remont(1050, 450);
            repairs[2] = new Remont(1250, 450);

            // Создаем массивы указателей на Point для универсальной обработки
            Point* BufMines[mineCount];
            Point* BufRepairs[repairCount];

            for (int i = 0; i < mineCount; i++) {
                BufMines[i] = mines[i];
                mines[i]->Show();
            }

            for (int i = 0; i < repairCount; i++) {
                BufRepairs[i] = repairs[i];
                repairs[i]->Show();
            }

            cout << "Battlefield prepared with mines and repair kits positioned in center!" << endl;
            Sleep(1000);

            // Создаем все танки на разных позициях
            Tank mainTank(700, 800, 250, 120, 120, 70, 150, 40);
            HeavyTank heavy(2000, 1000);
            LightTank light(900, 200);
            DamageTank damage(1200, 200);

            // Массив танков для удобного управления
            Tank* BufTank[4] = {&mainTank, &heavy, &light, &damage};
            string tankNames[4] = {"Main Tank", "Heavy Tank", "Light Tank", "Damage Tank"};

            for (int i = 0; i < 4; i++) {
                BufTank[i]->Show();
            }

            cout << "All tanks deployed at equal distances!" << endl;
            for (int i = 0; i < 4; i++) {
                cout << tankNames[i] << ": " << BufTank[i]->GetHealth() << " HP" << endl;
            }

            // Управление танками в цикле
            for (int tankIndex = 0; tankIndex < 4; tankIndex++) {
                cout << "\nPress " << (tankIndex + 1) << " to control " << tankNames[tankIndex] << endl;
                while (1)
                {
                    if (KEY_DOWN(49 + tankIndex)) break; // 49 = '1' в ASCII
                    Sleep(100);
                }

                cout << tankNames[tankIndex] << " control active (arrows, ESC to stop)" << endl;
                ControlTank(BufTank[tankIndex], BufMines, mineCount, BufRepairs, repairCount);


            }

            cout << "\n=== ROCKET ATTACK DEMONSTRATION ===" << endl;

            // Массив целей для ракет (исключаем первый танк)
            Tank* targets[3] = {&light, &heavy, &damage};
            string targetNames[3] = {"Light Tank", "Heavy Tank", "Damage Tank"};
            int rocketPowers[3] = {50, 80, 70};
            int launchPositions[3][2] = {{100, 100}, {1800, 200}, {100, 1000}};

            for (int i = 0; i < 3; i++) {
                cout << "\nPress " << (i + 5) << " to launch rocket at " << targetNames[i] << endl;
                while (1)
                {
                    if (KEY_DOWN(53 + i)) break; // 53 = '5' в ASCII
                    Sleep(100);
                }

                if (!targets[i]->IsDestroyed()) {
                    cout << "Rocket launched at " << targetNames[i] << "!" << endl;
                    Rocket rocket(launchPositions[i][0], launchPositions[i][1],
                                targets[i]->GetX() + targets[i]->GetBodyWidth()/2,
                                targets[i]->GetY() + targets[i]->GetBodyHeight()/2,
                                rocketPowers[i]);
                    rocket.Show();

                    for (int j = 0; j < 200 && rocket.IsActive(); j++) {
                        rocket.MoveToTarget();

                        if (rocket.CheckCollision(targets[i])) {
                            cout << "ROCKET HIT! " << targetNames[i] << "!" << endl;
                            targets[i]->TakeDamage(rocket.GetPower());
                            rocket.Deactivate();

                            // Эффект взрыва
                            HPEN pen = CreatePen(PS_SOLID, 5, RGB(255, 255, 0));
                            HBRUSH brush = CreateSolidBrush(RGB(255, 150, 0));
                            SelectObject(hdc, pen); SelectObject(hdc, brush);
                            Ellipse(hdc, rocket.GetX() - 40, rocket.GetY() - 40,
                                     rocket.GetX() + 40, rocket.GetY() + 40);
                            DeleteObject(brush); DeleteObject(pen);
                            Sleep(300);
                            break;
                        }
                        Sleep(50);
                    }
                } else {
                    cout << targetNames[i] << " already destroyed!" << endl;
                }
            }

            // Последняя ракета по главному танку
            cout << "\nPress 8 to launch rocket at Main Tank" << endl;
            while (1)
            {
                if (KEY_DOWN(56)) break;
                Sleep(100);
            }

            if (!mainTank.IsDestroyed()) {
                cout << "Rocket launched at Main Tank!" << endl;
                Rocket rocket4(1500, 100, mainTank.GetX() + mainTank.GetBodyWidth()/2,
                              mainTank.GetY() + mainTank.GetBodyHeight()/2, 60);
                rocket4.Show();

                for (int i = 0; i < 200 && rocket4.IsActive(); i++) {
                    rocket4.MoveToTarget();

                    if (rocket4.CheckCollision(&mainTank)) {
                        cout << "ROCKET HIT! Main Tank!" << endl;
                        mainTank.TakeDamage(rocket4.GetPower());
                        rocket4.Deactivate();

                        // Эффект взрыва
                        HPEN pen = CreatePen(PS_SOLID, 5, RGB(255, 255, 0));
                        HBRUSH brush = CreateSolidBrush(RGB(255, 150, 0));
                        SelectObject(hdc, pen); SelectObject(hdc, brush);
                        Ellipse(hdc, rocket4.GetX() - 40, rocket4.GetY() - 40,
                                 rocket4.GetX() + 40, rocket4.GetY() + 40);
                        DeleteObject(brush); DeleteObject(pen);
                        Sleep(300);
                        break;
                    }
                    Sleep(50);
                }
            } else {
                cout << "Main Tank already destroyed!" << endl;
            }

            cout << "\n=== FINAL BATTLE REPORT ===" << endl;
            for (int i = 0; i < 4; i++) {
                cout << tankNames[i] << ": " << (BufTank[i]->IsDestroyed() ? "DESTROYED" : "ALIVE")
                     << " (" << BufTank[i]->GetHealth() << " HP)" << endl;
            }

            cout << "\nPress ENTER to exit..." << endl;
            while (1)
            {
                if (KEY_DOWN(VK_RETURN)) break;
                Sleep(100);
            }

            // Очистка памяти
            for (int i = 0; i < mineCount; i++) delete mines[i];
            for (int i = 0; i < repairCount; i++) delete repairs[i];

            ReleaseDC(hwnd, hdc);
        }
    }

    system("pause");
    return 0;
}
