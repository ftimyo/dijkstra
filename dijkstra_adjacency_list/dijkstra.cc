#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <stdio.h>
using std::string;
using std::vector;
using std::map;
static const int INFINITE = 1 << 10;

struct vertex {
	struct out_edge {
		vertex	*v;
		int weight;
		out_edge(vertex *vp, int w):v{vp}, weight{w}{}
	};

	vertex(const string& name):id{name}, adj{}, dist{INFINITE}, path{NULL}{}
	virtual ~vertex() {}
	string id;
	vector<out_edge> adj;
	int dist;
	vertex *path;
};

using std::push_heap;
using std::pop_heap;
using std::make_heap;

struct graph {
	struct edge {
		string v_start;
		string v_end;
		int weight;
		edge(const string& s, const string& e, int w): v_start{s},
		  v_end{e}, weight{w}{}
	};
	map<string,vertex*> origin;
	vector<vertex*>	paths;
	graph():origin{}, paths{}{}
	virtual ~graph() {
		for_each(origin.begin(), origin.end(), [](map<string,vertex*>::value_type& p) {
			delete p.second;
		});
	}
	
	struct vertex_comp {
		bool operator()(const vertex *v1, const vertex *v2) {
			return v1->dist > v2->dist;
		}
	};
	void get_paths(const string& source);
	void print_all_paths(FILE *os);
	void print_path(FILE *os, vertex *v);
	void print_vertices(FILE *os);
	void load_graph(const vector<edge>& E);
};

void graph::print_path(FILE *os, vertex *v)
{
	if (v->path != NULL) {
		print_path(os, v->path);
		fprintf(os, " -> %s", v->id.c_str());
	} else {
		fprintf(os, "%s", v->id.c_str());
	}
}

void graph::print_all_paths(FILE *os)
{
	for(auto& v : paths) {
		fprintf(os, "%s (distance %d) path:\t", v->id.c_str(), v->dist);
		print_path(os, v);
		fprintf(os, "\n");
	}
}

void graph::get_paths(const string& source)
{
	vertex *s = origin[source];
	vector<vertex*> heap;
	if (s == NULL) {
		fprintf(stderr, "source %s is not in the graph\n", source.c_str());
	}

	s->dist = 0;
	s->path = NULL;
	heap.push_back(s);
	push_heap(heap.begin(), heap.end(), vertex_comp{});

	while (!heap.empty()) {
		vertex *v = heap.front();
		paths.push_back(v);
		pop_heap(heap.begin(), heap.end(), vertex_comp{});
		heap.pop_back();
		for (auto& e : v->adj) {
			int dist = v->dist + e.weight;
			if (e.v->dist == INFINITE) {
				e.v->dist = dist;
				e.v->path = v;
				heap.push_back(e.v);
				push_heap(heap.begin(), heap.end(), vertex_comp{});
			} else if (dist < e.v->dist) {
				e.v->dist = dist;
				e.v->path = v;
				make_heap(heap.begin(), heap.end(), vertex_comp{});
			}
		}
	}
}

void graph::load_graph(const vector<graph::edge>& E)
{
	for (auto& e : E) {
		auto& start = origin[e.v_start];
		auto& end = origin[e.v_end];
		if (start == NULL) {
			start = new vertex{e.v_start};
		}
		if (end == NULL) {
			end = new vertex{e.v_end};
		}
		start->adj.push_back(vertex::out_edge{end, e.weight});
	}
}

void graph::print_vertices(FILE *os)
{
	fprintf(os, "origin V set (%lu vertice)\n", origin.size());
	for (auto& v : origin) {
		fprintf(os, "%s:\t", v.second->id.c_str());
		for (auto& e : v.second->adj) {
			fprintf(os, " %s", e.v->id.c_str());
		}
		fprintf(os, "\n");
	}
}

void load_graph_from_file(const char* filename, std::vector<graph::edge>& E)
{
	std::ifstream is{filename};
	string s, e;
	int w;
	while(is>>s>>e>>w) {
		E.emplace_back(s, e, w);
	}
}

int main()
{
	vector<graph::edge> E;
	load_graph_from_file("digraph", E);
	graph djikstra;
	djikstra.load_graph(E);
//	djikstra.get_paths(E[0].v_start.c_str());
	djikstra.get_paths("v1");
	djikstra.print_all_paths(stdout);
	return 0;
}
