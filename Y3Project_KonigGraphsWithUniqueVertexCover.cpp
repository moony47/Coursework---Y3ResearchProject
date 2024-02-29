// Y3Project_KonigGraphsWithUniqueVertexCover.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <chrono>


using namespace std;

class Edge {
public:
	int v[2];
	bool directed;

	Edge(int u1, int u2, bool Directed = false) {
		directed = Directed;
		if (directed || u1 < u2) {
			v[0] = u1;
			v[1] = u2;
		}
		else {
			v[0] = u2;
			v[1] = u1;
		}
	}

	void operator=(const Edge& e) {
		v[0] = e.v[0];
		v[1] = e.v[1];
	}

	bool operator==(const Edge& f) const {
		return	(f.get(0) == v[0] && f.get(1) == v[1]) ||
				(f.get(1) == v[0] && f.get(0) == v[1]) && !directed;
	}

	int get(int i) const {
		return v[i];
	}

	bool meet(const Edge& e) const {
		if (e.get(0) == v[0] || e.get(0) == v[1] || e.get(1) == v[0] || e.get(1) == v[1])
			return true;
		return false;
	}
}; 

class Graph {
public:
	list<int>* adjList;
	int n;
	int m;
	bool directed;

	//Alternating tree labels
	bool* explored;
	list<int> even;
	list<int> odd;

	~Graph() {
		delete[] adjList;
		delete[] explored;
	}

	Graph(int N, bool Directed = false) {
		adjList = new list<int>[N];
		n = N;
		m = 0;
		directed = Directed;
		explored = new bool[N];
		for (int i = 0; i < N; i++)
			explored[i] = false;
	}

	Graph(const Graph& G) {
		n = G.n;
		m = G.m;
		directed = G.directed;

		if (!G.even.empty())
			even = list<int>(G.even);
		if (!G.odd.empty())
			odd = list<int>(G.odd);

		adjList = new list<int>[n];
		for (int i = 0; i < n; i++)
			for (int j : G.adjList[i])
				adjList[i].push_back(j);

		explored = new bool[n]; 
		for (int i = 0; i < n; i++)
			explored[i]= G.explored[i];
	}

	void addEdge(int u, int v) {
		if (find(adjList[u].begin(), adjList[u].end(), v) != adjList[u].end())
			return;

		adjList[u].push_back(v);
		if (!directed) {
			adjList[v].push_back(u);
		}
		m++;
	}

	void print(string label = "Graph:") {
		cout << label << "\n";
		for (int i = 0; i < n; i++) {
			cout << i << " --> ";
			for (int it : adjList[i]) {
				cout << it << " ";
			}
			cout << endl;
		}
		cout << endl;
	}

	void removeEdge(int v, int u) {
		list<int> t1 = adjList[v];
		list<int> t2 = adjList[u];
		t1.remove(u);
		t2.remove(v);
		adjList[v] = t1;
		adjList[u] = t2;
		m--;
	}

	void removeEdge(Edge e) {
		removeEdge(e.get(0), e.get(1));
	}

	void removeEdgesFromVertex(int v) {
		if (directed)
			cout << "Warning this function does not handle directed graphs correctly.";

		m -= adjList[v].size();
		adjList[v].clear();

		for (int i = 0; i < n; i++) {
			adjList[i].remove(v);
		}
	}
};

class EdgeSet {
public:
	list<Edge> E;

	void operator=(EdgeSet S) {
		E = S.E;
	}

	bool coversVertex(int i) const {
		for (const Edge& it : E) {
			if (it.get(0) == i || it.get(1) == i)
				return true;
		}
		return false;
	}

	void addEdges(EdgeSet s) {
		for (const Edge& it : s.E)
			E.push_back(it);
	}

	void addEdge(int u, int v, bool directed = false) {
		Edge e(u, v);
		E.push_back(e);
	}

	void addEdge(Edge e) {
		E.push_back(e);
	}

	void removeEdge(int u, int v) {
		for (const Edge& it : E) {
			if (it.get(0) == u && it.get(1) == v) {
				E.remove(it);
				return;
			} 
		}
		cout << "EdgeSet.removeEdge(u, v) > ERROR: edge (" << u << ", " <<  v << ") to be removed not found";
	}

	void removeEdge(Edge e) {
		E.remove(e);
	}

	void print(string label = "EdgeSet:", bool alternating = false) {
		cout << label << "	";
		bool matched = false;
		for (const Edge& it : E) {
			if (!alternating)
				cout << "(" << it.get(0) << ", " << it.get(1) << ") ";
			else if (matched)
				cout << "+(" << it.get(0) << ", " << it.get(1) << ") ";
			else
				cout << "-(" << it.get(0) << ", " << it.get(1) << ") ";
			matched = !matched;
		}
		cout << "\n";
	}

	EdgeSet augmentBy(EdgeSet P) {
		EdgeSet L;
		L.addEdges(P);
		L.addEdges(*this);

		for (const Edge& it : E) {
			for (const Edge& itP : P.E) {
				if (it == itP) {
					L.removeEdge(it);
				}
			}
		}

		return L;
	}

	void sort(bool alt = false) {
		if (alt) {
			list<Edge> t;
			for (int i = 0; i < E.size(); i++) {
				Edge e(-1, -1);
				for (const Edge& it : E) {
					if (find(t.begin(), t.end(), it) == t.end()) {
						if (it.get(1) > e.get(1))
							e = it;
					}
				}
				if (e.get(1) != -1)
					t.push_front(e);
			}
			E = t;
		} else {
			list<Edge> t;
			for (int i = 0; i < E.size(); i++) {
				Edge e(-1, -1);
				for (const Edge& it : E) {
					if (find(t.begin(), t.end(), it) == t.end()) {
						if (it.get(0) > e.get(0))
							e = it;
					}
				}
				if (e.get(0) != -1)
					t.push_front(e);
			}
			E = t;
		}
	}
};

void PrintVertices(const string& str, const list<int>& lst) {
	cout << str << "	{";
	for (int i : lst) {
		cout << i << ", ";
	}
	cout << "}\n";
}

list<int> IntersectVertices(const list<int>& A, const list<int>& B) {
	list<int> C;
	for (int i : A)
		if (find(B.begin(), B.end(), i) != B.end())
			C.push_back(i);
	return C;
}

list<int> UnionVertices(const list<int>& A, list<int>& B) {
	list<int> C(A);
	C.merge(B);
	C.sort();
	C.unique();
	return C;
}

list<int> SetMinusVertices(const list<int>& A, const list<int>& B) {
	list<int> C;
	for (int i : A)
		if (find(B.begin(), B.end(), i) == B.end())
			C.push_back(i);
	return C;
}

EdgeSet findPathThroughBlossom(Graph& G, const EdgeSet& M, list<int> blossom, const int base, const int target, EdgeSet& originalPath) {
	//				P'
	//				||
	//			   base
	//			  /   \
	//			v1	   v2
	//			||	   ||
	//			v1'	-- v2'
	//		   /
	//	     target

	//G.print();
	EdgeSet path;
	int end = -1;

	for (const Edge& it : M.E)
		if (it.get(0) == target) {
			if (find(blossom.begin(), blossom.end(), it.get(1)) != blossom.end())
				end = it.get(1);
			break;
		} else if (it.get(1) == target) {
			if (find(blossom.begin(), blossom.end(), it.get(0)) != blossom.end())
				end = it.get(0);
			break;
		}

	if (end == -1) {
		for (int v : blossom) {
			if (find(G.adjList[target].begin(), G.adjList[target].end(), v) != G.adjList[target].end()) {
				end = v;
				break;
			}
		}
	}

	EdgeSet originalPathR = originalPath;
	originalPathR.E.reverse();

	if (find (G.even.begin(), G.even.end(), base) != G.even.end() && find(G.even.begin(), G.even.end(), end) != G.even.end()) {
		//traverse blossom forwards
		bool reachedBlossom = false;
		for (const Edge& it : originalPath.E) {
			if (reachedBlossom) {
				path.addEdge(it);
			} else if (it.get(0) == base || it.get(1) == base) {
				reachedBlossom = true;
				if (it == originalPath.E.front() && base != end)
					path.addEdge(it);
			}
			if (it.get(0) == end || it.get(1) == end)
				break;
		}
	} else {
		int last = base;
		bool reachedEnd = false;
		//traverse blossom backwards
		for (const Edge& it : originalPathR.E) {
			//relabel the blossom vertices
			if (it.get(0) == last) {
				if (find(G.even.begin(), G.even.end(), it.get(1)) != G.even.end()) {
					G.even.remove(it.get(1));
					G.odd.push_back(it.get(1));
					last = it.get(1);
				} else {
					G.odd.remove(it.get(1));
					G.even.push_back(it.get(1));
					last = it.get(1);
				}
			} else {
				if (find(G.even.begin(), G.even.end(), it.get(0)) != G.even.end()) {
					G.even.remove(it.get(0));
					G.odd.push_back(it.get(0));
					last = it.get(0);
				}
				else {
					G.odd.remove(it.get(0));
					G.even.push_back(it.get(0));
					last = it.get(0);
				}
			}
			if (!reachedEnd) {
				path.addEdge(it);
				if (it.get(0) == end || it.get(1) == end) {
					reachedEnd = true;
					break;
				}
			} else if (it.get(0) == base || it.get(1) == base) {
				break;
			}
		}
	}

	path.addEdge(end, target);
	return path;
}

EdgeSet exploreEven(Graph& G, const EdgeSet& M, const int v, EdgeSet path, int maxDepth);

EdgeSet exploreOdd(Graph& G, const EdgeSet& M, const int v, EdgeSet path, int maxDepth) {
	if (path.E.size() > maxDepth)
		return EdgeSet();

	G.odd.push_back(v);

	bool found = false;

	for (const Edge& it : M.E) {
		if (it.get(0) == v) {
			found = true;
			EdgeSet newPath = path;
			newPath.addEdge(v, it.get(1), G.directed);
			newPath = exploreEven(G, M, it.get(1), newPath, maxDepth);
			if (!newPath.E.empty())
				return newPath;
			break;
		} else if (!it.directed && it.get(1) == v) {
			found = true;
			EdgeSet newPath = path;
			newPath.addEdge(v, it.get(0), G.directed);
			newPath = exploreEven(G, M, it.get(0), newPath, maxDepth);
			if (!newPath.E.empty())
				return newPath;
			break;
		}
	}

	if (found) {
		G.odd.remove(v);
		return EdgeSet();	//following matched edge returned no result so return empty
	} else
		return path; //odd vertex is free so path found
}

EdgeSet exploreEven(Graph& G, const EdgeSet& M, const int v, EdgeSet path, int maxDepth) {
	if (path.E.size() > maxDepth)
		return EdgeSet();

	G.even.push_back(v);


	//if (M.E.size() == 49)
	//	cout << "This one";

	for (int u : G.adjList[v]) {
		//if (v == 97 && u == 9)
		//	cout << "Should start here";
		if (find(G.even.begin(), G.even.end(), u) != G.even.end()) {
			//Merge the blossom into a single vertex and continue the search, then unfold the blossom vertex if a path is found
			EdgeSet pathToBase;
			EdgeSet M_H;

			Graph H(G);
			EdgeSet pathR = path;
			pathR.E.reverse();

			list<int> blossom;

			for (const Edge& it : pathR.E) {
				blossom.push_back(it.get(0));
				blossom.push_back(it.get(1));
				if (it.get(0) == u || it.get(1) == u)
					break;
			}

			blossom.sort();
			blossom.unique();
			//PrintVertices("Blossom ", blossom);

			for (int w : blossom) {
				for (int i : G.adjList[w])
					if (find(blossom.begin(), blossom.end(), i) == blossom.end()) {
						H.addEdge(i, u);
					}
				if (w != u) {
					H.adjList[w].clear();
					for (int j = 0; j < G.n; j++)
						H.adjList[j].remove(w);
				}
			}

			for (const Edge& it : path.E) {
				//Get the next edge in the path to check this is an in-edge not an out-edge
				list<Edge> temp(path.E);
				temp.pop_front();
				Edge next = temp.front();

				if ((it.get(0) == u || it.get(1) == u) && (it == path.E.front() && next.get(0) != u && next.get(1) != u))
					break;
				pathToBase.addEdge(it);
				if (it.get(0) == u || it.get(1) == u)
					break;
			}

			for (const Edge& it : M.E) {
				if (it.get(0) == u || it.get(1) == u)
					M_H.addEdge(it);
				if (find(blossom.begin(), blossom.end(), it.get(0)) == blossom.end() && 
					find(blossom.begin(), blossom.end(), it.get(1)) == blossom.end())
					M_H.addEdge(it);
			}

			EdgeSet newPath = exploreEven(H, M_H, u, pathToBase, maxDepth);

			if (!newPath.E.empty()) {
				EdgeSet unfoldedPath;
				bool reachedBase = false;
				int target = -1;
				for (const Edge& it : newPath.E) {
					if (target == -1) {
						if (it.get(0) == u) {
							//Get the next edge in the path to check this is an in-edge not an out-edge
							list<Edge> temp(newPath.E);
							Edge next(-1, -1);
							if (!temp.empty())
								temp.pop_front();
							if (!temp.empty())
								next = temp.front();

							if (find(G.adjList[u].begin(), G.adjList[u].end(), it.get(1)) == G.adjList[u].end()) {
								//Find path through blossom to target
								target = it.get(1);
								EdgeSet p(path);
								p.addEdge(u, v);
								unfoldedPath.addEdges(findPathThroughBlossom(G, M, blossom, u, target, p));
								continue;
							}
						} else if (it.get(1) == u) {
							//Get the next edge in the path to check this is an in-edge not an out-edge
							list<Edge> temp(newPath.E);
							Edge next(-1, -1);
							if (!temp.empty())
								temp.pop_front();
							if (!temp.empty())
								next = temp.front();

							if (find(G.adjList[u].begin(), G.adjList[u].end(), it.get(0)) == G.adjList[u].end()) {
								//Find path through blossom to target
								target = it.get(0);
								EdgeSet p(path);
								p.addEdge(u, v);
								unfoldedPath.addEdges(findPathThroughBlossom(G, M, blossom, u, target, p));
								continue;
							}
						}
					}
					unfoldedPath.addEdge(it);
				}
				//unfoldedPath.print("Unfolded Path ");
				return unfoldedPath;
			}
		} else if (find(G.odd.begin(), G.odd.end(), u) != G.odd.end()) {
			//Ignore this edge
		} else {
			//Continue as normal
			EdgeSet newPath = path;
			newPath.addEdge(v, u, G.directed);
			newPath = exploreOdd(G, M, u, newPath, maxDepth);
			if (!newPath.E.empty())
				return newPath;
		}
	}
	
	G.even.remove(v);
	return EdgeSet();
}

EdgeSet FindAugmentingPath(Graph& G, const EdgeSet& M) {
	EdgeSet P;

	//if (M.E.size() == 49)
	//	cout << "HERE";

	//for (int i = G.n - 1; i >= 0; i--) {
	for (int i = 0; i < G.n; i++) {
		if (!M.coversVertex(i)) {
			int maxDepth = 1;
			while (P.E.empty()) {
				G.odd.clear();
				G.even.clear();
				P = exploreEven(G, M, i, P, maxDepth+=2);
			}
			break;
		}
	}

	return P;
}

EdgeSet FindMaxMatching(Graph& G, EdgeSet M) {
	EdgeSet P = FindAugmentingPath(G, M);

	if (P.E.size() != 0) {
		EdgeSet aug = M.augmentBy(P);
		//P.sort();
		//P.print("P Length " + to_string(P.E.size()));
		//aug.sort();
		//aug.print("M Length " + to_string(aug.E.size()));
		return FindMaxMatching(G, aug);
	} else
		return M;
}

bool Rule2(Graph& G, const EdgeSet& M, list<int>& unmarked, int newOdd);

bool Rule1(Graph& G, const EdgeSet& M, list<int>& unmarked, int newEven = -1, int newOdd = -1) {
	list<int> newMarked;
	if (newEven != -1)
		newMarked.push_back(newEven);

	if (unmarked.empty())
		return true;

	for (const int v : G.odd)
		for (const int u : G.adjList[v])
			if (find(unmarked.begin(), unmarked.end(), u) != unmarked.end())
				newMarked.push_back(u);

	if (newMarked.empty())
		return true;

	newMarked.sort();
	newMarked.unique();

	for (const Edge& e : M.E) {
		if (find(newMarked.begin(), newMarked.end(), e.get(0)) != newMarked.end() && find(newMarked.begin(), newMarked.end(), e.get(1)) != newMarked.end())
			return false;
	}

	for (int v : newMarked) {
		G.even.push_back(v);
		unmarked.remove(v);
	}

	return Rule2(G, M, unmarked, newOdd);
}

bool Rule2(Graph& G, const EdgeSet& M, list<int>& unmarked, int newOdd = -1) {
	list<int> newMarked;
	if (newOdd != -1)
		newMarked.push_back(newOdd);

	if (unmarked.empty())
		return true;

	for (const Edge& e : M.E) {
		if (find(unmarked.begin(), unmarked.end(), e.get(1)) != unmarked.end() && find(G.even.begin(), G.even.end(), e.get(0)) != G.even.end())
			newMarked.push_back(e.get(1));
		else if (find(unmarked.begin(), unmarked.end(), e.get(0)) != unmarked.end() && find(G.even.begin(), G.even.end(), e.get(1)) != G.even.end())
			newMarked.push_back(e.get(0));
	}

	if (newMarked.empty())
		return true;

	newMarked.sort();
	newMarked.unique();

	for (int v : newMarked) {
		for (int u : G.adjList[v]) {
			if (find(newMarked.begin(), newMarked.end(), u) != newMarked.end())
				return false;
		}
	}

	for (int v : newMarked) {
		G.odd.push_back(v);
		unmarked.remove(v);
	}

	return Rule1(G, M, unmarked);
}

list<int> MinVertexCover(Graph& G, EdgeSet& M, list<int>& unmarked, int newEven = -1, int newOdd = -1) {
	if (!Rule1(G, M, unmarked, newEven, newOdd))
		return list<int>();

	if (G.odd.size() + G.even.size() == G.n)
		return G.even;
	else {
		for (int v = 0; v < G.n; v++) {
			if (find(unmarked.begin(), unmarked.end(), v) != unmarked.end()) {
				for (int u : G.adjList[v]) {
					if (find(unmarked.begin(), unmarked.end(), u) != unmarked.end()) {
						Graph H(G); 
						Graph I(G);
						list<int> backupUnmarked(unmarked);
						list<int> S = MinVertexCover(I, M, unmarked, u, v);
						if (S.empty())
							return MinVertexCover(H, M, backupUnmarked, v, u);
						else
							return S;
					}
				}
			}
		}
	}

	cout << "Error: Min Vertex Cover method failed";
	return list<int>();
}

list<int> MinVertexCoverStart(const Graph& G, EdgeSet M) {
	//Assume M is maximum matching of G and G Konig-Egervary

	Graph H(G);
	H.even.clear();
	list<int> freeVertices;
	list<int> unmarked;
	for (int i = 0; i < H.n; i++)
		if (!M.coversVertex(i))
			freeVertices.push_back(i);
		else
			unmarked.push_back(i);
	H.odd = freeVertices;

	return MinVertexCover(H, M, unmarked);
}

Graph ConstructG_bip(const Graph& G, const EdgeSet& M, list<int>& A) {
	Graph G_bip(G.n, true);
	for (const Edge& e : M.E) {
		if (find(A.begin(), A.end(), e.get(0)) != A.end())
			G_bip.addEdge(e.get(0), e.get(1));
		else
			G_bip.addEdge(e.get(1), e.get(0));
	}
	for (int j : A) {
		for (const Edge& it : M.E) {
			if (it.get(0) == j) {
				G_bip.addEdge(it.get(1), j);
			} else if (it.get(1) == j) {
				G_bip.addEdge(it.get(0), j);
			}
		}
	}

	return G_bip;
}

list<int> UnsaturatedVertices(const Graph& G, const EdgeSet& M) {
	list<int> U;
	for (int i = 0; i < G.n; i++) {
		if (!M.coversVertex(i))
			U.push_back(i);
	}
	return U;
}

list<int> ReachableFromVertices(Graph& G, list<int>& U) {
	list<int> reachable;
	list<int> frontier(U);

	while (!frontier.empty()) {
		int v = frontier.front();
		frontier.pop_front();
		reachable.push_back(v);
		for (int u : G.adjList[v])
			if (find(reachable.begin(), reachable.end(), u) == reachable.end())
				frontier.push_back(u);
	}

	return reachable;
}

Graph RemoveVerticesFromGraph(Graph G, list<int>& V) {
	for (int i : V)
		G.removeEdgesFromVertex(i);
	return G;
}

bool exploreEven(Graph& G, const EdgeSet& M, const int v, list<int> firstEven, list<int> firstOdd);

bool exploreOdd(Graph& G, const EdgeSet& M, const int v, list<int> firstEven, list<int> firstOdd) {
	G.odd.push_back(v);

	if (!firstOdd.empty() && find(firstOdd.begin(), firstOdd.end(), v) == firstOdd.end()) {
		//v was labelled odd in this tree but not in the first tree
		return false;
	}

	bool ans = true;

	for (int u : G.adjList[v]) {
		if (find(G.even.begin(), G.even.end(), u) != G.even.end()) {
			//Ignore this edge
		}
		else if (find(G.odd.begin(), G.odd.end(), u) != G.odd.end()) {
			//If two odd vertices are adjacent, return false
			return false;
		}
		else {
			//Continue as normal
			ans &= exploreEven(G, M, u, firstEven, firstOdd);
		}
	}

	return ans;
}

bool exploreEven(Graph& G, const EdgeSet& M, const int v, list<int> firstEven, list<int> firstOdd) {
	G.even.push_back(v);

	if (!firstEven.empty() && find(firstEven.begin(), firstEven.end(), v) == firstEven.end()) {
		//v was labelled even in this tree but not in the first tree
		return false;
	}

	bool ans = true;

	for (const Edge& it : M.E) {
		if (it.get(0) == v) {
			//Matched edge has two even endpoints
			if (find(G.even.begin(), G.even.end(), it.get(1)) != G.even.end()) {
				return false;
			}
			ans &= exploreOdd(G, M, it.get(1), firstEven, firstOdd);
		}
		else if (!it.directed && it.get(1) == v) {
			//Matched edge has two even endpoints
			if (find(G.even.begin(), G.even.end(), it.get(0)) != G.even.end()) {
				return false;
			}
			ans &= exploreOdd(G, M, it.get(0), firstEven, firstOdd);
		}
	}

	return ans;
}

list<int> CalculateS1(const Graph& G, const list<int>& R, const EdgeSet& M) {
	//Construct alternating tree from each vertex in R
	//Ensure labellings match up, if not return FAIL
	//Ensure no two odd vertices are adjacent, if not return FAIL
	//Ensure no two matched vertices are both even, if not return FAIL
	//Else return the even vertices
	bool ans = true;
	list<int> firstEven;
	list<int> firstOdd;

	for (int i : R) {
		Graph H(G);
		H.odd.clear();
		H.even.clear();
		ans &= exploreEven(H, M, i, firstEven, firstOdd);
		if (i == R.front()) {
			firstEven = H.even;
			firstOdd = H.odd;
		}
	}

	if (ans == false) {
		//One of the trees contains a matching between even vertices or two adjacent odd vertices
		return list<int>();
	} else {
		return firstEven;
	}
}

list<int> CalculateS2(const Graph& G, const EdgeSet& M, const list<int>& B, const list<int>& S1) {
	list<int> S2;

	for (const Edge& e : M.E) {
		if (find(S1.begin(), S1.end(), e.get(0)) == S1.end() && find(S1.begin(), S1.end(), e.get(1)) == S1.end()) {
			S2.push_back(e.get(0));
			S2.push_back(e.get(0));
		}
	}

	SetMinusVertices(S2, B);

	return S2;
}

bool TestS(const Graph& G, const list<int>& S, const int size, const list<int>& Z) {
	Graph H(G);
	for (int i : S)
		H.removeEdgesFromVertex(i);
	
	list<int> I = IntersectVertices(S, Z);
	I.sort();

	return H.m == 0 && S.size() == size && I == Z;
}

list<int> MinimumVertexCoverContaining(Graph& G, list<int>& Z, EdgeSet& M, list<int>& A, list<int>& B, list<int>& R_U, Graph& G_U) {
	Z.sort();

	if (M.E.empty()) {
		//Calculate Max Matching if not already done
		M = FindMaxMatching(G, M);
	}

	if (A.empty()) {
		//Calculate (A,B) partition if not already done
		A = MinVertexCoverStart(G, M);
	}

	if (B.empty()) {
		//Calculate (A,B) partition if not already done
		for (int i = 0; i < G.n; i++)
			if (find(A.begin(), A.end(), i) == A.end())
				B.push_back(i);
	}

	//Calculate L and R sets
	//list<int> L = IntersectVertices(Z, A);
	list<int> R = IntersectVertices(Z, B);

	if (R_U.empty()) {
		//Calculate G_bip(A->B, M) graph
		Graph G_bip = ConstructG_bip(G, M, A);

		//Calculate U, R(U)
		list<int> U = UnsaturatedVertices(G, M);
		list<int> R_U = ReachableFromVertices(G_bip, U);
	}

	list<int> X = IntersectVertices(A, R_U);

	//Check (Z intersect R(U)) - X is empty
	if (!SetMinusVertices(IntersectVertices(Z, R_U), X).empty()) {
		return list<int>();
	}

	if (G_U.n == 0) {
		//Calculate G/U graph
		Graph G_U = RemoveVerticesFromGraph(G, R_U);
	}

	list<int> t = SetMinusVertices(R, R_U);
	list<int> S1 = CalculateS1(G_U, t, M);
	S1.sort();

	list<int> S2 = CalculateS2(G, M, B, S1);
	S2.sort();

	list<int> S = UnionVertices(S1, S2);

	if (TestS(G, S, A.size(), Z)) {
		PrintVertices("Min Vertex Cover: ", S);
		return S;
	}

	return list<int>();
}

list<int> UniqueMinimumVertexCover(Graph& G) {
	//Calculate Max Matching
	EdgeSet M;
	M = FindMaxMatching(G, M);
	//M.sort();
	M.print("M Length " + to_string(M.E.size()) + ":");

	//Calculate (A,B) partition
	list<int> A = MinVertexCoverStart(G, M);
	//A.sort();
	PrintVertices("Min Vertex Cover: ", A);
	//PrintVertices("A:", A);
	list<int> B;
	for (int i = 0; i < G.n; i++)
		if (find(A.begin(), A.end(), i) == A.end())
			B.push_back(i);
	//PrintVertices("B:", B);

	//Calculate G_bip(A->B, M) graph
	Graph G_bip = ConstructG_bip(G, M, A);

	//Calculate U, R(U), X sets
	list<int> U = UnsaturatedVertices(G, M);
	list<int> R_U = ReachableFromVertices(G_bip, U);

	//Calculate G/U graph
	Graph G_U = RemoveVerticesFromGraph(G, R_U);

	//For every matching edge mi = (ai, bi), we want to find a min vertex cover containing bi
	//If any of these result in a minimum vertex cover as well, then we know A is not unique

	for (const Edge& e : M.E) {
		if (find(A.begin(), A.end(), e.get(0)) != A.end()) {
			Graph H(G);
			list<int> Z;
			Z.push_back(e.get(1));
			if (!MinimumVertexCoverContaining(H, Z, M, A, B, R_U, G_U).empty())
				return list<int>();
		} else {
			Graph H(G);
			list<int> Z;
			Z.push_back(e.get(0));
			if (!MinimumVertexCoverContaining(H, Z, M, A, B, R_U, G_U).empty())
				return list<int>();
		}
	}

	return A;
}

list<int> MinimumVertexCoverContaining(Graph& G, list<int>& Z) {
	EdgeSet M;
	list<int> A, B, R_U;
	Graph G_U(0);
	return MinimumVertexCoverContaining(G, Z, M, A, B, R_U, G_U);
}

//	Read graph files for testing dataset
//		Should be easy as files are format:
//		1		n m
//		2		v0 u0
//		3		v1 u1
//		|		  |
//		m+1		vm um

Graph ReadInGraph(string fileName) {
	string text;
	ifstream file(fileName + ".txt");

	//  n   m - Graph parameters
	getline(file, text);
	int n = stoi(text.substr(2, text.find("   ")));

	Graph G(n);

	//vi ui - An edge in the graph
	while (getline(file, text)) {
		int v = stoi(text.substr(0, text.find(' ')));
		int u = stoi(text.substr(text.find(' '), text.size()));
		G.addEdge(v, u);
	}

	file.close();

	return G;
}

//Requires the input graphs to be such that the Bipartite is split with vertices 1,...,|L| all in L
//and the remainder |L|+1,....,|L|+|R| are in R and the General has at most |L| vertices
Graph CombineGeneralBipartite(string generalPath, string bipartitePath) {
	Graph G = ReadInGraph(bipartitePath);

	string text;
	ifstream file(generalPath + ".txt");

	//  n   m - Graph parameters
	getline(file, text);
	int n = stoi(text.substr(2, text.find("   ")));

	//vi ui - An edge in the graph
	while (getline(file, text)) {
		int v = stoi(text.substr(0, text.find(' ')));
		int u = stoi(text.substr(text.find(' '), text.size()));
		G.addEdge(v - 1, u - 1);
	}

	file.close();

	//Add a perfect matching to ensure the graph is Konig
	for (int i = 0; i < n; i++)
	{
		G.addEdge(i, i + n);
	}

	return G;
}

void runAlgorithm(Graph& G, string name = "G") {
	//G.print("G:");
	cout << name + " - n=" << G.n << " m=" << G.m << "\n";

	unsigned __int64 start = chrono::duration_cast<chrono::milliseconds>(
		chrono::system_clock::now().time_since_epoch()).count();

	list<int> S = UniqueMinimumVertexCover(G);

	unsigned __int64 end = chrono::duration_cast<chrono::milliseconds>(
		chrono::system_clock::now().time_since_epoch()).count();

	ofstream results = ofstream("results.csv", ios_base::app);

	results << name + "," + to_string(G.n) + "," + to_string(G.m);

	if (S.empty()) {
		cout << "G does not have a unique minimum vertex cover\n";
		results << ",Not Unique,";
	} else {
		cout << "This is the unique vertex cover in G\n";
		results << ",Unique,";
	}

	cout << "Time taken: " << (end - start) << "ms\n\n";

	results << (end - start) << "\n";

	results.close();
}

void WriteGraphToFile(const Graph& G, const string fileName) {
	ofstream file(fileName + ".txt");

	file << "  " << G.n << "   " << G.m << "\n";

	for (int i = 0; i < G.n; i++)
		for (int j : G.adjList[i])
			file << i << " " << j << "\n";

	file.close();
}

void TestDatasetForUniqueVertexCovers() {
	//	50		1-18
	//	100		1-18
	//	150		1-18
	//	200		1-18
	//	250		1-18
	//	300		1-18
	//	350		1-18

	int minN = -1;		//50
	int maxN = -1;		//350
	int minNum = -1;	//1
	int maxNum = -1;	//18*18 = 324
	bool overwrite = false;

	while (!(minN >= 50 && minN <= 350 && maxN >= minN && maxN <= 350 && minNum >= 1 && minNum <= 324 && maxNum >= minNum && maxNum <= 324)) {
		cout << "Warning: Inputting anything but a number will break the program here.\n";
		cout << "Provide minN (50 - 350): ";
		cin >> minN;
		cout << "Provide maxN (" + to_string(minN) + " - 350): ";
		cin >> maxN;
		cout << "Provide minNum (1 - 324): ";
		cin >> minNum;
		cout << "Provide maxNum (" + to_string(minNum) + " - 324): ";
		cin >> maxNum;
		cout << "1/0 - Overwrite current results file: ";
		cin >> overwrite;
	}

	if (overwrite) {
		ofstream results = ofstream("results.csv");
		results << "Graph,n,m,Result,Time Taken(ms),,Total(s),=SUM(E:E)/1000\n";
		results.close();
	}

	//Load each Konig graph and run it
	for (int n = minN; n <= maxN; n += 50) {
		for (int i = minNum; i <= maxNum; i++) {
			Graph G = ReadInGraph("data/KonigGraph-" + to_string(n) + "-" + to_string(i));
			//G.print("KonigGraph-" + to_string(n) + "-" + to_string(i));
			runAlgorithm(G, "data/KonigGraph-" + to_string(n) + "-" + to_string(i));
		}
	}
}

int main() {
	
	TestDatasetForUniqueVertexCovers();

	cout << "\n";
	system("pause");
}
