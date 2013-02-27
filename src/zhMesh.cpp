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

#include "zhMesh.h"

namespace zh
{

Mesh::Mesh( unsigned short id, const std::string& name ) : mId(id), mName(name)
{
}

Mesh::~Mesh()
{
}

unsigned short Mesh::getId() const
{
	return mId;
}

const std::string& Mesh::getName() const
{
	return mName;
}

void Mesh::createMorphTarget( const std::string& name )
{
	mMTNames.insert( make_pair( name, getNumMorphTargets() ) );

	Vector weights( mMTNames.size() );
	for( unsigned int i = 0; i < mMTWeights.size(); ++i )
	{
		weights.set( i, mMTWeights.get(i) );
	}
	mMTWeights = weights;
}

void Mesh::deleteMorphTarget( const std::string& name )
{
	if( mMTNames.count(name) <= 0 )
		return;

	unsigned int i = mMTNames.find(name)->second;
	mMTNames.erase(name);

	Vector weights( mMTNames.size() );
	for( unsigned int j = 0; j < mMTWeights.size(); ++j )
	{
		if( j < i )
			weights.set( j, mMTWeights.get(j) );
		else if( j > i )
			weights.set( j - 1, mMTWeights.get(j) );
		else
			continue;
	}
	mMTWeights = weights;
}

void Mesh::deleteMorphTarget( unsigned int index )
{
	zhAssert( index < getNumMorphTargets() );

	std::string name;
	for( std::map<std::string, unsigned int>::iterator mti = mMTNames.begin();
		mti != mMTNames.end(); ++mti )
	{
		if( mti->second == index )
		{
			name = mti->first;
			break;
		}
	}

	unsigned int i = index;
	mMTNames.erase(name);

	Vector weights( mMTNames.size() );
	for( unsigned int j = 0; j < mMTWeights.size(); ++j )
	{
		if( j < i )
			weights.set( j, mMTWeights.get(j) );
		else if( j > i )
			weights.set( j - 1, mMTWeights.get(j) );
		else
			continue;
	}
	mMTWeights = weights;
}

void Mesh::deleteAllMorphTargets()
{
	mMTNames.clear();
	mMTWeights = Vector();
}

bool Mesh::hasMorphTarget( const std::string& name ) const
{
	return mMTNames.count(name) > 0;
}

unsigned int Mesh::getNumMorphTargets() const
{
	return mMTWeights.size();
}

float Mesh::getMorphTargetWeight( const std::string& name ) const
{
	zhAssert( hasMorphTarget(name) );

	return mMTWeights.get( mMTNames.find(name)->second );
}

void Mesh::setMorphTargetWeight( const std::string& name, float weight )
{
	zhAssert( hasMorphTarget(name) );

	mMTWeights.set( mMTNames.find(name)->second, weight );
}

const Vector& Mesh::getMorphTargetWeights() const
{
	return mMTWeights;
}

void Mesh::setMorphTargetWeights( const Vector& weights )
{
	zhAssert( weights.size() == getNumMorphTargets() );

	mMTWeights = weights;
}

}
