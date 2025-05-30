#pragma once

class Location {
protected:
    int X, Y;
public:
    Location(int InitX = 0, int InitY = 0);
    virtual ~Location();
    int GetX() const;
    int GetY() const;
    void SetX(int NewX);
    void SetY(int NewY);
    void SetPosition(int NewX, int NewY);
};

class Point : public Location {
protected:
    bool Visible;
public:
    Point(int InitX = 0, int InitY = 0);
    virtual ~Point();
    bool IsVisible() const;
    virtual void Show();
    virtual void Hide();
    virtual void MoveTo(int NewX, int NewY);
    virtual void Drag(int Step);
};

class Tank : public Point {
protected:
    int bodyWidth, bodyHeight, turretWidth, turretHeight;
    int gunLength, trackHeight;
    int health;
    int speed;
    bool isDestroyed;
public:
    Tank(int InitX, int InitY, int bW, int bH, int tW, int tH, int gL, int trH, int spd = 30);
    virtual ~Tank();
    virtual void Show();
    virtual void Hide();
    virtual void Drag(int step);
    virtual void TakeDamage(int dmg);
    void Heal(int amount);
    void Explode();
    int GetHealth() const;
    int GetSpeed() const;
    int GetBodyWidth() const;
    int GetBodyHeight() const;
    bool IsDestroyed() const;
};

class HeavyTank : public Tank {
private:
    int targetX, targetY;
public:
    HeavyTank(int InitX, int InitY);
    ~HeavyTank();
    void Show() override;
    virtual void TakeDamage(int dmg);
    void SetTarget(int x, int y);
    void ShowTargetingSystem();
};

class LightTank : public Tank {
public:
    LightTank(int InitX, int InitY);
    ~LightTank();
    virtual void Show();
    virtual void TakeDamage(int dmg);
};

class DamageTank : public Tank {
public:
    DamageTank(int InitX, int InitY);
    ~DamageTank();
    virtual void Show();
    virtual void TakeDamage(int dmg);
};

class Rocket : public Point {
private:
    int power;
    int directionX, directionY;
    bool isActive;
    int width, height, speed;
public:
    Rocket(int InitX, int InitY, int targetX, int targetY, int pwr, int w = 36, int h = 12, int spd = 15);
    ~Rocket();
    virtual void Show();
    virtual void Hide();
    virtual void MoveTo(int NewX, int NewY);
    bool CheckCollision(Tank* t);
    void MoveToTarget();
    int GetPower() const;
    bool IsActive() const;
    void Deactivate();
};

class Mina : public Point {
private:
    int damage;
    bool isExploded;
    int width, height;
public:
    Mina(int InitX, int InitY, int dmg = 50, int w = 20, int h = 20);
    ~Mina();
    virtual void Show();
    virtual void Hide();
    bool CheckCollision(Tank* t);
    void Explode();
    int GetDamage() const;
    bool IsExploded() const;
};

class Remont : public Point {
private:
    int healAmount;
    bool isUsed;
    int width, height;
public:
    Remont(int InitX, int InitY, int heal = 30, int w = 40, int h = 30);
    ~Remont();
    virtual void Show();
    virtual void Hide();
    bool CheckCollision(Tank* t);
    void Use();
    int GetHealAmount() const;
    bool IsUsed() const;
};
