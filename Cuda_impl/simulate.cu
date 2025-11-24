/*
 * simulate.cu
 *
 * Implementation of a wave equation simulation, parallelized on the GPU using
 * CUDA.
 *
 * You are supposed to edit this file with your implementation, and this file
 * only.
 *
 */

#include <cstdlib>
#include <iostream>

#include "simulate.hh"

using namespace std;


/* Utility function, use to do error checking for CUDA calls
 *
 * Use this function like this:
 *     checkCudaCall(<cuda_call>);
 *
 * For example:
 *     checkCudaCall(cudaMalloc((void **) &deviceRGB, imgS * sizeof(color_t)));
 * 
 * Special case to check the result of the last kernel invocation:
 *     kernel<<<...>>>(...);
 *     checkCudaCall(cudaGetLastError());
**/
static void checkCudaCall(cudaError_t result) {
    if (result != cudaSuccess) {
        cerr << "cuda error: " << cudaGetErrorString(result) << endl;
        exit(EXIT_FAILURE);
    }
}

__global__ void computeAmplitude( const double *old_arr, const double *cur_arr,
								  double *next_arr, long n, double c){
    unsigned index = blockIdx.x * blockDim.x + threadIdx.x;

    if (index >= n) return;

    if (index == 0 || index == n - 1) {
        next_arr[index] = 0.0;
        return;
    }

    next_arr[index] = 2.0 * cur_arr[index] - old_arr[index] +
					c * (cur_arr[index - 1] - 2 * cur_arr[index] + cur_arr[index + 1]);
}


/* Function that will simulate the wave equation, parallelized using CUDA.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * block_size: how many threads per block you should use
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 * 
 */
double *simulate(const long i_max, const long t_max, const long block_size,
                 double *old_array, double *current_array, double *next_array) {

    // YOUR CODE HERE
	const double c = 0.15;
	double* device_old = NULL;
 	double* device_current = NULL;
	double* device_next = NULL;


	checkCudaCall(cudaMalloc((void **) &device_old, i_max * sizeof(double)));
	if(device_old== NULL){
		cerr << "Could not allocate old array on GPU." << endl;
        return NULL;
	}

	checkCudaCall(cudaMalloc((void **) &device_current, i_max * sizeof(double)));
    if (device_current == NULL) {
        checkCudaCall(cudaFree(device_old));
        cerr << "Could not allocate current array on GPU." << endl;
        return NULL;
    }

	checkCudaCall(cudaMalloc((void **) &device_next, i_max * sizeof(double)));
    if (device_next == NULL) {
        checkCudaCall(cudaFree(device_old));
		checkCudaCall(cudaFree(device_current));
        cerr << "Could not allocate next array on GPU." << endl;
        return NULL;
    }

    checkCudaCall(cudaMemcpy(device_old,  old_array,     i_max * sizeof(double), cudaMemcpyHostToDevice));
    checkCudaCall(cudaMemcpy(device_current,  current_array, i_max * sizeof(double), cudaMemcpyHostToDevice));

	// ceil of i_max/block_size
 	long num_blocks = (i_max + block_size - 1) / block_size;


    for (long t = 0; t < t_max; t++) {

        computeAmplitude<<<num_blocks, block_size>>>(device_old, device_current, device_next, i_max, c);
        checkCudaCall(cudaGetLastError());

		checkCudaCall(cudaDeviceSynchronize());  // wait before rotation starts

        double* temp = device_old;
        device_old  = device_current;
        device_current  = device_next;
        device_next = temp;
    }

 	checkCudaCall(cudaGetLastError());
 	checkCudaCall(cudaMemcpy(current_array, device_current, i_max * sizeof(double), cudaMemcpyDeviceToHost));

    checkCudaCall(cudaFree(device_old));
    checkCudaCall(cudaFree(device_current));
    checkCudaCall(cudaFree(device_next));

    return current_array;
}








