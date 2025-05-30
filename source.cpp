#include <windows.h>
#include "Header.h"
#include <iostream>
#include <cmath>
using namespace std;

extern HDC hdc;
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

/*----------------------------------------*/
/*        МЕТОДЫ КЛАССА Location          */
/*----------------------------------------*/
Location::Location(int InitX, int InitY) { X = InitX; Y = InitY; }
Location::~Location() {}
int Location::GetX() const { return X; }
int Location::GetY() const { return Y; }
void Location::SetX(int NewX) { X = NewX; }
void Location::SetY(int NewY) { Y = NewY; }
void Location::SetPosition(int NewX, int NewY) { X = NewX; Y = NewY; }

/*----------------------------------------*/
/*        МЕТОДЫ КЛАССА Point             */
/*----------------------------------------*/
Point::Point(int InitX, int InitY) : Location(InitX, InitY) { Visible = false; }
Point::~Point() {}
bool Point::IsVisible() const { return Visible; }
void Point::Show()
{
    Visible = true;
    HPEN pen = CreatePen(PS_SOLID, 4, RGB(255, 0, 0));
    SelectObject(hdc, pen);
    MoveToEx(hdc, X - 5, Y, NULL); LineTo(hdc, X + 5, Y);
    MoveToEx(hdc, X, Y - 5, NULL); LineTo(hdc, X, Y + 5);
    Ellipse(hdc, X - 8, Y - 8, X + 8, Y + 8);
    DeleteObject(pen);
}
void Point::Hide()
{
    Visible = false;
    HPEN pen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X - 10, Y - 10, X + 10, Y + 10);
    DeleteObject(brush); DeleteObject(pen);
}
void Point::MoveTo(int NewX, int NewY)
{
    Hide(); SetPosition(NewX, NewY); Show();
}
void Point::Drag(int Step)
{
    int FigX = GetX(), FigY = GetY();
    while (1)
    {
        if (KEY_DOWN(VK_ESCAPE)) break;
        if (KEY_DOWN(VK_LEFT)) { FigX -= Step; MoveTo(FigX, FigY); Sleep(200); }
        if (KEY_DOWN(VK_RIGHT)) { FigX += Step; MoveTo(FigX, FigY); Sleep(200); }
        if (KEY_DOWN(VK_DOWN)) { FigY += Step; MoveTo(FigX, FigY); Sleep(200); }
        if (KEY_DOWN(VK_UP)) { FigY -= Step; MoveTo(FigX, FigY); Sleep(200); }
    }
}

/*----------------------------------------*/
/*        МЕТОДЫ КЛАССА Tank              */
/*----------------------------------------*/
Tank::Tank(int InitX, int InitY, int bW, int bH, int tW, int tH, int gL, int trH, int spd)
    : Point(InitX, InitY)
{
    bodyWidth = bW; bodyHeight = bH; turretWidth = tW; turretHeight = tH;
    gunLength = gL; trackHeight = trH; speed = spd;
    health = 100;
    isDestroyed = false;
}
Tank::~Tank() {}
void Tank::Drag(int step)
{
    if (isDestroyed) return;

    int figX = GetX(), figY = GetY();
    while (1)
    {
        if (KEY_DOWN(VK_ESCAPE)) break;
        if (KEY_DOWN(VK_LEFT)) { figX -= speed; MoveTo(figX, figY); Sleep(200); }
        if (KEY_DOWN(VK_RIGHT)) { figX += speed; MoveTo(figX, figY); Sleep(200); }
        if (KEY_DOWN(VK_UP)) { figY -= speed; MoveTo(figX, figY); Sleep(200); }
        if (KEY_DOWN(VK_DOWN)) { figY += speed; MoveTo(figX, figY); Sleep(200); }
    }
}
void Tank::Show()
{
    if (isDestroyed) return;

    Visible = true;
    HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH brush = CreateSolidBrush(RGB(0, 128, 0));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X, Y + bodyHeight, X + bodyWidth, Y + bodyHeight + trackHeight);
    DeleteObject(brush);
    brush = CreateSolidBrush(RGB(0, 100, 0)); SelectObject(hdc, brush);
    Rectangle(hdc, X + bodyWidth / 6, Y, X + bodyWidth - bodyWidth / 6, Y + bodyHeight);
    DeleteObject(brush);

    // Башня (если есть)
    if (turretHeight > 0) {
        int turretTop = Y - turretHeight;
        brush = CreateSolidBrush(RGB(0, 80, 0)); SelectObject(hdc, brush);
        Rectangle(hdc, X + (bodyWidth - turretWidth) / 2, turretTop,
                  X + (bodyWidth + turretWidth) / 2, turretTop + turretHeight);
        DeleteObject(brush);

        // ПУШКА из центра башни (если есть)
        if (gunLength > 0) {
            int turretCenterX = X + bodyWidth/2;
            int turretCenterY = turretTop + turretHeight/2;
            int barrelWidth = gunLength;
            int barrelHeight = 20;
            brush = CreateSolidBrush(RGB(70, 70, 70)); SelectObject(hdc, brush);
            Rectangle(hdc, turretCenterX, turretCenterY - barrelHeight/2,
                      turretCenterX + barrelWidth, turretCenterY + barrelHeight/2);
            DeleteObject(brush);
        }
    }

    // Колёса/опоры
    brush = CreateSolidBrush(RGB(20,20,20)); SelectObject(hdc, brush);
    Ellipse(hdc, X + bodyWidth/6, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/6 + trackHeight/2, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth/2 - trackHeight/4, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/2 + trackHeight/4, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth - bodyWidth/6 - trackHeight/2, Y + bodyHeight + trackHeight/4,
        X + bodyWidth - bodyWidth/6, Y + bodyHeight + trackHeight - trackHeight/4);
    DeleteObject(brush); DeleteObject(pen);
}
void Tank::Hide()
{
    Visible = false;
    HPEN blackPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, blackPen); SelectObject(hdc, blackBrush);
    Rectangle(hdc, X - 30, Y - turretHeight - 30,
        X + bodyWidth + gunLength + 40, Y + bodyHeight + trackHeight + 30);
    DeleteObject(blackBrush); DeleteObject(blackPen);
}
void Tank::TakeDamage(int dmg)
{
    if (isDestroyed) return;

    health -= dmg;
    cout << "Tank took " << dmg << " damage! Health: " << health << endl;
    if (health <= 0) {
        health = 0;
        Explode();
    }
}
void Tank::Heal(int amount)
{
    if (isDestroyed) return;

    health += amount;
    if (health > 100) health = 100;
    cout << "Tank healed by " << amount << " HP! Health: " << health << endl;
}
void Tank::Explode()
{
    if (isDestroyed) return;

    cout << "TANK DESTROYED! Explosion at (" << X << ", " << Y << ")" << endl;

    Hide();

    // Большой взрыв танка
    HPEN pen = CreatePen(PS_SOLID, 5, RGB(255, 100, 0));
    HBRUSH brush = CreateSolidBrush(RGB(255, 200, 0));
    SelectObject(hdc, pen); SelectObject(hdc, brush);

    // Несколько кругов взрыва
    Ellipse(hdc, X - 60, Y - 60, X + bodyWidth + 60, Y + bodyHeight + 60);
    DeleteObject(brush);

    brush = CreateSolidBrush(RGB(255, 150, 0)); SelectObject(hdc, brush);
    Ellipse(hdc, X - 40, Y - 40, X + bodyWidth + 40, Y + bodyHeight + 40);
    DeleteObject(brush);

    brush = CreateSolidBrush(RGB(255, 255, 0)); SelectObject(hdc, brush);
    Ellipse(hdc, X - 20, Y - 20, X + bodyWidth + 20, Y + bodyHeight + 20);
    DeleteObject(brush); DeleteObject(pen);

    Sleep(1000);

    // Очистка взрыва
    HPEN blackPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, blackPen); SelectObject(hdc, blackBrush);
    Rectangle(hdc, X - 70, Y - 70, X + bodyWidth + 70, Y + bodyHeight + 70);
    DeleteObject(blackBrush); DeleteObject(blackPen);

    isDestroyed = true;
}
int Tank::GetHealth() const { return health; }
int Tank::GetSpeed() const { return speed; }
int Tank::GetBodyWidth() const { return bodyWidth; }
int Tank::GetBodyHeight() const { return bodyHeight; }
bool Tank::IsDestroyed() const { return isDestroyed; }

/*-------------------- HEAVY TANK ---------------------*/
HeavyTank::HeavyTank(int InitX, int InitY)
    : Tank(InitX, InitY, 320, 140, 140, 90, 200, 50, 20)
{
    health = 150;
    targetX = InitX;
    targetY = InitY - 300;
}
HeavyTank::~HeavyTank() {}

void HeavyTank::SetTarget(int x, int y)
{
    targetX = x;
    targetY = y;
}

void HeavyTank::ShowTargetingSystem()
{
    // Убрана красная линия наведения
}

void HeavyTank::Show()
{
    if (isDestroyed) return;

    HPEN pen = CreatePen(PS_SOLID, 3, RGB(50, 0, 0));
    HBRUSH brush = CreateSolidBrush(RGB(100, 50, 50));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X, Y + bodyHeight, X + bodyWidth, Y + bodyHeight + trackHeight);
    DeleteObject(brush);
    brush = CreateSolidBrush(RGB(90, 30, 30)); SelectObject(hdc, brush);
    Rectangle(hdc, X + bodyWidth / 6, Y, X + bodyWidth - bodyWidth / 6, Y + bodyHeight);
    DeleteObject(brush);

    int turretTop = Y - turretHeight;
    brush = CreateSolidBrush(RGB(80, 20, 20)); SelectObject(hdc, brush);
    Rectangle(hdc, X + (bodyWidth - turretWidth)/2, turretTop,
              X + (bodyWidth + turretWidth)/2, turretTop + turretHeight);
    DeleteObject(brush);

    // Артиллерийская установка направлена ВВЕРХ
    int turretCenterX = X + bodyWidth/2;
    int turretCenterY = turretTop + turretHeight/2;

    // Основная артиллерийская пушка - направлена вверх под углом 45 градусов
    double artilleryAngle = -M_PI/4; // -45 градусов (вверх и вправо)
    int gunEndX = turretCenterX + (int)(gunLength * cos(artilleryAngle));
    int gunEndY = turretCenterY + (int)(gunLength * sin(artilleryAngle));

    HPEN gunPen = CreatePen(PS_SOLID, 12, RGB(110, 60, 60));
    SelectObject(hdc, gunPen);
    MoveToEx(hdc, turretCenterX, turretCenterY, NULL);
    LineTo(hdc, gunEndX, gunEndY);
    DeleteObject(gunPen);

    // Артиллерийские направляющие
    HPEN rocketPen = CreatePen(PS_SOLID, 6, RGB(120, 120, 120));
    SelectObject(hdc, rocketPen);
    int offsetY = 25;
    int rocketEndX = turretCenterX + (int)((gunLength - 40) * cos(artilleryAngle));
    int rocketEndY1 = turretCenterY + (int)((gunLength - 40) * sin(artilleryAngle)) - offsetY;
    int rocketEndY2 = turretCenterY + (int)((gunLength - 40) * sin(artilleryAngle)) + offsetY;

    MoveToEx(hdc, turretCenterX, turretCenterY - offsetY, NULL);
    LineTo(hdc, rocketEndX, rocketEndY1);
    MoveToEx(hdc, turretCenterX, turretCenterY + offsetY, NULL);
    LineTo(hdc, rocketEndX, rocketEndY2);
    DeleteObject(rocketPen);

    brush = CreateSolidBrush(RGB(40,20,20)); SelectObject(hdc, brush);
    Ellipse(hdc, X + bodyWidth/6, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/6 + trackHeight/2, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth/2 - trackHeight/4, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/2 + trackHeight/4, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth - bodyWidth/6 - trackHeight/2, Y + bodyHeight + trackHeight/4,
        X + bodyWidth - bodyWidth/6, Y + bodyHeight + trackHeight - trackHeight/4);
    DeleteObject(brush); DeleteObject(pen);
}
void HeavyTank::TakeDamage(int dmg)
{
    if (isDestroyed) return;

    health -= dmg / 2; // повышенная защита
    cout << "Heavy Tank took " << (dmg/2) << " damage (armor reduced)! Health: " << health << endl;
    if (health <= 0) {
        health = 0;
        Explode();
    }
}

/*-------------------- LIGHT TANK ---------------------*/
LightTank::LightTank(int InitX, int InitY)
    : Tank(InitX, InitY, 160, 70, 0, 0, 0, 20, 50)
{
    health = 60;
}
LightTank::~LightTank() {}
void LightTank::Show()
{
    if (isDestroyed) return;

    HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 128));
    HBRUSH brush = CreateSolidBrush(RGB(100, 100, 180));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X, Y + bodyHeight, X + bodyWidth, Y + bodyHeight + trackHeight);
    DeleteObject(brush);

    // Корпус без башни - более обтекаемый, БЕЗ ПУШКИ
    brush = CreateSolidBrush(RGB(80, 80, 180)); SelectObject(hdc, brush);
    Rectangle(hdc, X + bodyWidth / 8, Y, X + bodyWidth - bodyWidth / 8, Y + bodyHeight);
    DeleteObject(brush);

    // Меньше колес для легкости
    brush = CreateSolidBrush(RGB(60,60,100)); SelectObject(hdc, brush);
    Ellipse(hdc, X + bodyWidth/4, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/4 + trackHeight/2, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth*3/4 - trackHeight/2, Y + bodyHeight + trackHeight/4,
        X + bodyWidth*3/4, Y + bodyHeight + trackHeight - trackHeight/4);
    DeleteObject(brush); DeleteObject(pen);
}
void LightTank::TakeDamage(int dmg)
{
    if (isDestroyed) return;

    health -= dmg * 2; // слабая защита
    cout << "Light Tank took " << (dmg*2) << " damage (weak armor)! Health: " << health << endl;
    if (health <= 0) {
        health = 0;
        Explode();
    }
}

/*-------------------- DAMAGE TANK ---------------------*/
DamageTank::DamageTank(int InitX, int InitY)
    : Tank(InitX, InitY, 240, 100, 100, 60, 120, 35, 35)
{
    health = 120;
}
DamageTank::~DamageTank() {}
void DamageTank::Show()
{
    if (isDestroyed) return;

    HPEN pen = CreatePen(PS_SOLID, 3, RGB(100, 100, 0));
    HBRUSH brush = CreateSolidBrush(RGB(180, 180, 80));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X, Y + bodyHeight, X + bodyWidth, Y + bodyHeight + trackHeight);
    DeleteObject(brush);
    brush = CreateSolidBrush(RGB(180, 170, 60)); SelectObject(hdc, brush);
    Rectangle(hdc, X + bodyWidth / 6, Y, X + bodyWidth - bodyWidth / 6, Y + bodyHeight);
    DeleteObject(brush);

    int turretTop = Y - turretHeight;
    brush = CreateSolidBrush(RGB(130, 130, 30)); SelectObject(hdc, brush);
    Rectangle(hdc, X + (bodyWidth - turretWidth)/2, turretTop,
              X + (bodyWidth + turretWidth)/2, turretTop + turretHeight);
    DeleteObject(brush);

    // Двойные пулеметы на башне
    int turretCenterX = X + bodyWidth/2;
    int turretCenterY = turretTop + turretHeight/2;

    // Верхний пулемет
    brush = CreateSolidBrush(RGB(220, 200, 40)); SelectObject(hdc, brush);
    Rectangle(hdc, turretCenterX, turretCenterY - 20,
              turretCenterX + gunLength, turretCenterY - 12);

    // Нижний пулемет
    Rectangle(hdc, turretCenterX, turretCenterY + 12,
              turretCenterX + gunLength, turretCenterY + 20);
    DeleteObject(brush);

    // Основная пушка по центру
    brush = CreateSolidBrush(RGB(200, 180, 20)); SelectObject(hdc, brush);
    Rectangle(hdc, turretCenterX, turretCenterY - 6,
              turretCenterX + gunLength + 20, turretCenterY + 6);
    DeleteObject(brush);

    brush = CreateSolidBrush(RGB(100, 100, 0)); SelectObject(hdc, brush);
    Ellipse(hdc, X + bodyWidth/6, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/6 + trackHeight/2, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth/2 - trackHeight/4, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/2 + trackHeight/4, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth - bodyWidth/6 - trackHeight/2, Y + bodyHeight + trackHeight/4,
        X + bodyWidth - bodyWidth/6, Y + bodyHeight + trackHeight - trackHeight/4);
    DeleteObject(brush); DeleteObject(pen);
}
void DamageTank::TakeDamage(int dmg)
{
    if (isDestroyed) return;

    health -= dmg;
    cout << "Damage Tank took " << dmg << " damage! Health: " << health << endl;
    if (health <= 0) {
        health = 0;
        Explode();
    }
}

/*-------------------- Остальные классы без констант ---------------------*/
Rocket::Rocket(int InitX, int InitY, int targetX, int targetY, int pwr, int w, int h, int spd)
    : Point(InitX, InitY), power(pwr), isActive(true), width(w), height(h), speed(spd)
{
    // Вычисляем направление к цели
    int deltaX = targetX - InitX;
    int deltaY = targetY - InitY;
    double distance = sqrt(deltaX * deltaX + deltaY * deltaY);

    if (distance > 0) {
        directionX = (int)(speed * deltaX / distance);
        directionY = (int)(speed * deltaY / distance);
    } else {
        directionX = speed;
        directionY = 0;
    }
}
Rocket::~Rocket() {}

void Rocket::Show()
{
    if (!isActive) return;

    Visible = true;
    HPEN penBody = CreatePen(PS_SOLID, 3, RGB(255, 100, 0));
    HBRUSH brushBody = CreateSolidBrush(RGB(255, 200, 0));
    SelectObject(hdc, penBody); SelectObject(hdc, brushBody);
    Ellipse(hdc, X - width/2, Y - height/2, X + width/2, Y + height/2);
    DeleteObject(brushBody); DeleteObject(penBody);

    // Хвост ракеты
    HPEN penTail = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    SelectObject(hdc, penTail);
    MoveToEx(hdc, X - width/2, Y, NULL); LineTo(hdc, X - width, Y);
    DeleteObject(penTail);
}

void Rocket::Hide()
{
    if (!isActive) return;

    Visible = false;
    HPEN blackPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, blackPen); SelectObject(hdc, blackBrush);
    Rectangle(hdc, X - width, Y - height, X + width, Y + height);
    DeleteObject(blackBrush); DeleteObject(blackPen);
}

void Rocket::MoveTo(int NewX, int NewY)
{
    if (!isActive) return;
    Hide();
    SetPosition(NewX, NewY);
    Show();
}

void Rocket::MoveToTarget()
{
    if (!isActive) return;

    int newX = X + directionX;
    int newY = Y + directionY;

    // Проверяем границы экрана
    if (newX < 0 || newX > 2000 || newY < 0 || newY > 1200) {
        Deactivate();
        return;
    }

    MoveTo(newX, newY);
}

bool Rocket::CheckCollision(Tank* t)
{
    if (!isActive || t->IsDestroyed()) return false;

    int tankCenterX = t->GetX() + t->GetBodyWidth() / 2;
    int tankCenterY = t->GetY() + t->GetBodyHeight() / 2;
    int distance = sqrt((tankCenterX - X) * (tankCenterX - X) + (tankCenterY - Y) * (tankCenterY - Y));
    return distance < 60;
}

void Rocket::Deactivate()
{
    isActive = false;
    Hide();
}

int Rocket::GetPower() const { return power; }
bool Rocket::IsActive() const { return isActive; }

Mina::Mina(int InitX, int InitY, int dmg, int w, int h)
    : Point(InitX, InitY), damage(dmg), isExploded(false), width(w), height(h) {}
Mina::~Mina() {}

void Mina::Show()
{
    if (isExploded) return;

    Visible = true;
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(50, 50, 50));
    HBRUSH brush = CreateSolidBrush(RGB(100, 100, 100));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Ellipse(hdc, X - width/2, Y - height/2, X + width/2, Y + height/2);
    DeleteObject(brush);

    // Шипы мины
    MoveToEx(hdc, X - width/2, Y, NULL); LineTo(hdc, X - width/2 - 5, Y);
    MoveToEx(hdc, X + width/2, Y, NULL); LineTo(hdc, X + width/2 + 5, Y);
    MoveToEx(hdc, X, Y - height/2, NULL); LineTo(hdc, X, Y - height/2 - 5);
    MoveToEx(hdc, X, Y + height/2, NULL); LineTo(hdc, X, Y + height/2 + 5);

    DeleteObject(pen);
}

void Mina::Hide()
{
    Visible = false;
    HPEN blackPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, blackPen); SelectObject(hdc, blackBrush);
    Rectangle(hdc, X - width, Y - height, X + width, Y + height);
    DeleteObject(blackBrush); DeleteObject(blackPen);
}

bool Mina::CheckCollision(Tank* t)
{
    if (isExploded || t->IsDestroyed()) return false;

    int tankCenterX = t->GetX() + t->GetBodyWidth() / 2;
    int tankCenterY = t->GetY() + t->GetBodyHeight() / 2;
    int distance = sqrt((tankCenterX - X) * (tankCenterX - X) + (tankCenterY - Y) * (tankCenterY - Y));
    return distance < 40;
}

void Mina::Explode()
{
    if (isExploded) return;

    Hide();
    // Анимация взрыва
    HPEN pen = CreatePen(PS_SOLID, 3, RGB(255, 255, 0));
    HBRUSH brush = CreateSolidBrush(RGB(255, 100, 0));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Ellipse(hdc, X - 30, Y - 30, X + 30, Y + 30);
    DeleteObject(brush); DeleteObject(pen);
    Sleep(500);

    // Очистка взрыва
    HPEN blackPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, blackPen); SelectObject(hdc, blackBrush);
    Rectangle(hdc, X - 35, Y - 35, X + 35, Y + 35);
    DeleteObject(blackBrush); DeleteObject(blackPen);

    isExploded = true;
}

int Mina::GetDamage() const { return damage; }
bool Mina::IsExploded() const { return isExploded; }

Remont::Remont(int InitX, int InitY, int heal, int w, int h)
    : Point(InitX, InitY), healAmount(heal), isUsed(false), width(w), height(h) {}
Remont::~Remont() {}

void Remont::Show()
{
    if (isUsed) return;

    Visible = true;

    // Зеленый прямоугольник
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 100, 0));
    HBRUSH brush = CreateSolidBrush(RGB(0, 200, 0));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X - width/2, Y - height/2,
              X + width/2, Y + height/2);
    DeleteObject(brush);

    // Буква H
    pen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
    SelectObject(hdc, pen);
    MoveToEx(hdc, X - 10, Y - 10, NULL); LineTo(hdc, X - 10, Y + 10);
    MoveToEx(hdc, X + 10, Y - 10, NULL); LineTo(hdc, X + 10, Y + 10);
    MoveToEx(hdc, X - 10, Y, NULL); LineTo(hdc, X + 10, Y);

    DeleteObject(pen);
}

void Remont::Hide()
{
    Visible = false;
    HPEN blackPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, blackPen); SelectObject(hdc, blackBrush);
    Rectangle(hdc, X - width, Y - height,
              X + width, Y + height);
    DeleteObject(blackBrush); DeleteObject(blackPen);
}

bool Remont::CheckCollision(Tank* t)
{
    if (isUsed || t->IsDestroyed()) return false;

    int tankCenterX = t->GetX() + t->GetBodyWidth() / 2;
    int tankCenterY = t->GetY() + t->GetBodyHeight() / 2;
    return (abs(tankCenterX - X) < 50 && abs(tankCenterY - Y) < 40);
}

void Remont::Use()
{
    isUsed = true;
    Hide();
}

int Remont::GetHealAmount() const { return healAmount; }
bool Remont::IsUsed() const { return isUsed; }
