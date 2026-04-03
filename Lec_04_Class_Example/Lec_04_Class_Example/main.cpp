#include "Rectangle.h"
#include "Pet.h"
//using namespace std;     std안쓰고 만들기 (선언)    이거 안하면 std::cout로 해야함
//cout << "Rectangle 1: " 
using namespace std;
int main() {
	// Rectangle 클래스로 rect1 객체생성
	Rectangle rect1(4, 6);
	std::cout << "Rectangle 1: " << std::endl;
	//printf("Rectangle 1: \n");
	cout << "Area: " << rect1.getArea() << endl;
	//printf("Area: %lf\n", rect1.getArea()):
	cout << "Perimeter: " << rect1.getPerimeter() << endl;
	cout << "Is Square?" << boolalpha << rect1.isSquare() << endl;
	printf("==================================\n");

	Pet myPet("Lucky", 5, "dog");
	cout << "My pet's name is " << myPet.getName() << endl;
	Puppy myPuppy("Kong", 3, "dog", "Puddle");
	cout << "My puppy's breed is " << myPuppy.getBreed() << endl;

	return 0;
}