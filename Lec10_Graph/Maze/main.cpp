#include "Location2D.h"
#include <stack>
#include <queue>
#include <stdio.h>
using namespace std;

const int MAZE_SIZE = 6;
char map[MAZE_SIZE][MAZE_SIZE] = {
	{'1', '1', '1', '1', '1', '1'},
	{'e', '0', '1', '0', '0', '1'},
	{'1', '0', '0', '0', '1', '1'},
	{'1', '0', '1', '0', '1', '1'},
	{'1', '0', '1', '0', '0', 'x'},
	{'1', '1', '1', '1', '1', '1'},
};

bool isValidLoc(int r, int c)
{
	if (r < 0 || c < 0 || r >= MAZE_SIZE || c >= MAZE_SIZE) return false;
	else return map[r][c] == '0' || map[r][c] == 'x';
}

int main() {
	int ds_num;
	printf("DFS 데이터 구조 라이브러리 선택: 1)stack, 2)BFS \n");
	scanf_s("%d", &ds_num);
	stack<Location2D> locStack;
	queue<Location2D> locqueue;
	switch (ds_num)
	{
	case 1: {
		stack<Location2D> locStack;
		Location2D entry(1, 0);
		locStack.push(entry);

		while (locStack.empty() == false)
		{
			Location2D here = locStack.top();
			locStack.pop();

			int r = here.row;
			int c = here.col;

			printf("(%d,%d) ", r, c);
			if (map[r][c] == 'x')
			{
				printf("미로 탐색 성공\n");
				return 0;
			}
			else {
				map[r][c] = '.';
				if (isValidLoc(r - 1, c)) locStack.push(Location2D(r - 1, c));
				if (isValidLoc(r + 1, c)) locStack.push(Location2D(r + 1, c));
				if (isValidLoc(r, c - 1)) locStack.push(Location2D(r, c - 1));
				if (isValidLoc(r, c + 1)) locStack.push(Location2D(r, c + 1));
			}
		}
		printf("미로 탐색 실패\n");
	}

	case 2: {
		printf("BFS 미로탐색\n");

		queue<Location2D> locQueue;

		Location2D entry(1, 0);
		locQueue.push(entry);

		while (!locQueue.empty())
		{
			Location2D here = locQueue.front();
			locQueue.pop();

			int r = here.row;
			int c = here.col;

			printf("(%d,%d) ", r, c);

			if (map[r][c] == 'x')
			{
				printf("\n미로 탐색 성공\n");
				return 0;
			}

			map[r][c] = '.';

			// BFS는 enqueue 순서대로 탐색됨
			// 오른 → 아래 → 왼 → 위
			if (isValidLoc(r - 1, c))
				locQueue.push(Location2D(r - 1, c)); // 위

			if (isValidLoc(r + 1, c))
				locQueue.push(Location2D(r + 1, c)); // 아래

			if (isValidLoc(r, c - 1))
				locQueue.push(Location2D(r, c - 1)); // 왼

			if (isValidLoc(r, c + 1))
				locQueue.push(Location2D(r, c + 1)); // 오른
		}

		printf("\n미로 탐색 실패\n");
	}

	}
	}
	




	
