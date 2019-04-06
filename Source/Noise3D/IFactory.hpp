
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

	/*
	IFactory should be inherited by those classes which wants to 
	create certain kinds of products,

	for example:
	TextureManager should inherit from IFactory<ITexture>

	the reason why I don't use cohesion to integrate a factory (but inheritance), 
	is that cohesion requires CreateGraphicObject method to be Public, which
	sabotage encapsulation and authorized user to create product by their own.

	the reason I use such "Factory Mode" is that controlling the life cycle (produce and
	destroy) of interfaces will cause less chaos, more stability, more information is in control.
	
	this is like CENTRALISM, objects must be created and monitored by certain
	manager class.


	NOTE that product class T should make some adjustments:
		1,constructor/destructor should be PRIVATE
		2,T should friend IFactory<T> (IFactory should have access to its constructor)
	*/

	//(2019.3.15)for manager that only handles simple logic, 
	//concrete manager class just need to directly inherit from IFactory<> to get its common interfaces
	//some manager like TextManager inherit from more than 1 IFactory<XX>, i think it should use 'protected' inherit
	//because it's not so convenient for the user to explicitly use IFactory<> and template param.

	//maxCount is the max count of child that can be created.
	template<typename objType>
	class /*_declspec(dllexport)*/ IFactory
	{
	public:
		IFactory() = delete;

		//constructor
		IFactory(uint32_t maxCount):
			mMaxObjectCount(maxCount),
		m_pChildObjectList ( new std::vector<N_ChildObjectInfo<objType>>),
		m_pUidToIndexHashTable (new std::unordered_map<N_UID, UINT>)
		{
		};

		//destructor
		~IFactory()
		{
			DestroyAllObject();

			delete m_pChildObjectList;
			delete m_pUidToIndexHashTable;
		};
		
		objType*	GetObjectPtr(UINT objIndex) const
		{
			if (objIndex < m_pChildObjectList->size())
			{
				return m_pChildObjectList->at(objIndex)._pObjPtr;
			}
			else
			{
				return nullptr;
			}
		}

		objType*	GetObjectPtr(N_UID objUID) const
		{
			//get number index from UID-index hash map
			auto iter = m_pUidToIndexHashTable->find(objUID);
			if (iter != m_pUidToIndexHashTable->cend())
			{
				UINT objIndex = iter->second;
				if (objIndex < m_pChildObjectList->size())
				{
					return m_pChildObjectList->at(objIndex)._pObjPtr;
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

		uint32_t	GetObjectID(N_UID uid) const
		{
			auto iter = m_pUidToIndexHashTable->find(uid);
			//need to assure that UID don't conflict
			if (iter != m_pUidToIndexHashTable->end())
			{
				return iter->second;//index
			}
			else
			{
				return NOISE_MACRO_INVALID_ID;
			}
		}

		uint32_t	GetUID(uint32_t index) const
		{
			if (index < m_pChildObjectList->size())
			{
				auto& obj = m_pChildObjectList->at(index);
				return obj._uid;
			}
			else
			{
				return "";//index invalid
			}
		}

		uint32_t	GetObjectCount()  const
		{
			return m_pChildObjectList->size();
		}

		//can be used to validate uid's existence
		bool		FindUid(N_UID uid) const
		{
			if (m_pUidToIndexHashTable->find(uid) != m_pUidToIndexHashTable->end())
			{
				return true;
			}
			else
			{
				return false;
			}
		};

		bool		DestroyObject(UINT objIndex)
		{
			auto iter = m_pChildObjectList->begin();

			if (objIndex >= m_pChildObjectList->size())return false;

			//delete name-index pair
			N_UID deleteObjUID = m_pChildObjectList->at(objIndex)._uid;
			auto uidIndexPairIter = m_pUidToIndexHashTable->find(deleteObjUID);
			//..............(the check might be useless under such strict management...
			//in case that I made mistakes
			if (uidIndexPairIter != m_pUidToIndexHashTable->end())
			{
				m_pUidToIndexHashTable->erase(uidIndexPairIter);
			}

			//deletions always happen in the middle of a list , so elements after it 
			//will be affected.
			for (auto& pair : *m_pUidToIndexHashTable)
			{
				if (pair.second>objIndex)--pair.second;//uid-index pair
			}

			//move delete iterator to target position
			iter += objIndex;
			//safe_release SRV  interface
			//ReleaseCOM(m_pTextureObjectList->at(texID).m_pSRV);
			//m_pTextureObjectList->at(texID).mPixelBuffer.clear();
			m_pChildObjectList->erase(iter);
			return true;
		}

		bool		DestroyObject(N_UID objUID)
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
				for (auto& pair : *m_pUidToIndexHashTable)
				{
					if (pair.second>objIndex)--pair.second;//uid-index pair
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		bool		DestroyObject(objType* pObject)
		{
			if (pObject != nullptr)
			{
				for (UINT i = 0; i < m_pChildObjectList->size(); i++)
				{
					//uid-ptr pair,find pObject by linearly comparation
					auto childObjInfo = m_pChildObjectList->at(i);
					if (childObjInfo._pObjPtr == pObject)
					{
						//invoke another overload (which use index to delete)
						return DestroyObject(UINT(i));
					}
				}
				//deletion failed
				return false;
			}
			else
			{
				return false;
			}
		};

		void		DestroyAllObject()
		{
			for (auto childObjInfo : *m_pChildObjectList)
			{
				//delete child object pointers
				delete childObjInfo._pObjPtr;
				childObjInfo._pObjPtr = nullptr;
			}

			//then clear the list
			m_pChildObjectList->clear();
			m_pUidToIndexHashTable->clear();
		};

	protected:
		//runtime creation of objects, of which the max count is limited
		objType*	CreateObject(N_UID objUID)
		{
			/*if (objUID == N_UID(""))
			{
			ERROR_MSG("IFactory: UID invalid, can't be empty.");
			return nullptr;
			}*/

			//the count of child object is  limited
			if (m_pChildObjectList->size() < mMaxObjectCount)
			{
				auto iter = m_pUidToIndexHashTable->find(objUID);

				//need to assure that UID don't conflict
				if (iter == m_pUidToIndexHashTable->cend())
				{
					objType* pNewObject = new objType;
					m_pChildObjectList->push_back(N_ChildObjectInfo<objType>(pNewObject, objUID));
					m_pUidToIndexHashTable->insert(std::make_pair(objUID, m_pChildObjectList->size() - 1));

					return pNewObject;
				}
				else
				{
					ERROR_MSG("IFactory:Object UID existed");
					return nullptr;
				}
			}
			else
			{
				ERROR_MSG("IFactory:Object Count Exceeded Limit");
				return nullptr;
			}
		};

	private:
		uint32_t mMaxObjectCount;

		//uid-ptr pair(not mapping)
		std::vector<N_ChildObjectInfo<objType>>*			m_pChildObjectList;

		//UID - index mapping, the UINT is index for vector
		std::unordered_map<N_UID, uint32_t>*					m_pUidToIndexHashTable;
	};

	//(2019.3.26)some mananger classes inherit from several IFactory, then the method will
	//be ambiguous. So i use template tricks(?) to resolve this.
	//explicit template function will be defined at the top level of template param expansion
	//, which will hide all the functions with the same name in later's recursion.
	//(2019.3.27)constructor list are expanded using std::initializer list and recursion, and const function(c++14)

	template<typename ...args> class IFactoryEx {};//declaration

	template<typename T, typename ...Rest>//recursive declaration
	class IFactoryEx<T, Rest...> :
		protected IFactory<T>,
		public IFactoryEx<Rest...>
	{

	public:
		IFactoryEx() = delete;

		//constructor(start recursion)
		IFactoryEx(const std::initializer_list<uint32_t>& list) :
			IFactoryEx(list, 0){}

		//constructor(recursion, recursively get i-th param and init IFactory<T_i>, 
		//and pass the N-i-1 param to the next recursive level)
		IFactoryEx(const std::initializer_list<uint32_t>& maxCountList, int param_id) :
			IFactory<T>(mFunc_CompileTile_GetInitListParam(maxCountList, param_id)),
			IFactoryEx<Rest...>(maxCountList, param_id + 1)
		{}

		//destructor
		~IFactoryEx() {};

		template <typename obj_t>
		obj_t* GetObjectPtr(uint32_t objIndex) const
		{ return IFactory<obj_t>::GetObjectPtr(objIndex); }

		template <typename obj_t>
		obj_t* GetObjectPtr(N_UID uid) const
		{
			return IFactory<obj_t>::GetObjectPtr(uid);
		}

		template <typename obj_t>
		uint32_t	GetObjectID(N_UID uid) const
		{ return IFactory<obj_t>::GetObjectID(uid); }

		template <typename obj_t>
		N_UID	GetUID(uint32_t index) const
		{ return  IFactory<obj_t>::GetUID(index);}

		template <typename obj_t>
		uint32_t	GetObjectCount()  const
		{ return  IFactory<obj_t>::GetObjectCount(); }

		template <typename obj_t>
		bool		FindUid(N_UID uid) const
		{ return IFactory<obj_t>::FindUid(uid);}

		template <typename obj_t>
		bool		DestroyObject(uint32_t objIndex)
		{ return IFactory<obj_t>::DestroyObject(objIndex);}

		template <typename obj_t>
		bool		DestroyObject(N_UID objUID)
		{ return IFactory<obj_t>::DestroyObject(objUID);}

		template <typename obj_t>
		bool	DestroyObject(obj_t* pObject)
		{ return IFactory<obj_t>::DestroyObject(pObject);}

		template <typename obj_t>
		void	DestroyAllObject()
		{IFactory<obj_t>::DestroyAllObject();}

	protected:

		template <typename obj_t>
		obj_t*	CreateObject(N_UID uid){
			return IFactory<obj_t>::CreateObject(uid);
		}

		//[Compile Time const expr] Get given param at given param_id of initializer_list
		const uint32_t mFunc_CompileTile_GetInitListParam(const std::initializer_list<uint32_t>& _list, int _param_id)
		{
			auto iter = _list.begin();
			for (int i = 0; i < _param_id; ++i, iter++) {}
			return *iter;
		};
	};

	//recursion's terminate declaration
	template<>
	class IFactoryEx<> 
	{
	public:
		IFactoryEx() = delete;

		//ctor's terminate
		IFactoryEx(std::initializer_list<uint32_t> list, uint32_t param_id) {};
	};
}