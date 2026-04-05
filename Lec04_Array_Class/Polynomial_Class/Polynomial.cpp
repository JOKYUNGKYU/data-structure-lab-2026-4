#include <iostream>
#include <cmath>
using namespace std;

#define MAX_DEGREE 80  // 다항식의 최대 차수 + 1

class Polynomial {
private:
    int   degree;              // 다항식의 최고 차수
    float coef[MAX_DEGREE];    // 각 항의 계수 배열 (coef[i] = x^i의 계수)

public:
    Polynomial() { degree = 0; }

    bool isZero() { return degree == 0 && coef[0] == 0.0f; }

    void negate() {
        for (int i = 0; i <= degree; i++)
            coef[i] = -coef[i];
    }
    void read() {
        printf("Enter the highest degree of the polynomial : ");
        scanf_s("%d", &degree);
        printf("Enter the coefficient of each term(total % d) : ", degree + 1);
        for (int i = 0; i <= degree; i++)
            scanf_s("%f", &coef[i]);
    }

    void display(const char* str = "Poly = ") {
        printf("\t%s", str);
        for (int i = 0; i < degree; i++)
            printf("%5.1f x^%d + ", coef[i], degree - i);
        printf("%4.1f\n", coef[degree]);
    }

    void add(Polynomial a, Polynomial b) {
        if (a.degree > b.degree) {
            *this = a;
            for (int i = 0; i <= b.degree; i++)
                coef[i + (degree - b.degree)] += b.coef[i];
        }
        else {
            *this = b;
            for (int i = 0; i <= a.degree; i++)
                coef[i + (degree - a.degree)] += a.coef[i];
        }
    }
};

// ───────────────────────────── main ─────────────────────────────
int main() {
    Polynomial a, b, c;

    printf("=== Polynomial A input ===\n");
    a.read();
    a.display("A = ");

    printf("\n=== Polynomial B input ===\n");
    b.read();
    b.display("B = ");

    // 덧셈
    c.add(a, b);
    printf("\n");
    c.display("A+B = ");
    return 0;
}