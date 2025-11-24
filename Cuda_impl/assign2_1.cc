/*
 * assign2_1.cc
 *
 * Contains code for setting up and finishing the simulation.
 *
 * NOTE: YOU SHOULD NOT CHANGE THIS FILE
 *
 */

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <limits>

#include "file.hh"
#include "timer.hh"
#include "simulate.hh"
#include <chrono>

using namespace std;


/* Initializes the given double array with a sinus function. */
void fill(double *array, int offset, int range, double sample_start,
          double sample_end) {
    int i;
    float dx;

    dx = (sample_end - sample_start) / range;
    for (i = 0; i < range; i++) {
        array[i + offset] = sin(sample_start + i * dx);
    }
}

/* Entry point for the program. */
int main(int argc, char* argv[]) {
    // Check if the correct arguments are given
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " i_max t_max block_size" << endl;
        cout << " - i_max: number of discrete amplitude points, should be >2"
             << endl;
        cout << " - t_max: number of discrete timesteps, should be >=1"
             << endl;
        cout << " - block_size: number of CUDA-threads in each cuda-block. "
                "Should be a multiple of 32" << endl;
        return EXIT_FAILURE;
    }

    // Parse the input
    const long i_max      = strtol(argv[1], NULL, 10);
    const long t_max      = strtol(argv[2], NULL, 10);
    const long block_size = strtol(argv[3], NULL, 10);
    if (i_max < 3) {
        cerr << "Argument error: i_max should be and integer of >2." << endl;
        return EXIT_FAILURE;
    }
    if (t_max < 1) {
        cerr << "Argument error: t_max should be and integer of >=1." << endl;
        return EXIT_FAILURE;
    }
    if (block_size < 1) {
        cerr << "Argument error: block_size should be and integer of >=1."
             << endl;
        return EXIT_FAILURE;
    }

    // Initialize the timer
    timer waveTimer("wave timer");

    // Declare the arrays
    double *old_array     = new double[i_max]();
    double *current_array = new double[i_max]();
    double *next_array    = new double[i_max]();

    // Fill the first two with a sinus
    fill(old_array, 1, i_max/4, 0, 2*3.14);
    fill(current_array, 2, i_max/4, 0, 2*3.14);

    // Time & run the wave equation simulation in simulate.cc
    waveTimer.start();
    double *result_array = simulate(
        i_max, t_max, block_size,
        old_array, current_array, next_array
    );
    waveTimer.stop();

    // Print the time it took and write the result to a result.txt
    cout << waveTimer;
    file_write_double_array("result.txt", result_array, i_max);

    // Clean the arrays
    delete[] old_array;
    delete[] current_array;
    delete[] next_array;

    return EXIT_SUCCESS;

}



/**-------------------------------------------------
THIS CODE SNIPPED WAS USED FOR ONLY TESTING PURPOSES
----------------------------------------------------*/

// EXPERIMENTATION
/**
int runner(int i_max, int t_max, int block_size) {
    if (i_max < 3) {
        cerr << "Argument error: i_max should be and integer of >2." << endl;
        return EXIT_FAILURE;
    }
    if (t_max < 1) {
        cerr << "Argument error: t_max should be and integer of >=1." << endl;
        return EXIT_FAILURE;
    }
    if (block_size < 1) {
        cerr << "Argument error: block_size should be and integer of >=1."
             << endl;
        return EXIT_FAILURE;
    }

    // Initialize the timer
    timer waveTimer("wave timer");

    // Declare the arrays
    double *old_array     = new double[i_max]();
    double *current_array = new double[i_max]();
    double *next_array    = new double[i_max]();

    fill(old_array, 1, i_max/4, 0, 2*3.14);
    fill(current_array, 2, i_max/4, 0, 2*3.14);


    auto start = chrono::high_resolution_clock::now();
    simulate(i_max, t_max, block_size, old_array, current_array, next_array);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;

    delete[] old_array;
    delete[] current_array;
    delete[] next_array;
    return elapsed.count();
}


void experienmnt1(){
	FILE *fp = fopen("./results_ex1.csv", "w");
    if (!fp) {
        perror("Could not open results_ex1.csv");
        return;
    }

    fprintf(fp, "i_max,t_max,block_size,time_seconds\n");

    int i_max_values[] = {1000, 10000, 100000, 1000000, 10000000};
	int num_i = sizeof(i_max_values) / sizeof(i_max_values[0]);

    int t_max = 100;
    int block_size = 512;

	printf("Running Experiment 1: Scalability\n");
    for (int i = 0; i < num_i; i++) {
        int i_max = i_max_values[i];
        double elapsed_time =  runner(i_max, t_max, block_size);
        fprintf(fp, "%d,%d,%d,%.9f\n", i_max, t_max, block_size, elapsed_time);
    }
    fclose(fp);
    printf("All results saved to results_ex1.csv\n");
}


void experienmnt2(){
    FILE *fp = fopen("./results_ex2.csv", "w");
    if (!fp) {
        perror("Could not open results_ex2.csv");
        return;
    }

    fprintf(fp, "block_size,i_max,t_max,time_seconds,normalized_time\n");

    int block_sizes[] = {32, 64, 128, 256, 512, 1024};
    int time_sizes[] = {100, 10000, 100000};  // Fixed: removed duplicate 100000, added comma
    int num_blocks = sizeof(block_sizes) / sizeof(block_sizes[0]);
    int num_times = sizeof(time_sizes) / sizeof(time_sizes[0]);

    int i_max = 1000000;

    printf("Running Experiment 2: Block Size Impact and time size impact\n");

    for(int t = 0; t < num_times; t++) {
        int t_max = time_sizes[t];

        for (int i = 0; i < num_blocks; i++) {
            int block_size = block_sizes[i];
            double elapsed_time = runner(i_max, t_max, block_size);
    		double normalized_time = elapsed_time / (i_max * t_max);

           fprintf(fp, "%d,%d,%d,%.6f,%.9f\n", block_size, i_max, t_max, elapsed_time, normalized_time);
        }
    }
    fclose(fp);
    printf("All results saved to results_ex2.csv\n");
}

int main() {
//	experienmnt1();
	experienmnt2();
	return 0;
}
*/
