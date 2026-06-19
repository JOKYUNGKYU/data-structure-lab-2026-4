#include "WGraphMST.h"
#include "WGraphPrim.h"
#include "WGraphDijkstra.h"

int main() {
    // WGraphMST g;
    // g.load("graph.txt");
    // printf("MST By Kruskal's Algorithm\n");
    // g.Kruskal();

    WGraphPrim g;
    g.load("graph.txt");
    printf("MST By Prim's Algorithm\n");
    g.Prim(0);          // 정점 0(A)에서 시작
    return 0;

    // WGraphDijkstra g;
    // g.load("graph_sp.txt");
    // printf("Shortest Path By Dijkstra Algorithm\n");
    // g.ShortestPath(0);      // 정점 0(A)에서 시작
    // return 0;
}