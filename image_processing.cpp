#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <omp.h>
#include <thread>

#ifdef PARALLEL_THREAD
#include <vector>
#endif

using namespace cv;
using namespace std;

// Include the necessary OpenCV header
#include <opencv2/core.hpp>

// Function to convert RGB to grayscale using the luminosity method
void convertToGrayscale(Mat& input, Mat& output, int startRow, int endRow) {
    for (int r = startRow; r < endRow; r++) {
        for (int c = 0; c < input.cols; c++) {
            Vec3b pixel = input.at<Vec3b>(r, c);
            int grayValue = 0.299 * pixel[0] + 0.587 * pixel[1] + 0.114 * pixel[2];
            output.at<uchar>(r, c) = grayValue;
        }
    }
}

// Function to process the image sequentially
void sequentialProcessing(Mat& input, Mat& output) {
    convertToGrayscale(input, output, 0, input.rows);
}

// Function to process the image in parallel using threads
void parallelThreadProcessing(Mat& input, Mat& output, int numThreads) {
    vector<thread> threads;
    int rowsPerThread = input.rows / numThreads;

    for (int i = 0; i < numThreads; i++) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? input.rows : (i + 1) * rowsPerThread;

        threads.emplace_back(convertToGrayscale, ref(input), ref(output), startRow, endRow);
    }

    for (auto& t : threads) {
        t.join();
    }
}

// Function to process the image in parallel using OpenMP
void parallelOpenMPProcessing(Mat& input, Mat& output, int numThreads) {
    #pragma omp parallel for num_threads(numThreads)
    for (int r = 0; r < input.rows; r++) {
        convertToGrayscale(input, output, r, r + 1);
    }
}

int main(int argc, char** argv) {
    if (argc != 3 && argc != 4) {
        cerr << "Usage: " << argv[0] << " <input_image> <output_image> [num_threads]" << endl;
        return EXIT_FAILURE;
    }

    // Read the image
    Mat inputImage = imread(argv[1], IMREAD_COLOR);
    if (inputImage.empty()) {
        cerr << "Error: Couldn't read the image." << endl;
        return EXIT_FAILURE;
    }

    // Create an output matrix for the grayscale image
    Mat outputImage(inputImage.rows, inputImage.cols, CV_8U);
  int numThreads = 1; // Change this to (int numThreads=1; )Default to 1 thread for sequential version
#ifdef _OPENMP
   

    if (argc == 4) {
        // If OpenMP is available and the user provides a third argument, use it as the number of threads
        numThreads = stoi(argv[3]);
    } else {
        // If OpenMP is available and the user does not provide a third argument, ask the user for the number of threads
        cout << "Enter the number of threads: ";
        cin >> numThreads;
    }
#endif

    // Measure execution time using chrono
    auto startTime = chrono::high_resolution_clock::now();

    // Choose between sequential, thread-based parallel, or OpenMP parallel processing
    // comment this if you dont want sequentialprocecssing
    sequentialProcessing(inputImage, outputImage);

#ifdef PARALLEL_THREAD
    if (argc == 4) {

        numThreads = stoi(argv[3]);
    } else {
        // If Parallel Thread is available and the user does not provide a third argument, ask the user for the number of threads
        cout << "Enter the number of threads: ";
        cin >> numThreads;
    }
        parallelThreadProcessing(inputImage, outputImage, numThreads);

#else
    parallelOpenMPProcessing(inputImage, outputImage, numThreads);
#endif

    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count() / 1e6;

    // Save the grayscale image
    imwrite(argv[2], outputImage);

    // Output execution time
    cout << "Loading image..." << endl;
    cout << "Rows (height): " << inputImage.rows << endl;
    cout << "Cols (width): " << inputImage.cols << endl;
    cout << "Start conversion..." << endl;
    cout << "End conversion..." << endl;
    cout << "Total time spent in seconds is " << duration << "." << endl;

    return EXIT_SUCCESS;
}
