
#include <iostream>
#include "file_system_demos.h"



int boost_fs_demos_1() {
	std::string path = "D:/Tric_working_directory/aabbcc";//D:/Tric_working_directory/Tric_usrworkspace/workspace111

	boost::system::error_code ec;
	if (!boost::filesystem::create_directory(path, ec)) {
		std::cerr << "couldn't create directory\n";
		std::cerr << ec.message() << "\n";
		return -1;
	}
}