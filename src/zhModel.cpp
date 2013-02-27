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

#include "zhModel.h"

namespace zh
{

const Model::Situation Model::Situation::Identity = Model::Situation();

Model::Situation::Situation()
{
}

Model::Situation::Situation( float posX, float posZ, float orientY )
{
	mPos = Vector3( posX, 0, posZ );
	mOrient = Quat( Vector3::YAxis, orientY );
}

Model::Situation::Situation( const Vector3& pos, const Quat& orient )
: mPos(pos), mOrient(orient)
{
}

Vector3 Model::Situation::getPosition() const
{
	return mPos;
}

void Model::Situation::setPosition( const Vector3& pos )
{
	mPos = pos;
}

Quat Model::Situation::getOrientation() const
{
	return mOrient;
}

void Model::Situation::setOrientation( const Quat& orient )
{
	mOrient = orient;
}

float Model::Situation::getPosX() const
{
	return mPos.x;
}

float Model::Situation::getPosZ() const
{
	return mPos.z;
}

float Model::Situation::getOrientY() const
{
	float ax, ay, az;
	mOrient.getEuler( ax, ay, az );
	
	return ay;
}

Model::Situation& Model::Situation::invert()
{
	mPos = -mPos;
	mOrient.invert();

	return *this;
}

Model::Situation Model::Situation::getInverse() const
{
	Situation sit = *this;
	sit.invert();

	return sit;
}

Model::Situation& Model::Situation::transform( const Situation& transf )
{
	mPos += transf.mPos.getRotated(mOrient);
	mOrient *= transf.mOrient;

	return *this;
}

Model::Situation Model::Situation::getTransformed( const Situation& transf ) const
{
	Situation sit = *this;
	sit.transform(transf);

	return sit;
}

Model::Situation Model::Situation::getTransformTo( const Situation& sit ) const
{
	Quat inv_orient = mOrient.getInverse();
	Situation transf;
	transf.mOrient = inv_orient * sit.mOrient;
	transf.mPos = sit.mPos - mPos;
	transf.mPos.rotate(inv_orient);

	return transf;
}

const Model::Situation& Model::Situation::projectToGround()
{
	mPos.y = 0;
	float ax, ay, az;
	mOrient.getEuler( ax, ay, az );
	mOrient = Quat( 0, ay, 0 );

	return *this;
}

Model::Situation Model::Situation::getProjToGround() const
{
	Situation sit = *this;
	sit.projectToGround();

	return sit;
}

Model::Model() : mSkel(0)
{
	mSkel = new Skeleton();
}

Model::~Model()
{
	delete mSkel;
	deleteAllMeshes();
}

Skeleton* Model::getSkeleton() const
{
	return mSkel;
}

Model::Situation Model::getSituation() const
{
	Bone* root = mSkel->getRoot();

	if( root == NULL )
		return Model::Situation();

	return Model::Situation( root->getPosition(), root->getOrientation() )/*.getProjToGround()*/;
}

void Model::setSituation( const Situation& sit )
{
	Bone* root = mSkel->getRoot();

	if( root != NULL )
	{
		root->setPosition( sit.getPosition() );
		root->setOrientation( sit.getOrientation() );
	}
}

Mesh* Model::createMesh( unsigned short id, const std::string& name )
{
	zhAssert( !hasMesh(id) && !hasMesh(name) );

	Mesh* mesh = new Mesh( id, name );
	mMeshesById[id] = mesh;
	mMeshesByName[name] = mesh;

	return mesh;
}

void Model::deleteMesh( unsigned short id )
{
	Mesh* mesh = getMesh(id);

	if( mesh == NULL )
		return;

	mMeshesById.erase(id);
	mMeshesByName.erase( mesh->getName() );
	delete mesh;
}

void Model::deleteMesh( const std::string& name )
{
	Mesh* mesh = getMesh(name);

	if( mesh == NULL )
		return;

	mMeshesById.erase( mesh->getId() );
	mMeshesByName.erase(name);
	delete mesh;
}

void Model::deleteAllMeshes()
{
	for( std::map<unsigned short, Mesh*>::iterator mi = mMeshesById.begin();
		mi != mMeshesById.end(); ++mi )
		delete mi->second;

	mMeshesById.clear();
	mMeshesByName.clear();
}

bool Model::hasMesh( unsigned short id )
{
	return mMeshesById.count(id) > 0;
}

bool Model::hasMesh( const std::string& name )
{
	return mMeshesByName.count(name) > 0;
}

Mesh* Model::getMesh( unsigned short id )
{
	std::map<unsigned short, Mesh*>::const_iterator mi = mMeshesById.find(id);

	if( mi != mMeshesById.end() )
		return mi->second;

	return NULL;
}

Mesh* Model::getMesh( const std::string& name )
{
	std::map<std::string, Mesh*>::const_iterator mi = mMeshesByName.find(name);

	if( mi != mMeshesByName.end() )
		return mi->second;

	return NULL;
}

unsigned int Model::getNumMeshes() const
{
	return mMeshesById.size();
}

Model::MeshIterator Model::getMeshIterator()
{
	return MeshIterator( mMeshesById );
}

Model::MeshConstIterator Model::getMeshConstIterator() const
{
	return MeshConstIterator( mMeshesById );
}

}
