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

#ifndef __zhObjectFactory_h__
#define __zhObjectFactory_h__

#include "zhPrereq.h"
#include "zhIterators.h"

namespace zh
{

#define zhClass_NULL 0xFFFFFFFF

#define zhDeclare_BaseClass( BaseClass, classId, className, IdType ) \
protected: \
	IdType mId; \
public: \
	static unsigned long ClassId() { return classId; } \
	static const std::string& ClassName() { static std::string cln = className; return cln; } \
	const IdType& getId() const { return mId; } \
	virtual unsigned long getClassId() const = 0; \
	virtual const std::string& getClassName() const = 0; \
	virtual bool isClass( unsigned long cId ) const { return cId == ClassId(); } \
	virtual bool isClass( const std::string& cName ) const { return cName == ClassName(); }

#define zhDeclare_Class( BaseClass, ThisClass, classId, className, IdType ) \
public: \
	static unsigned long ClassId() { return classId; } \
	static const std::string& ClassName() { static std::string cln = className; return cln; } \
	static unsigned long SuperClassId() { return BaseClass::ClassId(); } \
	static const std::string& SuperClassName() { return BaseClass::ClassName(); } \
	static BaseClass* Create( const IdType& id ) { ThisClass* obj = new ThisClass(); obj->mId = id; return obj; } \
	unsigned long getClassId() const { return ClassId(); } \
	const std::string& getClassName() const { return ClassName(); } \
	unsigned long getSuperClassId() { return BaseClass::ClassId(); } \
	const std::string& getSuperClassName() { return BaseClass::ClassName(); } \
	bool isClass( unsigned long cId ) const { return cId == ClassId() || BaseClass::isClass(cId); } \
	bool isClass( const std::string& cName ) const { return cName == ClassName() || BaseClass::isClass(cName); }

/**
* Registers the specified class.
* Returns false if the class is already registered, true otherwise.
*/
#define zhRegister_Class( objFactory, BaseClass, NewClass ) \
	objFactory->registerClass( NewClass::ClassId(), NewClass::ClassName(), &NewClass::Create )

/**
* Unregisters the specified class.
*/
#define zhUnregister_Class( objFactory, BaseClass, RegClass ) \
	objFactory->unregisterClass( RegClass::ClassId() )

/**
* @brief Simple object factory class template.
*
* ObjectFactory class template is a simple implementation of object factory
* design pattern. Object factories are used throughout ZombieHorse and form
* the cornerstone of its extension mechanisms.
*
* Suppose you have an existing object factory implementation called
* ShapeFactory that produces objects of base class Shape. You want
* to implement a new derived class of Shape called Circle and register
* it with ShapeFactory. You can implement the subclass as follows:
*
* \code
* class Circle : public Shape
* {
* 
* 	zhDeclare_Class( Shape, Circle, 1, "Circle", unsigned short ) // class ID is 1 in this example, but it can be anything except zhClass_NULL
*
* 	Circle()
* 	{
* 		// implement constructor as normal
* 	}
*
* 	~Circle()
* 	{
* 		// implement destructor as normal
* 	}
* 
*	// all class members specific to Circle go here
* 
* };
* \endcode
*
* To register the new class, put this somewhere in your code
* (typically a plug-in initialization method):
* 
* \code
* zhRegister_Class( ShapeFactory::Instance(), Shape, Circle ) // here we assume ShapeFactory is Singleton
* \endcode
*
* Rarely you may want to unregister a derived class. This can be done
* as follows:
*
* \code
* zhUnregister_Class( ShapeFactory::Instance(), Shape, Circle ) // here we assume ShapeFactory is Singleton
* \endcode
*
* Sometimes you may want to implement an object factory. Suppose you have
* a base class Vehicle and you want to implement an object factory for
* objects of its derived classes. Define the class Vehicle as follows:
*
* \code
* class Vehicle
* {
*
* public:
* 
* 	zhDeclare_BaseClass( Vehicle, 0, "Vehicle", unsigned short )
* 
* 	Vehicle()
*	{
*		// default constructor must be defined
*	}
*
*   virtual ~Vehicle()
*	{
*		// you will probably want to define a virtual destructor
*	}
*	
*	// all class members specific to Vehicle go here
* 
* };
* \endcode
*
* Next, implement the VehicleFactory. There are many ways you can go about
* doing that, what follows is just one possibility:
*
* \code
* class VehicleFactory : public ObjectFactory<Vehicle>,
* 	public Singleton<VehicleFactory> // note that VehicleFactory doesn't *have* to be singleton
* {
* 
* public:
* 
*	VehicleFactory()
* 	{
* 	}
* 
* 	~VehicleFactory()
* 	{
* 		for( std::map<unsigned short, Vehicle*>::iterator shi = mVehiclesById.begin();
* 			shi != mVehiclesById.end(); ++shi )
* 		{
* 			delete shi->second;
* 		}
* 
* 		mVehiclesById.clear();
* 		mVehiclesByName.clear();
* 	} 
* 
* 	Vehicle* createVehicle( unsigned long classId, unsigned short id, const std::string& name )
* 	{
* 		if( mVehiclesById.count(id) != 0 ||
* 			mVehiclesByName.count(name) != 0 )
* 			return NULL;
* 
* 		Vehicle* obj = createObject<Vehicle>( classId, id, name );
* 		if( obj == NULL )
* 			return NULL;
* 		mVehiclesById[id] = obj;
* 		mVehiclesByName[name] = obj;
* 		
* 		return obj;
* 	}
* 
* 	// TODO: functions for deleting vehicles, getting them by ID and name
*	// getting iterators over vehicle maps etc.
* 
* private:
* 
* 	std::map<unsigned short, Vehicle*> mVehiclesById;
* 	std::map<string, Vehicle*> mVehiclesByName;
* 
* };
* \endcode
*/
template <class T, typename IdT = unsigned long>
class ObjectFactory
{

public:

	typedef MapIterator< std::map< unsigned long, T*(*)( const IdT& ) > >
		ClassIterator;
	typedef MapConstIterator< std::map< unsigned long, T*(*)( const IdT& ) > >
		ClassConstIterator;

	ObjectFactory()
	{
	}

	virtual ~ObjectFactory()
	{
	}

	T* createObject( unsigned long classId, const IdT& id )
	{
		zhAssert( isClassRegistered(classId) );

		return mObjectCreators[classId](id);
	}

	bool registerClass( unsigned long classId, const std::string& className,
		T*( *creatorFunc )( const IdT& ) )
	{
		zhAssert( className != "" );

		if( isClassRegistered(classId) )
			return false;

		mObjectCreators.insert( make_pair( classId, creatorFunc ) );
		mClassNames.insert( make_pair( classId, className ) );

		return true;
	}

	void unregisterClass( unsigned long classId )
	{
		if( !isClassRegistered(classId) )
			return;

		mObjectCreators.erase( mObjectCreators.find(classId) );
		mClassNames.erase( mClassNames.find(classId) );
	}

	ClassIterator getClassIterator() const
	{
		return ClassIterator(mObjectCreators);
	}

	ClassConstIterator getClassConstIterator() const
	{
		return ClassConstIterator(mObjectCreators);
	}

	bool isClassRegistered( unsigned long classId ) const
	{
		return mObjectCreators.find(classId) != mObjectCreators.end();
	}

	unsigned long findClassIdByName( const std::string& className )
	{
		for( std::map<unsigned long, std::string>::const_iterator clni = mClassNames.begin();
			clni != mClassNames.end(); ++clni )
		{
			if( clni->second == className )
				return clni->first;
		}

		return 0xFFFFFFFF;
	}

private:

	std::map<unsigned long, T*(*)( const IdT& )> mObjectCreators;
	std::map<unsigned long, std::string> mClassNames;

};

}

#endif // __zhObjectFactory_h__
