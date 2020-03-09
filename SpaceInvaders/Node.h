#pragma once
#include <vector>


template<typename T>
class Node
{
public:
	T data;
	std::vector<Node> children;
	Node* parent;
public:
	Node(T d) : data(d), parent(nullptr) {}
	T GetData() const { return data; }
	std::vector<Node> GetChildren() { return children; }
	void AddChild(T d) { Node n(d); n.parent = this; children.push_back(n); }
	Node& operator[](T s)
	{
		for (unsigned int i = 0; i < children.size(); ++i)
		{
			if (children[i].data == s)
				return children[i];
		}
	}
};