#ifndef _SIMPLIFIER_H_
#define _SIMPLIFIER_H_
#include "Constants.h"

#include "Simplygon.h"
#include "SimplygonLoader.h"

#include "MeshBase/MeshBase.h"

class AI3D_API Simplifier {
public:

	Simplifier();
	~Simplifier();


	//µ¥Àý
	int simplify_obj(std::string& input_file, std::string& output_file, double mesh_reduction);
	int simplify_obj(std::string& input_file, std::string& output_file, double mesh_reduction, MeshBase* mesh_base);
private:
	void MultiLayeredTextureMapping(const std::float_t Re_Ratio, const std::string& readFrom, const std::vector<std::string> diffuseBaseFile_, const std::string& rootpath, const std::string& writeTo, const uint32_t repair_flag=0);
	void AddSimplygonTexture(Simplygon::spMaterial mat, Simplygon::spTextureTable texTable, const char* channelName, const char* texFilePath, const char* namePrefix = "");
	Simplygon::ISimplygon* sg = NULL;



private:

};

#endif//_SIMPLIFIER_H_