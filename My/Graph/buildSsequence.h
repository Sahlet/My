//buildSsequence.h
#pragma once
#include <list>
#include <iostream>
#include "Graph.h"

namespace My {
	//typedef graph< void*, int, std::less<int> > GRAPH;
	
	template< typename GRAPH >
	std::list< std::list< typename GRAPH::vertex_identifier_type > > buildSsequence(GRAPH&& G, const bool S_top = false) {
		if (G.is_recursive()) {
			std::invalid_argument err("G.is_recursive()");
			std::cerr << err.what() << std::endl;
			throw err;
		}
		std::list< std::list< GRAPH::vertex_identifier_type > > res;

		if (G.get_vertices().empty()) {
			return std::move(res);
		}

		std::list< GRAPH::vertex_identifier_type > current_column;

		std::list< GRAPH::vertex* > vertices_list = G.get_vertices_ptrs();

		std::set< GRAPH::vertex* > vertices_set;
		for (auto& vert : vertices_list) {
			vertices_set.insert(vert);
		}

		for (GRAPH::vertex* vert : vertices_list) {
			if (S_top) {
				if (!vert->get_links().empty()) {
					vertices_set.erase(vert);
				}
			} else {
				for (GRAPH::vertex* link : vert->get_links()) {
					vertices_set.erase(link);
				}
			}
		}

		if (vertices_set.empty()) {
			std::invalid_argument err("there is cycle in G");
			std::cerr << err.what() << std::endl;
			throw err;
		}

		for (GRAPH::vertex* vert : vertices_set) {
			current_column.emplace_back(vert->get_identifier());
			G.del(vert->get_identifier());
		}

		if (!G.get_vertices().empty()) {
			res = std::move(buildSsequence(std::move(G), S_top));
		}

		if (S_top) {
			res.emplace_back(std::move(current_column));
		} else {
			res.emplace_front(std::move(current_column));
		}

		return std::move(res);
	}
	
	template< typename GRAPH >
	std::list< std::list< typename GRAPH::vertex_identifier_type > > buildSsequence(const GRAPH& G, const bool S_top = false) {
		return buildSsequence(GRAPH(G), S_top);
	}
}

template< class T >
std::ostream& operator << (std::ostream& out, const std::list< std::list< T > >& _list_) {
	bool first1 = true;
	out << "\n<\n";
	for (auto& l : _list_) {
		if (!first1) { out << ";\n"; }
		first1 = false;
		out << "\t";
		bool first2 = true;
		for (auto& val : l) {
			if (!first2) { out << ", "; }
			first2 = false;
			out << val;
		}
	}
	out << "\n>\n";
	return out;
}