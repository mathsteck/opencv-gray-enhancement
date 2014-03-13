/*
    Author: Matheus Steck Cardoso
    email: mathsteck@gmail.com

    The MIT License (MIT)

    Copyright (c) <year> <copyright holders>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE
*/

#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace cv;
using namespace std;

float iminvert(float pixel_value) {                         // Invert the value of a pixel
    float value = 255 - pixel_value;

    if (value < 0)
        return 0;

    return value;
}

float find_max_value(Mat Image) {                           // Get the greates intensity on a image

    float max_value = 0, pixel_value = 0;

    for (int x = 0; x < Image.rows; x++) {
      for (int y = 0; y < Image.cols; y++) {
          pixel_value = Image.at<uchar>(x,y);

          if(pixel_value > max_value)
              max_value = pixel_value;
      }
    }
    return max_value;
}

float find_min_value(Mat Image) {                           // Get the lowest intensity on a image

    float min_value = 255, pixel_value = 0;

    for (int x = 0; x < Image.rows; x++) {
      for (int y = 0; y < Image.cols; y++) {
          pixel_value = Image.at<uchar>(x,y);

          if(pixel_value < min_value)
              min_value = pixel_value;
      }
    }
    return min_value;
}

float imcontrast(float pixel_value, float max_value, float min_value) { // Change the contrast of a pixel based on max and min values
    return (pixel_value - min_value) * (255 / (max_value - min_value));
}

float imlog(float pixel_value, float max_intensity, float constant) {   // Changes the value of a pixel based on the maximum intensity and a constant
    if(constant == 0)
        constant = 255 / log(1 + max_intensity);
    return constant * log(1 + abs(pixel_value));
}

float imgamma(float pixel_value, float constant, float gamma) { // Changes the gama of a pixel based on a constant and a gamma
    if (constant == 0)
        constant = 1;
    return constant * pow(pixel_value, gamma);
}

void print_chart(int value) {

    cout << "|";
    for (int i = 0; i < value; i++)
        cout << "*";
    cout << "\n";
}

void imhistogram(Mat Image) {                                   // Plot a chart based on intensity values of an Image

    int chart[8] = {0, 0, 0, 0, 0, 0, 0, 0};                    // Vector that stores the number of intensities based on a 32 interval
                                                                // id = 0 store the values from 0 - 31, id = 1 stores the values from 32 - 63

    for (int x = 0; x < Image.rows; x++)
      for (int y = 0; y < Image.cols; y++)
        for (int i = 0; i < 8; i++) {
            float pixel_value = Image.at<uchar>(x, y);

            if (pixel_value >= i*32 && pixel_value < (i+1)*32) {
                chart[i]++;
                break;
            }
        }

    int max_value = 0;
    for (int i = 0; i < 8; i++)
        if (chart[i] > max_value)
            max_value = chart[i];

    if(max_value > 0)
        for (int i = 0; i < 8; i++) {
            cout << " " << chart[i];
            chart[i] = (chart[i] * 10) / max_value;
        }

    cout << "\n";

    for (int i = 0; i < 8; i++)
        print_chart(chart[i]);
}

int main(int argc, char ** argv) {
    int function_selector = -1;                                 // Constant that defines what function will be used to change the image                     
    float constant = 0, gamma = 0;                              // Constant values used on imlog and imgamma
    string input_file, output_file;                             // Stores the path to an input and output image

    /*  User interface  */

    cout << "Please insert the image name: ";
    getline(cin, input_file);
    cout << "Please insert the output image name: ";
    getline(cin, output_file);
    
    while(function_selector <= 0 || function_selector > 4) {
        cout << "Please insert the function to be used:\n";
        cout << "1 - Negative\n";
        cout << "2 - Contrast\n";
        cout << "3 - Logarithm\n";
        cout << "4 - Power\n";
        cout << "Input: ";
        cin >> function_selector;
    }

    if (function_selector > 2) {
        cout << "Please insert a constant value (0 = optional): ";
        cin >> constant;
    }
    //FIXME mudar a ordem dos atributos... o gamma tem que vir antes do constante no caso 4
    if (function_selector == 4) {
        cout << "Please insert a gamma value: ";
        cin >> gamma;
    }

    /* Image reading */

    Mat Image = imread(input_file, CV_LOAD_IMAGE_UNCHANGED);    // Read a image from a file
    Mat ImageOutput(Image);                                     // Create a copy of the original image

    if (Image.empty()){
        cout << "ERROR: Image can't be loaded. Press any key to exit\n";
        system("pause");
        return -1;
    }

    float max_intensity = find_max_value(Image);                // Get the maximum intensity value from the image
    float min_intensity = find_min_value(Image);                // Get the minimum intensity value from the image

    /* Take the red pill and navigate on the matrix */

    for (int x = 0; x < Image.cols; x++) {
      for (int y = 0; y < Image.rows; y++) {
         float pixel_value = Image.at<uchar>(x, y);

         if(function_selector == 1)
             pixel_value = saturate_cast<uchar>(iminvert(pixel_value));
         if(function_selector == 2)
             //FIXME mudar para imcontrast(pixel_value)
             pixel_value = saturate_cast<uchar>(imcontrast(pixel_value, max_intensity, min_intensity));
         if(function_selector == 3)
             pixel_value = saturate_cast<uchar>(imlog(pixel_value, max_intensity, constant));
         if(function_selector == 4)
             pixel_value = saturate_cast<uchar>(imgamma(pixel_value, constant, gamma));

         ImageOutput.at<uchar>(x, y) = pixel_value;
      }
    }

    imhistogram(ImageOutput);                                   // Plot the histogram chart
    namedWindow(output_file, CV_WINDOW_AUTOSIZE );              // Create a window for display.
    imshow(output_file, ImageOutput);                           // Show our image inside it.
    
    waitKey(0);                                                 // Wait for any key
    
    destroyWindow(input_file);                                  // Destroy the window
    imwrite(output_file, ImageOutput);                          // Write the new image on a file
    
    return 0;
}
