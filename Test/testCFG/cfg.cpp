

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/progress.hpp>
#include <boost/foreach.hpp>
namespace po = boost::program_options;
using namespace std;
namespace opts
{
	std::string      strSceneFilePath;        
	std::string      strMeshFilePath;         
	std::string      strOutFilePath;  
	std::string  cfgfile;
	std::string logdir;
	
	bool gpu;
	int level;
}

void generate_configFile(boost::program_options::options_description& options_)
{
	std::string strConfigName = "sample.cfg";

	if (!boost::filesystem::exists(strConfigName)) 
	{
		boost::filesystem::ofstream ostream(strConfigName, std::ios_base::out);
		ostream.close();

		boost::filesystem::ofstream configFile(strConfigName);
		boost::property_tree::ptree tree;
		for (auto& option : options_.options())
		{
			std::string name = option->long_name();
			boost::any defaultValue;
			bool a = option->semantic()->apply_default(defaultValue);
			std::cout << a << std::endl;
			if (name == "help")
				continue;
			if (defaultValue.empty())
			{
				//std::string val = boost::any_cast<std::string>("");
				tree.put(name, "");
			}
			else if (defaultValue.type() == typeid(std::string))
			{
				std::string val = boost::any_cast<std::string>(defaultValue);
				tree.put(name, val);
			}
			else if (defaultValue.type() == typeid(int)) {
				int val = boost::any_cast<int>(defaultValue);
				tree.put(name, val);
			}
			else if (defaultValue.type() == typeid(float)) {
				float val = boost::any_cast<float>(defaultValue);
				tree.put(name, val);
			}
			else if (defaultValue.type() == typeid(double)) {
				double val = boost::any_cast<double>(defaultValue);
				tree.put(name, val);
			}
			else if (defaultValue.type() == typeid(unsigned)) {
				double val = boost::any_cast<unsigned>(defaultValue);
				tree.put(name, val);
			}
			else if (defaultValue.type() == typeid(bool)) {
				double val = boost::any_cast<bool>(defaultValue);
				tree.put(name, val);
			}

		}
		boost::property_tree::write_ini(configFile, tree);
	}
}


void initArgs(int argc, char** argv)
{
	try
	{
		po::options_description generic("基本信息");
		generic.add_options()
			("help,h", "帮助")
			("prj_path,p", po::value<std::string>(&opts::strSceneFilePath), "输入路径")
			("log_dir,l", po::value<std::string>(&opts::logdir), "日志文件路径")
			("image_path,i", po::value<std::string>(&opts::strMeshFilePath)->default_value(""))
			("SFM.use_gpu,g", po::value<bool>(&opts::gpu)->default_value(true))//SFM.
			("SFM.level,l", po::value<int>(&opts::level)->default_value(0))//SFM.
			;
		boost::program_options::options_description hidden("Hidden options");

		hidden.add_options()

			("cfg", po::value<std::string>(&opts::cfgfile), "配置文件")
			;
		boost::program_options::options_description cmdline_options;
		cmdline_options.add(generic).add(hidden);

		boost::program_options::options_description config_file_options;
		config_file_options.add(generic);

		po::variables_map vm;
		po::positional_options_description p;

		if (argc <= 1) 
		{
			generate_configFile(config_file_options);		
		}

		po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
		po::notify(vm);
		// parse configuration file
		std::ifstream ifs((opts::cfgfile));
		if (ifs)
		{
			boost::program_options::store(parse_config_file(ifs, config_file_options), vm);
			boost::program_options::notify(vm);
		}
		if (vm.count("help") || !vm.count("prj_path"))
		{
			cout << cmdline_options;
		}

	}

	catch (exception& e)
	{
		cerr << "error: " << e.what() << endl;
		return;
	}

	catch (...)
	{
		cerr << "Exception of unknown type!" << endl;
	}
}

int main(int argc, char** argv)
{


	//参数的解析
	initArgs(argc, argv);

	std::string prj_path = opts::strSceneFilePath;
	
	std::cout << opts::gpu << " " << prj_path<<std::endl;
	
	return EXIT_SUCCESS;
}
