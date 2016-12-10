/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 CTTC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 * Author: Nhat Pham <nhatphd@kaist.ac.kr>
 */
#include <iostream>

#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/spectrum-model-ism2400MHz-res1MHz.h>
#include <ns3/spectrum-model-300kHz-300GHz-log.h>
#include <ns3/wifi-spectrum-value-helper.h>
#include <ns3/multi-model-spectrum-channel.h>
#include <ns3/waveform-generator.h>
#include <ns3/spectrum-analyzer.h>
#include <ns3/log.h>
#include <string>
#include <ns3/friis-spectrum-propagation-loss.h>
#include <ns3/propagation-delay-model.h>
#include <ns3/mobility-module.h>
#include <ns3/spectrum-helper.h>
#include <ns3/applications-module.h>
#include <ns3/adhoc-aloha-noack-ideal-phy-helper.h>
#include <ns3/waveform-generator-helper.h>
#include <ns3/spectrum-analyzer-helper.h>
#include <ns3/non-communicating-net-device.h>
#include <ns3/microwave-oven-spectrum-value-helper.h>

#include "acss_anim.h"
#include "acss_debug.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("ANN in CSS");

/* Global variables and definitions */
/* CONFIGURATION */
static const char animFileName[] = "test.xml";
static const int numOfPUs = 4;
static const int numOfNoiseGenerator = 1;
static const int numOfSUs = 20;
static const int simTime = 120; // in seconds

static const char puActivityFileNamePrefix[] = "PUs/pu_activity_";
static const char puActivityFileNameSuffix[] = ".tr";
static const char puTimeFrameFilename[] = "PUs/pu_activity_timeframes.tr";

static const char suSensingFileNameFrefix[] = "SUs/spectrum-analyzer-output";
static const char suSensingFileNameSuffix[] = "-0.tr";
static const char suEngeryVectorFileName[] = "SUs/energy_vectors.tr";

static bool g_verbose = true;

#define SSTR( x ) static_cast< ostringstream & >( \
        ( ostringstream() << dec << x ) ).str()

/* Static Functions */
void
PhyTxStartTrace (string context, Ptr<const Packet> p)
{
  string contextOfNoiseGen = "/NodeList/" + SSTR(numOfPUs) + "/DeviceList/0/Phy/TxStart";
  const char *context_chars;
  context_chars = context.c_str();

  if(context.compare(contextOfNoiseGen) != 0) {
    cout << Simulator::Now().GetSeconds() << " " << context << " PHY TX START p: " << p << endl;

    /* Write to pu activity file */
    ofstream puFile;
    string filename = puActivityFileNamePrefix + SSTR(context_chars[10]) + puActivityFileNameSuffix;

    puFile.open(filename.c_str(), ios::out | ios::app | ios::ate);

    puFile << "S " << Simulator::Now().GetSeconds() << endl;

    puFile.close();
  }
}


void
PhyTxEndTrace (string context, Ptr<const Packet> p)
{
  string contextOfNoiseGen = "/NodeList/" + SSTR(numOfPUs) + "/DeviceList/0/Phy/TxEnd";
  const char *context_chars;
  context_chars = context.c_str();

  if(context.compare(contextOfNoiseGen) != 0) {
    cout << Simulator::Now().GetSeconds() << " " << context << " PHY TX END p: " << p << endl;

    /* Write to pu activity file */
    ofstream puFile;
    string filename = puActivityFileNamePrefix + SSTR(context_chars[10]) + puActivityFileNameSuffix;

    puFile.open(filename.c_str(), ios::out | ios::app | ios::ate);

    puFile << "E " << Simulator::Now().GetSeconds() << endl;

    puFile.close();
  }
}

void
PhyRxStartTrace (string context, Ptr<const Packet> p)
{
  if (g_verbose)
    {
      cout << context << " PHY RX START p:" << p << endl;
    }
}

void
PhyRxEndOkTrace (string context, Ptr<const Packet> p)
{
  if (g_verbose)
    {
      cout << context << " PHY RX END OK p:" << p << endl;
    }
}

void
PhyRxEndErrorTrace (string context, Ptr<const Packet> p)
{
  if (g_verbose)
    {
      cout << context << " PHY RX END ERROR p:" << p << endl;
    }
}


void
ReceivePacket (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  uint64_t bytes = 0;
  while ((packet = socket->Recv ()))
    {
      bytes += packet->GetSize ();
    }
  if (g_verbose)
    {
      cout << "SOCKET received " << bytes << " bytes" << endl;
    }
}

Ptr<Socket>
SetupPacketReceive (Ptr<Node> node)
{
  TypeId tid = TypeId::LookupByName ("ns3::PacketSocketFactory");
  Ptr<Socket> sink = Socket::CreateSocket (node, tid);
  sink->Bind ();
  sink->SetRecvCallback (MakeCallback (&ReceivePacket));
  return sink;
}

/* Main */
int main (int argc, char** argv)
{
  CommandLine cmd;
  cmd.AddValue ("verbose", "Print trace information if true", g_verbose);
  cmd.Parse (argc, argv);

  /* Truncate PU activity files */
  ofstream puFile;

  for (int count = 0; count < numOfPUs; count++) {
    string filename = puActivityFileNamePrefix + SSTR(count) + puActivityFileNameSuffix;

    puFile.open(filename.c_str(), ios::out | ios::trunc);
    puFile.close();
  }

  /* Enable LOG */
  //LogComponentEnable("OnOffApplication", LOG_LEVEL_LOGIC);

  /* Create and setup nodes locations and mobility model */
  NodeContainer puNodes;
  NodeContainer noiseGeneratorNodes;
  NodeContainer suNodes;
  NodeContainer allNodes;

  puNodes.Create (numOfPUs);
  noiseGeneratorNodes.Create (numOfNoiseGenerator);
  suNodes.Create (numOfSUs);
  allNodes.Add (puNodes);
  allNodes.Add (noiseGeneratorNodes);
  allNodes.Add (suNodes);

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
                           "X", StringValue ("ns3::UniformRandomVariable[Min=0|Max=50]"),
                           "Y", StringValue ("ns3::UniformRandomVariable[Min=0|Max=50]"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (allNodes);

  /* Set up channel with default ConstantSpeedPropagationDelayModel and
   * FriisSpectrumPropagationLossModel */
  SpectrumChannelHelper channelHelper = SpectrumChannelHelper::Default ();
  channelHelper.SetChannel ("ns3::MultiModelSpectrumChannel");
  Ptr<SpectrumChannel> channel = channelHelper.Create ();

  /******************* Configure PU nodes *******************/
  WifiSpectrumValue5MhzFactory sf;

  double txPower = 0.5; // Watts
  uint32_t channelNumber = 4;
  Ptr<SpectrumValue> txPsd =  sf.CreateTxPowerSpectralDensity (txPower, channelNumber);

  // for the noise, we use the Power Spectral Density of thermal noise
  // at room temperature. The value of the PSD will be constant over the band of interest.
  const double k = 1.381e-23; //Boltzmann's constant
  const double T = 290; // temperature in Kelvin
  double noisePsdValue = k * T; // watts per hertz
  Ptr<SpectrumValue> noisePsd = sf.CreateConstant (noisePsdValue);


  AdhocAlohaNoackIdealPhyHelper adhocAlohaOfdmHelper;
  adhocAlohaOfdmHelper.SetChannel (channel);
  adhocAlohaOfdmHelper.SetTxPowerSpectralDensity (txPsd);
  adhocAlohaOfdmHelper.SetNoisePowerSpectralDensity (noisePsd);
  adhocAlohaOfdmHelper.SetPhyAttribute ("Rate", DataRateValue (DataRate ("10Kbps")));
  NetDeviceContainer ofdmDevices = adhocAlohaOfdmHelper.Install (puNodes);

  PacketSocketHelper packetSocket;
  packetSocket.Install (puNodes);

  /* Setup simulation to send data from node 1 to node 0 */
  PacketSocketAddress socketFrom1to0;
  socketFrom1to0.SetSingleDevice (ofdmDevices.Get (1)->GetIfIndex ());
  socketFrom1to0.SetPhysicalAddress (ofdmDevices.Get (0)->GetAddress ());
  socketFrom1to0.SetProtocol (1);

  OnOffHelper onoff ("ns3::PacketSocketFactory", Address (socketFrom1to0));
  onoff.SetAttribute ("OnTime", StringValue ("ns3::UniformRandomVariable[Min=2|Max=10]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::UniformRandomVariable[Min=7|Max=10]"));
  onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("10Kbps")));
  onoff.SetAttribute ("PacketSize", UintegerValue (1500));

  ApplicationContainer apps = onoff.Install (puNodes.Get (1));
  apps.Start (Seconds (0.0));
  apps.Stop (Seconds (simTime));

  /* Setup simulation to send data from node 2 to node 0 */
  PacketSocketAddress socketFrom2to0;
  socketFrom2to0.SetSingleDevice (ofdmDevices.Get (2)->GetIfIndex ());
  socketFrom2to0.SetPhysicalAddress (ofdmDevices.Get (0)->GetAddress ());
  socketFrom2to0.SetProtocol (1);

  OnOffHelper onoff2 ("ns3::PacketSocketFactory", Address (socketFrom2to0));
  onoff2.SetAttribute ("OnTime", StringValue ("ns3::UniformRandomVariable[Min=2|Max=10]"));
  onoff2.SetAttribute ("OffTime", StringValue ("ns3::UniformRandomVariable[Min=5|Max=10]"));
  onoff2.SetAttribute ("DataRate", DataRateValue (DataRate ("10Kbps")));
  onoff2.SetAttribute ("PacketSize", UintegerValue (1500));

  ApplicationContainer apps2 = onoff2.Install (puNodes.Get (2));
  apps2.Start (Seconds (simTime/3));
  apps2.Stop (Seconds (2*simTime/3));

  /* Setup simulation to send data from node 3 to node 0 */
  PacketSocketAddress socketFrom3to0;
  socketFrom3to0.SetSingleDevice (ofdmDevices.Get (3)->GetIfIndex ());
  socketFrom3to0.SetPhysicalAddress (ofdmDevices.Get (0)->GetAddress ());
  socketFrom3to0.SetProtocol (1);

  OnOffHelper onoff3 ("ns3::PacketSocketFactory", Address (socketFrom3to0));
  onoff3.SetAttribute ("OnTime", StringValue ("ns3::UniformRandomVariable[Min=2|Max=10]"));
  onoff3.SetAttribute ("OffTime", StringValue ("ns3::UniformRandomVariable[Min=2|Max=10]"));
  onoff3.SetAttribute ("DataRate", DataRateValue (DataRate ("10Kbps")));
  onoff3.SetAttribute ("PacketSize", UintegerValue (1500));

  ApplicationContainer apps3 = onoff3.Install (puNodes.Get (3));
  apps3.Start (Seconds (2*simTime/3 + 1));
  apps3.Stop (Seconds (simTime));

  Ptr<Socket> recvSink = SetupPacketReceive (puNodes.Get (0));

  /******************* Configure noise generator *******************/
  Ptr<SpectrumValue> mwoPsd =  MicrowaveOvenSpectrumValueHelper::CreatePowerSpectralDensityMwo1 ();
  NS_LOG_INFO ("mwoPsd : " << *mwoPsd);

  WaveformGeneratorHelper waveformGeneratorHelper;
  waveformGeneratorHelper.SetChannel (channel);
  waveformGeneratorHelper.SetTxPowerSpectralDensity (mwoPsd);

  waveformGeneratorHelper.SetPhyAttribute ("Period", TimeValue (Seconds (1.0 / 60)));   // corresponds to 60 Hz
  waveformGeneratorHelper.SetPhyAttribute ("DutyCycle", DoubleValue (0.5));
  NetDeviceContainer waveformGeneratorDevices = waveformGeneratorHelper.Install (noiseGeneratorNodes);

  Simulator::Schedule (Seconds (0), &WaveformGenerator::Start,
                       waveformGeneratorDevices.Get (0)->GetObject<NonCommunicatingNetDevice> ()->GetPhy ()->GetObject<WaveformGenerator> ());


  /******************* Configure spectrum sensing nodes *******************/
  SpectrumAnalyzerHelper spectrumAnalyzerHelper;
  spectrumAnalyzerHelper.SetChannel (channel);
  spectrumAnalyzerHelper.SetRxSpectrumModel (SpectrumModelIsm2400MhzRes1Mhz);
  spectrumAnalyzerHelper.SetPhyAttribute ("Resolution", TimeValue (MilliSeconds (100)));
  spectrumAnalyzerHelper.SetPhyAttribute ("NoisePowerSpectralDensity", DoubleValue (1e-15));  // -120 dBm/Hz
  spectrumAnalyzerHelper.EnableAsciiAll (suSensingFileNameFrefix);
  NetDeviceContainer spectrumAnalyzerDevices = spectrumAnalyzerHelper.Install (suNodes);

  /*
    you can get a nice plot of the output of SpectrumAnalyzer with this gnuplot script:

    unset surface
    set pm3d at s
    set palette
    set key off
    set view 50,50
    set xlabel "time (ms)"
    set ylabel "freq (MHz)"
    set zlabel "PSD (dBW/Hz)" offset 15,0,0
    splot "./spectrum-analyzer-output-3-0.tr" using ($1*1000.0):($2/1e6):(10*log10($3))
  */


  Config::Connect ("/NodeList/*/DeviceList/*/Phy/TxStart", MakeCallback (&PhyTxStartTrace));
  Config::Connect ("/NodeList/*/DeviceList/*/Phy/TxEnd", MakeCallback (&PhyTxEndTrace));
//  Config::Connect ("/NodeList/*/DeviceList/*/Phy/RxStart", MakeCallback (&PhyRxStartTrace));
//  Config::Connect ("/NodeList/*/DeviceList/*/Phy/RxEndOk", MakeCallback (&PhyRxEndOkTrace));
//  Config::Connect ("/NodeList/*/DeviceList/*/Phy/RxEndError", MakeCallback (&PhyRxEndErrorTrace));

  /* Anim */
  acss_anim test_anim("test.xml");

  for (int count = 0; count < numOfPUs; count++) {
      test_anim.update_pu_icon(puNodes.Get(count)->GetId());
  }

  for (int count = 0; count < numOfSUs; count++) {
      test_anim.update_su_icon(suNodes.Get(count)->GetId());
  }

  for (int count = 0; count < numOfNoiseGenerator; count++) {
      test_anim.update_noise_gen_icon(noiseGeneratorNodes.Get(count)->GetId());
  }

  Simulator::Stop (Seconds(simTime));

  Simulator::Run ();

  Simulator::Destroy ();

  /* Post simulation files handling */
  /* Extract PU activities */
//  double time_frame = 0; // in seconds
//
//  /* open output file */
//  puTimeFrameFilename
//  for (time_frame = 0; time_frame < simTime; time_frame += 0.1) {
//
//  }

  /* Extract SUs energy vectors */
  ofstream energyVectorFile;
  energyVectorFile.open(suEngeryVectorFileName, ios::out | ios::trunc);

  /* Open SUs trace files */
  ifstream suFiles[numOfSUs];

  for (int count = 0; count < numOfSUs; count++) {
    string filename = suSensingFileNameFrefix;
    filename +="-" + SSTR(suNodes.Get(count)->GetId()) + suSensingFileNameSuffix;

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

          if ((readFreq == 2.435e+09)) {
            cout << "SU " << SSTR(suNodes.Get(count)->GetId()) << " found a right energy" << endl;
            cout << readTimeFrame << " " << readFreq << " " << readPower << endl;
            energyVectorFile << readPower << " ";

            break;
          }
        }
      }
      else {
        cout << "Can't open su " << suNodes.Get(count)->GetId() << endl;
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


