/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tglory <tglory@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/14 15:36:17 by allanganoun       #+#    #+#             */
/*   Updated: 2022/03/21 15:20:13 by tglory           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <memory>
#include <algorithm>
#include <iostream>
#include "tree_iterator.hpp"
#include "../vector.hpp"

namespace ft
{
	template <class Value, class Key, class Compare = std::less<Key>, class Value_alloc = std::allocator<Value>
				, class Node = ft::_node<Value>, class Node_alloc = std::allocator<Node> >
	class tree
	{
	public :
		typedef Value							data_type;
		typedef Node_alloc						node_alloc;
		typedef Node							node_type;
		typedef Node *							node_pointer;
		typedef typename node_alloc::size_type	size_type;
		typedef ft::tree_iterator<node_type>	iterator;

		tree(const node_alloc &alloc = node_alloc(), const Compare &comp = Compare() )
		: _end(NULL), _start(NULL), _root(NULL), _node_alloc(alloc), _comp(comp), _size(0)
		{

		}

		~tree()
		{
			clear();
		}

		pair<iterator,bool> insert(const data_type& val) // iterateur sur la valeur insérée + True pour dire valeur ajoutée ou false pour déjà éxistante
		{
			node_pointer new_node;
			node_pointer current;
			node_pointer tmp;

			new_node = _node_alloc.allocate(1);
			if (_root == NULL)
			{
				_node_alloc.construct(new_node, Node(val));
				_root = new_node;
				++_size;
				return (make_pair(iterator(_root), true));
			}
			iterator find_it = find(val.first);
			if (find_it != iterator(_end))
			{
				return make_pair(find_it, false);
			}
			current = _node_min(_root);
			while (_comp(current->data.first, val.first))
			{
				tmp = _node_next(current);
				if (tmp != NULL)
					current = tmp;
				else
					break;
			}
			if (current->data.first == val.first)
				return make_pair(iterator(current), false);
			while (current !=_start && !_node_has_leaf(current))
				current = _node_prev(current);
			if (this->_comp(current->data.first, val.first))
			{
				_node_alloc.construct(new_node, Node(val));
				current->right = new_node;
				new_node->parent = current;
			}
			else
			{
				_node_alloc.construct(new_node, Node(val));
				current->left = new_node;
				new_node->parent = current;
			}
			_start = _node_min(_root);
			++_size;
			return make_pair(iterator(new_node), true);
		}
		iterator insert (iterator position, const data_type& val)
		{}

		iterator begin()
		{
			if (_root)
				// return iterator(_node_min(_root));
				return iterator(_start);
			return iterator(_end);
		}

		iterator end()
		{
			return iterator(_end);
		}

		size_type size() const
		{
			return _size;
		}

		// clear degeux qui fonctionne avec vector
		void clear()
		{
			if (!_root)
				return;
			_deallocate(_root);
			//_iterator_all_under(_root, &tree::clear2);
			/*node_pointer tmp1 = _start, tmp2;
			while (tmp1 != NULL)
			{
				while (!_node_has_leaf(tmp1))
				{
					
				}
				//std::cout << "Hey" << std::endl;
				tmp2 = _node_next(tmp1);
				_node_alloc.deallocate(tmp1, 1);
				tmp1 = tmp2;
			}*/
			/*ft::vector<node_pointer> vector;

			node_pointer tmp1 = _start, tmp2;
			while (tmp1 != NULL)
			{
				tmp2 = _node_next(tmp1);
				vector.push_back(tmp1);
				//_node_alloc.deallocate(tmp1, 1);
				tmp1 = tmp2;
			}
			typename ft::vector<node_pointer>::iterator it = vector.begin();
			while (it != vector.end())
			{
				_node_alloc.deallocate(*it, 1);
				it++;
			}*/
			_size = 0;
			_start = NULL;
			_root = NULL;
		}

		iterator find(const Key& k)
		{
			node_pointer node = _start;
			while (node != NULL)
			{
				data_type pair = node->data;
				if (pair.first == k)
					return iterator(node);
				node = _node_next(node);
			}
			return iterator(_end);
		}
			
		size_type count(const Key& k) const
		{
			if (this->find(k) != iterator(_end))
				return 1;
			else
				return 0;
		}
		
		// A check
		size_type erase(const Key& k)
		{
			node_pointer node = _start;
			while (node != NULL)
			{
				data_type pair = node->data;
				if (pair.first == k)
					break;
				node = _node_next(node);
			}
			if (!node)
				return 0;
			_destroy(node);
			/*iterator it = find(k);
			node_pointer node;
			if (it == iterator(end()))
				return 0;
			// TODO Need to remove address in childrens & parents
			_node_alloc.deallocate(*it, 1);*/
		}
	
	private :

		void _destroy(node_type node)
		{
			if (_is_left_node(node))
			{
				node->parent->left = NULL;
			}
			else if (_is_right_node(node))
			{
				node->parent->right = NULL;
			}
			if (!node->right)
			{
				_destroy(node->right);
			}
			if (!node->left)
			{
				_destroy(node->left);
			}
			insert(node->value);
			_deallocate(node);
		}

		void _deallocate(node_pointer node) 
		{
			if (node->left)
				_deallocate(node->left);
			if (node->right)
				_deallocate(node->right);
			_node_alloc.deallocate(node, 1);
		}

		/**
		 * Based on https://stephane.glondu.net/projets/tipe/transparents.pdf#page=4
		 */
		void _rotateRight(node_pointer node)
		{
			node_pointer a = node->left;
			node_pointer b = node;
			node_pointer c = node->right;
			node_pointer d = node->parent;
			node_pointer e = node->parent->left;

			b->parent = d->parent;
			b->right = d;
			d->parent = b;
			d->left = c;
			c->parent = d;
		}

		void _rotateLeft(node_pointer node)
		{
			node_pointer a = node->parent->right;
			node_pointer b = node->parent;
			node_pointer c = node->left;
			node_pointer d = node;
			node_pointer e = node->right;

			d->parent = b->parent;
			d->left = b;
			b->parent = d;
			b->right = c;
			c->parent = b;
		}

		unsigned int _nodeHeigh(node_pointer node)
		{
			unsigned int i = 0;
			node_pointer tempNode;
			do {
				tempNode = node;
				++i;
			} while (tempNode->parent);
			return i;
		}

	private :
		node_pointer _end;
		node_pointer _start;
		node_pointer _root;
		node_alloc	_node_alloc;
		Compare		_comp;
		size_type	_size;
	};
}

template <typename T>
std::ostream &operator<<(std::ostream &outputFile, ft::_node<T> &node)
{
	outputFile << node->first << "=" << node->second;
	return outputFile;
}

template <typename T, typename U>
std::ostream &operator<<(std::ostream &outputFile, ft::pair<T, U> &pair)
{
	outputFile << pair->first << "=" << pair->second;
	return outputFile;
}
