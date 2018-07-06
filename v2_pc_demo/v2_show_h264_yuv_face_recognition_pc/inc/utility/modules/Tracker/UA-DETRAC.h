/*
* File containing all the related functions to use the
* UA-DETRAC dataset
* More info on <http://detrac-db.rit.albany.edu/>
* Added by Lucas
*/
//#pragma once
#ifndef UA_DETRAC_H
#define UA_DETRAC_H
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iomanip>

#include "Tracker/tracking.h"
#include "Tracker/CSV.h"

using namespace std;
using namespace FaceSDK;

// Read all the provided detections on a vector of vectors
void read_detections(ifstream& file, vector< vector<BoundingBox> >& detections)
{
	vector<BoundingBox> frame_detections;
	int actualFrame = 1;

	// If the column number X doesn't exists, it will do nothing
	for (CSVIterator loop(file); loop != CSVIterator(); ++loop)
	{
		int det_frame = stoi((*loop)[0]);
		//cout << "Detected in Frame N " << det_frame << "\n";
		while (actualFrame < det_frame) // We read all detections for previous frame
		{
			actualFrame++;
			detections.push_back(frame_detections);
			frame_detections.clear();
		}
		BoundingBox b = { 
			stof((*loop)[2]), stof((*loop)[3]),
			stof((*loop)[4]), stof((*loop)[5]), 
			stof((*loop)[6]) };
		//printf("Flag is: %d\n", stoi((*loop)[1]));
		int flag_has_object = stoi((*loop)[1]);
        if(flag_has_object != 0){
			frame_detections.push_back(b);        	
        }
	}
	// Last detections are not added in the loop, so we add them here
	detections.push_back(frame_detections);
	// Then we check how 
}

// To output only up to 2 decimals
string to_string_with_precision(float value)
{
	int decimals = 0;
	if (value != 0)
		decimals = 2;

	ostringstream out;
	out << fixed << setprecision(decimals) << value;
	return out.str();
}

#if 1

// Store results in a file in a format that will be loaded later and reshaped
// into a matrix by a matlab script
void write_results(string& sequence, string& output_folder, int total_frames,
	vector< Track >& tracks)
{
	char filename[255];
	snprintf(filename, sizeof(filename), "%s/%s.txt", output_folder.data(), sequence.data());
	ofstream out(filename);
	string line;

	cout << "Creating result file" << endl;
	for (auto t : tracks)
	{
//		cout << "Track " << t.id << " ~ started in frame -> " << t.startFrame  << endl;
		for (auto b : t.boxes)
		{
			line.append(to_string_with_precision(b.x) + ",");
			line.append(to_string_with_precision(b.y) + ",");
			line.append(to_string_with_precision(b.w) + ",");
			line.append(to_string_with_precision(b.h) + ",");
			line.append( to_string(t.startFrame+1) + "," );
			line.append( to_string(t.id) );
			// Write to files
			out << line << endl;
			// Empty the strings
			line.clear();
		}
	}
	cout << "Finish writing result file" << endl;
	// Close the file
	out.close();

}

#endif

#endif
