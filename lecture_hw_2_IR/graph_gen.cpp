#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct Node {
  std::string id;
  std::string label;
};

struct Edge {
  std::string from;
  std::string to;
  std::string label;
};

std::string escape_dot(const std::string &s) {
  auto is_graphviz_escape = [](char c) {
    return c == 'l' || c == 'n' || c == 'r';
  };

  std::string res;
  res.reserve(s.size() * 2);
  for (size_t i = 0; i < s.size(); ++i) {
    switch (s[i]) {
    case '\n':
      res += "\\l";
      break;
    case '"':
      res += "\\\"";
      break;
    case '\\':
      res += (i + 1 < s.size() && is_graphviz_escape(s[i + 1])) ? "\\" : "\\\\";
      break;
    default:
      res += s[i];
      break;
    }
  }
  return res;
}

void print_dot(const std::vector<Node> &nodes, const std::vector<Edge> &edges) {
  std::cout << "digraph CFG {\n";
  std::cout << "    rankdir=TB;\n";
  std::cout << "    node [shape=box, fontname=\"Courier New\"];\n";
  std::cout << "    edge [fontname=\"Courier New\"];\n\n";

  for (const auto &node : nodes) {
    std::cout << "    " << node.id << " [label=\"" << escape_dot(node.label)
              << "\"];\n";
  }

  std::cout << "\n";

  for (const auto &edge : edges) {
    std::cout << "    " << edge.from << " -> " << edge.to;
    if (!edge.label.empty()) {
      std::cout << " [label=\"" << escape_dot(edge.label) << "\"]";
    }
    std::cout << ";\n";
  }

  std::cout << "}\n";
}

std::vector<Node> make_basic_blocks() {
    return {
      
    {"bb0", R"(bb0:
        result0 = 0;
        i0 = 0;
        br bb1;
    )"},
      
    {"bb1", R"(bb1:
        result1 = phi(result0:bb0, result3:bb7);
        i1 = phi(i0:bb0, i2:bb7);
        c0 = i1 < n;
        br c0, bb2, bb8;
        )"},
      {"bb2", R"(bb2:
        x0 = 5;
        j0 = 0;
        br bb3;
        )"},
      
    {"bb3", R"(bb3:
        result2 = phi(result1:bb2, result5:bb5, result4:bb6);
        j1 = phi(j0:bb2, j2:bb5, j3:bb6);
        c1 = j1 < n;
        br c1, bb4, bb7;
        )"},
      
    {"bb4", R"(bb4:
        t0 = j1 % 2;
        c2 = t0 == 0;
        br c2, bb5, bb6;
        )"},
    
    {"bb5", R"(bb5:
        result5 = result2;
        j2 = j1 + 1;
        br bb3;
        )"},
    
    {"bb6", R"(bb6:
        addr0 = a + j1;
        val0 = *addr0;
        t1 = 2 * 3;
        t2 = t1 * val0;
        t3 = t2 * x0;
        t4 = t3 - i1;
        t5 = t4 - 2;
        result4 = result2 + t5;
        j3 = j1 + 1;
        br bb3;
        )"},
    {"bb7", R"(bb7:
        result3 = result2;
        i2 = i1 + 1;
        br bb1;
        )"},
      
    {"bb8", R"(bb8:
        return result1;
        )"}};
}

std::vector<Edge> make_cfg_edges() {
  return {
      {"bb0", "bb1", ""},     {"bb1", "bb2", "true"},  {"bb1", "bb8", "false"},
      {"bb2", "bb3", ""},     {"bb3", "bb4", "true"},  {"bb3", "bb7", "false"},
      {"bb4", "bb5", "true"}, {"bb4", "bb6", "false"}, {"bb5", "bb3", ""},
      {"bb6", "bb3", ""},     {"bb7", "bb1", ""}};
}

std::vector<Edge> make_dom_tree_edges() {
  return {
      {"bb0", "bb1", ""}, {"bb1", "bb2", ""}, {"bb1", "bb8", ""},
      {"bb2", "bb3", ""}, {"bb3", "bb4", ""}, {"bb3", "bb7", ""},
      {"bb4", "bb5", ""}, {"bb4", "bb6", ""}};
}

std::vector<Edge> compute_dom_frontier_edges(const std::vector<Node> &nodes,
                                             const std::vector<Edge> &cfg_edges,
                                             const std::vector<Edge> &dom_edges) {
  std::unordered_map<std::string, std::string> idom;
  for (const auto &edge : dom_edges) {
    idom[edge.to] = edge.from;
  }

  std::unordered_map<std::string, std::vector<std::string>> preds;
  for (const auto &edge : cfg_edges) {
    preds[edge.to].push_back(edge.from);
  }

  std::unordered_map<std::string, std::set<std::string>> frontier;
  for (const auto &node : nodes) {
    auto it_preds = preds.find(node.id);
    if (it_preds == preds.end() || it_preds->second.size() < 2) {
      continue;
    }

    auto it_idom = idom.find(node.id);
    if (it_idom == idom.end()) {
      continue;
    }

    const std::string &stop = it_idom->second;
    for (const auto &pred : it_preds->second) {
      std::string runner = pred;
      while (runner != stop) {
        frontier[runner].insert(node.id);

        auto it_runner_idom = idom.find(runner);
        if (it_runner_idom == idom.end()) {
          break;
        }
        runner = it_runner_idom->second;
      }
    }
  }

  std::vector<Edge> edges;
  for (const auto &node : nodes) {
    auto it = frontier.find(node.id);
    if (it == frontier.end()) {
      continue;
    }
    for (const auto &to : it->second) {
      edges.push_back({node.id, to, ""});
    }
  }
  return edges;
}

int gen_cfg() {
    std::vector<Node> nodes = make_basic_blocks();
    std::vector<Edge> edges = make_cfg_edges();

    std::ofstream fout("ssa_ir_cfg.dot");
    if (!fout) {
      std::cerr << "cannot open output file\n";
      return 1;
    }

    auto *old_buf = std::cout.rdbuf(fout.rdbuf());
    print_dot(nodes, edges);
    std::cout.rdbuf(old_buf);

    return 0;
}

int gen_dom_tree() {
    std::vector<Node> nodes = make_basic_blocks();
    std::vector<Edge> dom_edges = make_dom_tree_edges();

    std::ofstream fout("dominator_tree.dot");
    if (!fout) {
        std::cerr << "cannot open output file\n";
        return 1;
    }

    fout << "digraph DominatorTree {\n";
    fout << "    rankdir=TB;\n";
    fout << "    node [shape=box, style=\"rounded\", fontname=\"Courier New\"];\n";
    fout << "    edge [fontname=\"Courier New\"];\n\n";

    for (const auto &node : nodes) {
        fout << "    " << node.id << " [label=\"" << escape_dot(node.label)
             << "\"];\n";
    }
    fout << "\n";

    for (const auto &edge : dom_edges) {
        fout << "    " << edge.from << " -> " << edge.to << ";\n";
    }

    fout << "}\n";

    return 0;
}

int gen_dom_frontier() {
    std::vector<Node> nodes = make_basic_blocks();
    std::vector<Edge> frontier_edges =
        compute_dom_frontier_edges(nodes, make_cfg_edges(), make_dom_tree_edges());

    std::ofstream fout("dominance_frontier.dot");
    if (!fout) {
        std::cerr << "cannot open output file\n";
        return 1;
    }

    fout << "digraph DominanceFrontier {\n";
    fout << "    rankdir=TB;\n";
    fout << "    node [shape=box, style=\"rounded\", fontname=\"Courier New\"];\n";
    fout << "    edge [fontname=\"Courier New\", color=\"#2f6db3\"];\n\n";

    for (const auto &node : nodes) {
        fout << "    " << node.id << " [label=\"" << escape_dot(node.label)
             << "\"];\n";
    }
    fout << "\n";

    for (const auto &edge : frontier_edges) {
        fout << "    " << edge.from << " -> " << edge.to << ";\n";
    }

    fout << "}\n";
    return 0;
}

void print_usage(const char *prog) {
    std::cerr << "Usage: " << prog
              << " [--gen-cfg] [--gen-dom-tree] [--gen-dom-frontier]\n";
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    int exit_code = 0;

    for (int i = 1; i < argc; i++) {
        std::string_view arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        }

        if (arg == "--gen-cfg") {
            exit_code |= gen_cfg();
            continue;
        }

        if (arg == "--gen-dom-tree") {
            exit_code |= gen_dom_tree();
            continue;
        }

        if (arg == "--gen-dom-frontier") {
            exit_code |= gen_dom_frontier();
            continue;
        }

        std::cerr << "Unknown flag: " << arg << '\n';
        print_usage(argv[0]);
        return 1;
    }

    return exit_code;
}
