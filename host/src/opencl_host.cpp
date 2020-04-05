//============================================================================
// Name        : 
// Author      : Howard
// Version     :
// Copyright   : Your copyright notice
// Description : 
//============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "math.h"
#include <algorithm>
#include "opencl_host.h"
#include "AOCLUtils/aocl_utils.h"
using namespace aocl_utils;


OPENCL_HOST::OPENCL_HOST(void):
m_context(NULL),
m_program(NULL),
m_aocx_filename("reduction.aocx"),
m_aocx_stream(NULL),
m_kernel(NULL),
m_kernel_name("reduction_scalar"),
m_input_array_size(1024),
m_local_size(m_input_array_size/8),
m_num_work_groups(m_input_array_size/m_local_size),
m_input_data(NULL),
m_scalar_data(NULL),
m_local_data(NULL),
m_queue(NULL)

{
	try
	{
		init_problem();
		init_opencl_context();
		init_program();
		init_kernels();
		init_cmd_queue();
		check_results();
		
		
	}
	catch(cl::Error e) {std::cout << e.what() << " : Error code " << e.err() << std::endl;}
	catch(...) {std::cout << "Unexpected Error" << std::endl;}
	
}
void OPENCL_HOST::init_problem(void)
{
	for(uint i = 0; i < m_input_array_size; i++)
		m_input_data_array.push_back(1.0f*i);
	m_scalar_sum.resize(m_num_work_groups);
	
}
void OPENCL_HOST::check_results(void)
{
	float device_data[m_num_work_groups];
	
	m_queue-> enqueueReadBuffer(*m_scalar_data, CL_TRUE, 0, m_num_work_groups*sizeof(float), &device_data, NULL, NULL);
		
	float device_sum(0.0f);
	for(uint i = 0; i < m_num_work_groups; i++)
		device_sum += device_data[i];
	
	float host_sum(0.0f);
	for(uint i = 0; i < m_input_data_array.size(); i++)
		host_sum += m_input_data_array[i];
		
	

	if(fabs(device_sum - host_sum) <= 0.01)
	{
		std::cout <<"Passed" <<std::endl;
		
		std::cout << "Execution Time: " <<
			m_profile_event.getProfilingInfo<CL_PROFILING_COMMAND_END>() -
			m_profile_event.getProfilingInfo<CL_PROFILING_COMMAND_START>() <<"ns" << std::endl;
	}
	else
	{
		std::cout << "Failed: Error: " << fabs(device_sum - host_sum) <<std::endl;
		
	}
	
		
}

void OPENCL_HOST::init_cmd_queue(void)
{
	cl_int status(CL_SUCCESS);
	m_queue = new cl::CommandQueue(*m_context,m_devices[0], CL_QUEUE_PROFILING_ENABLE, &status);
	checkError(status, "Error creating queue");
	cl::NDRange offset;
	cl::NDRange global_size(m_input_array_size);
	cl::NDRange local_size(m_local_size);

	m_queue->enqueueNDRangeKernel(*m_kernel, offset,global_size,local_size,NULL, &m_profile_event);
	m_queue->finish();
}


void OPENCL_HOST::init_kernels(void)
{
	cl_int status(CL_SUCCESS);
	m_input_data = new cl::Buffer(*m_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
								  sizeof(float)*m_input_data_array.size(), &m_input_data_array[0],&status);
	checkError(status, "Error creating input data buffer");

	m_scalar_data = new cl::Buffer(*m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
								  sizeof(float)*m_scalar_sum.size(), &m_scalar_sum[0] ,&status);
	checkError(status, "Error creating scalar data buffer");

	m_kernel = new cl::Kernel(*m_program, m_kernel_name.c_str());
	m_kernel->setArg(0,*m_input_data);
	cl::LocalSpaceArg arg = cl::__local(sizeof(float) * 8);
	m_kernel->setArg(1, arg);
	m_kernel->setArg(2,*m_scalar_data);
		
}

void OPENCL_HOST::init_program(void)
{
	std::ifstream aocx_stream(m_aocx_filename.c_str(), std::ios::binary);
	std::string aocx_file(std::istreambuf_iterator<char>(aocx_stream),
						  (std::istreambuf_iterator<char>()));
	cl::Program::Binaries aocx_bin(1,std::make_pair(aocx_file.c_str(),aocx_file.length()+1));
	m_program = new cl::Program(*m_context,m_devices,aocx_bin);
}

void OPENCL_HOST::init_opencl_context(void)
{
	//1. Get a nold of the "Altera Platform
	checkError(cl::Platform::get(&m_platform_vector), "Failed retriving platform");
	
	m_altera_platform = std::find_if(m_platform_vector.begin(),
									 m_platform_vector.end(),
									 is_altera_platform);
	if(m_altera_platform == m_platform_vector.end())
	{
		throw cl::Error(CL_INVALID_PLATFORM,"Altera OpenCL Platform not found");
	}
	std::cout << "Platform: " << m_altera_platform->getInfo<CL_PLATFORM_NAME>() << std::endl;
	
	//2. Get the devices on the platform
	checkError(m_altera_platform->getDevices(CL_DEVICE_TYPE_ALL, &m_devices), "Error getting devices");
	for(uint i = 0; i < m_devices.size(); i++)
		std::cout << "Device: " << m_devices[0].getInfo<CL_DEVICE_NAME>() << std::endl;
	
	//3. Setup Context
	m_context = new cl::Context(m_devices, NULL, oclContextCallback);
	
}


OPENCL_HOST::~OPENCL_HOST(void)
{
	delete(m_context);
	delete(m_program);
	delete(m_aocx_stream);
	delete(m_kernel);
	delete(m_input_data);
	delete(m_scalar_data);
	delete(m_local_data);
	delete(m_queue);
	
	
}
bool is_altera_platform(cl::Platform platform)
{
	std::string platform_name;
	platform.getInfo(CL_PLATFORM_NAME, &platform_name);
	return (platform_name.find("Altera") != std::string::npos)?true:false ;
}
