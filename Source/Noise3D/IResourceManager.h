#pragma once

/***********************************************************************

							CLASS:  IResourceManager

		(2018.7.23)Every Resource Manager(mesh, texture, graphicObj, material, lights....)
		has some managing interfaces that do common job like create/get/delete
		based on IFactory<childObject_t> class. And after a lot of repetitive
		dull work of adding similar interfaces to managers, i decided to
		extract/abstract a base class IResourceManager to do common
		child object managing work...
		
************************************************************************/


#pragma once

namespace Noise3D
{
	//maxCount is the max count of child that can be created.
	//the default implementation of IResourceManager is simply direct invocation to IFactory functions
	//but overriding Factory<> functions is DANGEROUS and complicated, so IResourceManager provides a simple encapsulation of 
	//IFactory and can be overrided by some specific Manager class.
	template<typename resource_t>
	class /*_declspec(dllexport)*/ IResourceManager:
		IFactory<resource_t>
	{
	public:
		IResourceManager() = delete;

		//constructor
		IResourceManager(const UINT maxChildObjectCount) :IFactory<resource_t>(maxChildObjectCount){};

		//destructor
		~IResourceManager()
		{
			IFactory<resource_t>::DestroyAllObject();
		};


		/*virtual resource_t*	CreateResource(N_UID objName) { return IFactory<resource_t>::CreateObject(objName); }

		virtual resource_t*	GetResource(N_UID objName) { return IFactory<resource_t>::GetObjectPtr(objName);}

		virtual resource_t*	GetResource(UINT index) { return IFactory<resource_t>::GetObjectPtr(index); }

		virtual bool	DestroyResource(N_UID objName) { return IFactory<resource_t>::Destroy(objName); }

		virtual  bool	DestroyResource(resource_t* pObj) { return IFactory<resource_t>::Destroy(pObj); }

		virtual void	DestroyAllResources() { IFactory<resource_t>()::DestroyAllObject(); }

		virtual UINT GetResourceCount() { IFactory<resource_t>()::GetObjectCount(); }

		virtual bool	IsResourceExisted(N_UID objName) { return  IFactory<resource_t>::FindUid(objName); }

		virtual bool	IsResourceExisted(resource_t* pObj) { return  IFactory<resource_t>::FindUid(pObj); }*/

	protected:

	};
};