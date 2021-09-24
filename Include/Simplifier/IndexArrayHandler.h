
#ifdef _WIN32
#ifdef SAAS
#include <SimplygonSDKCloudLoader.h>
#else
#include <Simplifier/SimplygonLoader.h>
#endif
#elif defined(__linux__)
#include <SimplygonSDKNixLoader.h>
#endif

#include "Vector4D.h"
#include <map>
#include <iostream>

// Takes an array that contains a vector (2d,3d or 4d) for each geometry-element (mainly triangle, or trianglecorner)
// and creates a new array with all unique vectors, and another new array with indexes for each element
// to this array of unique vectors.
inline bool CreateIndexArray(spRealArray sourcevalues, spRealArray targetvalues, spRidArray targetindexes)
{
	/*
	Array source contains #tuplecount tuples for each element.
	Tuplesize is for example for texturecoordinates 2, for normals 3 and for colors 4.
	Tuplecount is for example #triangles in the Geometry, or #triangles*3 for each corner in the Geometry.
	*/

	unsigned int tuplesize = sourcevalues->GetTupleSize();
	unsigned int tuplecount = sourcevalues->GetTupleCount();

	// If for some reason matrices are used, this can be re-set to 9 or 16 for example
	// However: the Vector4d class would have to be rewritten to handle more than 4 values.
	if (tuplesize < 1 || tuplesize > 4)
	{
		std::cerr << "Wrong tuplesize";
		return false;
	}

	if (tuplecount < 1)
	{
		std::cerr << "Illegal tuplecount";
		return false;
	}

	targetindexes->Clear();
	targetvalues->Clear();

	targetindexes->SetTupleSize(1);
	targetindexes->SetTupleCount(tuplecount);


	std::map< Vector4D, unsigned int, std::less<Vector4D> > HashMap;

	Vector4D tempVec;
	tempVec[0] = 0.0;
	tempVec[1] = 0.0;
	tempVec[2] = 0.0;
	tempVec[3] = 0.0;
	unsigned int index = 0;

	// Find all unique tuples
	for (unsigned int i = 0; i < tuplecount; i++)
	{
		// Read in the tuple
		for (unsigned int t = 0; t < tuplesize; t++)
		{
			tempVec[t] = sourcevalues->GetRealItem(i * tuplesize + t);
		}

		//If we already had a tuple similar to the current one, use that one instead for the index for the current corner
		if (HashMap.find(tempVec) != HashMap.end())
		{
			targetindexes->SetItem(i, HashMap[tempVec]);
		}
		else //If no tuple alread loaded is similar, then add this tuple, and use it for index for the current corner
		{
			//To know where this value was found in the sourcevalues-array
			tempVec.index = i;
			HashMap[tempVec] = index;
			targetindexes->SetItem(i, index);
			index++;
		}
	}

	targetvalues->SetTupleSize(tuplesize);
	targetvalues->SetTupleCount((unsigned int)(HashMap.size()));

	for (std::map <Vector4D, unsigned int, std::less<Vector4D> >::const_iterator iter = HashMap.begin();
		iter != HashMap.end();
		iter++)
	{
		tempVec = iter->first;
		index = iter->second;

		// Copies the tuple from sourcevalues at tempVec.index, to targetvalues at index.
		targetvalues->CopyTuple(sourcevalues, index, tempVec.index);
	}

	return true;
}

// Takes an array that contains a vector (2d,3d or 4d) for each geometry-element (mainly triangle, or trianglecorner)
// and creates a new array with all unique vectors, and another new array with indexes for each element
// to this array of unique vectors.
inline bool CreateIndexArray(IValueArray* sourcevalues, IValueArray* targetvalues, IRidArray* targetindexes)
{
	/*
	Array source contains #tuplecount tuples for each element.
	Tuplesize is for example for texturecoordinates 2, for normals 3 and for colors 4.
	Tuplecount is for example #triangles in the Geometry, or #triangles*3 for each corner in the Geometry.
	*/

	unsigned int tuplesize = sourcevalues->GetTupleSize();
	unsigned int tuplecount = sourcevalues->GetTupleCount();

	// If for some reason matrices are used, this can be re-set to 9 or 16 for example
	// However: the Vector4d class would have to be rewritten to handle more than 4 values.
	if (tuplesize < 1 || tuplesize > 4)
	{
		std::cerr << "Wrong tuplesize";
		return false;
	}

	if (tuplecount < 1)
	{
		std::cerr << "Illegal tuplecount";
		return false;
	}

	targetindexes->Clear();
	targetvalues->Clear();

	targetindexes->SetTupleSize(1);
	targetindexes->SetTupleCount(tuplecount);


	std::map< Vector4D, unsigned int, std::less<Vector4D> > HashMap;

	Vector4D tempVec;
	tempVec[0] = 0.0;
	tempVec[1] = 0.0;
	tempVec[2] = 0.0;
	tempVec[3] = 0.0;
	unsigned int index = 0;

	// Find all unique tuples
	for (unsigned int i = 0; i < tuplecount; i++)
	{
		// Read in the tuple
		for (unsigned int t = 0; t < tuplesize; t++)
		{
			tempVec[t] = sourcevalues->GetRealItem(i * tuplesize + t);
		}

		//If we already had a tuple similar to the current one, use that one instead for the index for the current corner
		if (HashMap.find(tempVec) != HashMap.end())
		{
			targetindexes->SetItem(i, HashMap[tempVec]);
		}
		else //If no tuple alread loaded is similar, then add this tuple, and use it for index for the current corner
		{
			//To know where this value was found in the sourcevalues-array
			tempVec.index = i;
			HashMap[tempVec] = index;
			targetindexes->SetItem(i, index);
			index++;
		}
	}

	targetvalues->SetTupleSize(tuplesize);
	targetvalues->SetTupleCount((unsigned int)(HashMap.size()));

	for (std::map <Vector4D, unsigned int, std::less<Vector4D> >::const_iterator iter = HashMap.begin();
		iter != HashMap.end();
		iter++)
	{
		tempVec = iter->first;
		index = iter->second;

		// Copies the tuple from sourcevalues at tempVec.index, to targetvalues at index.
		targetvalues->CopyTuple(sourcevalues, index, tempVec.index);
	}

	return true;
}

inline bool CreateIndependentArray(IValueArray* sourcevalues, IRidArray* sourceindexes, IValueArray* targetarray)
{
	unsigned int tuplecount = sourceindexes->GetTupleCount();

	targetarray->Clear();
	targetarray->SetTupleSize(sourcevalues->GetTupleSize());
	targetarray->SetTupleCount(tuplecount);

	if (tuplecount < 1)
	{
		std::cerr << "Illegal tuplecount";
		return false;
	}

	unsigned int sourceindex;
	for (unsigned int i = 0; i < tuplecount; i++)
	{
		sourceindex = sourceindexes->GetItem(i);
		targetarray->CopyTuple(sourcevalues, i, sourceindex);
	}
	return true;
}
