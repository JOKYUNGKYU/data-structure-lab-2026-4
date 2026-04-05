#pragma once
#include <iostream>
#include <cstring>
using namespace std;

class Car {
protected:
    int  speed;
    char name[40];

public:
    int gear;

    Car() : speed(0), gear(1) {
        strcpy_s(name, "My Car");
    }
    ~Car() {}

    Car(int s, char* n, int g)
        : speed(s), gear(g) {
        strcpy_s(name, n);
    }

    void changeGear(int g = 4) {
        gear = g;
    }

    void speedUp() {
        speed += 5;
    }

    void display() {
        printf("[%s] : gear=%d speed=%dkmph\n", name, gear, speed);
    }

    void whereAmI() { printf("object address = %p\n", this); }
};