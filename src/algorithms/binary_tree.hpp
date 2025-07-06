#pragma once

namespace feng {

	template<typename T>
	struct binary_tree_node {
		T data;
		binary_tree_node* left;
		binary_tree_node* right;

		binary_tree_node(T value)
			: data(value), left(nullptr), right(nullptr) {}

	};

	template<typename T>
	class binary_tree {
	public:
		binary_tree()
			: _root(nullptr) {}

		void insert(T value) {
			binary_tree_node<T>* node = new binary_tree_node<T>(value);

			if (!_root) {
				_root = node;
				return;
			}

			return insert(_root, node);
		}

		void remove(T value) {
			_root = remove(_root, value);
		}

		bool contains(T value) {
			return contains(_root, value);
		}

	private:
		binary_tree_node<T>* _root;

		void insert(binary_tree_node<T>* to_node, binary_tree_node<T>* v_node) {
			if (v_node->data < to_node->data) {
				if (to_node->left != nullptr) {
					return insert(to_node->left, v_node);
				}
				else {
					to_node->left = v_node;
					return;
				}
			}

			if (v_node->data > to_node->data) {
				if (to_node->right != nullptr) {
					return insert(to_node->right, v_node);
				}
				else {
					to_node->right = v_node;
					return;
				}
			}
		}

		binary_tree_node<T>* find_min(binary_tree_node<T>* node) {
			while (node->left != nullptr)
				node = node->left;
			return node;
		}

		binary_tree_node<T>* remove(binary_tree_node<T>* from_node, T value) {
			if (from_node == nullptr)
				return nullptr;

			if (value < from_node->data) {
				from_node->left = remove(from_node->left, value);
			}
			else if (value > from_node->data) {
				from_node->right = remove(from_node->right, value);
			}
			else {
				if (from_node->left == nullptr && from_node->right == nullptr) {
					delete from_node;
					return nullptr;
				}
				if (from_node->left == nullptr) {
					binary_tree_node<T>* temp = from_node->right;
					delete from_node;
					return temp;
				}
				if (from_node->right == nullptr) {
					binary_tree_node<T>* temp = from_node->left;
					delete from_node;
					return temp;
				}

				binary_tree_node<T>* mnode = find_min(from_node->right);
				from_node->data = mnode->data;
				from_node->right = remove(from_node->right, mnode->data);
			}
			return from_node;
		}

		bool contains(binary_tree_node<T>* node, T value) {
			if (node == nullptr) return false;
			if (node->data == value) return true;
			return contains(node->left, value) || contains(node->right, value);
		}

	};

}