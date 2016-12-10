/*
 * pus_activity_extraction.cpp
 *
 *  Created on: Dec 9, 2016
 *      Author: nhatpham
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <stdio.h>
#include <string.h>

using namespace std;

#define SSTR( x ) static_cast< ostringstream & >( \
        ( ostringstream() << dec << x ) ).str()

bool AreSame(double a, double b)
{
    return fabs(a - b) < 0.1;
}

static const char puActivityFileNameFrefix[] = "pu_activity_";
static const char puActivityFileNameSuffix[] = ".tr";
static const char puExtActivityFileName[] = "pus_activity_timeframe.tr";

static const int numOfPUs = 4;
static const int PUStartIndex = 0;

static const int simTime = 120; // in seconds
static const double timeFramePeriod = 0.1; // in seconds

typedef struct {
  double start;
  double end;
} puActTimePeriod_t;

/* input should be opened in advance */
int findNextPairOfTimePeriod(ifstream &file, puActTimePeriod_t &outputTimePeiod);

int main(void) {
  /* Extract PU activity */
  ofstream puExtrFile;
  puExtrFile.open(puExtActivityFileName, ios::out | ios::trunc);

  /* Open SUs trace files */
  ifstream puFiles[numOfPUs];

  for (int count = 0; count < numOfPUs; count++) {
    string filename = puActivityFileNameFrefix + SSTR(PUStartIndex + count)
        + puActivityFileNameSuffix;

    cout << "opening " << filename << endl;
    puFiles[count].open(filename.c_str());
  }

  /* Get first time periods */
  puActTimePeriod_t suOnTimePeriods[numOfPUs];
  memset(suOnTimePeriods, 0, sizeof(suOnTimePeriods));

  for (int count = 0; count < numOfPUs; count++) {
    cout << "PU " << PUStartIndex + count << endl;
    findNextPairOfTimePeriod(puFiles[count], suOnTimePeriods[count]);
  }

  for (double time_frame = 0.1; time_frame < simTime; time_frame += timeFramePeriod) {
    /* Write timeframe to output file */
    puExtrFile << time_frame << " ";
    cout << "at time frame " << time_frame << endl;

    /* Check current time frame with time period of each PU */
    bool isChannelBusy = false;
    for (int pu_count = 0; pu_count < numOfPUs; pu_count++) {
      cout << "check time " << time_frame << " with PU " << PUStartIndex + pu_count << endl;

      while (1) {
        if (time_frame < suOnTimePeriods[pu_count].start) {
          /* do nothing */
          cout << time_frame << " < start: " << suOnTimePeriods[pu_count].start << endl;
          break;
        }
        else if (time_frame < suOnTimePeriods[pu_count].end){
          /* busy */
          cout << " < start: " << suOnTimePeriods[pu_count].start
              << " time_frame < end: " << suOnTimePeriods[pu_count].end
              << "=> busy" << endl;

          isChannelBusy = true;
          break;
        }
        else {
          /* time_frame > suOnTimePeriods[pu_count].end, find next time period */
          cout << time_frame << " > end: " << suOnTimePeriods[pu_count].end << endl;
          if(findNextPairOfTimePeriod(puFiles[pu_count], suOnTimePeriods[pu_count]) == 0) {
            /* Found a new pair of time period */
            cout << "Found a new pair, start: " << suOnTimePeriods[pu_count].start
                << " end: " << suOnTimePeriods[pu_count].end << endl;
          }
          else {
            /* Can't find a new pair */
            cout << "Can't find a new pair" << endl;
            break;
          }
        }

      }

      if (isChannelBusy) {
        break;
      }
    } /* end for all pus */

    /* Write to pu activity file */
    cout << "Write to file: " << isChannelBusy << endl;
    puExtrFile << (int)isChannelBusy;

    /* end one timeframe */
    puExtrFile << endl;
  }

  for (int count = 0; count < numOfPUs; count++) {
    puFiles[count].close();
  }

  puExtrFile.close();
}

int findNextPairOfTimePeriod(ifstream &file, puActTimePeriod_t &outputTimePeiod) {
  if (file.is_open()) {
    char readChar;
    double readStartTime, readEndTime;

    if (file.eof()) {
      return -1;
    }

    while (!file.eof()) {
      /* File the first pair of S and E */
      file >> readChar >> readStartTime;

      /* Find S */
      if (readChar == 'S') {
        cout <<"Found Start " << readChar << " " << readStartTime << endl;

        /* Find E */
        file >> readChar >> readEndTime;
        if (readChar == 'E') {
          cout << "Found End " << readChar << " " << readEndTime << endl;

          /* Sanity check */
          if (readStartTime > readEndTime) {
            cout << "Something wrong. Start: " << readStartTime << " End: "
                << readEndTime << endl;
          }
          else {
            /* Ok, update new time period */
            outputTimePeiod.start = readStartTime;
            outputTimePeiod.end = readEndTime;

            cout << "New time period: "
                << "Start " << outputTimePeiod.start
                << " End " << outputTimePeiod.end << endl;
          }

          /* Found both S and E, break while loop */
          break;
        }/* end if found E */
      }/* end if found S */

    }/* end file */
  }
  else {
    cout << "Can't open file" << endl;
    return -1;
  }

  return 0;
}


