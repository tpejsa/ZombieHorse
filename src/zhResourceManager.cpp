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

#include "zhResourceManager.h"
#include "zhString.h"
#include "zhFileSystem.h"

#include <limits>

namespace zh
{

Resource::Resource( unsigned long id, const std::string& name, ResourceManager* mgr ) :
mId(id), mName(name),
mMgr(mgr), mPath(""), mState(ResourceState_Created)
{
	zhAssert( mgr != NULL );
}

Resource::~Resource()
{
}

unsigned long Resource::getId() const
{
	return mId;
}

const std::string& Resource::getName() const
{
	return mName;
}

ResourceClass Resource::getClassId() const
{
	return mMgr->getClassId();
}

ResourcePtr Resource::getPtr() const
{
	return mMgr->getResource(mId);
}

ResourceManager* Resource::getManager() const
{
	return mMgr;
}

const std::string& Resource::getPath() const
{
	return mPath;
}

ResourceState Resource::getState() const
{
	return mState;
}

void Resource::_setManager( ResourceManager* mgr )
{
	zhAssert( mgr != NULL );

	mMgr = mgr;
}

void Resource::_setPath( const std::string& path )
{
	mPath = path;
}

void Resource::_setState( ResourceState state )
{
	mState = state;
}

ResourceManager::ResourceManager() : mMemUsage(0)
{
	mMaxMemUsage = std::numeric_limits<std::size_t>::max();
}

ResourceManager::~ResourceManager()
{
	deleteAllResources();

	for( std::vector< ResourceLoader* >::iterator rli = mResourceLoaders.begin();
		rli != mResourceLoaders.end(); ++rli )
		delete *rli;

	for( std::vector< ResourceSerializer* >::iterator rsi = mResourceSerializers.begin();
		rsi != mResourceSerializers.end(); ++rsi )
		delete *rsi;
}

ResourcePtr ResourceManager::createResource( unsigned long id, const std::string& name )
{
	zhAssert( !hasResource(id) && !hasResource(name) );

	ResourcePtr res = _createResource( id, name );
	res->_setManager(this);
	res->_setState( ResourceState_Created );
	mResourcesById.insert( make_pair( id, res ) );
	mResourcesByName.insert( make_pair( name, res ) );

	zhLog( "ResourceManager", "createResource",
		"Creating Resource %d %u, %s.",
		getClassId(), id, name.c_str() );

	return res;
}

void ResourceManager::deleteResource( unsigned long id )
{
	ResourcePtr res = getResource(id);

	if( res == NULL )
		return;

	std::string name = res->getName();

	zhLog( "ResourceManager", "deleteResource",
		"Deleting Resource %d %u, %s.", res->getClassId(), id, name.c_str() );

	if( res->getState() == ResourceState_Loaded )
		unloadResource(id);

	res->_setState( ResourceState_Deleted );
	mResourcesById.erase( mResourcesById.find(id) );
	mResourcesByName.erase( mResourcesByName.find(name) );
}

void ResourceManager::deleteResource( const std::string& name )
{
	ResourcePtr res = getResource(name);

	if( res == NULL )
		return;

	deleteResource( res->getId() );
}

void ResourceManager::deleteAllResources()
{
	zhLog( "ResourceManager", "deleteAllResources",
		"Deleting all resources." );

	ResourceIterator ri = getResourceIterator();
	ResourcePtr res;

	while( !ri.end() )
	{
		res = ri.next();

		if( res->getState() == ResourceState_Loaded )
			unloadResource( res->getId() );

		res->_setState( ResourceState_Deleted );
	}

	mResourcesById.clear();
	mResourcesByName.clear();
}

bool ResourceManager::loadResource( unsigned long id, const std::string& path, bool tryLoad )
{
	zhAssert( hasResource(id) );

	ResourcePtr res = getResource(id);
	std::string name = res->getName();

	zhLog( "ResourceManager", "loadResource",
		"Loading Resource %d %u, %s.", res->getClassId(), id, name.c_str() );

	zhAssert( res->getState() != ResourceState_Deleted ); // can't load a deleted resource
	
	if( res->getState() == ResourceState_Loaded )
		// resource already loaded, unload
		unloadResource(id);

	// first we must determine full resource path
	std::string res_path;
	if( path == "" )
	{
		// reloading an existing resource
		res_path = res->getPath();

		// resource better be in unloaded state...
		zhAssert( res->getState() == ResourceState_Unloaded );

		if( res_path == "" )
			// attempting to reload a resource which was never loaded is a bad idea
			zhAssert(true);
	}
	else
	{
		std::string dir, filename, prefix, ext;
		parsePathStr( path, dir, filename, prefix, ext );

		if( dir != "" )
		{
			// full path specified
			res_path = path;
		}
		else
		{
			// try each of the resource directories
			for( unsigned int rdi = 0; rdi < getNumResourceDirectories(); ++rdi )
			{
				res_path = getResourceDirectory(rdi) + filename;
				
				ifstream tfin( res_path.c_str() );
				if( tfin.is_open() )
				{
					// found the resource file, can open it
					break;
				}
				res_path = "";
			}

			if( res_path == "" )
			{
				zhLog( "ResourceManager", "loadResource",
					"ERROR: Failed to load resource file %s for Resource %d %u, %s. File not found.",
					filename.c_str(), res->getClassId(), id, name.c_str() );

				zhSetErrorCode( ResourceMgrError_FileNotFound );
				return false;
			}
		}
	}

	// now we can load the resource
	for( std::vector< ResourceLoader* >::iterator rli = mResourceLoaders.begin();
		rli != mResourceLoaders.end(); ++rli )
	{
		if( (*rli)->tryLoad( res, res_path ) )
		{
			res->_setPath( res_path );

			if(tryLoad)
			{
				res->_setState( ResourceState_Unloaded );
				return true;
			}
			
			if( (*rli)->load( res, res_path ) )
			{
				res->_setState( ResourceState_Loaded );
				break;
			}
		}
	}

	if( res->getState() != ResourceState_Loaded )
	{
		zhLog( "ResourceManager", "loadResource",
			"ERROR: Failed to load resource file %s for Resource %d %u, %s. ResourceLoader not found.",
			res_path.c_str(), res->getClassId(), id, name.c_str() );
		
		zhSetErrorCode( ResourceMgrError_LoaderNotFound );
		return false;
	}

	// check memory usage
	size_t mem_usage = res->_calcMemoryUsage();
	if( !_updateMemoryUsage(mem_usage) )
	{
		// memory budget exceeded, unload
		res->_unload();
		res->_setState( ResourceState_Unloaded );

		zhLog( "ResourceManager", "loadResource",
			"ERROR: Not enough memory to load resource file %s for Resource %d %u, %s. Resource requires: %u. Available: %u / %u.",
			res_path.c_str(), res->getClassId(), id, name.c_str(),
			mem_usage, getMaxMemoryUsage() - getMemoryUsage(), getMaxMemoryUsage() );

		zhSetErrorCode( ResourceMgrError_OutOfMemory );
		return false;
	}

	zhSetErrorCode( ResourceMgrError_None );
	return true;
}

bool ResourceManager::loadResource( const std::string& name, const std::string& path, bool tryLoad )
{
	zhAssert( hasResource(name) );

	ResourcePtr res = getResource(name);

	return loadResource( res->getId(), path, tryLoad );
}


void ResourceManager::unloadResource( unsigned long id )
{
	zhAssert( hasResource(id) );

	ResourcePtr res = getResource(id);

	zhLog( "ResourceManager", "unloadResource",
		"Unloading Resource %d %u, %s.",
		res->getClassId(), id, res->getName().c_str() );

	if( res->getState() == ResourceState_Loaded )
	{
		_updateMemoryUsage( res->_calcMemoryUsage(), true );

		res->_unload();
		res->_setState( ResourceState_Unloaded );
	}

	return;
}

void ResourceManager::unloadResource( const std::string& name )
{
	zhAssert( hasResource(name) );

	ResourcePtr res = getResource(name);
	unloadResource( res->getId() );

	return;
}

void ResourceManager::unloadAllResources()
{
	zhLog( "ResourceManager", "unloadAllResources",
		"Unloading all resources." );

	ResourceIterator ri = getResourceIterator();
	ResourcePtr res;

	while( !ri.end() )
	{
		res = ri.next();

		if( res->getState() == ResourceState_Loaded )
			unloadResource( res->getId() );
	}
}

bool ResourceManager::serializeResource( unsigned long id, const std::string& path )
{
	zhAssert( hasResource(id) );

	ResourcePtr res = getResource(id);
	std::string name = res->getName();

	zhLog( "ResourceManager", "serializeResource",
		"Serializing Resource %d %u, %s.", res->getClassId(), id, name.c_str() );

	zhAssert( res->getState() == ResourceState_Created ||
		res->getState() == ResourceState_Loaded );

	// first we must determine full resource path

	std::string new_path;

	if( path == "" )
	{
		// serializing to current path
		new_path = res->getPath();

		if( new_path == "" ) // attempting to reserialize a resource which was never loaded is a bad idea
			zhAssert(true);
	}
	else
	{
		std::string new_dir, new_filename, new_prefix, new_ext;
		parsePathStr( path, new_dir, new_filename, new_prefix, new_ext );

		if( new_dir != "" )
		{
			// full path specified
			new_path = path;
		}
		else
		{
			// use current directory
			std::string dir, filename, prefix, ext;
			parsePathStr( res->getPath(), dir, filename, prefix, ext );
			new_path = dir + path;

			if( new_path == "" ) // attempting to reserialize a resource which was never loaded is a bad idea
				zhAssert(true);
		}
	}

	// now we can serialize the resource

	bool success = false;
	for( std::vector< ResourceSerializer* >::iterator rsi = mResourceSerializers.begin();
		rsi != mResourceSerializers.end(); ++rsi )
	{
		if( (*rsi)->trySerialize( res, new_path ) && (*rsi)->serialize( res, new_path ) )
		{
			res->_setPath( new_path );
			res->_setState( ResourceState_Loaded );
			success = true;

			break;
		}
	}

	if( !success )
	{
		zhLog( "ResourceManager", "serializeResource",
			"ERROR: Failed to serialize resource file %s for Resource %d %u, %s. ResourceSerializer not found.",
			new_path.c_str(), res->getClassId(), id, name.c_str() );

		zhSetErrorCode( ResourceMgrError_SerializerNotFound );
		return false;
	}

	zhSetErrorCode( ResourceMgrError_None );
	return true;
}

bool ResourceManager::serializeResource( const std::string& name, const std::string& path )
{
	zhAssert( hasResource(name) );

	ResourcePtr res = getResource(name);

	return serializeResource( res->getId(), path );
}

ResourcePtr ResourceManager::cloneResource( unsigned long id,
										   unsigned long cloneId, const std::string& cloneName )
{
	zhAssert( hasResource(id) );

	ResourcePtr res = getResource(id);
	std::string name = res->getName();

	zhLog( "ResourceManager", "cloneResource",
		"Cloning Resource %d %u, %s.", res->getClassId(), id, name.c_str() );

	zhAssert( cloneId != id && cloneName != name );
	zhAssert( res->getState() == ResourceState_Loaded );

	ResourcePtr cres = createResource( cloneId, cloneName );
	res->_clone( cres.getRawPtr() );

	size_t mem_usage = cres->_calcMemoryUsage();
	if( !_updateMemoryUsage(mem_usage) )
	{
		// memory budget exceeded, delete
		deleteResource(cloneId);

		zhLog( "ResourceManager", "loadResource",
			"ERROR: Not enough memory to clone Resource %d %u, %s. Resource requires: %u. Available: %u / %u.",
			res->getClassId(), id, name.c_str(), mem_usage,
			getMaxMemoryUsage() - getMemoryUsage(), getMaxMemoryUsage() );

		zhSetErrorCode( ResourceMgrError_OutOfMemory );
		return false;
	}

	cres->_setState( ResourceState_Loaded );
	cres->_setPath( res->getPath() );

	zhSetErrorCode( ResourceMgrError_None );
	return cres;
}

ResourcePtr ResourceManager::cloneResource( const std::string& name,
										   unsigned long cloneId, const std::string& cloneName )
{
	zhAssert( hasResource(name) );

	ResourcePtr res = getResource(name);

	return cloneResource( res->getId(), cloneId, cloneName );
}

bool ResourceManager::hasResource( unsigned long id ) const
{
	return mResourcesById.count(id) > 0;
}

bool ResourceManager::hasResource( const std::string& name ) const
{
	return mResourcesByName.count(name) > 0;
}

ResourcePtr ResourceManager::getResource( unsigned long id ) const
{
	std::map<unsigned long, ResourcePtr>::const_iterator ri = mResourcesById.find(id);
	if( ri != mResourcesById.end() )
		return ri->second;

	return NULL;
}

ResourcePtr ResourceManager::getResource( const std::string& name ) const
{
	std::map<std::string, ResourcePtr>::const_iterator ri = mResourcesByName.find(name);
	if( ri != mResourcesByName.end() )
		return ri->second;

	return NULL;
}

ResourceManager::ResourceIterator ResourceManager::getResourceIterator()
{
	return ResourceIterator( mResourcesById );
}

ResourceManager::ResourceConstIterator ResourceManager::getResourceConstIterator() const
{
	return ResourceConstIterator( mResourcesById );
}

unsigned int ResourceManager::getNumResources() const
{
	return mResourcesById.size();
}

void ResourceManager::addResourceDirectory( const std::string& dir )
{
	mResourceDirs.push_back( processDirStr(dir) );
}

void ResourceManager::removeResourceDirectory( unsigned int dirIndex )
{
	zhAssert( dirIndex < mResourceDirs.size() );

	mResourceDirs.erase( mResourceDirs.begin() + dirIndex );
}

void ResourceManager::removeAllResourceDirectories()
{
	mResourceDirs.clear();
}

const std::string& ResourceManager::getResourceDirectory( unsigned int dirIndex ) const
{
	zhAssert( dirIndex < mResourceDirs.size() );

	return mResourceDirs[dirIndex];
}

unsigned int ResourceManager::getNumResourceDirectories() const
{
	return mResourceDirs.size();
}

bool ResourceManager::registerLoader( unsigned long classId, const std::string& className,
									 ResourceLoader*( *creatorFunc )( const unsigned long& ) )
{
	zhLog( "ResourceManager", "registerLoader",
		"Registering ResourceLoader %u, %s.",
		classId, className.c_str() );

	if( !mResourceLoaderFactory.registerClass( classId, className, creatorFunc ) )
	{
		zhLog( "ResourceManager", "registerLoader",
			"ERROR: ResourceLoader %u, %s already registered.",
			classId, className.c_str() );

		zhSetErrorCode( ResourceMgrError_ClassRegistered );
		return false;
	}

	mResourceLoaders.push_back(
		mResourceLoaderFactory.createObject( classId, classId )
		);

	zhSetErrorCode( ResourceMgrError_None );
	return true;
}

bool ResourceManager::registerSerializer( unsigned long classId, const std::string& className,
										 ResourceSerializer*( *creatorFunc )( const unsigned long& ) )
{
	zhLog( "ResourceManager", "registerSerializer",
		"Registering ResourceSerializer %u, %s.",
		classId, className.c_str() );

	if( !mResourceSerializerFactory.registerClass( classId, className, creatorFunc ) )
	{
		zhLog( "ResourceManager", "registerSerializer",
			"ERROR: ResourceSerializer %u, %s already registered.",
			classId, className.c_str() );

		zhSetErrorCode( ResourceMgrError_ClassRegistered );
		return false;
	}

	mResourceSerializers.push_back(
		mResourceSerializerFactory.createObject( classId, classId )
		);

	zhSetErrorCode( ResourceMgrError_None );
	return true;
}

void ResourceManager::unregisterLoader( unsigned long classId )
{
	zhLog( "ResourceManager", "registerLoader",
		"Unregistering ResourceLoader %u.",
		classId );

	for( unsigned int rli = 0; rli < mResourceLoaders.size(); ++rli )
	{
		if( mResourceLoaders[rli]->getId() == classId )
		{
			delete mResourceLoaders[rli];
			mResourceLoaders.erase( mResourceLoaders.begin() + rli );
		}
	}

	mResourceLoaderFactory.unregisterClass(classId);
}

void ResourceManager::unregisterSerializer( unsigned long classId )
{
	zhLog( "ResourceManager", "registerSerializer",
		"Unregistering ResourceSerializer %u.",
		classId );

	for( unsigned int rsi = 0; rsi < mResourceSerializers.size(); ++rsi )
	{
		if( mResourceSerializers[rsi]->getId() == classId )
		{
			delete mResourceSerializers[rsi];
			mResourceSerializers.erase( mResourceSerializers.begin() + rsi );
		}
	}

	mResourceSerializerFactory.unregisterClass(classId);
}

size_t ResourceManager::getMemoryUsage() const
{
	return mMemUsage;
}

size_t ResourceManager::getMaxMemoryUsage() const
{
	return mMaxMemUsage;
}

void ResourceManager::setMaxMemoryUsage( size_t amount )
{
	mMaxMemUsage = amount >= mMemUsage ? amount : mMemUsage;
	// TODO: handle case when amount < mMemUsage better
}

bool ResourceManager::_updateMemoryUsage( size_t amount, bool subtract )
{
	size_t mem_usage =
		subtract ? mMemUsage - amount :
		mMemUsage + amount;

	if( mem_usage > mMaxMemUsage )
		return false;
	else
		mMemUsage = mem_usage;

	return true;
}

}
