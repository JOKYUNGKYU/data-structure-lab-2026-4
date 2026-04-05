#include <iostream>
#include <cmath>
using namespace std;

#define MAX_DEGREE 80  // 다항식의 최대 차수 + 1

class Polynomial {
private:
    int   degree;              // 다항식의 최고 차수
    float coef[MAX_DEGREE];    // 각 항의 계수 배열 (coef[i] = x^i의 계수)

public:
    // 생성자: 최고 차수를 0으로 초기화
    Polynomial() { degree = 0; }

    // 다항식이 영다항식인지 확인
    bool isZero() { return degree == 0 && coef[0] == 0.0f; }

    // 모든 계수의 부호를 반전 (단항 연산)
    void negate() {
        for (int i = 0; i <= degree; i++)
            coef[i] = -coef[i];
    }

    // 키보드로 다항식 입력
    void read() {
        printf("Enter the highest degree of the polynomial : ");
        scanf_s("%d", &degree);
        printf("Enter the coefficient of each term(total % d) : ", degree + 1);
        for (int i = 0; i <= degree; i++)
            scanf_s("%f", &coef[i]);
    }

    // 다항식 출력 (내림차순: x^degree ~ x^0)
    void display(const char* str = "Poly = ") {
        printf("\t%s", str);
        for (int i = 0; i < degree; i++)
            printf("%5.1f x^%d + ", coef[i], degree - i);
        printf("%4.1f\n", coef[degree]);
    }

    // 덧셈: c = a + b
    // 슬라이드의 단순화 방법(c=a+b → c=a; c+=b) 구현
    void add(Polynomial a, Polynomial b) {
        if (a.degree > b.degree) {
            // a의 차수가 더 크면: 먼저 자신을 a로 복사
            *this = a;
            // b의 각 항을 더함 (차수를 맞춰서)
            for (int i = 0; i <= b.degree; i++)
                coef[i + (degree - b.degree)] += b.coef[i];
        }
        else {
            // b의 차수가 크거나 같으면: 먼저 자신을 b로 복사
            *this = b;
            // a의 각 항을 더함
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