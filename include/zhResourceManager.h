/******************************************************************************
Copyright (C) 2013 Tomislav Pejsa

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef __zhResourceManager_h__
#define __zhResourceManager_h__

#include "zhPrereq.h"
#include "zhLogger.h"
#include "zhError.h"
#include "zhIterators.h"
#include "zhObjectFactory.h"
#include "zhSmartPtr.h"

#include <fstream>

namespace zh
{

enum ResourceClass
{
	Resource_AnimationSet,
	Resource_AnimationSpace,
	Resource_AnimationTree,
	Resource_AnimationIndex
};

enum ResourceState
{
	ResourceState_Created,
	ResourceState_Loaded,
	ResourceState_Unloaded,
	ResourceState_Deleted
};

#define zhDeclare_ResourceLoader( ThisClass, classId, className ) \
	zhDeclare_Class( ResourceLoader, ThisClass, classId, className, unsigned long )
#define zhDeclare_ResourceSerializer( ThisClass, classId, className ) \
	zhDeclare_Class( ResourceSerializer, ThisClass, classId, className, unsigned long )
#define zhRegister_ResourceLoader( resMgr, Loader ) \
	resMgr->registerLoader( Loader::ClassId(), Loader::ClassName(), &Loader::Create )
#define zhRegister_ResourceSerializer( resMgr, Serializer ) \
	resMgr->registerSerializer( Serializer::ClassId(), Serializer::ClassName(), &Serializer::Create )
#define zhUnregister_ResourceLoader( resMgr, Loader ) \
	resMgr->unregisterSerializer( Loader::ClassId() )
#define zhUnregister_ResourceSerializer( resMgr, Serializer ) \
	resMgr->unregisterSerializer( Serializer::ClassId() )

class Resource;
class ResourceManager;
class ResourceLoader;
class ResourceSerializer;

typedef SmartPtr<Resource> ResourcePtr;

/**
* @brief Base class template for all resources.
*/
class zhDeclSpec Resource
{

	friend class ResourceManager;

public:

	/**
	* Constructor.
	* 
	* @param id Resource ID.
	* @param name Resource name.
	* @param mgr Owning ResourceManager.
	*/
	Resource( unsigned long id, const std::string& name, ResourceManager* mgr );

	/**
	* Destructor.
	*/
	virtual ~Resource();

	/**
	* Gets the resource ID.
	*/
	unsigned long getId() const;

	/**
	* Gets the resource name.
	*/
	const std::string& getName() const;

	/**
	* Gets the Resource class ID.
	*/
	ResourceClass getClassId() const;

	/**
	* Gets a pointer to the resource.
	*/
	ResourcePtr getPtr() const;

	/**
	* Gets a pointer to the owning resource manager.
	*/
	ResourceManager* getManager() const;

	/**
	* Gets the resource path.
	*/
	const std::string& getPath() const;

	/**
	* Gets the resource state.
	*/
	ResourceState getState() const;

	/**
	* Calculates the resource memory usage.
	*
	* This pure virtual function should be implemented in
	* the derived resource classes.
	*/
	virtual size_t _calcMemoryUsage() const = 0;

	/**
	* Unloads the resource, freeing up the memory it occupies.
	*
	* This pure virtual function should be implemented in
	* the derived resource classes.
	*/
	virtual void _unload() = 0;

	/**
	* Creates a deep copy of the resource.
	*
	* This pure virtual function should be implemented in
	* the derived resource classes.
	*
	* @param clonePtr Pointer to the copy.
	*/
	virtual void _clone( Resource* clonePtr ) const = 0;

protected:

	virtual void _setManager( ResourceManager* mgr );
	virtual void _setPath( const std::string& path );
	virtual void _setState( ResourceState state );

	unsigned long mId;
	std::string mName;

	ResourceManager* mMgr;
	std::string mPath;
	ResourceState mState;

};

enum ResourceManagerError
{
	ResourceMgrError_None,
	ResourceMgrError_FileNotFound,
	ResourceMgrError_LoaderNotFound,
	ResourceMgrError_OutOfMemory,
	ResourceMgrError_SerializerNotFound,
	ResourceMgrError_ClassRegistered
};

/**
* @brief Generic resource manager.
*/
class zhDeclSpec ResourceManager
{

public:

	typedef MapIterator< std::map<unsigned long, ResourcePtr> > ResourceIterator;
	typedef MapConstIterator< std::map<unsigned long, ResourcePtr> > ResourceConstIterator;

	typedef ObjectFactory<ResourceLoader> ResourceLoaderFactory;
	typedef ObjectFactory<ResourceSerializer> ResourceSerializerFactory;

	zhDeclare_ErrorState

	/**
	* Constructor.
	*/
	ResourceManager();

	/**
	* Destructor.
	*/
	virtual ~ResourceManager();

	/**
	* Gets the Resource class ID.
	*
	* This pure virtual function should be implemented in concrete
	* ResourceManager classes.
	*/
	virtual ResourceClass getClassId() const = 0;

	/**
	* Creates a new, uninitialized resource.
	*
	* @param id Resource ID.
	* @param name Resource name.
	* @return Pointer to the resource.
	*/
	virtual ResourcePtr createResource( unsigned long id, const std::string& name );

	/**
	* Deletes an existing resource.
	*
	* @param id Resource ID.
	*/
	virtual void deleteResource( unsigned long id );

	/**
	* Deletes an existing resource.
	*
	* @param name Resource name.
	*/
	virtual void deleteResource( const std::string& name );

	/**
	* Deletes all resources.
	*/
	virtual void deleteAllResources();

	/**
	* Loads the specified resource from a file.
	*
	* @param id Resource ID.
	* @param path Resource path or filename.
	* @param tryLoad If true, resource manager will verify
	* if it can load the resource from the given location
	* (i.e. if the file exists, if a loader exists and
	* if there is enough memory).
	* @return true if resource has been successfully loaded, false otherwise.
	* Sets error codes:
	* - ResourceMgrError_None - no errors
	* - ResourceMgrError_FileNotFound - could not find or open resource file
	* - ResourceMgrError_LoaderNotFound - could not find a loader that can load the resource
	* - ResourceMgrError_OutOfMemory - not enough memory for the resource
	* @remark The resource must be created before it may be loaded.
	* If full resource path is specified, the resource is loaded directly.
	* If only the filename is specified, the resource is loaded
	* from one of the predefined resource directories.
	* If nothing is specified, the resource is loaded
	* from its previously specified path
	* (this is done to reload resources that have been previously unloaded).
	*/
	virtual bool loadResource( unsigned long id, const std::string& path = "", bool tryLoad = false );

	/**
	* Loads the specified resource from a file.
	*
	* @param name Resource name.
	* @param path Resource path or filename.
	* @param tryLoad If true, resource manager will verify
	* if it can load the resource from the given location
	* (i.e. if the file exists, if a loader exists and
	* if there is enough memory).
	* @return true if resource has been successfully loaded, false otherwise.
	* Sets error codes:
	* - ResourceMgrError_None - no errors
	* - ResourceMgrError_FileNotFound - could not find or open resource file
	* - ResourceMgrError_LoaderNotFound - could not find a loader that can load the resource
	* - ResourceMgrError_OutOfMemory - not enough memory for the resource
	* @remark The resource must be created before it may be loaded.
	* If full resource path is specified, the resource is loaded directly.
	* If only the filename is specified, the resource is loaded
	* from one of the predefined resource directories.
	* If nothing is specified, the resource is loaded
	* from its previously specified path
	* (this is done to reload resources that have been previously unloaded).
	*/
	virtual bool loadResource( const std::string& name, const std::string& path = "", bool tryLoad = false );

	/**
	* Unloads the specified resource, keeping it in the system but freeing
	* the memory it occupies.
	*/
	virtual void unloadResource( unsigned long id );

	/**
	* Unloads the specified resource, keeping it in the system but freeing
	* the memory it occupies.
	*/
	virtual void unloadResource( const std::string& name );

	/**
	* Unloads all loaded resources.
	*/
	virtual void unloadAllResources();

	/**
	* Serializes the resource to a file.
	*
	* @param id Resource ID.
	* @param path Resource path or filename.
	* @return true if serialization has been successful, false otherwise.
	* Sets error codes:
	* - ResourceMgrError_None - no errors
	* - ResourceMgrError_SerializerNotFound - could not find a serializer that can serialize the resource
	* @remark The resource must be loaded before it can be serialized.
	* If resource path is specified, the resource is serialized to that path
	* and the specified path becomes the new default resource path.
	* If only the filename is specified, the resource is serialized to
	* its original directory.
	* If nothing is specified, the resource is serialized
	* to its original path.
	*/
	virtual bool serializeResource( unsigned long id, const std::string& path = "" );

	/**
	* Serializes the resource to a file.
	*
	* @param name Resource name.
	* @param path Resource path or filename.
	* @return true if serialization has been successful, false otherwise.
	* Sets error codes:
	* - ResourceMgrError_None - no errors
	* - ResourceMgrError_SerializerNotFound - could not find a serializer that can serialize the resource
	* @remark The resource must be loaded before it can be serialized.
	* If resource path is specified, the resource is serialized to that path
	* and the specified path becomes the new default resource path.
	* If only the filename is specified, the resource is serialized to
	* its original directory.
	* If nothing is specified, the resource is serialized
	* to its original path.
	*/
	virtual bool serializeResource( const std::string& name, const std::string& path = "" );

	/**
	* Creates a deep copy of the specified resource.
	*
	* @param id Resource ID.
	* @param cloneId ID for the copy.
	* @param cloneName Name for the copy.
	* @return Pointer to the the copy or NULL if cloning failed.
	* Sets error codes:
	* - ResourceMgrError_None - no errors
	* - ResourceMgrError_OutOfMemory - not enough memory for the copy
	* @remark The resource must be loaded before it can be cloned.
	* The copy will have the same path as the original resource.
	* You can change that path when serializing the resource.
	*/
	virtual ResourcePtr cloneResource( unsigned long id,
		unsigned long cloneId, const std::string& cloneName );

	/**
	* Creates a deep copy of the specified resource.
	*
	* @param name Resource name.
	* @param cloneId ID for the copy.
	* @param cloneName Name for the copy.
	* @return Pointer to the the copy or NULL if cloning failed.
	* Sets error codes:
	* - ResourceMgrError_None - no errors
	* - ResourceMgrError_OutOfMemory - not enough memory for the copy
	* @remark The resource must be loaded before it can be cloned.
	* The copy will have the same path as the original resource.
	* You can change that path when serializing the resource.
	*/
	virtual ResourcePtr cloneResource( const std::string& name,
		unsigned long cloneId, const std::string& cloneName );

	/**
	* Returns true if the specified resource exists, otherwise false.
	*/
	virtual bool hasResource( unsigned long id ) const;

	/**
	* Returns true if the specified resource exists, otherwise false.
	*/
	virtual bool hasResource( const std::string& name ) const;

	/**
	* Gets a pointer to the specified resource.
	*
	* @param id Resource ID.
	* @return Pointer to the resource or NULL if the resource doesn't exist.
	*/
	virtual ResourcePtr getResource( unsigned long id ) const;

	/**
	* Gets a pointer to the specified resource.
	*
	* @param name Resource name.
	* @return Pointer to the resource or NULL if the resource doesn't exist.
	*/
	virtual ResourcePtr getResource( const std::string& name ) const;

	/**
	* Gets an iterator over the map of resources.
	*/
	virtual ResourceIterator getResourceIterator();

	/**
	* Gets a const iterator over the map of resources.
	*/
	virtual ResourceConstIterator getResourceConstIterator() const;

	/**
	* Gets the number of resources.
	*/
	virtual unsigned int getNumResources() const;

	/**
	* Adds a resource directory.
	*/
	virtual void addResourceDirectory( const std::string& dir );

	/**
	* Removes a resource directory.
	*/
	virtual void removeResourceDirectory( unsigned int dirIndex );

	/**
	* Removes all resource directories.
	*/
	virtual void removeAllResourceDirectories();

	/**
	* Gets the resource directory at the specified index.
	*/
	virtual const std::string& getResourceDirectory( unsigned int dirIndex ) const;

	/**
	* Gets the number of resource directories.
	*/
	virtual unsigned int getNumResourceDirectories() const;

	/**
	* Registers a new resource loader.
	*
	* @param classId Loader class ID.
	* @param className Loader class name.
	* @param creatorFunc Loader instance creation function.
	* @return true if loader has been successfully registered, false otherwise.
	* Sets error codes:
	* - ResourceMgrError_None - no errors
	* - ResourceMgrError_ClassRegistered - loader already registered
	*/
	virtual bool registerLoader( unsigned long classId, const std::string& className,
		ResourceLoader*( *creatorFunc )( const unsigned long& ) );

	/**
	* Registers a new resource serializer.
	*
	* @param classId Serializer class ID.
	* @param className Serializer class name.
	* @param creatorFunc Serializer instance creation function.
	* @return true if serializer has been successfully registered, false otherwise.
	* Sets error codes:
	* - ResourceMgrError_None - no errors
	* - ResourceMgrError_ClassRegistered - loader already registered
	*/
	virtual bool registerSerializer( unsigned long classId, const std::string& className,
		ResourceSerializer*( *creatorFunc )( const unsigned long& ) );

	/**
	* Unregisters a resource loader.
	*/
	virtual void unregisterLoader( unsigned long classId );

	/**
	* Unregisters a resource serializer.
	*/
	virtual void unregisterSerializer( unsigned long classId );

	/**
	* Gets the amount of memory used by the loaded resources (in bytes).
	*/
	virtual size_t getMemoryUsage() const;

	/**
	* Gets the maximum amount of memory available
	* to the resource manager (in bytes).
	*/
	virtual size_t getMaxMemoryUsage() const;

	/**
	* Sets the maximum amount of memory available
	* to the resource manager (in bytes).
	*/
	virtual void setMaxMemoryUsage( size_t amount );

protected:

	virtual bool _updateMemoryUsage( size_t amount, bool subtract = false );
	virtual Resource* _createResource( unsigned long id, const std::string& name ) = 0;


	std::map<unsigned long, ResourcePtr> mResourcesById;
	std::map<std::string, ResourcePtr> mResourcesByName;

	std::vector<std::string> mResourceDirs;
	std::vector<ResourceLoader*> mResourceLoaders;
	std::vector<ResourceSerializer*> mResourceSerializers;
	ResourceLoaderFactory mResourceLoaderFactory;
	ResourceSerializerFactory mResourceSerializerFactory;

	size_t mMemUsage;
	size_t mMaxMemUsage;

};

/**
* @brief Base class template for resource loaders.
*/
class zhDeclSpec ResourceLoader
{

public:

	zhDeclare_BaseClass( ResourceLoader, 0, "ResourceLoader", unsigned long )

	ResourceLoader() { }

	virtual ~ResourceLoader() { }

	/**
	* Checks if the loader can load the specified resource
	* (i.e. if it is supported file type).
	* This pure virtual function needs to be implemented in
	* a derived resource IO class.
	*
	* @param resource Pointer to the resource that should be initialized
	* from the file.
	* @param path Resource file path.
	* @return true if resource can be loaded, otherwise false.
	*/
	virtual bool tryLoad( ResourcePtr resource, const std::string& path ) = 0;

	/**
	* Loads the resource from a file.
	* This pure virtual function needs to be implemented in
	* a derived resource IO class.
	*
	* @param resource Pointer to the resource that should be initialized
	* from the file.
	* @param path Resource file path.
	* @return true if resource has been successfully loaded, otherwise false.
	*/
	virtual bool load( ResourcePtr resource, const std::string& path ) = 0;

};

/**
* @brief Base class template for resource serializers.
*/
class zhDeclSpec ResourceSerializer
{

public:

	zhDeclare_BaseClass( ResourceSerializer, 0, "ResourceSerializer", unsigned long )

	ResourceSerializer() { }

	virtual ~ResourceSerializer() { }

	/**
	* Checks if the serializer can load the specified resource
	* (i.e. if it is supported file type).
	* This pure virtual function needs to be implemented in
	* a derived resource IO class.
	*
	* @param resource Pointer to the resource that should be initialized
	* from the file.
	* @param path Resource file path.
	* @return true if resource can be loaded, otherwise false.
	*/
	virtual bool trySerialize( ResourcePtr resource, const std::string& path ) = 0;

	/**
	* Serializes the resource to a file.
	* This pure virtual function needs to be implemented in
	* a derived resource IO class.
	*
	* @param resource Pointer to the resource that should be written
	* to the file.
	* @param path Resource file path.
	* @return true if resource has been successfully written, otherwise false.
	*/
	virtual bool serialize( ResourcePtr resource, const std::string& path ) = 0;

};

}

#endif // __zhResourceManager_h__
