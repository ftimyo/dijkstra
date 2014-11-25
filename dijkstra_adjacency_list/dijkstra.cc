#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <fstream>
#include <stdio.h>
#include <utility>
#include <tuple>
#include <limits>

namespace dijkstra_algo {

using std::string;
using std::vector;
using std::push_heap;
using std::pop_heap;
using std::make_heap;
using std::pair;
using std::tuple;
using std::get;
using std::set;

class Graph {
 public:
	static const int kInfinite = std::numeric_limits<int>::max();

	typedef tuple<string, string, int> edge_type;

	struct Vertex {
		Vertex(const string& vertex_id) : id{vertex_id}, adj{},
				dist{Graph::kInfinite}, path{NULL} {}
		virtual ~Vertex(){}

		string	id;
		int			dist;
		Vertex	*path;
		vector<pair<Vertex*, int>> adj;
	};

	struct VertexComp {
		bool operator() (Vertex *v1, Vertex *v2) {
		 return v1->dist > v2->dist;
		}
	};

	struct VertexIdComp {
		bool operator() (Vertex *v1, Vertex *v2) {
			return v1->id < v2->id;
		}
	};

	typedef set<Vertex*, VertexIdComp> VertexSet;

 public:
	Graph() : edges_{}, paths_{} {}
	virtual ~Graph() {
		for (auto& v : paths_) {
			delete v;
		}
	}

	void GetShortestPaths(const string& id);
	void PrintAllPaths(FILE *os);
	void AddEdge(const string& start, const string& end, int weight);

 private:
	void GetShortestPaths(Vertex *source, VertexSet& v_set);
	void PrintPath(FILE *os, Vertex *v);
	Vertex* UpdateSet(VertexSet& set, const string& id);
	void GetAdjacencyList(VertexSet& adj);
	vector<edge_type> edges_;
	vector<Vertex*> paths_;
}; 

void Graph::GetShortestPaths(const string& id) {
	VertexSet v_set;
	GetAdjacencyList(v_set);
	Vertex v{id};
	auto pos = v_set.find(&v);
	if (pos == v_set.end()) {
		return;
	} else {
		GetShortestPaths(*pos, v_set);
	}
}

void Graph::GetShortestPaths(Graph::Vertex *source, Graph::VertexSet& v_set) {
	vector<Vertex*> heap;
	source->dist = 0;
	heap.push_back(source);
	push_heap(heap.begin(), heap.end(), VertexComp{});

	while (!heap.empty()) {
		auto s = heap.front();
		pop_heap(heap.begin(), heap.end(), VertexComp{});
		v_set.erase(s);
		heap.pop_back();
		paths_.push_back(s);

		for (auto v : s->adj) {
			int dist = s->dist + v.second;

			if (v.first->dist == Graph::kInfinite) {
				v_set.erase(v.first);
				v.first->dist = dist;
				v.first->path = s;
				heap.push_back(v.first);
				push_heap(heap.begin(), heap.end(), VertexComp{});
			} else if (dist < v.first->dist) {
				v.first->dist = dist;
				v.first->path = s;
				make_heap(heap.begin(), heap.end(), VertexComp{});
			} 
		}
	}
	
	for (auto v : v_set) {
		paths_.push_back(v);
	}
}


Graph::Vertex* Graph::UpdateSet(VertexSet& v_set, const string& id) {
	Vertex v{id};
	auto pos = v_set.equal_range(&v);
	if (pos.first == pos.second) {
		auto v = new Vertex{id};
		v_set.emplace_hint(pos.first, v);
		return v;
	} 
	return *(pos.first);
}

void Graph::GetAdjacencyList(VertexSet& v_set) {
	for (auto& edge : edges_) {
		Graph::Vertex *s = UpdateSet(v_set, get<0>(edge));
		Graph::Vertex *e = UpdateSet(v_set, get<1>(edge));
		s->adj.emplace_back(e, get<2>(edge));
	}
}

void Graph::PrintPath(FILE *os, Graph::Vertex *v) {
	if (v->path != NULL) {
		PrintPath(os, v->path);
		fprintf(os, " -> %s", v->id.c_str());
	} else {
		fprintf(os, "%s", v->id.c_str());
	}
}

void Graph::PrintAllPaths(FILE *os) {
	fprintf(os, "Source vertex: %s\n", paths_[0]->id.c_str());
	for(auto& v : paths_) {
		if (v->dist == Graph::kInfinite) {
			fprintf(os, "%s (distance Inifinte) No path\n", v->id.c_str());
		} else {
			fprintf(os, "%s (distance %d) path:\t", v->id.c_str(), v->dist);
			PrintPath(os, v);
			fprintf(os, "\n");
		}
	}
}

void Graph::AddEdge(const string& start, const string& end, int weight) {
	edges_.emplace_back(start, end, weight);
}

}//namespace dijkstra

int main()
{
	std::ifstream is{"digraph"};
	std::string s, e;
	int w;
	dijkstra_algo::Graph dijkstra;
	while(is>>s>>e>>w) {
		dijkstra.AddEdge(s, e, w);
	}
	dijkstra.GetShortestPaths("v1");
	dijkstra.PrintAllPaths(stdout);
	return 0;
}
