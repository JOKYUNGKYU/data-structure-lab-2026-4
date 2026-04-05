#include <iostream>

#define HEIGHT 8
#define WIDTH 8

// 최대 화소 밝기를 구해서 반환하는 함수
int get_max_brightness(int img[HEIGHT][WIDTH]) {
    // 1. 처음에는 배열의 첫 번째 요소를 최댓값으로 가정합니다.
    int max_val = img[0][0];

    // 2. 2중 for문을 통해 배열의 모든 요소를 순회합니다.
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            // 3. 만약 현재 요소가 현재까지의 최댓값보다 크다면 업데이트합니다.
            if (img[i][j] > max_val) {
                max_val = img[i][j];
            }
        }
    }

    // 4. 최종적으로 찾은 최댓값을 반환합니다.
    return max_val;
}

int main() {
    // 슬라이드에 주어진 예제 이미지 데이터
    int image[HEIGHT][WIDTH] = {
        { 10, 30, 55, 80, 120, 160, 200, 230 },
        { 20, 45, 70, 100, 140, 180, 220, 210 },
        { 35, 60, 90, 130, 170, 210, 240, 190 },
        { 50, 80, 115, 150, 190, 230, 255, 170 },
        { 40, 65, 100, 140, 175, 215, 235, 150 },
        { 25, 50, 80, 115, 155, 185, 210, 130 },
        { 15, 35, 60, 90, 130, 165, 185, 110 },
        {  5, 20, 40, 70, 105, 140, 160,  90 }
    };

    // 함수 호출 및 결과 출력
    int max_brightness = get_max_brightness(image);

    std::cout << "이지미의 최대 밝기 화소 : " << max_brightness << std::endl;

    return 0;
}