///////////////////////////////////////////////////////////////////////////
//
//  System:    Simplygon
//  File:      WavefrontExample.cpp
//  Language:  C++
//
//  Copyright (c) 2019 Microsoft. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////
//
//	#Description# 
//
//  This example shows how to manually import/export geometry
//	data to/from Simplygon. The example implements a wavefront importer/
//	exporter, and also a per-corner data indexer/packer, to remove duplicate
//	data items when outputting to a wavefront file.
//
///////////////////////////////////////////////////////////////////////////

#include "Simplifier/Example.h"
#include "Simplifier/OBJReader.h"
#include "Simplifier/IndexArrayHandler.h"
#include "Simplifier/Simplifier.h"

#include "boost/filesystem.hpp"

#include <osg/Group>
#include <osg/Notify>
#include <osg/Geometry>
#include <osg/ArgumentParser>
#include <osg/ApplicationUsage>
#include <osg/Texture2D>
#include <osg/Geode>
#include <osg/PagedLOD>

#include <osgViewer/Viewer>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>

#include <osgUtil/Optimizer>


#define OBJ_INDEX_OFFSET 1

/*========================  defination ========================*/

#define PATH_SEPARATOR _T('/')
#define PATH_SEPARATOR_STR _T("/")
#define REVERSE_PATH_SEPARATOR _T('\\')

class DefaultNormalsGeometryVisitor
	: public osg::NodeVisitor
{
public:

	DefaultNormalsGeometryVisitor()
		: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {
	}

	virtual void apply(osg::Geode& geode)
	{
		for (unsigned int ii = 0; ii < geode.getNumDrawables(); ++ii)
		{
			osg::ref_ptr< osg::Geometry > geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(ii));
			if (geometry.valid())
			{
				osg::ref_ptr< osg::Vec3Array > newnormals = new osg::Vec3Array;
				newnormals->push_back(osg::Z_AXIS);
				geometry->setNormalArray(newnormals.get(), osg::Array::BIND_OVERALL);
			}
		}
	}

	virtual void apply(osg::Node& node)
	{
		traverse(node);
	}

};

int merge_obj(std::vector<std::string> file_name, std::string fileNameOut);
spGeometryData ReadObj(const std::string& path);
void WriteObj(IGeometryData* data, const std::string& path);


class ProgressObserver : public Simplygon::Observer
{
public:
	virtual void Execute(
		IObject* subject,
		rid EventId,
		void* EventParameterBlock,
		unsigned int EventParameterBlockSize)
	{
		// only care for progress events
		//if (EventId == SG_EVENT_PROGRESS)
		//{
			// get the progress in percent
		int val = *((int*)EventParameterBlock);
		// tell the process to continue
		// this is required by the progress event
		*((int*)EventParameterBlock) = 1;
		// output the progress update
		PrintProgressBar(val);
		//}
	}
} progressObserver;

void Simplifier::MultiLayeredTextureMapping(const std::float_t Re_Ratio, const std::string& readFrom, const std::vector<std::string> diffuseBaseFile_, const std::string& output_file_path, const std::string& output_base_name, const uint32_t repair_flag)
{
	std::string assetPath = output_file_path;
	std::string exePath = GetExecutablePath();

	std::string output_geometry_filename = assetPath + output_base_name + ".obj";

	int num_texture = diffuseBaseFile_.size();
	std::string output_diffuse_filename = assetPath + output_base_name + ".jpg";

	for (int i = 0; i < num_texture; i++)
	{
		if (diffuseBaseFile_[i].find(".png") != std::string::npos) {
			output_diffuse_filename = assetPath + output_base_name + ".png";
		}
	}

	// Load from file
	spWavefrontImporter objReader = sg->CreateWavefrontImporter();
	objReader->SetImportFilePath(readFrom.c_str());
	if (!objReader->RunImport())
		throw std::exception("Failed to load input file!");


	spScene scene = objReader->GetScene();
	spMaterialTable sceneMaterials = scene->GetMaterialTable();
	spTextureTable sceneTextures = scene->GetTextureTable();
	spReductionProcessor red = sg->CreateReductionProcessor();
	red->SetScene(scene);

	// Find length of the diagonal of the geometry.
	scene->CalculateExtents();

	///float inf[3], sup[3];//-
	Simplygon::spRealData inf = scene->GetInf(); //+
	Simplygon::spRealData sup = scene->GetSup(); //+

	float geom_diameter = 0.0f;
	geom_diameter += (inf[0] - sup[0]) * (inf[0] - sup[0]);
	geom_diameter += (inf[1] - sup[1]) * (inf[1] - sup[1]);
	geom_diameter += (inf[2] - sup[2]) * (inf[2] - sup[2]);

	geom_diameter = sqrt(geom_diameter);

	//<add author=zlc time=20210318>
	if (repair_flag == 1) {
		// Set the Repair Settings.
		spRepairSettings repair_settings = red->GetRepairSettings();
		// Will take care of holes that are of size 0.2 or less, so small gaps etc are removed.
		repair_settings->SetUseWelding(true);
		repair_settings->SetWeldOnlyBorderVertices(true);
		repair_settings->SetWeldDist(5.0f);
		repair_settings->SetUseTJunctionRemover(true);
		//repair_settings->SetTjuncDist(0.2f);
	}
	//</add>

	// Set the Reduction Settings.
	spReductionSettings reductionSettings = red->GetReductionSettings();

	///reductionSettings->SetReductionHeuristics(SG_REDUCTIONHEURISTICS_FAST);//- //Choose between "fast" and "consistent" processing.
	reductionSettings->SetReductionHeuristics(Simplygon::EReductionHeuristics::Fast); // +

	// The actual reduction triangle target are controlled by these three settings
	///reductionSettings->SetStopCondition(SG_STOPCONDITION_ANY);//- //The reduction stops when any of the targets below is reached
	reductionSettings->SetReductionTargetStopCondition(Simplygon::EStopCondition::Any); //+
	///reductionSettings->SetReductionTargets(SG_REDUCTIONTARGET_ALL);//- //Selects which targets should be considered when reducing
	reductionSettings->SetReductionTargets(Simplygon::EStopCondition::Any, true, true, true, true);

	///reductionSettings->SetTriangleRatio(Re_Ratio);//- //Targets at 50% of the original triangle count
	///reductionSettings->SetTriangleCount(10);//- //Targets when only 10 triangle remains
	///reductionSettings->SetMaxDeviation(REAL_MAX);//- //Targets when an error of the specified size has been reached. As set here it never happens.
	///reductionSettings->SetOnScreenSize(50);//- //Targets when the LOD is optimized for the selected on screen pixel size

	reductionSettings->SetReductionTargetTriangleRatio(Re_Ratio); //+
	reductionSettings->SetReductionTargetTriangleCount(10); //+
	reductionSettings->SetReductionTargetMaxDeviation(REAL_MAX); //+
	reductionSettings->SetReductionTargetOnScreenSize(50); //+										
	//不要缩减边界
	reductionSettings->SetLockGeometricBorder(true);


	// Set the Normal Calculation Settings.
	spNormalCalculationSettings normal_settings = red->GetNormalCalculationSettings();
	normal_settings->SetReplaceNormals(true);
	///normal_settings->SetHardEdgeAngleInRadians(3.14159f * 90.f / 180.0f);//-
	normal_settings->SetHardEdgeAngle(3.14159f * 90.f / 180.0f); //+

	// Set the Image Mapping Settings.
	spMappingImageSettings mapping_settings = red->GetMappingImageSettings();
	// Without this we cannot fetch data from the original geometry, and thus not
	// generate diffuse and normalmaps later on.
	mapping_settings->SetGenerateMappingImage(true);

	// Set to generate new texture coordinates.
	mapping_settings->SetUseFullRetexturing(true);
	mapping_settings->SetGenerateTexCoords(true);
	//<add author=zlc time=20210131>
	///mapping_settings->SetTexCoordGeneratorType(SG_TEXCOORDGENERATORTYPE_PARAMETERIZER_OLD);//-
	mapping_settings->SetTexCoordGeneratorType(Simplygon::ETexcoordGeneratorType::Parameterizer); //+
	//</add>
	// The higher the number, the fewer texture-borders.
	//<add author=zlc time=20210127>
	///mapping_settings->SetParameterizerMaxStretch(0.3f);//-
	spParameterizerSettings image_parameter_setting = mapping_settings->GetParameterizerSettings(); //+
	image_parameter_setting->SetMaxStretch(0.3f); //+
	//</add>
	// Buffer space for when texture is mip-mapped, so color values dont blend over.
	///mapping_settings->SetGutterSpace(4);//-
	///mapping_settings->SetWidth(image_lr);//-
	///mapping_settings->SetHeight(image_lr);//-
	

	std::uint32_t image_lr = 512;
	std::uint32_t texture_width = 512;
	std::uint32_t texture_height = 512;

	Simplygon::spMappingImageOutputMaterialSettings sgOutputMaterialSettings = mapping_settings->GetOutputMaterialSettings(0); //+
	sgOutputMaterialSettings->SetGutterSpace(4);//+
	sgOutputMaterialSettings->SetTextureWidth(image_lr);//+
	sgOutputMaterialSettings->SetTextureHeight(image_lr);//+

	if (texture_width * texture_height > 0) {
		sgOutputMaterialSettings->SetTextureWidth(texture_width);//+
		sgOutputMaterialSettings->SetTextureHeight(texture_height);//+
	}


	///mapping_settings->SetMultisamplingLevel(2);//-
	sgOutputMaterialSettings->SetMultisamplingLevel(2); //+

	// Add progress observer
	///red->AddObserver(&progressObserver, SG_EVENT_PROGRESS);//-
	red->AddObserver(&progressObserver); //+

	red->RunProcessing();

	// Mapping image is needed later on for texture casting.
	spMappingImage mapping_image = red->GetMappingImage();

	// Cast diffuse texture and normal map data into a new material
	// Create new material table.
	spMaterialTable output_materials = sg->CreateMaterialTable();
	spTextureTable output_textures = sg->CreateTextureTable();
	//	Create new material for the table.
	spMaterial output_material = sg->CreateMaterial();
	output_material->SetName("BakedMaterial");
	output_materials->AddMaterial(output_material);

	// Cast diffuse texture data
	{
		// Cast the data using a color caster
		spColorCaster cast = sg->CreateColorCaster();
		///cast->SetColorType(SG_MATERIAL_CHANNEL_DIFFUSE);//-
		Simplygon::spColorCasterSettings sgDiffuseCasterSettings = cast->GetColorCasterSettings(); //+
		sgDiffuseCasterSettings->SetMaterialChannel(SG_MATERIAL_CHANNEL_DIFFUSE);//+
		sgDiffuseCasterSettings->SetOutputImageFileFormat(Simplygon::EImageOutputFormat::JPEG);//+
		sgDiffuseCasterSettings->SetOutputPixelFormat(Simplygon::EPixelFormat::R8G8B8);//+

		//<add zk date = 2021/06/25>
		 // <add tag=1725 date=2021-05-12>目前仅考虑单纹理文件输出
		for (int i = 0; i < diffuseBaseFile_.size(); i++)
		{
			std::string texture_path = diffuseBaseFile_[i];
			if (texture_path.find(".png") != std::string::npos) {
				sgDiffuseCasterSettings->SetOutputImageFileFormat(Simplygon::EImageOutputFormat::PNG);
				sgDiffuseCasterSettings->SetOutputPixelFormat(Simplygon::EPixelFormat::R8G8B8A8);
			}
		}
		//</add>
		//</add>

		cast->SetSourceMaterials(sceneMaterials);
		cast->SetSourceTextures(sceneTextures);
		cast->SetMappingImage(mapping_image); // The mapping image we got from the remeshing process.
		///cast->SetOutputChannels(3);//- // RGB, 3 channels! (1 would be for gray scale, and 4 would be for RGBA.)
		///cast->SetOutputChannelBitDepth(8);//- // 8 bits per channel. So in this case we will have 24bit colors RGB.
		///cast->SetDilation(10);//- // To avoid mip-map artifacts, the empty pixels on the map needs to be filled to a degree as well.
		sgDiffuseCasterSettings->SetDilation(10);
		cast->SetOutputFilePath(output_diffuse_filename.c_str()); // Where the texture map will be saved to file.
		cast->RunProcessing(); // Fetch!

		// Set material to point to created texture filename.
		AddSimplygonTexture(output_material, output_textures, SG_MATERIAL_CHANNEL_DIFFUSE, output_diffuse_filename.c_str());
	}

	//Copy the new material table to the reduced scene
	scene->GetMaterialTable()->Copy(output_materials);
	scene->GetTextureTable()->Copy(output_textures);

	spWavefrontExporter objexp = sg->CreateWavefrontExporter();

	objexp->SetExportFilePath(output_geometry_filename.c_str());
	objexp->SetScene(scene);
	objexp->RunExport();
}


void Simplifier::AddSimplygonTexture(Simplygon::spMaterial mat, Simplygon::spTextureTable texTable, const char* channelName, const char* texFilePath, const char* namePrefix)
{
	char texName[256];
	sprintf_s(texName, "%s%s", namePrefix, channelName);

	spTexture newTex = sg->CreateTexture();
	//newTex->SetFilePath( texFilePath );
	//<add author=zlc time=20210104>
	std::string texfilename = texFilePath;
	size_t texfilename_i = texfilename.rfind('/');
	if (texfilename_i != std::string::npos)
		texfilename = texfilename.substr(texfilename_i + 1);
	std::cout << "\n计算的纹理jpg名字" << texfilename << std::endl;
	//</add>
	newTex->SetFilePath(texfilename.c_str());
	newTex->SetName(texName);
	texTable->AddTexture(newTex);
	spShadingTextureNode texNode = sg->CreateShadingTextureNode();
	texNode->SetTextureName(texName);
	///texNode->SetTextureLevel( 0 );//-
	texNode->SetTexCoordLevel(0);//+
	mat->SetShadingNetwork(channelName, texNode);
}


// This function reads an .obj file and returns a geometry object
spGeometryData ReadObj(const std::string& path)
{
	Simplygon::ISimplygon* sg = nullptr;
	ObjRead rd;

	// Create the Geometry. All geometrydata will be loaded into this object
	spGeometryData geom = sg->CreateGeometryData();

	// Array with vertex-coordinates. Will contain 3 real-values for each vertex in the geometry.
	spRealArray coords = geom->GetCoords();

	// Array with triangle-data. Will contain 3 ids for each corner of each triangle, so the triangles know what vertices to use.
	spRidArray vertex_ids = geom->GetVertexIds();

	// Data from the file, stores on per-vertex basis. 
	spRealArray tmpnormals = sg->CreateRealArray();
	spRealArray tmptexcoords = sg->CreateRealArray();

	// Arrays that data will be stored in, before loading it to the GeometryData
	// since the stored data need to be in per-triangle basis when loaded to the GeometryData.
	spRealArray cornernormals = sg->CreateRealArray();
	spRealArray cornertexcoords = sg->CreateRealArray();

	// Regular arrays that the obj-reader will store the data into.
	int vertex_count;
	int triangle_count;
	float* pCoords;
	int* pTriangles;
	float* pNormals = NULL;
	float* pTexcoords = NULL;

	// Attempt to load an obj-file.
	if (!rd.Load(
		path.c_str(),
		vertex_count,
		triangle_count,
		&pCoords,
		&pTexcoords,
		&pNormals,
		&pTriangles,
		false))
	{
		fprintf(stderr, "Error: Failed to load input file\n");
		exit(-1);
		return spGeometryData();
	}

	// NOTE: must set tuplesize before tuplecount, or the arraysize will be wrong

	// Normals contains 3 real-values for each tuple
	tmpnormals->SetTupleSize(3);
	// Textures contains 3 real-values for each tuple
	tmptexcoords->SetTupleSize(2);
	// Data from the file is stored on per-vertex basis, so size on these arrays must be same size
	tmpnormals->SetTupleCount(vertex_count);
	tmptexcoords->SetTupleCount(vertex_count);

	// Set vertex- and triangle-counts for the Geometry.
	geom->SetVertexCount(vertex_count);
	geom->SetTriangleCount(triangle_count);

	// Arrays that normals and texcoords will be loaded into, per triangle-corner basis
	spRealArray normals;
	spRealArray texcoords;
	if (pNormals)
	{
		geom->AddNormals();
		normals = geom->GetNormals();
	}
	if (pTexcoords)
	{
		geom->AddTexCoords(0);
		texcoords = geom->GetTexCoords(0);
	}

	// add vertex-coordinates to the Geometry, and also, if they exist, normal and texture-data to the
	// arrays that will be used to convert to data that fits the Geometry.
	for (int v = 0; v < vertex_count; ++v)
	{
		coords->SetTuple(v, &pCoords[v * 3]);
		if (pNormals)
			tmpnormals->SetTuple(v, &pNormals[v * 3]);
		if (pTexcoords)
			tmptexcoords->SetTuple(v, &pTexcoords[v * 2]);
	}

	// add triangles to the Geometry. (The ids for what vertices each triangle use)
	for (int t = 0; t < triangle_count; ++t)
	{
		vertex_ids->SetItem(t * 3 + 0, pTriangles[t * 3 + 0]);
		vertex_ids->SetItem(t * 3 + 1, pTriangles[t * 3 + 1]);
		vertex_ids->SetItem(t * 3 + 2, pTriangles[t * 3 + 2]);
	}

	// Load data into the Geometry, first convert it from per-vertex basis to per-triangle-corner basis,
	// then copy the data to the arrays belonging to the Geometry.

	if (pNormals)
	{
		///CreateIndependentArray( tmpnormals, vertex_ids, cornernormals );//-
		CreateIndependentArray(static_cast<IValueArray*>(tmpnormals.GetInterface()), \
			static_cast<IRidArray*>(vertex_ids.GetInterface()), static_cast<IValueArray*>(cornernormals.GetInterface()));//+

		normals->DeepCopy(cornernormals);
	}

	if (pTexcoords)
	{
		///CreateIndependentArray( tmptexcoords, vertex_ids, cornertexcoords );//-
		CreateIndependentArray(static_cast<IValueArray*>(tmptexcoords.GetInterface()), \
			static_cast<IRidArray*>(vertex_ids.GetInterface()), static_cast<IValueArray*>(cornertexcoords.GetInterface()));//+

		texcoords->DeepCopy(cornertexcoords);
	}

	// make sure the bounding box for the geometry is up-to-date
	geom->CalculateExtents(true);

	// run the geometry through the validator to make sure all is setup correctly

	spGeometryValidator validator = sg->CreateGeometryValidator();
	bool valid = validator->ValidateGeometry(geom);

	//<delete author=zlc>
	/*
	// if not valid, report the geometry, and exit
	if( !valid )
		{
		rid error_value = validator->GetErrorValue();
		rstring error_string = validator->GetErrorString();

		fprintf( stderr, "Validation failed:\n\tError: %s\n", error_string.GetText() );
		exit( -1 );
		}
	*/
	//</delete>

	if (pNormals) delete[] pNormals;
	if (pTexcoords) delete[] pTexcoords;
	if (pCoords) delete[] pCoords;
	if (pTriangles) delete[] pTriangles;

	return geom;
}


void WriteObj(IGeometryData* data, const std::string& path)
{
	Simplygon::ISimplygon* sg = nullptr;
	FILE* fp = fopen(path.c_str(), "wt");
	if (!fp)
	{
		printf("Error: Cannot write file\n");
		return;
	}

	unsigned int vc = data->GetVertexCount();
	unsigned int tc = data->GetTriangleCount();
	unsigned int i;

	spRealArray vdata = data->GetCoords();
	spRidArray tdata = data->GetVertexIds();
	spRealArray texdata = data->GetTexCoords(0);
	spRealArray normaldata = data->GetNormals();

	spRidArray texcoordsindexarray = sg->CreateRidArray();
	spRidArray normalsindexarray = sg->CreateRidArray();
	spRealArray texcoordsvaluearray = sg->CreateRealArray();
	spRealArray normalsvaluearray = sg->CreateRealArray();

	// The data in texdata and normals data is stored on per triangle-corner basis,
	// this data will be stored in unique values i the values arrays, and with indexes for the triangle-corners
	// in the index arrays.
	if (!texdata.IsNull())
		CreateIndexArray(texdata, texcoordsvaluearray, texcoordsindexarray);
	if (!normaldata.IsNull())
		CreateIndexArray(normaldata, normalsvaluearray, normalsindexarray);

	// write vertices
	fprintf(fp, "\n# %i Vertices \n\n", int(vc));
	for (i = 0; i < vc; ++i)
	{
		fprintf(fp, "v %f %f %f\n",
			vdata->GetItem(i * 3 + 0),
			vdata->GetItem(i * 3 + 1),
			vdata->GetItem(i * 3 + 2)
		);
	}

	// write texcoords
	if (!texdata.IsNull())
	{
		fprintf(fp, "\n# %i Texcoords \n\n", int(texcoordsvaluearray->GetTupleCount()));
		for (i = 0; i < texcoordsvaluearray->GetTupleCount(); ++i)
		{
			fprintf(fp, "vt %f %f\n",
				texcoordsvaluearray->GetItem(i * 2 + 0),
				texcoordsvaluearray->GetItem(i * 2 + 1)
			);
		}
	}

	// write normals
	if (!normaldata.IsNull())
	{
		fprintf(fp, "\n# %i Normals \n\n", int(normalsvaluearray->GetTupleCount()));
		for (i = 0; i < normalsvaluearray->GetTupleCount(); ++i)
		{
			fprintf(fp, "vn %f %f %f\n",
				normalsvaluearray->GetItem(i * 3 + 0),
				normalsvaluearray->GetItem(i * 3 + 1),
				normalsvaluearray->GetItem(i * 3 + 2)
			);
		}
	}

	// Write the triangles

	unsigned int v_i;
	unsigned int vt_i;
	unsigned int vn_i;

	fprintf(fp, "\n# %i Triangles \n\n", int(tc));
	for (unsigned int t = 0; t < tc; ++t)
	{
		fprintf(fp, "f ");
		for (i = 0; i < 3; ++i)
		{
			v_i = tdata->GetItem(t * 3 + i);
			if (!texdata.IsNull())
			{
				vt_i = texcoordsindexarray->GetItem(t * 3 + i);
				if (!normaldata.IsNull())
				{
					vn_i = normalsindexarray->GetItem(t * 3 + i);

					// all indiced
					fprintf(fp, "%d/%d/%d ", v_i + 1, vt_i + 1, vn_i + 1);
				}
				else
				{
					// vertices and texture coordinates indiced
					fprintf(fp, "%d/%d ", v_i + 1, vt_i + 1);
				}
			}
			else
			{
				if (!normaldata.IsNull())
				{
					vn_i = normalsindexarray->GetItem(t * 3 + i);
					// vertices and normals indiced
					fprintf(fp, "%d//%d ", v_i + 1, vn_i + 1);
				}
				else
				{
					// only vertices indiced
					fprintf(fp, "%d ", v_i + 1);
				}
			}
		}
		fprintf(fp, "\n");
	}

	fclose(fp);
}



// int main()
// {
// 
// 
// 
// 
// 	Simplifier sim;
// 	std::string inputobjfile = "F:/handwork_data/data_out/27_MJSJHY/Tile_+039_+197.obj";
// 	sim.simplify_obj(inputobjfile, inputobjfile,0.6);
// 
// 
// 	return 0;
// }

std::string name_reptosuff(std::string name, std::string repstr, std::string suff) {
	std::string temp_name = name;
	return temp_name.replace(temp_name.find(repstr), repstr.size(), suff);
}

bool Str_Split(const std::string& str, std::vector<std::string>& ret, std::string sep) {
	if (str.empty()) {
		return false;
	}
	std::string temp;
	std::string::size_type begin = str.find_first_not_of(sep);
	std::string::size_type pos = 0;
	while (begin != std::string::npos) {
		pos = str.find(sep, begin);
		if (pos != std::string::npos) {
			temp = str.substr(begin, pos - begin);
			begin = pos + sep.length();
		}
		else {
			temp = str.substr(begin);
			begin = pos;
		}
		if (!temp.empty()) {
			ret.push_back(temp);
			temp.clear();
		}
	}
	return true;
}
int file_update_content(std::string filename) {
	if (!boost::filesystem::exists(filename)) {
		return -1;
	}
	std::ifstream file(filename);
	std::string line;
	std::ofstream outfile(filename + "temp", std::ios::out | std::ios::trunc);
	while (!file.eof()) {
		getline(file, line);
		int find_flag = 0;
		if (line.find("map_") != std::string::npos)//如果要修改内容就在这修改line的内容,再存到文件中就行了
		{
			std::vector<std::string> str_sp;
			int index = Str_Split(line, str_sp, " ");
			if (str_sp.size() > 1) {
				if (str_sp[1].find_last_of(PATH_SEPARATOR) != std::string::npos) {
					find_flag = 1;
					outfile << str_sp[0] << " " << str_sp[1].substr(str_sp[1].find_last_of(PATH_SEPARATOR) + 1) << std::endl;
				}
			}
		}
		if (find_flag == 0) {
			outfile << line << std::endl;
		}
	}
	outfile.close();
	file.close();
	std::ofstream outfile1(filename, std::ios::out | std::ios::trunc);
	std::fstream file1(filename + "temp");
	while (!file1.eof()) {
		getline(file1, line);
		outfile1 << line << std::endl;
	}
	outfile1.close();
	file1.close();
	boost::filesystem::remove(filename + "temp");//删除中间文件
	return 0;
}


Simplifier::Simplifier() {
	Simplygon::AddSearchPath("simplygon9_dll\\");
	Simplygon::EErrorCodes initval = Simplygon::Initialize(&sg, nullptr, nullptr);
	if (initval != Simplygon::EErrorCodes::NoError)
	{
		std::cerr << "Simplygon::EErrorCodes::NoError:" << int(initval) << std::endl;;
	}
}

Simplifier::~Simplifier() {
	Simplygon::Deinitialize(sg);
}


//接口设计：

int Simplifier::simplify_obj(std::string& input_file, std::string& output_file, double mesh_reduction) {

	MeshBase mesh_base;
	mesh_base.read_obj_file(input_file);
	return simplify_obj(input_file, output_file, mesh_reduction, &mesh_base);
}
int Simplifier::simplify_obj(std::string& input_file, std::string& output_file, double mesh_reduction, MeshBase* mesh_base)
{
	if (mesh_base == nullptr)
		return -1;

	std::vector<std::string> texture_filename;
	texture_filename = mesh_base->get_texture_filename();

	std::string file_base_name;
	std::string output_file_base_name;
	std::string output_file_path;//std::string output_file_path = file_path;

	std::filesystem::path path(input_file);
	std::filesystem::path output_path(output_file);
	{
		if (path.has_stem()) {
			file_base_name = path.stem().string();
		}
		if (output_path.has_stem()) {
			output_file_base_name = output_path.stem().string();
		}
		if (output_path.has_parent_path()) {
			output_file_path = output_path.parent_path().string();
		}
		output_file_path += "/";
	}
	std::vector<std::string> obj_Texture;

	for (auto data : texture_filename) {
		obj_Texture.push_back(data);
	}

	if (obj_Texture.empty())
	{
		printf("texture file does not exist, press enter to exit.\n");
		return -1;
	}
#if 1//todo：替换为openmesh处理
	std::vector<std::string> merge_obj_file_name;
	merge_obj_file_name.push_back(input_file);
	if (merge_obj(merge_obj_file_name, output_file) == 0) {
		std::cout << "成功" << std::endl;
	}
	//dedulv(output_file);
	std::string merge_mtl_fileNameOut = name_reptosuff(output_file, ".obj", ".mtl");
	//std::string mtl_fileNameOut = output_file_path + "seg_shared.mtl";
	file_update_content(merge_mtl_fileNameOut);
#else
// 	MeshBase tmp_mesh_base;
// 	tmp_mesh_base.read_obj_file(input_file);
// 	tmp_mesh_base.recompute_face_normal();
// 	tmp_mesh_base.write_obj_file(output_file);
// 	
// 
// 	std::string merge_mtl_fileNameOut = name_reptosuff(output_file, ".obj", ".mtl");
// 	file_update_content(merge_mtl_fileNameOut);
#endif

	//1.先把纹理压缩走通
	//下边这个函数走API，其他逻辑放到exe中
	MultiLayeredTextureMapping(mesh_reduction, output_file, obj_Texture, output_file_path, output_file_base_name, 0);


}

int merge_obj(std::vector<std::string> file_name, std::string fileNameOut) {
	bool do_overallNormal = false;
	osg::ref_ptr<osg::Node> root;
	std::vector<osg::ref_ptr<osg::Node>> nodes;
	for (int file_index = 0; file_index < file_name.size(); file_index++) {
		osg::ref_ptr<osgDB::ReaderWriter::Options> read_options = new osgDB::ReaderWriter::Options;
		read_options->setOptionString("noRotation");
		osg::ref_ptr<osg::Object> object = osgDB::readObjectFile(file_name[file_index], read_options);
		if (object.valid()) {
			if (object->asNode()) nodes.push_back(object->asNode());
		}
	}
	if (nodes.size() == 1) {
		root = nodes.front();
	}
	else if (nodes.size() > 1) {
		osg::ref_ptr<osg::Group> group = new osg::Group;
		for (std::vector<osg::ref_ptr<osg::Node>>::iterator itr = nodes.begin(); itr != nodes.end(); ++itr) {
			group->addChild(itr->get());
		}
		root = group;
	}
	if (root.valid()) {
		osgUtil::Optimizer optimizer;
		optimizer.optimize(root.get());
	}
	if (do_overallNormal)
	{
		DefaultNormalsGeometryVisitor dngv;
		root->accept(dngv);
	}

	osg::ref_ptr<osgDB::ReaderWriter::Options> obj_options = new osgDB::ReaderWriter::Options;
	obj_options->setOptionString("OutputTextureFiles");          // 输出纹理
	bool result = osgDB::writeNodeFile(*root, fileNameOut, obj_options);
	if (result) {
		std::cout << "Data written to '" << fileNameOut << "'." << std::endl;
		return 0;
	}
	else {
		std::cout << "Data written error" << std::endl;
		return 1;
	}
}
