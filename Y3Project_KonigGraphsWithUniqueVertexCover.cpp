// Y3Project_KonigGraphsWithUniqueVertexCover.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <list>
#include <set>
#include <tuple>
using namespace std;

class Graph {
public:
	list<int> *adjList;
	int n;
	bool directed;

	Graph(int N, bool Directed = false) {
		adjList = new list<int>[N];
		n = N;
		directed = Directed;
	}

	void addEdge(int u, int v) {
		adjList[u].push_back(v);
		if (!directed)
			adjList[v].push_back(u);
	}

	void print() {
		cout << "Graph:\n";
		for (int i = 0; i < n; i++) {
			cout << i << " --> ";
			for (auto it : adjList[i]) {
				cout << it << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
};

class Edge {
public:
	int v[2];
	const bool directed = false;

	Edge(int u1, int u2) {
		v[0] = u1;
		v[1] = u2;
	}

	void operator=(const Edge& e) {
		v[0] = e.v[0];
		v[1] = e.v[1];
	}

	bool operator==(const Edge& f) const {
		return	(f.get(0) == v[0] && f.get(1) == v[1]) ||
				(f.get(1) == v[0] && f.get(0) == v[1]);
	}

	int get(int i) const {
		return v[i];
	}
}; 

class DEdge : Edge {
public:
	int v[2];
	const bool directed = true;

	using Edge::Edge;

	bool operator==(const DEdge& f) const {
		return	(f.get(0) == v[0] && f.get(1) == v[1]);
	}

	using Edge::get;
};

class EdgeSet {
public:
	list<Edge> E;
	int n = 0;

	bool coversVertex(int i) const {
		for (const Edge& it : E) {
			if (it.get(0) == i || it.get(1) == i)
				return true;
		}
		return false;
	}

	void addEdges(EdgeSet s) {
		for (const Edge& it : s.E) {
			E.push_back(it);
			n++;
		}
	}

	void addEdge(int u, int v) {
		Edge e(u, v);
		E.push_back(e);
		n++;
	}

	void removeEdge(int u, int v) {
		for (const Edge& it : E) {
			if (it.get(0) == u && it.get(1) == v) {
				E.remove(it);
				n--;
				return;
			} 
		}
		cout << "EdgeSet.removeEdge(u, v) > ERROR: edge (" << u << ", " <<  v << ") to be removed not found";
	}

	void removeEdge(Edge e) {
		E.remove(e);
		n--;
	}

	void print() {
		cout << "EdgeSet:\n";
		for (const Edge& it : E) {
			cout << "(" << it.get(0) << ", " << it.get(1) << ") ";
		}
		cout << "\n";
	}

	EdgeSet augmentBy(EdgeSet P) {
		EdgeSet L;
		L.print();
		L.addEdges(P);
		L.print();
		L.addEdges(*this);
		L.print();

		for (const Edge& it : E) {
			for (const Edge& itP : P.E) {
				cout << "Checking edge (" << it.get(0) << ", " << it.get(1) << ") and (" << itP.get(0) << ", " << itP.get(1) << ")\n";
				if (it == itP) {
					L.removeEdge(it);
				}
			}
		}

		return L;
	}
};



list<int> even, odd;

EdgeSet exploreEven(const Graph G, const EdgeSet M, const int v, EdgeSet& path);

EdgeSet exploreOdd(const Graph G, const EdgeSet M, const int v, EdgeSet& path) {
	odd.push_back(v);

	EdgeSet& max = path;

	for (const Edge& it : M.E) {
		if (it.get(0) == v) {
			path.addEdge(v, it.get(1));
			path = exploreEven(G, M, it.get(1), path);
			if (path.n > max.n)
				max = path;
		} else if (!it.directed && it.get(1) == v) {
			path.addEdge(it.get(0), v);
			path = exploreEven(G, M, it.get(0), path);
			if (path.n > max.n)
				max = path;
		}
	}

	return max;
}

EdgeSet exploreEven(const Graph G, const EdgeSet M, const int v, EdgeSet& path) {
	even.push_back(v);

	cout << "Evens: ";
	for (int i : even) {
		cout << i << ", ";
	}
	cout << "\nOdds: ";
	for (int i : odd) {
		cout << i << ", ";
	}
	cout << "\n";

	EdgeSet& max = path;

	for (const int u : G.adjList[v]) {
		if (find(even.begin(), even.end(), u) != even.end()) {
			//blossom found
			cout << "Blossom found and not handled correctly\n";
		} else if (find(odd.begin(), odd.end(), u) != odd.end()) {
			//ignore this vertex
		} else {
			path.addEdge(v, u);
			path = exploreOdd(G, M, u, path);
			if (path.n > max.n)
				max = path;
		}
	}

	return max;
}

EdgeSet FindAugmentingPath(const Graph G, const EdgeSet M) {
	int v0 = -1;

	for (int i = 0; i < G.n; i++) {
		if (!M.coversVertex(i)) {
			v0 = i;
			break;
		}
	}

	EdgeSet p;
	if (v0 == -1) return p;

	p = exploreEven(G, M, v0, p);

	return p;
}

EdgeSet FindMaxMatching(Graph G, EdgeSet M) {
	EdgeSet P = FindAugmentingPath(G, M);

	//cout << "P - ";
	//P.print();

	if (P.n != 0) {
		//cout << "M augmented by P - ";
		//M.augmentBy(P).print();
		return FindMaxMatching(G, M.augmentBy(P));
	} else
		return M;
}

int main()
{

	Graph G(6);
	G.addEdge(0, 1);
	G.addEdge(1, 2);
	G.addEdge(4, 2);
	G.addEdge(1, 3);
	G.addEdge(4, 3);
	G.addEdge(1, 4);
	G.addEdge(0, 5);

	G.print();

	EdgeSet M;

	M = FindMaxMatching(G, M);

	cout << "M - ";
	M.print();

	system("pause");
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
