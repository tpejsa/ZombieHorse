#include"zhBVHLoader.h"
#include "zhBoneAnimationTrack.h"
#include "zhMeshAnimationTrack.h"
#include "zhString.h"
#include "zhFileSystem.h"
#include "boost\regex.hpp"
#include <sstream>
namespace zh
{
	bool BVHLoader::tryLoad( ResourcePtr res, const std::string& path )
	{
		zhAssert( res != NULL );
		zhAssert( res->getClassId() == Resource_AnimationSet );

		std::string dir, filename, prefix, ext;
		parsePathStr( path, dir, filename, prefix, ext );

		if( ext != "bvh" )
			return false;

		return true;
	}
	bool BVHLoader::load( ResourcePtr res, const std::string& path )
	{
		zhAssert( res != NULL );
		zhAssert( res->getClassId() == Resource_AnimationSet );

		mAnimSet = AnimationSetPtr::DynamicCast<Resource>(res);
		mAnim = NULL;
		mPath = path;

		// open the ZHA file as binary
		FILE* bvhf = fopen( path.c_str(), "rb" );
		if( bvhf == NULL )
		{
			zhLog( "BVHLoader", "load", "ERROR: Failed to open BVH file %s for reading.",
				path.c_str() );
			return false;
		}

		// load file contents into memory
		int bvhf_size;
		char* bvhf_content = NULL;
		fseek( bvhf, 0, SEEK_END );
		bvhf_size = ftell(bvhf);
		fseek( bvhf, 0, SEEK_SET );
		bvhf_content = ( char* )malloc(bvhf_size+1 );
		if(bvhf_size != fread(bvhf_content, sizeof(char), bvhf_size, bvhf ) )
		{
			// error reading file
			zhLog( "BVHLoader", "load", "ERROR: Failed to read BVH file %s.",
				path.c_str() );

			free(bvhf_content);
			fclose(bvhf);
			return false;
		}
		bvhf_content[bvhf_size] = 0;
		fclose(bvhf);
		//parsing!!
		const boost::regex fileStructure("HIERARCHY[:space:]*ROOT[:space:]*[:alnum:]");
		// done parsing, free memory
		free(bvhf_content);
		return result;
	}
}