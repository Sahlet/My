//Graph.h
#pragma once
#include <set>
#include <map>
#include <list>
#include <vector>
#include <memory>
#include <exception>
#include <initializer_list>

namespace My {
	
	struct illegal_vertex_state : public std::logic_error {
		illegal_vertex_state() : logic_error("illegal_vertex_state") {}
		illegal_vertex_state(std::string text) : logic_error(std::move(text)) {}
	};

	/*
	typedef int vertex_identifier_T;
	typedef void* content_T;
	typedef std::less<vertex_identifier_T> vertex_identifier_T_less;
	/**/
	template<
		typename content_T = void*,
		typename vertex_identifier_T = int,
		typename vertex_identifier_T_less = std::less<vertex_identifier_T>>
	class graph {
		struct graph_ref {
			graph* me;
			graph_ref(graph& me) : me(&me) {}
		};
		std::shared_ptr< graph_ref > graph_identifier;
	public:
		typedef graph GRAPH;
		typedef content_T content_type;
		typedef vertex_identifier_T vertex_identifier_type;
		typedef vertex_identifier_T_less vertex_identifier_type_less;

		//is digraph
		bool is_orgraph() const { return orgraph; }
		
		//can vertex have link with itself
		bool is_recursive() const { return recursive; }

		class vertex {
			friend class graph;
			std::set<vertex*> links;
			vertex_identifier_T identifier;
			std::weak_ptr< graph_ref > graph_identifier;
		public:
			vertex(std::weak_ptr< graph_ref > graph_identifier, vertex_identifier_T identifier) :
				graph_identifier(std::move(graph_identifier)), identifier(std::move(identifier)) {}

			vertex(const vertex&) = delete;
			vertex(vertex&&) = delete;
			vertex& operator=(const vertex&) = delete;
			vertex& operator=(vertex&&) = delete;

			content_T content;
			const vertex_identifier_T& get_identifier() const {
				return identifier;
			}
			const std::set<vertex*>& get_links() const { return links; }
			
			vertex* add_link(vertex* link) throw (std::invalid_argument, illegal_vertex_state) {
				if (!link) { throw std::invalid_argument("link is nullptr"); }
				auto real_graph_identifier = graph_identifier.lock();
				if (!real_graph_identifier || link->graph_identifier.lock() != real_graph_identifier) { throw illegal_vertex_state(); }

				graph& G = *real_graph_identifier->me;
				
				if (link == this && !G.is_recursive()) { throw std::invalid_argument("link == this && !g.is_recursive()"); }

				this->links.insert(link);
				if (!G.is_orgraph() && link != this) {
					link->links.insert(this);
				}
				return this;
			}
			vertex* add_link(const vertex_identifier_T& identifier) throw (std::invalid_argument, illegal_vertex_state) {
				auto real_graph_identifier = graph_identifier.lock();
				if (!real_graph_identifier) { throw illegal_vertex_state(); }
				graph& G = *real_graph_identifier->me;
				return add_link(G.find(identifier));
			}
			template<class _T_>
			vertex* add_link(const std::vector<_T_>& objs) throw (std::invalid_argument, illegal_vertex_state) {
				for (auto& obj : objs) { add_link(obj); }
				return this;
			}
			template<class _T_>
			vertex* add_link(const std::initializer_list<_T_>& objs) throw (std::invalid_argument, illegal_vertex_state) {
				for (auto& obj : objs) { add_link(obj); }
				return this;
			}
		};

	private:
		struct vertex_identifier_T_ptr_less {
			bool operator()(const vertex_identifier_T* const left, const vertex_identifier_T* const right) const {
				return less(*left, *right);
			}
		private:
			vertex_identifier_T_less less;
		};

		bool orgraph; //digraph
		bool recursive; //can vertex have link with itself
		std::list< vertex > vertices_list;
		typedef typename std::list< vertex >::iterator list_iter_type;
		std::map< const vertex_identifier_T*, list_iter_type, vertex_identifier_T_ptr_less > vertices_map;
	public:
		using ptr = std::shared_ptr<graph>;
		using wptr = std::shared_ptr<graph>;

		graph(bool orgraph /*digraph*/ = false, bool recursive = false) : graph_identifier(new graph_ref(*this)), orgraph(orgraph), recursive(recursive) {}

		graph& operator=(const graph& G) {
			if (this == &G) { return *this; }
			this->graph_identifier.reset(new graph_ref(*this));
			this->orgraph = G.orgraph;
			this->recursive = G.recursive;
			for (const vertex& G_vert : G.get_vertices()) {
				this->add(G_vert.get_identifier());
			}

			for (const vertex& G_vert : G.get_vertices()) {
				vertex* my_vert = this->find(G_vert.get_identifier());
				for (vertex* G_link : G_vert.get_links()) {
					my_vert->add_link(G_link->get_identifier());
				}
			}
			return *this;
		}
		graph(const graph& G) {
			if (this == &G) { return; }
			*this = G;
		}

		graph& operator=(graph&& G) {
			if (this == &G) { return *this; }
			this->graph_identifier = std::move(G.graph_identifier);
			this->graph_identifier->me = this;
			G.graph_identifier.reset(new graph_ref(G));
			this->orgraph		= std::move(G.orgraph);
			this->recursive		= std::move(G.recursive);
			this->vertices_list	= std::move(G.vertices_list);
			this->vertices_map	= std::move(G.vertices_map);
			return *this;
		}
		graph(graph&& G) {
			if (this == &G) { return; }
			*this = std::move(G);
		}

		const std::list< vertex >& get_vertices() const { return vertices_list; }
		
		std::list< vertex* > get_vertices_ptrs() const {
			std::list< vertex* > res;
			for (auto& vert : vertices_list) {
				res.push_back(const_cast<vertex*>(&vert));
			}
			return std::move(res);
		}

		std::pair<vertex&, bool> add(vertex_identifier_T identifier) {
			auto map_iter = vertices_map.find(&identifier);
			bool insert = false;
			if (map_iter == vertices_map.end()) {
				vertices_list.emplace_back(graph_identifier, std::move(identifier));
				auto list_iter = std::prev(vertices_list.end());
				//std::pair<const vertex_identifier_T*, std::list< vertex >::iterator>
				map_iter = vertices_map.insert({ &list_iter->get_identifier(), list_iter }).first;
				insert = true;
			}

			return { *map_iter->second, insert };
		}
		
		bool del(const vertex_identifier_T& identifier) {
			auto map_iter = vertices_map.find(&identifier);
			if (map_iter == vertices_map.end()) { return false; }
			auto list_iter = map_iter->second;
			vertices_map.erase(map_iter);
			vertex* link = list_iter.operator->();
			for (auto& vertex : vertices_list) {
				vertex.links.erase(link);
			}
			vertices_list.erase(list_iter);
		}

		vertex& operator[](const vertex_identifier_T& identifier) {
			return add(identifier).first;
		}

		vertex* find(const vertex_identifier_T& identifier) {
			auto map_iter = vertices_map.find(&identifier);
			if (map_iter == vertices_map.end()) { return nullptr; }
			return map_iter->second.operator->();
		}
	};
}