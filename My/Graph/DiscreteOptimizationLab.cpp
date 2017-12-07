// DiscreteOptimizationLab.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Graph.h"
#include "json11.hpp"
#include "JSONReader.h"
#include "buildSsequence.h"
#include <istream>
#include <ostream>
#include <strstream>
#include <iostream>
#include <fstream>

typedef int vertex_key;
typedef std::map< vertex_key, std::vector<vertex_key> > vertex_map;

#define GraphStringify_LIST(DEFINITION)	\
	DEFINITION(bool, orgraph, "orgraph")	\
	DEFINITION(bool, recursive, "recursive")	\
	DEFINITION(vertex_map, vertices, "vertices")

JSON_class_gen(GraphStringify, GraphStringify_LIST);

template< class content_T >
std::ostream& operator << (std::ostream& out, const My::graph< content_T, vertex_key >& G) {
	GraphStringify parsable;
	parsable.orgraph = G.is_orgraph();
	parsable.recursive = G.is_recursive();
	for (auto& vert : G.get_vertices()) {
		auto& links = parsable.vertices[vert.get_identifier()];
		links.reserve(vert.get_links().size());
		for (auto& link : vert.get_links()) {
			links.emplace_back(link->get_identifier());
		}
	}

	out << parsable.dump();

	return out;
}

template< class content_T >
std::istream& operator >> (std::istream& in, My::graph< content_T, vertex_key >& G) {
	std::strstream str;
	int count = 0;
	while (in && !count) {
		char symbol = in.get();
		if (symbol == '{') {
			count++;
		}
	}

	if (!count) {
		LOGW("FIND ME in %s : !count", __func__);
		return in;
	}

	str.put('{');

	while (in && count) {
		char symbol = in.get();
		if (symbol == '{') {
			count++;
		} else if (symbol == '}') {
			count--;
		}
		str.put(symbol);
	}

	str.put('\0');

	GraphStringify parsable;
	if (!parsable.parse(str.str())) { return in; }

	G = std::remove_reference<decltype(G)>::type(parsable.orgraph, parsable.recursive);

	for (auto& pair : parsable.vertices) {
		G.add(pair.first);
	}

	bool there_was_unhandled_identifiers = false;

	for (auto& pair : parsable.vertices) {
		auto vert = G.find(pair.first);
		if (!vert) {
			LOGE("FIND ME in %s : !vert - impossible", __func__);
		}
		auto& links = pair.second;
		for (auto& vert_identifier : links) {
			auto link = G.find(vert_identifier);
			if (!link) {
				there_was_unhandled_identifiers = true;
				continue;
			}
			vert->add_link(link);
		}
	}

	if (there_was_unhandled_identifiers) { LOGE("FIND ME in %s : !link - bad serialization data (there_was_unhandled_identifiers)", __func__); }

	return in;
}

#define IN

#ifdef IN
std::ifstream in(
	"Graph2.JSON"
);
#else
std::ofstream out("Graph.JSON");
#endif

int main(int argc, const char **argv) {
	struct guard_t {
		~guard_t() {
			std::cout << std::endl;
			system("pause");
		}
	} guard;

	My::graph< vertex_key > G(true);

	try {
#ifdef IN
		in >> G;
#else
		G.add(1);
		G.add(2);
		G.add(3);
		G.add(4);
		G.add(5);
		G.add(6);
		G.add(7);
		G.add(8);
		G.add(9);
		G.add(10);
		G.add(11);
		G.add(12);

		G.find(1)->add_link({ 4, 5 });
		G.find(2)->add_link({ 5, 6 });
		G.find(3)->add_link({ 4, 5, 6 });
		G.find(4)->add_link({ 5 });
		G.find(5)->add_link({ 6 });
		G.find(6)->add_link({ 8, 9 });
		G.find(7)->add_link({ 5, 6, 8, 4 });
		G.find(8)->add_link({ 9, 10, 12 });

		out << G;
#endif
	} catch (std::exception e) {
		std::cerr << e.what();
		return 1;
	}

	std::list< std::list< vertex_key > > S_bottom, S_top;

	try {
		S_bottom = My::buildSsequence(G, false);
		S_top = My::buildSsequence(G, true);
	} catch (const std::exception&) {
		return 2;
	}

	std::cout
		<< "G = " << G << std::endl << std::endl
		<< "S_bottom = " << S_bottom << std::endl
		<< "S_top = " << S_top << std::endl
		<< std::endl;

    return 0;
}

