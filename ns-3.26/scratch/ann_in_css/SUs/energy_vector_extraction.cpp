/*
 * energy_vector_extraction.cpp
 *
 *  Created on: Dec 9, 2016
 *      Author: nhatpham
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

using namespace std;

#define SSTR( x ) static_cast< ostringstream & >( \
        ( ostringstream() << dec << x ) ).str()

static const char suSensingFileNameFrefix[] = "spectrum-analyzer-output";
static const char suSensingFileNameSuffix[] = "-0.tr";
static const char suEngeryVectorFileName[] = "energy_vectors.tr";

static const int numOfSUs = 20;
static const int SUStartIndex = 5;

static const int simTime = 30; // in seconds

bool AreSame(double a, double b)
{
    return fabs(a - b) < 0.1;
}

int main(void) {
  /* Extract SUs energy vectors */
  ofstream energyVectorFile;
  energyVectorFile.open(suEngeryVectorFileName, ios::out | ios::trunc);

  /* Open SUs trace files */
  ifstream suFiles[numOfSUs];

  for (int count = 0; count < numOfSUs; count++) {
    string filename = suSensingFileNameFrefix;
    filename +="-" + SSTR(SUStartIndex + count) + suSensingFileNameSuffix;

    cout << "opening " << filename << endl;
    suFiles[count].open(filename.c_str());
  }

  for (double time_frame = 0.1; time_frame < simTime; time_frame += 0.1) {
    /* Write timeframe to output file */
    energyVectorFile << time_frame << " ";
    cout << "at time frame " << time_frame << endl;

    for (int count = 0; count < numOfSUs; count++) {
      if (suFiles[count].is_open()) {
        double readTimeFrame;
        double readFreq, readPower;

        while (!suFiles[count].eof()) {
          suFiles[count] >> readTimeFrame >> readFreq >> readPower;

          if ((AreSame(readFreq, 2.435e+09))) {
            cout << "SU " << SSTR(SUStartIndex + count) << " found a right energy" << endl;
            cout << readTimeFrame << " " << readFreq << " " << readPower << endl;
            energyVectorFile << readPower << " ";

            break;
          }
        }
      }
      else {
        cout << "Can't open su " << SUStartIndex + count << endl;
      }
    }

    /* end one energy vector */
    energyVectorFile << endl;
  }

  for (int count = 0; count < numOfSUs; count++) {
    suFiles[count].close();
  }

  energyVectorFile.close();
}


