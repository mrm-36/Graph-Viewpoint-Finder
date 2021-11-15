#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <functional>
#include <tuple>
#include <map>
#include <unordered_map>

struct Graph {
  int V, E;
  std::map<int, std::vector<int>> adj;

  Graph(int _V, int _E)
  : V{ _V }, E{ _E }
  {}

  // Inverse the edges in the graph
  // RunTime: O(V + E)
  Graph* getTranspose() const {
    Graph* GT = new Graph(V, E);
    for (const auto& p : adj) {
       GT->adj[p.first]; // for the node to show in the map
      for (int v : p.second) GT->adj[v].push_back(p.first);
    }
    return GT;
  }

  // Returns a topological order of the graph
  // RunTim: O(V + E)
  std::stack<int> getTopoSort() {
    std::stack<int> stk;
    std::vector<bool> vis(V, 0);

    std::function<void(int)> buildTopo = [&](int u) {
      vis[u] = 1;
      for (int v : adj[u]) if (!vis[v]) buildTopo(v);
      stk.push(u);
    };
    
    for (int u = 0; u < V; ++u) if (!vis[u]) buildTopo(u);

    return stk;
  }

  // Returns (nb_of_SCCs, roots_of_SCCs)
  // RunTime: O(V + E)
  std::tuple<int, int*> getSCCRoots(std::stack<int>& stk) const {
    std::vector<bool> vis(V, 0);
    std::vector<int> component;
    int* roots = new int[V];
    int root_cnt = 0;

    Graph* GT = getTranspose();

    std::function<void(int)> buildGraph = [&](int u) {
      vis[u] = 1;
      component.push_back(u);
      for (int v : GT->adj[u]) if (!vis[v]) buildGraph(v);
    };

    while (!stk.empty()) {
      const int u = stk.top(); stk.pop();
      if (vis[u]) continue;

      buildGraph(u);

      int root = component.front();
      for (int u : component) roots[u] = root;
      ++root_cnt;

      component.clear();
    }

    return std::make_tuple(root_cnt, roots);
  }

  // Get a graph of the SCCs of G
  // RunTime: O(V + E)
  Graph* getMetaGraph() {
    std::stack<int> topoSort = this->getTopoSort();
    int *roots, root_cnt;
    std::tie(root_cnt, roots) = this->getSCCRoots(topoSort);

    Graph* GP = new Graph(root_cnt, 0);
    for (const auto& p : adj) for (int v : p.second) 
      if (roots[p.first] != roots[v]) {
        GP->adj[roots[v]];
        GP->adj[roots[p.first]].push_back(roots[v]);
        ++GP->E;
      }

    return GP;
  }

  // Returns a source SCC if found, -1 otherwise
  // RunTime: O(V + E)
  int getSourceSCC() const {
    std::unordered_map<int, int> in_deg;
    for (const auto& p : adj) {
      in_deg[p.first];
      for (int v : p.second) ++in_deg[v];
    }
    
    int source = -1;
    for (const auto& p : in_deg) {
      if (p.second == 0) {
        if (source != -1) return -1;
        source = p.first;
      }
    }
    return source;
  }

  // Returns true if we can visit the whole graph from src
  // false otherwise
  // RunTime: O(V + E)
  bool visits_all(int src) {
    std::unordered_map<int, bool> vis;
    for (const auto& p : adj) vis[p.first] = 0;
    int cnt_vis = 0;

    std::function<void(int)> visit = [&] (int u) {
      ++cnt_vis; vis[u] = 1;
      for (int v : adj[u]) if (!vis[v]) visit(v);
    };

    visit(src);

    return V == cnt_vis;
  }

  friend std::ostream& operator << (std::ostream& out, const Graph G) {
    for (const auto& p : G.adj) {
      out << p.first << ": ";
      for (int v : p.second) out << v << ' ';
      out << '\n';
    }
    return out;
  }
};

Graph* read_initial_graph() {
  std::fstream fin{ "ex4.in" };

  int V, E; fin >> V >> E;
  Graph* G = new Graph(V, E);
  
  int u, v;
  for (int i = 0; i < G->E; ++i) {
    fin >> u >> v;
    G->adj[u].push_back(v);
  }

  return G;
}

// Returns a viewpoint if found, -1 otherwise
// RunTime: O(V + E)
int getViewPoint(Graph* G) {
  Graph* MetaG = G->getMetaGraph();

  int src_SCC = MetaG->getSourceSCC();

  if (src_SCC == -1 || !G->visits_all(src_SCC)) {
    std::cout << "No Viewpoint Exists in the graph G.";
    return -1;
  }

  std::cout << "Found ViewPoint: " << src_SCC << '\n';
  return src_SCC;
}

int main() {
  Graph* G = read_initial_graph();
  getViewPoint(G);
}
