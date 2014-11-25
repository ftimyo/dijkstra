#include <unordered_map>
#include <set>
#include <limits>
#include <tuple>
#include <vector>
#include <string>
#include <cstdio>
#include <fstream>
using std::string;
using std::pair;
using std::tuple;
using std::vector;
using std::get;
typedef std::unordered_map<string, tuple<int, string, vector<pair<string, int>>>> VertexEdgeMap;
typedef std::set<tuple<int, string>>	VertexSet;
const int kInfinite = std::numeric_limits<int>::max();

void ShortestPath(VertexEdgeMap& vertex_edge, string& source) {
	VertexSet vertex_set;
	auto& s = vertex_edge[source];
	get<0>(s) = 0;
	vertex_set.emplace(get<0>(s), source);
	while (!vertex_set.empty()) {
		const string& top_id = get<1>(*(vertex_set.begin()));
		auto& top = vertex_edge[get<1>(*(vertex_set.begin()))];
		for (auto& neighbor : get<2>(top)) {
			int dist = get<0>(top) + get<1>(neighbor);
			auto& v = vertex_edge[get<0>(neighbor)];
			if (get<0>(v) == kInfinite) {
				get<0>(v) = dist;
				get<1>(v) = top_id;
				vertex_set.emplace(dist, get<0>(neighbor));
			} else if (dist < get<0>(v)) {
				vertex_set.erase(std::make_tuple(get<0>(v), get<0>(neighbor)));
				get<0>(v) = dist;
				get<1>(v) = top_id;
				vertex_set.emplace(dist, get<0>(neighbor));
			}
		}
		vertex_set.erase(vertex_set.begin());
	}
}

void PrintPath(VertexEdgeMap& vertex_edge, const string& vertex, FILE *os) {
	auto& v = vertex_edge[vertex];
	if (get<1>(v) != "") {
		PrintPath(vertex_edge, get<1>(v), os);
		fprintf(os, " -> %s", vertex.c_str());
	} else {
		fprintf(os, "%s", vertex.c_str());
	}
}

void PrintPathInfo(VertexEdgeMap& vertex_edge, const string& vertex, FILE *os) {
	if (get<0>(vertex_edge[vertex]) == kInfinite) {
		fprintf(os, "%s dist (unreachable) No Path", vertex.c_str());
	} else {
		fprintf(os, "%s dist (%d) Path:\t ", vertex.c_str(), get<0>(vertex_edge[vertex]));
		PrintPath(vertex_edge, vertex, os);
	}
	fprintf(os, "\n");
}

void PrintAllPathInfo(VertexEdgeMap& vertex_edge, FILE *os) {
	for (auto& v : vertex_edge) {
		PrintPathInfo(vertex_edge, v.first.c_str(), os);
	}
}


void LoadGraph(std::ifstream& is, VertexEdgeMap& vertex_edge) {
	string s, e;
	int w;
	while(is>>s>>e>>w) {
		auto& vertex = vertex_edge[s];
		get<0>(vertex) = kInfinite;
		get<1>(vertex) = "";
		get<2>(vertex).emplace_back(e, w);
		auto& vertex_right = vertex_edge[e];
		get<0>(vertex_right) = kInfinite;
		get<1>(vertex_right) = "";
	}
}

int main() {
	std::ifstream is{"digraph"};
	std::string source{"v1"};
	VertexEdgeMap ve;
	LoadGraph(is, ve);
	ShortestPath(ve, source);
	PrintAllPathInfo(ve, stdout);
	return 0;
}
