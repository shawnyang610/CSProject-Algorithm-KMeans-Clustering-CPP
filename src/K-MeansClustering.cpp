// CSC700-34 Proj5 K-Means (C++)
// Prof. Tsaiyun Phillips
// Student: Shawn Yang

//#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cmath>
using namespace std;

class Point {
public:
	int coordinateX;
	int coordinateY;
	int label;
	double distance;
	static void printPoint(Point *inPoint, ofstream *outFile) {
		*outFile << inPoint -> coordinateX << " " << inPoint -> coordinateY << " " << inPoint -> label << endl;
	}
};

class K_Mean {
public:
	int k;
	struct xyCoord {
	public:
		int coordinateX;
		int coordinateY;
		int label;
		xyCoord() {
			coordinateX = 0;
			coordinateY = 0;
		}
	};
	xyCoord *Kcentroids;
	int numPts;
	Point *pointSet;
	int numRow;
	int numCol;
	int **imageArray;
	int changeLabel;
	int loadPointSetCounter;
	ofstream *outFile1;
	ofstream *outFile2;
	K_Mean(int inK, int inNumPts, int inNumRow, int inNumCol, ofstream *inOutFile1, ofstream *inOutFile2) {
		k = inK;
		numPts = inNumPts;
		numRow = inNumRow;
		numCol = inNumCol;
		Kcentroids = new xyCoord[k];
		pointSet = new Point[numPts];
		imageArray = new int*[numRow];
		for (int i = 0; i < numRow; i++) {
			imageArray[i] = new int[numCol];
		}
		loadPointSetCounter = 0;
		outFile1 = inOutFile1;
		outFile2 = inOutFile2;
		changeLabel = 0;
	}
	void loadPointSet(int inCoordX, int inCoordY) {
		if (loadPointSetCounter >= numPts) {
			cout << "error, pointSet array out of bound." << endl;
			exit(EXIT_FAILURE);
		}
		else {
			pointSet[loadPointSetCounter].coordinateX = inCoordX;
			pointSet[loadPointSetCounter].coordinateY = inCoordY;
			loadPointSetCounter++;
		}
	}
	void assignLabel() {
		int incrementor = 0;
		for (int i = 0; i < numPts; i++) {
			if (incrementor%(k+1) == 0) {
				incrementor++;
			}
				pointSet[i].label = incrementor%(k+1);
				incrementor++;
		}
		//the commented code below is the my original way of asigning labels
		//int numOfPtsPerCluster = ceil( numPts / (k+0.0));
		//int remainder = numPts % k;
		//int index = 0;
		//for (int i = 0; i < k; i++) {
		//	if (i<remainder){
		//		for (int j = 0; j < numOfPtsPerCluster; j++) {
		//			pointSet[index++].label = i;
		//		}
		//	}
		//	else {
		//		for (int j = 0; j < numOfPtsPerCluster - 1; j++) {
		//			pointSet[index++].label = i;
		//		}
		//	}	
		//}
		//printPointSet();
	}
	void mapPoint2Image() {
		for (int i = 0; i < numPts; i++) 
			imageArray[pointSet[i].coordinateX][pointSet[i].coordinateY] = pointSet[i].label;
		//print centroids on Image for testing purpose
		/*for (int i = 0; i < k; i++)
			imageArray[Kcentroids[i].coordinateX][Kcentroids[i].coordinateY] = i+100;*/
	}
	void setCentroids() {
		int *countGroups = new int[k];
		for (int i = 0; i < k; i++) {
			countGroups[i] = 0;
		}
		//add all each axis of points belong to same group together
		for (int i = 0; i < numPts; i++) {
			countGroups[pointSet[i].label-1]++; //count number of points in each group
			Kcentroids[pointSet[i].label-1].coordinateX += pointSet[i].coordinateX;
			Kcentroids[pointSet[i].label-1].coordinateY += pointSet[i].coordinateY;
		}
		//get the average for each centroid's two axis
		for (int i = 0; i < k; i++) {
			if (countGroups[i] == 0) {
				Kcentroids[i].coordinateX = -9999;
				Kcentroids[i].coordinateY = -9999;
				Kcentroids[i].label = i+1;
			}
			else {
				Kcentroids[i].coordinateX = Kcentroids[i].coordinateX / countGroups[i];
				Kcentroids[i].coordinateY = Kcentroids[i].coordinateY / countGroups[i];
				Kcentroids[i].label = i+1;
			}
		}
		
	}
	void relabelPoint(Point *inPoint) {
		//allocate distance array and initialize to 0, it's used to store distances between the point to each centroid
		double *distance = new double[k];
		for (int i = 0; i < k; i++) {
			distance[i] = 0;
		}
		//calculate distance between inPoint and each centroid, then stored in an array.
		for (int i = 0; i < k; i++) {
			distance[i] = sqrt(pow((inPoint->coordinateX - Kcentroids[i].coordinateX), 2) + pow((inPoint->coordinateY - Kcentroids[i].coordinateY), 2));
		}
		//find the shortest distance
		int shortestIndex=0;
		
		for (int i = 0; i < k - 1; i++) {
			if (distance[shortestIndex] > distance[i + 1]) {
				shortestIndex = i+1;
			}
		}
		//relabel point(to the closest centroid's group) if the closest centroid is of different group
		if (shortestIndex+1 != inPoint->label) {
			inPoint->label = shortestIndex+1;
			changeLabel++;
		}
	}
	void kMeanClustering (){
		assignLabel();   //step2
		do {	
			mapPoint2Image();//step3
			prettyPrint();   //step4
			//step5
			changeLabel = 0;
			setCentroids();
			//step6-7: find the closest Centroid, then check if they belong to same group, relabel
			//the point if it's in a different group.
			for (int i = 0; i < numPts; i++) {
				relabelPoint(&pointSet[i]);
			}
		} while (changeLabel>0);//step 8 repeat step 3-7 while changeLabel>0
		printPointSet();
	};
	void printPointSet() {
		*outFile1 << k << endl;
		*outFile1 << numPts << endl;
		*outFile1 << numRow << " " << numCol << endl;
		for (int i = 0; i < numPts; i++)
			Point::printPoint(&pointSet[i], outFile1);
	}
	void prettyPrint() {
		for (int i = 0; i < numRow; i++) {
			for (int j = 0; j < numCol; j++) {
				if (imageArray[i][j] < 0)
					*outFile2 << " ";
				else
					*outFile2 << imageArray[i][j];
			}
			*outFile2 << endl;
		}
		*outFile2 << "================================================================================" << endl;
	}
};


int main(int argc, char *argv[]){
	K_Mean *kMean;
	ifstream inFile;
	ofstream outFile1;
	ofstream outFile2;
	string initLine[3];
	int numClusters, numDataPoints, numRows, numCols, coordX, coordY;

	if (argc < 4) {
		cout << "Please specify 1 input file and 2 output files, and try again." << endl;
		return 0;
	}
	else {

		try {
			inFile.open(argv[1]);
			outFile1.open(argv[2]);
			outFile2.open(argv[3]);
		} catch (exception&) {}
		//read first 3 lines from input and instantiate K-mean class
		inFile >> numClusters >> numDataPoints >> numRows >> numCols;
		kMean = new K_Mean(numClusters, numDataPoints, numRows, numCols, &outFile1, &outFile2);
		//read all the points from input to pointset and close input file.
		while (inFile >> coordX >> coordY) {
			kMean->loadPointSet(coordX, coordY);		
		}
		//kMean->printPointSet();
		inFile.close();

		kMean->kMeanClustering();

		outFile1.close();
		outFile2.close();

		return 0;
	}
}

