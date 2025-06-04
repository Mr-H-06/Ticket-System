#ifndef BPT_BPLUS_TREE_HPP
#define BPT_BPLUS_TREE_HPP

#include "vector.hpp"
#include <iostream>
#include <fstream>

//constexpr int MAX_KEY_SIZE = 64;
constexpr int BLOCK_SIZE = 4096;
constexpr int INVALID_ADDR = -1;
//constexpr int MAX_KEY_PER_NODE = 170; //56
//constexpr int MIN_KEY_PER_NODE = MAX_KEY_PER_NODE / 2;

//BPT
enum NodeType { LEAF, INTERNAL };

template<typename T, size_t MAX_KEY_SIZE>
struct KeyValue {
  char key[MAX_KEY_SIZE];
  T value;

  bool operator<(const KeyValue &other) const {
    int cmp = strcmp(key, other.key);
    return cmp < 0 || (cmp == 0 && value < other.value);
  }
};

template<typename T,size_t MAX_KEY_SIZE, size_t MAX_KEY_PER_NODE>
struct Node {
  NodeType type;
  int num_entries;
  KeyValue<T, MAX_KEY_SIZE> entries[MAX_KEY_PER_NODE];
  int children[MAX_KEY_PER_NODE + 1]; //internal - children
  int next; // leaf
  int parent;
  int self_addr; //address

  Node() : type(LEAF), num_entries(0), next(-1), parent(INVALID_ADDR), self_addr(INVALID_ADDR) {
    memset(entries, 0, sizeof(entries));
    memset(children, INVALID_ADDR, sizeof(children));
  }

  int find_pos(const KeyValue<T, MAX_KEY_SIZE> &key_value) const {
    // find the minimun pos, key_value <= entries[pos]
    int l = 0, r = num_entries - 1, mid;
    while (l <= r) {
      mid = (l + r) / 2;
      if (entries[mid] < key_value) l = mid + 1;
      else r = mid - 1;
    }
    return l;
  }

  int find_key_pos(const char *key) const {
    // find the minimum pos, s.t. key <= entries[pos].key
    int l = 0, r = num_entries - 1, mid;
    while (l <= r) {
      mid = (l + r) / 2;
      if (strcmp(entries[mid].key, key) < 0) {
        l = mid + 1;
      } else {
        r = mid - 1;
      }
    }
    return l;
  }
};

template<typename T, size_t MAX_KEY_SIZE, size_t MAX_KEY_PER_NODE>
class BPlusTree {
private:
  const int MIN_KEY_PER_NODE = MAX_KEY_PER_NODE >> 1;
  std::fstream file;
  std::string filename;
  int root_addr;
  int first_leaf_addr;
  Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> root;

  void read_node(int addr, Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> &node) {
    if (addr == INVALID_ADDR) return; /*
    if (addr == root_addr) {
      node = root;
    }*/

    file.seekg(addr);
    file.read(reinterpret_cast<char *>(&node), sizeof(Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE>));
  }

  void write_node(Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> &node) {
    if (node.self_addr == INVALID_ADDR) return;
    file.seekp(node.self_addr);
    file.write(reinterpret_cast<char *>(&node), sizeof(Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> ));
  }

  int allocate_node() {
    file.seekp(0, std::ios::end);
    int addr = file.tellp();
    Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> new_node;
    new_node.self_addr = addr;
    write_node(new_node);
    return addr;
  }

  int find_leaf(const char *key) {
    if (root_addr == INVALID_ADDR) return INVALID_ADDR;
    Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> node = root;
    int pos;
    while (node.type != LEAF) {
      pos = node.find_key_pos(key);
      read_node(node.children[pos], node);
    }
    return node.self_addr;
  }

  int find_leaf(const KeyValue<T, MAX_KEY_SIZE> &key) {
    if (root_addr == INVALID_ADDR) return INVALID_ADDR;
    Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> node = root;
    int pos;
    while (node.type != LEAF) {
      pos = node.find_pos(key);
      read_node(node.children[pos], node);
    }
    return node.self_addr;
  }

  void insert_in_parent(Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> &l, Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> &r, const KeyValue<T, MAX_KEY_SIZE> &key) {
    if (l.parent == INVALID_ADDR) {
      //add new root
      Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> new_root;
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
      root = new_root;
      return;
    }
    Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> parent;
    read_node(l.parent, parent);
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
      if (parent.self_addr == root_addr) {
        root = parent;
      }
      write_node(parent);
    } else {
      // split parent
      Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> new_node;
      new_node.type = INTERNAL;
      new_node.self_addr = allocate_node();
      new_node.parent = parent.parent;

      int split_pos = parent.num_entries / 2;
      KeyValue<T, MAX_KEY_SIZE> promote_key = parent.entries[split_pos];

      //copy node
      Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> child;
      for (int i = split_pos + 1, j = 0; i < parent.num_entries; ++i, ++j) {
        new_node.entries[j] = parent.entries[i];
        new_node.children[j] = parent.children[i];
        read_node(parent.children[i], child);
        child.parent = new_node.self_addr;
        write_node(child);
      }
      new_node.children[parent.num_entries - split_pos - 1] = parent.children[parent.num_entries];
      read_node(parent.children[parent.num_entries], child);
      child.parent = new_node.self_addr;
      write_node(child);

      new_node.num_entries = parent.num_entries - split_pos - 1;
      parent.num_entries = split_pos;

      write_node(parent);
      write_node(new_node);

      insert_in_parent(parent, new_node, promote_key);
    }
  }

  void handle_merge(Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> &node) {
    if (node.num_entries >= MIN_KEY_PER_NODE || node.parent == INVALID_ADDR) {
      if (node.self_addr == root_addr) {
        root = node;
      }
      write_node(node);
      return;
    }

    Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> parent;
    read_node(node.parent, parent);
    int pos = parent.find_pos(node.entries[node.num_entries - 1]);
    if (parent.children[pos] != node.self_addr) {
      ++pos;
    }

    //  borrow from left
    if (pos > 0) {
      Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> left;
      read_node(parent.children[pos - 1], left);
      if (left.num_entries > MIN_KEY_PER_NODE) {
        // left is enough
        if (node.type == LEAF) {
          // leaf
          for (int i = node.num_entries; i > 0; --i) {
            node.entries[i] = node.entries[i - 1];
          }
          node.entries[0] = left.entries[left.num_entries - 1];
          --left.num_entries;
          ++node.num_entries;
          parent.entries[pos - 1] = node.entries[0];
        } else {
          // internal
          node.children[node.num_entries + 1] = node.children[node.num_entries];
          for (int i = node.num_entries; i > 0; --i) {
            node.entries[i] = node.entries[i - 1];
            node.children[i] = node.children[i - 1];
          }

          node.entries[0] = parent.entries[pos - 1];
          node.children[0] = left.children[left.num_entries];

          parent.entries[pos - 1] = left.entries[left.num_entries - 1];

          //if (node.children[0] != INVALID_ADDR) {
          Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> child;
          read_node(node.children[0], child);
          child.parent = node.self_addr;
          write_node(child);
          //}

          --left.num_entries;
          node.num_entries++;
        }
        write_node(left);
        write_node(node);
        if (parent.self_addr == root_addr) {
          root = parent;
        }
        write_node(parent);
        return;
      }
    }

    //borrow from right
    if (pos < parent.num_entries) {
      Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> right;
      read_node(parent.children[pos + 1], right);
      if (right.num_entries > MIN_KEY_PER_NODE) {
        // right is enough
        if (node.type == LEAF) {
          // leaf
          node.entries[node.num_entries] = right.entries[0];
          for (int i = 0; i < right.num_entries - 1; ++i) {
            right.entries[i] = right.entries[i + 1];
          }
          ++node.num_entries;
          --right.num_entries;

          parent.entries[pos] = right.entries[0];
        } else {
          // internal
          node.entries[node.num_entries] = parent.entries[pos];
          node.children[node.num_entries + 1] = right.children[0];

          parent.entries[pos] = right.entries[0];
          for (int i = 0; i < right.num_entries - 1; ++i) {
            right.entries[i] = right.entries[i + 1];
            right.children[i] = right.children[i + 1];
          }
          right.children[right.num_entries - 1] = right.children[right.num_entries];

          Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> child;
          read_node(node.children[node.num_entries + 1], child);
          child.parent = node.self_addr;
          write_node(child);

          ++node.num_entries;
          --right.num_entries;
        }
        write_node(right);
        write_node(node);
        if (parent.self_addr == root_addr) {
          root = parent;
        }
        write_node(parent);
        return;
      }
    }

    // merge from left
    if (pos > 0) {
      Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> left;
      read_node(parent.children[pos - 1], left);

      if (node.type == LEAF) {
        // leaf
        for (int i = 0; i < node.num_entries; ++i) {
          left.entries[left.num_entries + i] = node.entries[i];
        }
        left.num_entries += node.num_entries;
        left.next = node.next;
      } else {
        // internal
        left.entries[left.num_entries] = parent.entries[pos - 1];

        Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> child;
        for (int i = 0; i < node.num_entries; ++i) {
          left.entries[left.num_entries + 1 + i] = node.entries[i];
          left.children[left.num_entries + 1 + i] = node.children[i];

          read_node(node.children[i], child);
          child.parent = left.self_addr;
          write_node(child);
        }
        left.children[left.num_entries + 1 + node.num_entries] = node.children[node.num_entries];
        read_node(node.children[node.num_entries], child);
        child.parent = left.self_addr;
        write_node(child);

        left.num_entries += node.num_entries + 1;
      }

      for (int i = pos - 1; i < parent.num_entries - 1; ++i) {
        parent.entries[i] = parent.entries[i + 1];
        parent.children[i + 1] = parent.children[i + 2];
      }
      --parent.num_entries;


      // revise root
      if (parent.self_addr == root_addr && parent.num_entries == 0) {
        root_addr = left.self_addr;
        root = left;
        left.parent = INVALID_ADDR;
      }
      write_node(left);
      handle_merge(parent);
    } else if (pos < parent.num_entries) {
      Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> right;
      read_node(parent.children[pos + 1], right);

      if (node.type == LEAF) {
        //leaf

        for (int i = 0; i < right.num_entries; ++i) {
          node.entries[node.num_entries + i] = right.entries[i];
        }
        node.num_entries += right.num_entries;
        node.next = right.next;
      } else {
        //internal
        node.entries[node.num_entries] = parent.entries[pos];

        Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> child;
        for (int i = 0; i < right.num_entries; ++i) {
          node.entries[node.num_entries + 1 + i] = right.entries[i];
          node.children[node.num_entries + 1 + i] = right.children[i];

          //if (node.children[i] != INVALID_ADDR) {
          read_node(right.children[i], child);
          child.parent = node.self_addr;
          write_node(child);
          //}
        }
        node.children[node.num_entries + 1 + right.num_entries] = right.children[right.num_entries];
        read_node(right.children[right.num_entries], child);
        child.parent = node.self_addr;
        write_node(child);

        node.num_entries += right.num_entries + 1;
      }

      for (int i = pos; i < parent.num_entries - 1; ++i) {
        parent.entries[i] = parent.entries[i + 1];
        parent.children[i + 1] = parent.children[i + 2];
      }
      --parent.num_entries;

      //revise root
      if (parent.self_addr == root_addr && parent.num_entries == 0) {
        root_addr = node.self_addr;
        root = node;
        node.parent = INVALID_ADDR;
      }
      write_node(node);
      handle_merge(parent);
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
      file.seekg(root_addr);
      file.read(reinterpret_cast<char *>(&root), sizeof(root));
    }
  }

  void check() {
    int pos = first_leaf_addr;
    Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> node;
    while (pos != INVALID_ADDR) {
      read_node(pos, node);
      std::cout << node.num_entries << '\n';
      for (int k = 0; k < node.num_entries; ++k) {
        std::cout << node.entries[k].key << ' ' << node.entries[k].value << '\n';
      }
      pos = node.next;
    }
    std::cout << '\n';
    int addr[1000], head = 0, tail = 0;
    if (root_addr == INVALID_ADDR) return;
    addr[0] = root_addr;
    while (head <= tail) {
      pos = tail;
      for (int k = head; k <= pos; ++k) {
        read_node(addr[k], node);
        for (int i = 0; i < node.num_entries; ++i) {
          if (node.type == INTERNAL) {
            addr[++tail] = node.children[i];
          }
          std::cout << node.entries[i].key << ' ' << node.entries[i].value << "    ";
        }
        if (node.type == INTERNAL) {
          addr[++tail] = node.children[node.num_entries];
        }
        std::cout << "        ";
      }
      head = pos + 1;
      std::cout << '\n';
    }
  }

  ~BPlusTree() {
    //check();
    file.seekp(0);
    file.write(reinterpret_cast<char *>(&root_addr), sizeof(root_addr));
    file.write(reinterpret_cast<char *>(&first_leaf_addr), sizeof(first_leaf_addr));
    file.seekp(root_addr);
    file.write(reinterpret_cast<char *>(&root), sizeof(root));
    file.close();
  }

  void insert(const char *key, T value) {
    KeyValue<T, MAX_KEY_SIZE> key_value;
    strcpy(key_value.key, key);
    key_value.value = value;

    if (root_addr == INVALID_ADDR) {
      // 1. null -> create root(=leaf)
      Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> new_node;
      new_node.self_addr = allocate_node();
      new_node.entries[0] = key_value;
      new_node.num_entries = 1;

      root_addr = new_node.self_addr;
      first_leaf_addr = new_node.self_addr;
      root = new_node;

      write_node(new_node);
      return;
    }

    int leaf_addr = find_leaf(key_value);
    Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> leaf;
    read_node(leaf_addr, leaf);

    int pos = leaf.find_pos(key_value);

    // check if it has existed
    if (pos < leaf.num_entries && strcmp(leaf.entries[pos].key, key) == 0 && leaf.entries[pos].value == value) {
      return;
    }
    if (pos == leaf.num_entries && leaf.next != INVALID_ADDR) {
      Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> node;
      read_node(leaf.next, node);
      if (strcmp(node.entries[0].key, key) == 0 && node.entries[0].value == value) {
        return;
      }
    }

    // insert
    for (int i = leaf.num_entries; i > pos; --i) {
      leaf.entries[i] = leaf.entries[i - 1];
    }
    leaf.entries[pos] = key_value;
    ++leaf.num_entries;

    //write
    if (leaf.num_entries < MAX_KEY_PER_NODE) {
      if (leaf_addr == root_addr) {
        root = leaf;
      }
      write_node(leaf);
    } else {
      // split
      Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> new_leaf;
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

      KeyValue<T, MAX_KEY_SIZE> promote_key = new_leaf.entries[0];
      insert_in_parent(leaf, new_leaf, promote_key);
    }
  }

  void remove(const char *key, T value) {
    if (root_addr == INVALID_ADDR) return;

    KeyValue<T, MAX_KEY_SIZE> key_value;
    strcpy(key_value.key, key);
    key_value.value = value;

    int leaf_addr = find_leaf(key_value);
    Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> leaf;
    read_node(leaf_addr, leaf);
    int pos = leaf.find_pos(key_value);

    if (pos < leaf.num_entries && (strcmp(leaf.entries[pos].key, key) != 0 || leaf.entries[pos].value != value)) {
      return; //not found
    }
    if (pos == leaf.num_entries) {
      if (leaf.next == INVALID_ADDR) {
        return;
      }
      read_node(leaf.next, leaf);
      pos = 0;
      if (strcmp(leaf.entries[0].key, key) != 0 || leaf.entries[pos].value != value) {
        return;
      }
    }

    //remove
    for (int i = pos; i < leaf.num_entries - 1; ++i) {
      leaf.entries[i] = leaf.entries[i + 1];
    }
    --leaf.num_entries;
    handle_merge(leaf);
  }

  sjtu::vector<T> find(const char *key) {
    sjtu::vector<T> result;
    if (root_addr == INVALID_ADDR) return result;

    int leaf_addr = find_leaf(key);
    Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE> leaf;
    read_node(leaf_addr, leaf);
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
      if (leaf.next == INVALID_ADDR || (pos != leaf.num_entries && strcmp(leaf.entries[leaf.num_entries - 1].key, key)
                                        != 0)) {
        break;
      }

      read_node(leaf.next, leaf);
      pos = 0;
    }
    return result;
  }

  bool empty() {
    return root_addr == INVALID_ADDR;
  }

  void clear() {
    root_addr = INVALID_ADDR;
    first_leaf_addr = INVALID_ADDR;
    root = Node<T, MAX_KEY_SIZE, MAX_KEY_PER_NODE>();

    file.close();
    file.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
    file.close();
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    file.seekp(0);
    file.write(reinterpret_cast<char*>(&root_addr), sizeof(root_addr));
    file.write(reinterpret_cast<char*>(&first_leaf_addr), sizeof(first_leaf_addr));
  }
};

/*int main() {
  BPlusTree<int, 64, 170> bpt("bpt.txt");
  int n, value;
  std::string cmd, key;
  std::cin >> n;

  for (int i = 0; i < n; ++i) {
    std::cin >> cmd;

    if (cmd == "insert") {
      std::cin >> key >> value;
      bpt.insert(key.c_str(), value);
    } else if (cmd == "delete") {
      std::cin >> key >> value;
      bpt.remove(key.c_str(), value);
    } else if (cmd == "find") {
      //bpt.check();
      std::cin >> key;
      sjtu::vector<int> result = bpt.find(key.c_str());

      if (result.empty()) {
        std::cout << "null";
      } else {
        for (int j: result) {
          std::cout << j << ' ';
        }
      }
      std::cout << '\n';
    } else {
      return 0;
    }
  }
  return 0;
}*/

#endif