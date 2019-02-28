#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "window.hpp"
using namespace std;

//Load the image by typing in the file name;
void loadImage(vector<int>& image_array, int& width, int& height, int& greyscale, string file)
{
	ifstream image_in(file);
	string line;
	string dimensions[2];
	int i = 0;
	if (image_in)
	{
		getline(image_in, line);//Get .pgm "Magic number"
		if (line == "P2")
		{
			cout << "Valid P2/ASCII .pgm file found." << endl;
		}
		else{
			cout << "Not a valid File.  Must be .pgm P2/ASCII." << endl;
			return;
		}


		getline(image_in, line);
		while (line.at(0) == '#'){//Skip over any comments
			getline(image_in, line);
		}

		//Get width and height dimensions of image
		//Use string stream to break line up into a string array then convert to ints
		stringstream ssin(line);
		while (ssin && i < 2){
			ssin >> dimensions[i];
			i++;
		}

		width = stoi(dimensions[0]);//Now convert strings to integers
		height = stoi(dimensions[1]);
		cout << "The image dimensions are : " << width << "x" << height << endl;

		//Get greyscale value
		getline(image_in, line);
		greyscale = stoi(line);//String to int conversion
		cout << "The greyscale range for this image is 0 to " << greyscale << "." << endl;

		//Store numbers into the 2D int array
		for (int i = 0; i < width * height; i++){
			image_in >> ws;//Extracts as many whitespace characters as possible from the current position in the input sequence.
			getline(image_in, line, ' ');
			image_array.push_back(stoi(line));

		}
		image_in.close();//close ifstream
	}
	else{
		cout << "File not found." << endl;
	}

	cout << "Finished loading image." << endl << endl;
}

char* toRGB(vector<int> imageData, int width, int height){
	char* newImageData = new char[width*height*3];

	int j = 0;
	for(int i = 0; i < width*height; i++){
		newImageData[j] = imageData[i];//Red
		newImageData[j + 1] = imageData[i];//Blue
		newImageData[j + 2] = imageData[i];//Green
		j += 3;
	}

	return newImageData;
}

int main(){
	vector<int> imageData;
	int width;
	int height;
	int greyscale;
	string file= "mountian.pgm";

	loadImage(imageData, width, height, greyscale, file);

	//Convert greyscale image data to RGB data.
	char* rgbImageData = toRGB(imageData, width, height);

	cout << "Displaying image..." << endl;
	displayImage(rgbImageData, width, height, greyscale);

	delete[] rgbImageData;
	return 0;
}
