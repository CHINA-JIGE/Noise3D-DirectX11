
/***********************************************************************

							CLASS:  IFactory

		A Base Class that Manager Classes should inherit, it provides
		common interfaces for objects creation while constrains the 
		numbers of objects.  A FACTORY/Manager can manage life 
		cycle of child object in order to avoid dangerous situation
		(like unexpected deletion)
************************************************************************/

#pragma once

namespace Noise3D
{

	template<typename objType>
	struct N_ChildObjectInfo
	{
		N_ChildObjectInfo()
		{
			_pObjPtr = nullptr;
			_uid = N_UID("");
		}

		N_ChildObjectInfo(objType* pObjPtr, N_UID	uid)
		{
			_pObjPtr = pObjPtr;
			_uid = uid;
		};

		objType* _pObjPtr;
		N_UID	  _uid;
	};

	//maxCount is the max count of child that can be created.
	template<typename objType,const UINT maxCount>
	class _declspec(dllexport) IFactory
	{
	public:
		//constructor
		IFactory()
		{
			m_pChildObjectList = new std::vector<N_ChildObjectInfo<objType>>;
			m_pUidToIndexHashTable = new std::unordered_map<N_UID, UINT>;
		};

		//destructor
		~IFactory()
		{
			DestroyAllObject();

			delete m_pChildObjectList;
			delete m_pUidToIndexHashTable;
		};

		//runtime creation of objects, of which the max count is limited
		objType*	CreateObject(N_UID objUID)
		{
			if (m_pChildObjectList->size() < maxCount)
			{
				auto iter = m_pUidToIndexHashTable->find(objUID);

				//need to assure that UID don't conflict
				if (iter == m_pUidToIndexHashTable->cend())
				{
					objType* pNewObject = new objType;
					m_pChildObjectList->push_back(N_ChildObjectInfo<objType>(pNewObject, objUID));
					m_pUidToIndexHashTable->insert(std::make_pair(objUID, m_pChildObjectList->size() - 1);
					return pNewObject;
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}
		};

		objType*	GetObject(UINT objIndex)
		{
			if (objIndex < m_pChildObjectList->size())
			{
				return m_pChildObjectList[objIndex];
			}
			else
			{
				return nullptr;
			}
		}

		objType*	GetObject(N_UID objUID)
		{
			auto iter = m_pUidToIndexHashTable->find(objUID);
			if (iter != m_pUidToIndexHashTable->cend())
			{
				UINT objIndex = iter->second;
				if (objIndex < m_pChildObjectList->size())
				{
					return m_pChildObjectList[objIndex];
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}
		}

		BOOL	DestroyObject(UINT objIndex)
		{
			auto iter = m_pChildObjectList->begin();

			if (objIndex >= m_pChildObjectList->size())return FALSE;

			//delete name-index pair
			N_UID deleteObjUID = m_pChildObjectList->at(objIndex)._uid;
			auto uidIndexPairIter = m_pTextureObjectHashTable->find(deleteObjUID);
			//..............(the check might be useless...)
			if (uidIndexPairIter != m_pTextureObjectHashTable->end())
			{
				m_pTextureObjectHashTable->erase(uidIndexPairIter);
			}

			//clear TextureObject Data
			iter += texID;
			//safe_release SRV  interface
			ReleaseCOM(m_pTextureObjectList->at(texID).m_pSRV);
			m_pTextureObjectList->at(texID).mPixelBuffer.clear();
			m_pTextureObjectList->erase(iter);

		}

		BOOL	DestroyObject(N_UID objUID)
		{
			auto iter = m_pUidToIndexHashTable->find(objUID);
			//if UID of the object do exist
			if (iter != m_pUidToIndexHashTable->cend())
			{
				UINT objIndex = iter->second;

				//delete the object data block in list
				if (objIndex < m_pChildObjectList->size())
				{
					m_pChildObjectList->erase(m_pChildObjectList->begin() + objIndex);
				}

				//then erase the UID-index mapping pair
				m_pUidToIndexHashTable->erase(iter);

				//deletions always happen in the middle of a list , so elements after it 
				//will be affected.
				for (auto& pair : *m_pTextureObjectHashTable)
				{
					if (pair.second>deletedTexID_threshold)--pair.second;
				}

				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}

		//it will be set to nullptr no matter the deletion finish
		BOOL	DestroyObject(objType** ppObject)
		{
			if (ppObject != nullptr)
			{
				auto iter = std::find(
					m_pChildObjectList->begin(),
					m_pChildObjectList->end(),
					ppObject
					);
			}
			else
			{
				return FALSE;
			}
		};

		void		DestroyAllObject() 
		{
			for (auto ptr : *m_pChildObjectList)
			{
				delete ptr;
			}

			m_pChildObjectList->clear();
			m_pUidToIndexHashTable->clear();
		};

	private :
		const UINT mMaxObjectCount = maxCount;
		std::vector<N_ChildObjectInfo<objType>>*			m_pChildObjectList;
		std::unordered_map<N_UID, UINT>*					m_pUidToIndexHashTable;
	};


}