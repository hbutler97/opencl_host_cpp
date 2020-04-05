#ifndef INC_OPENCL_HOST_H
#define INC_OPENCL_HOST_H

#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include "CL/opencl.h"
#include "CL/cl.hpp"

bool is_altera_platform(cl::Platform platform);

class OPENCL_HOST
//  public virtual PGMHSELMO_ENGINE_INTERFACE
{
public:
	OPENCL_HOST();
	~OPENCL_HOST();
	
private:
	OPENCL_HOST(const OPENCL_HOST& opencl_host_copy);
	cl::Context *m_context;
	cl_platform_id  m_platform;
	std::vector<cl::Platform> m_platform_vector;
	std::vector<cl::Platform>::iterator m_altera_platform;
	std::vector<cl::Device> m_devices;
	cl::Program *m_program;
	std::string m_aocx_filename;
	std::ifstream *m_aocx_stream;
	cl::Kernel *m_kernel;
	std::string m_kernel_name;
	cl::Buffer *m_input_data;
	cl::Buffer *m_scalar_data;
	cl::Buffer *m_local_data;
	cl::CommandQueue *m_queue;
	uint m_input_array_size;
	std::vector<float> m_input_data_array;
	std::vector<float> m_scalar_sum;
	uint m_local_size;
	uint m_num_work_groups;
	cl::Event m_profile_event;
	

	void init_problem(void);
	void init_cmd_queue(void);
	
		
	void init_opencl_context(void);
	void init_program(void);
	void init_kernels(void);
	void launch_kernel(void);
	void check_results(void);
	
	
	
};





	
#endif //INC_OPENCL_HOST
