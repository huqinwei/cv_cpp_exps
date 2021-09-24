

#include <iostream>
#include <fstream>


#include <boost/program_options.hpp>
#include "SampleDLL/Sample.h"

void hi()
{
	std::cout << " wo shi c ;" << std::endl;
}

Sample::Sample()
{
	
}
Sample::~Sample()
{

}
void Sample::hi()
{
	std::cout << " hi I am class sample    " << std::endl;
}