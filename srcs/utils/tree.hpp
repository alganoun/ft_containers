/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tglory <tglory@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/14 15:36:17 by allanganoun       #+#    #+#             */
/*   Updated: 2022/03/24 18:08:07 by tglory           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <memory>
#include <algorithm>
#include <iostream>
#include <limits>
//#include "reverse_iterator.hpp"
#include "tree_iterator.hpp"
#include "../vector.hpp"

namespace ft
{
	template <class Value, class Key, class Compare, class Value_alloc = std::allocator<Value>
				, class Node = ft::_node<Value>, class Node_alloc = std::allocator<Node> >
	class tree
	{
	public :
		typedef Value											data_type;
		typedef Node_alloc										node_alloc;
		typedef Node											node_type;
		typedef Node *											node_pointer;
		typedef typename node_alloc::size_type					size_type;
		typedef ft::tree_iterator<node_type>					iterator;
		typedef ft::const_tree_iterator<node_type>				const_iterator;
		typedef ft::reverse_tree_iterator<node_type>			reverse_iterator;
		typedef ft::const_reverse_tree_iterator<node_type>		const_reverse_iterator;
		typedef typename Value_alloc::difference_type 			difference_type;

		tree(const node_alloc &alloc = node_alloc(), const Compare &comp = Compare() )
		: _end(NULL), _start(NULL), _root(NULL), _node_alloc(alloc), _comp(comp), _size(0)
		{

		}

		~tree()
		{
			clear();
			_root = NULL;
			_start = NULL;
			_end = NULL;
			_size = 0;
			
		}

		pair<iterator,bool> insert(const data_type& val) // iterateur sur la valeur insérée + True pour dire valeur ajoutée ou false pour déjà éxistante
		{
			node_pointer new_node;
			node_pointer current;

			if (_root == NULL)
			{
				new_node = _node_alloc.allocate(1);
				_node_alloc.construct(new_node, Node(val));
				_root = new_node;
				_start = _root;
				++_size;
				return (ft::make_pair(iterator(_root), true));
			}
			current = _root;
			while (current != NULL)
			{
				if (_comp(val.first, current->data.first) && current->left != NULL)
					current = current->left;
				else if (current->data.first == val.first)
					return ft::make_pair(iterator(current), false);
				else if (_comp(current->data.first, val.first) && current->right != NULL)
					current = current->right;
				else
					break;
			}
			new_node = _node_alloc.allocate(1);
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
			return ft::make_pair(iterator(new_node), true);
		}

		iterator insert (iterator position, const data_type& val)
		{
			(void)position;
			return(insert(val).first);
		}

		template <class InputIterator>
  		void insert (InputIterator first, InputIterator last)
		{
			for(InputIterator ite = first ; ite != last ; ite++)
				insert((*ite));
		}

		iterator begin()
		{
			if (_root)
				return iterator(_start);
			return NULL;
		}

		const_iterator begin() const
		{
			if (_root)
				return const_iterator(_start);
			return NULL;
		}

		iterator end()
		{
			return NULL;
		}

		const_iterator end() const
		{
			return NULL;
		}

		reverse_iterator rbegin()
		{
			if (_root)
				return reverse_iterator(_node_max(_root));
			return NULL;
		}

		reverse_iterator rend()
		{
			return reverse_iterator(_end);
		}

		const_reverse_iterator rbegin() const
		{
			if (_root)
				return const_reverse_iterator(_node_max(_root));
			return NULL;
		}

		const_reverse_iterator rend() const
		{
			return reverse_iterator(_end);
		}

		size_type size() const
		{
			return _size;
		}

		bool empty() const
		{
			return size() == 0;
		}

		void clear()
		{
			erase(this->begin(), this->end());
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

		const_iterator find(const Key& k) const
		{
			node_pointer node = _start;
			while (node != NULL)
			{
				data_type pair = node->data;
				if (pair.first == k)
					return const_iterator(node);
				node = _node_next(node);
			}
			return const_iterator(_end);
		}

		size_type count(const Key& k) const
		{
			if (this->find(k) != const_iterator(_end))
				return 1;
			else
				return 0;
		}

		size_type erase(const Key& k)
		{
			node_pointer current = _start;
			node_pointer next;

			while (current && current->data.first != k)
			{
				current = _node_next(current);
				if (current == NULL)
					return (0);
			}
			if (!_node_has_leaf(current))
			{
				next = _node_next(current);
				if (next && current->parent)
				{	
					if (_is_left_node(next))
						current->parent->left = NULL;
					else if (_is_right_node(next))
						current->parent->right = NULL;
					next->parent = current->parent;
				}
				else
					next->parent = NULL;

				if (current->left)
				{	
					current->left->parent = next;
					next->left = current->left;
				}
				if (_is_left_node(current))
					current->parent->left = next;
				else if (current->parent)
					current->parent->right = next;
				if (_node_is_root(next))
					_root = next;
			}
			else if (_node_is_root(current) && _node_has_leaf(current))
			{
				_node_alloc.destroy(current);
				_node_alloc.deallocate(current, 1);
				_size = 0;
				_start = NULL;
				_root = NULL;
				return (1);
			}
			else
			{
				if (_is_left_node(current))
					current->parent->left = NULL;
				else if (_is_right_node(current))
					current->parent->right = NULL;
			}
			_node_alloc.destroy(current);
			_node_alloc.deallocate(current, 1);
			_size--;
			_start = _node_min(_root);
			return (1);
		}

		void erase(iterator position) {
			erase((*position).first);
		}

		void erase(iterator first, iterator last)
		{
			while (first != last)
				erase((first++)->first);
		}

		iterator lower_bound(const Key& k) // je ne suis pas sûr d'avoir bein compris ce que cette fonction fait
		{
			iterator ite = this->begin();
			while (ite != this->end() && _comp((*ite).first, k))
				ite++;
			return (ite);
		}

		const_iterator lower_bound(const Key& k) const// je ne suis pas sûr d'avoir bein compris ce que cette fonction fait
		{
			const_iterator ite = this->begin();
			while (ite != this->end() && _comp((*ite).first, k))
				ite++;
			return (ite);
		}

		iterator upper_bound(const Key& k) // je ne suis pas sûr d'avoir bein compris ce que cette fonction fait
		{
			iterator ite = this->begin();
			while (ite != this->end() && !(_comp(k, (*ite).first)))
				ite++;
			return (ite++); // pourquoi est ce que ++ite ne marche pas ?
		}

		const_iterator upper_bound(const Key& k) const// je ne suis pas sûr d'avoir bein compris ce que cette fonction fait
		{
			const_iterator ite = this->begin();
			while (ite != this->end() && !(_comp(k, (*ite).first)))
				ite++;
			return (ite++);
		}


		// TODO Verify
		size_type max_size() const {
			return _node_alloc.max_size();
		}

		Compare key_comp() const { return _comp ; }

	private :

		void _destroy(node_pointer node)
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
			insert(node->data);
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

		unsigned int _treeHeigh(node_pointer node)
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
