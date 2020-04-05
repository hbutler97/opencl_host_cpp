//============================================================================
// Name        : matix
// Author      : Howard
// Version     :
// Copyright   : Your copyright notice
// Description : 
//============================================================================
#include "opencl_host.h"
#include <iostream>
using namespace std;

void cleanup();


int main() {

	OPENCL_HOST *test = new OPENCL_HOST();
	

	std::cout << "!!!Hello World!!! " << std::endl;
	delete(test);
	
	return 0;
}

void cleanup()
{}
