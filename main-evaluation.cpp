#include <iostream>
#include "BasicLevenshteinDistance.h"
#include "fstream"

using namespace std;

int main(int argc, char **argv) {

    BasicLevenshteinDistance editDistanceCalculator;

    string accuracyFilePath=argv[1];
    ifstream infile(argv[2]);


    int smallEditDistance=100;
    int currentEditDistance;

    string dictWord;
    string clstmOutput;
    string pathToEachLine;
    string gt;

    while (getline(infile, pathToEachLine)) {

        fstream lineInput(pathToEachLine);

        ifstream dict("/Users/talha/Documents/Workspace/backtoschool/dictionary.txt");
        getline(lineInput, clstmOutput);

        smallEditDistance=100;

        while (getline(dict, dictWord)) {

            currentEditDistance = editDistanceCalculator.calcualteDistance(dictWord.c_str(), clstmOutput.c_str());

            if (currentEditDistance < smallEditDistance) {
                smallEditDistance = currentEditDistance;
                gt = dictWord;

            }
        }

        if(smallEditDistance <= 3) {

            lineInput.close();
            ofstream newFile(pathToEachLine, ios::out);
            newFile << gt;
        }

        dict.close();
    }

    infile.close();

}