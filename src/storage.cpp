#include "vector.hpp"
#include <iostream>
#include <fstream>

constexpr int MAX_KEY_SIZE = 64;
constexpr int BLOCK_SIZE = 4000;
constexpr int INVALID_ADDR = -1;
constexpr int MAX_KEY_PER_NODE = 100;

//BPT
enum NodeType { LEAF, INTERNAL };

struct KeyValue {
  char key[MAX_KEY_SIZE];
  int value;

  bool operator<(const KeyValue &other) const {
    int cmp = strcmp(key, other.key);
    return cmp < 0 || (cmp == 0 && value < other.value);
  }
};

struct Node {
  NodeType type;
  int num_entries;
  KeyValue entries[MAX_KEY_PER_NODE];
  int children[MAX_KEY_SIZE + 1]; //internal - children
  int next; // leaf
  int parent;
  int self_addr; //address

  Node() : type(LEAF), num_entries(0), next(-1), parent(INVALID_ADDR), self_addr(INVALID_ADDR) {
    memset(entries, 0, sizeof(entries));
    memset(children, INVALID_ADDR, sizeof(children));
  }

  int find_pos(const KeyValue &key_value) const {
    int l = 0, r = num_entries - 1, mid;
    while (l <= r) {
      mid = (l + r) / 2;
      if (entries[mid] < key_value) l = mid + 1;
      else r = mid - 1;
    }
    return l;
  }

  int find_key_pos(const char *key) const {
    int l = 0, r = num_entries - 1, mid, cmp;
    while (l <= r) {
      mid = (l + r) / 2;
      cmp = strcmp(key, entries[mid].key); /*
      if (cmp == 0) {
        res = mid;
        r = mid - 1;
      } else if (cmp < 0) {
        r = mid - 1;
      } else {
        l = mid + 1;
      }*/
      if (cmp <= 0) {
        r = mid - 1;
      } else {
        l = mid + 1;
      }
    }
    return l;
  }
};

class BPlusTree {
private:
  std::fstream file;
  std::string filename;
  int root_addr;
  int first_leaf_addr;

  Node read_node(int addr) {
    Node node;
    if (addr == INVALID_ADDR) return node;
    file.seekg(addr);
    file.read(reinterpret_cast<char *>(&node), sizeof(Node));
    return node;
  }

  void write_node(Node &node) {
    if (node.self_addr == INVALID_ADDR) return;
    file.seekp(node.self_addr);
    file.write(reinterpret_cast<char *>(&node), sizeof(Node));
  }

  int allocate_node() {
    file.seekp(0, std::ios::end);
    int addr = file.tellp();
    Node new_node;
    new_node.self_addr = addr;
    write_node(new_node);
    return addr;
  }

  int find_leaf(const char *key) {
    if (root_addr == INVALID_ADDR) return INVALID_ADDR;
    Node node = read_node(root_addr);
    int pos;
    while (node.type != LEAF) {
      pos = node.find_key_pos(key);
      if (pos >= node.num_entries) pos = node.num_entries - 1;
      node = read_node(node.children[pos]);
    }
    return node.self_addr;
  }

  void insert_in_parent(Node &l, Node &r, const KeyValue &key) {
    if (l.parent == INVALID_ADDR) {
      //add new root
      Node new_root;
      new_root.type = INTERNAL;
      new_root.self_addr = allocate_node();
      new_root.entries[0] = key;
      new_root.children[0] = l.self_addr;
      new_root.children[1] = r.self_addr;
      new_root.num_entries = 1;

      l.parent = r.parent = new_root.self_addr;

      write_node(l);
      write_node(r);
      write_node(new_root);

      root_addr = new_root.self_addr;
      return;
    }
    Node parent = read_node(l.parent);
    int pos = parent.find_pos(key);

    for (int i = parent.num_entries; i > pos; --i) {
      parent.entries[i] = parent.entries[i - 1];
      parent.children[i + 1] = parent.children[i];
    }
    parent.entries[pos] = key;
    parent.children[pos + 1] = r.self_addr;
    ++parent.num_entries;

    r.parent = parent.self_addr;
    write_node(r);
    if (parent.num_entries < MAX_KEY_PER_NODE) {
      write_node(parent);
    } else {
      // split parent
      Node new_node;
      new_node.type = INTERNAL;
      new_node.self_addr = allocate_node();
      new_node.parent = parent.parent;

      int split_pos = parent.num_entries / 2;
      KeyValue promote_key = parent.entries[split_pos];

      //copy node
      for (int i = split_pos + 1, j = 0; i < parent.num_entries; ++i, ++j) {
        new_node.entries[j] = parent.entries[i];
        new_node.children[j] = parent.children[i];
        Node child = read_node(parent.children[i]);
        child.parent = new_node.self_addr;
        write_node(child);
      }
      new_node.children[parent.num_entries - split_pos - 1] = parent.children[parent.num_entries];
      Node last_child = read_node(parent.children[parent.num_entries]);
      last_child.parent = new_node.self_addr;
      write_node(last_child);

      new_node.num_entries = parent.num_entries - split_pos - 1;
      parent.num_entries = split_pos;

      write_node(parent);
      write_node(new_node);

      insert_in_parent(parent, new_node, promote_key);
    }
  }

public:
  BPlusTree(const std::string &fname) : filename(fname) {
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
      file.open(filename, std::ios::out | std::ios::binary);
      file.close();
      file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

      root_addr = INVALID_ADDR;
      first_leaf_addr = INVALID_ADDR;

      file.seekp(0);
      file.write(reinterpret_cast<char *>(&root_addr), sizeof(root_addr));
      file.write(reinterpret_cast<char *>(&first_leaf_addr), sizeof(first_leaf_addr));
    } else {
      file.seekg(0);
      file.read(reinterpret_cast<char *>(&root_addr), sizeof(root_addr));
      file.read(reinterpret_cast<char *>(&first_leaf_addr), sizeof(first_leaf_addr));
    }
  }

  ~BPlusTree() {
    file.seekp(0);
    file.write(reinterpret_cast<char *>(&root_addr), sizeof(root_addr));
    file.write(reinterpret_cast<char *>(&first_leaf_addr), sizeof(first_leaf_addr));
    file.close();
  }

  void insert(const char *key, int value) {
    KeyValue key_value;
    strncpy(key_value.key, key, MAX_KEY_SIZE);
    key_value.value = value;

    if (root_addr == INVALID_ADDR) {
      // 1. null -> create root(=leaf)
      Node new_node;
      new_node.self_addr = allocate_node();
      new_node.entries[0] = key_value;
      new_node.num_entries = 1;

      root_addr = new_node.self_addr;
      first_leaf_addr = new_node.self_addr;

      write_node(new_node);
      return;
    }

    int leaf_addr = find_leaf(key);
    Node leaf = read_node(leaf_addr);

    int pos = leaf.find_pos(key_value);

    // check if it has exist
    if (pos < leaf.num_entries && strcmp(leaf.entries[pos].key, key) == 0 && leaf.entries[pos].value == value) {
      return;
    }

    // insert
    for (int i = leaf.num_entries; i > pos; --i) {
      leaf.entries[i] = leaf.entries[i - 1];
    }
    leaf.entries[pos] = key_value;
    ++leaf.num_entries;

    //write
    if (leaf.num_entries <= MAX_KEY_PER_NODE) {
      write_node(leaf);
    } else {
      // split
      Node new_leaf;
      new_leaf.type = LEAF;
      new_leaf.self_addr = allocate_node();
      new_leaf.parent = leaf.parent;
      new_leaf.next = leaf.next;

      int split_pos = leaf.num_entries / 2;
      for (int i = split_pos, j = 0; i < leaf.num_entries; ++i, ++j) {
        new_leaf.entries[j] = leaf.entries[i];
      }
      new_leaf.num_entries = leaf.num_entries - split_pos;
      leaf.num_entries = split_pos;
      leaf.next = new_leaf.self_addr;

      write_node(leaf);
      write_node(new_leaf);

      KeyValue promote_key = new_leaf.entries[0];
      insert_in_parent(leaf, new_leaf, promote_key);
    }
  }

  void remove(const char *key, int value) {
    if (root_addr == INVALID_ADDR) return;

    KeyValue key_value;
    strncpy(key_value.key, key, MAX_KEY_SIZE);
    key_value.value = value;

    int leaf_addr = find_leaf(key);
    Node leaf = read_node(leaf_addr);

    int pos = leaf.find_pos(key_value);
    if (pos >= leaf.num_entries || strcmp(leaf.entries[pos].key, key) != 00 || leaf.entries[pos].value != value) {
      return; //not found
    }

    //remove
    for (int i = pos; i < leaf.num_entries - 1; ++i) {
      leaf.entries[i] = leaf.entries[i + 1];
    }
    --leaf.num_entries;
    write_node(leaf);
  }

  sjtu::vector<int> find(const char *key) {
    sjtu::vector<int> result;
    if (root_addr == INVALID_ADDR) return result;

    int leaf_addr = find_leaf(key);
    Node leaf = read_node(leaf_addr);
    int pos = leaf.find_key_pos(key);

    while (true) {
      for (int i = pos; i < leaf.num_entries; ++i) {
        if (strcmp(leaf.entries[i].key, key) == 0) {
          result.push_back(leaf.entries[i].value);
        } else {
          break;
        }
      }

      // check if next node needs search
      if (leaf.next == INVALID_ADDR ||  strcmp(leaf.entries[leaf.num_entries - 1].key, key) != 0) {
        break;
      }

      leaf = read_node(leaf.next);
      pos = 0;
    }
    return result;
  }
};

int main() {
  BPlusTree bpt("bpt.txt");
  int n, value;
  std::string cmd, key;
  std::cin >> n;

  for (int i = 0; i <n; ++i) {
    std::cin >> cmd;

    if (cmd == "insert") {
      std::cin >> key >> value;
      bpt.insert(key.c_str(), value);
    } else if (cmd == "delete") {
      std::cin >> key >> value;
      bpt.remove(key.c_str(), value);
    } else if (cmd == "find") {
      std::cin >> key;
      sjtu::vector<int> result = bpt.find(key.c_str());

      if (result.empty()) {
        std::cout << "null";
      } else {
        for (int j : result) {
          std::cout << j << ' ';
        }
      }
      std::cout << '\n';
    }
  }
  return 0;
}