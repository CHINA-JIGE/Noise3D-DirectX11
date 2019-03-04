/***********************************************************

						General Tree Node

			A Single Node of Tree.
		Each Node's transform are based on its parent node,
		so hierarchical transformation is available.

***********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::GeneralTreeNode::GeneralTreeNode():
	m_pParentNode(nullptr)
{
}

Noise3D::GeneralTreeNode::GeneralTreeNode(const GeneralTreeNode & rhs)
{
	this->mChildNodeList = rhs.mChildNodeList;//deep copy
	this->m_pParentNode = rhs.m_pParentNode;
}

GeneralTreeNode * Noise3D::GeneralTreeNode::GetParentNode()
{
	return m_pParentNode;
}

void Noise3D::GeneralTreeNode::AttachToParentNode(GeneralTreeNode * pNode)
{
	pNode->AttachChildNode(this);
}

void Noise3D::GeneralTreeNode::AttachChildNode(GeneralTreeNode * pNode)
{
	if (pNode != nullptr)
	{
		//delete pNode's original father's ref to pNode
		GeneralTreeNode* pOriginalParent = pNode->m_pParentNode;
		if (pOriginalParent != nullptr)
		{
			auto iter_ChildNodeRef = std::find(pOriginalParent->mChildNodeList.begin(), pOriginalParent->mChildNodeList.end(), pNode);
			pOriginalParent->mChildNodeList.erase(iter_ChildNodeRef);
		}

		pNode->m_pParentNode = this;
		mChildNodeList.push_back(pNode);
	}
}

bool Noise3D::GeneralTreeNode::IsAttachedToParentNode()
{
	return (m_pParentNode!=nullptr);
}

GeneralTreeNode * Noise3D::GeneralTreeNode::CreateChildNode()
{
	GeneralTreeNode * pNewChild = new GeneralTreeNode;
	GeneralTreeNode::AttachChildNode(pNewChild);
	return pNewChild;
}

GeneralTreeNode * Noise3D::GeneralTreeNode::GetChildNode(int index)
{
	if (index < mChildNodeList.size())
	{
		return mChildNodeList[index];
	}
	return nullptr;
}

uint32_t Noise3D::GeneralTreeNode::GetChildNodeCount()
{
	return mChildNodeList.size();
}

//protected; GeneralTree has the permission to delete GeneralTreeNode
Noise3D::GeneralTreeNode::~GeneralTreeNode()
{
}


/****************************************************

								General Tree

******************************************************/

Noise3D::GeneralTree::GeneralTree():
	m_pRoot(new GeneralTreeNode)
{
}

Noise3D::GeneralTree::~GeneralTree()
{
	//allowed to remove root
	mFunc_Remove<true>(m_pRoot);
}

void Noise3D::GeneralTree::Traverse_PreOrder(GeneralTreeNode * pNode, std::vector<GeneralTreeNode*>& outResult)
{
	if (pNode == nullptr)return;

	outResult.push_back(pNode);
	for (auto pn : pNode->mChildNodeList)
	{
		// nullptr's check will be done within the recursive call
		GeneralTree::Traverse_PreOrder(pn, outResult);
	}
}

void Noise3D::GeneralTree::Traverse_PostOrder(GeneralTreeNode * pNode, std::vector<GeneralTreeNode*>& outResult)
{
	if (pNode == nullptr)return;

	for (auto pn : pNode->mChildNodeList)
	{
		// nullptr's check will be done within the recursive call
		GeneralTree::Traverse_PreOrder(pn, outResult);
	}
	outResult.push_back(pNode);
}

void Noise3D::GeneralTree::Traverse_LayerOrder(GeneralTreeNode * pNode, std::vector<GeneralTreeNode*>& outResult)
{
	if (pNode == nullptr)return;

	//Breath First Search(BFS), stack needed, no recursive solution(?)
	std::queue<GeneralTreeNode*> queue;
	queue.push(pNode);

	//search until all nodes are popped up
	while (!queue.empty())
	{
		GeneralTreeNode* pStackTopNode = queue.front();
		outResult.push_back(pStackTopNode);
		queue.pop();//pop the head of queue

		//push child to queue tail
		for (auto pn : pStackTopNode->mChildNodeList)
		{
			if(pn!=nullptr)queue.push(pn);
		}
	}

}

void Noise3D::GeneralTree::Traverse_PreOrder(std::vector<GeneralTreeNode*>& outResult)
{
	GeneralTree::Traverse_PreOrder(m_pRoot, outResult);
}

void Noise3D::GeneralTree::Traverse_PostOrder(std::vector<GeneralTreeNode*>& outResult)
{
	GeneralTree::Traverse_PostOrder(m_pRoot, outResult);
}

void Noise3D::GeneralTree::Traverse_LayerOrder(std::vector<GeneralTreeNode*>& outResult)
{
	GeneralTree::Traverse_LayerOrder(m_pRoot, outResult);
}


void Noise3D::GeneralTree::Remove(GeneralTreeNode * pNode)
{
	//not allowed to remove root
	mFunc_Remove<false>(pNode);
}

GeneralTreeNode * Noise3D::GeneralTree::GetRoot()
{
	return m_pRoot;
}

template<bool permittedToRemoveRoot>
void Noise3D::GeneralTree::mFunc_Remove(GeneralTreeNode * pNode)
{
	//root can be removed only in destructor
	if (!permittedToRemoveRoot)
	{
		if (pNode == m_pRoot)
		{
			return;
		}
	}

	std::vector<GeneralTreeNode*> list;

	//remove pNode's ref from its father's 'ChildNodeList'
	GeneralTreeNode* pFatherNode = pNode->GetParentNode();
	if (pFatherNode != nullptr)
	{
		auto iter_ChildNodeRef = std::find(pFatherNode->mChildNodeList.begin(), pFatherNode->mChildNodeList.end(), pNode);
		pFatherNode->mChildNodeList.erase(iter_ChildNodeRef);
	}

	//traverse sub-tree starting from pNode
	GeneralTree::Traverse_PostOrder(pNode, list);

	//delete current and all children nodes including pNode
	//and remember to remove them from 'childNodeList'
	for (auto pn : list)delete pn;
}

void Noise3D::GeneralTree::mFunc_FindAndDeleteRefToChildNode(GeneralTreeNode * pFather, GeneralTreeNode * pChild)
{
	//remove father's reference to child node (remove record in father's ChildNodeList)
	if (pFather != nullptr)
	{
		//search child in father's ref list
		auto iter_ChildNodeRef = std::find(pFather->mChildNodeList.begin(), pFather->mChildNodeList.end(), pChild);
		pFather->mChildNodeList.erase(iter_ChildNodeRef);
	}
}

