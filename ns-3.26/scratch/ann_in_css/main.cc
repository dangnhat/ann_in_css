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

NS_LOG_COMPONENT_DEFINE ("ANN in CSS");

/* Global variables and definitions */
/* CONFIGURATION */
static const std::string animFileName[] = "test.xml";
static const int numOfPUs = 3;
static const int numOfSUs = 100;
static const int numOfNoiseGenerator = 1;
static const int simTime = 10; // in seconds

static bool g_verbose = false;

/* Static Functions */
void
PhyTxStartTrace (std::string context, Ptr<const Packet> p)
{
  if (g_verbose)
    {
      std::cout << context << " PHY TX START p: " << p << std::endl;
    }
}


void
PhyTxEndTrace (std::string context, Ptr<const Packet> p)
{
  if (g_verbose)
    {
      std::cout << context << " PHY TX END p: " << p << std::endl;
    }
}

void
PhyRxStartTrace (std::string context, Ptr<const Packet> p)
{
  if (g_verbose)
    {
      std::cout << context << " PHY RX START p:" << p << std::endl;
    }
}

void
PhyRxEndOkTrace (std::string context, Ptr<const Packet> p)
{
  if (g_verbose)
    {
      std::cout << context << " PHY RX END OK p:" << p << std::endl;
    }
}

void
PhyRxEndErrorTrace (std::string context, Ptr<const Packet> p)
{
  if (g_verbose)
    {
      std::cout << context << " PHY RX END ERROR p:" << p << std::endl;
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
      std::cout << "SOCKET received " << bytes << " bytes" << std::endl;
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
                           "X", StringValue ("ns3::UniformRandomVariable[Min=0|Max=100]"),
                           "Y", StringValue ("ns3::UniformRandomVariable[Min=0|Max=100]"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (allNodes);

  /* Set up channel with default ConstantSpeedPropagationDelayModel and
   * FriisSpectrumPropagationLossModel */
  SpectrumChannelHelper channelHelper = SpectrumChannelHelper::Default ();
  channelHelper.SetChannel ("ns3::MultiModelSpectrumChannel");
  Ptr<SpectrumChannel> channel = channelHelper.Create ();

  /******************* Configure PU nodes *******************/
  WifiSpectrumValue5MhzFactory sf;

  double txPower = 0.1; // Watts
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
  adhocAlohaOfdmHelper.SetPhyAttribute ("Rate", DataRateValue (DataRate ("1Mbps")));
  NetDeviceContainer ofdmDevices = adhocAlohaOfdmHelper.Install (puNodes);

  PacketSocketHelper packetSocket;
  packetSocket.Install (puNodes);

  PacketSocketAddress socket;
  socket.SetSingleDevice (ofdmDevices.Get (0)->GetIfIndex ());
  socket.SetPhysicalAddress (ofdmDevices.Get (1)->GetAddress ());
  socket.SetProtocol (1);

  OnOffHelper onoff ("ns3::PacketSocketFactory", Address (socket));
  onoff.SetAttribute ("OnTime", StringValue ("ns3::ExponentialRandomVariable[Mean=0.04]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ExponentialRandomVariable[Mean=0.01]"));
  onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("0.4Mbps")));
  onoff.SetAttribute ("PacketSize", UintegerValue (1500));

  ApplicationContainer apps = onoff.Install (puNodes.Get (0));
  apps.Start (Seconds (0.0));
  apps.Stop (Seconds (simTime));

  Ptr<Socket> recvSink = SetupPacketReceive (puNodes.Get (1));

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
  spectrumAnalyzerHelper.EnableAsciiAll ("outputs/spectrum-analyzer-output");
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
  Config::Connect ("/NodeList/*/DeviceList/*/Phy/RxStart", MakeCallback (&PhyRxStartTrace));
  Config::Connect ("/NodeList/*/DeviceList/*/Phy/RxEndOk", MakeCallback (&PhyRxEndOkTrace));
  Config::Connect ("/NodeList/*/DeviceList/*/Phy/RxEndError", MakeCallback (&PhyRxEndErrorTrace));

  /* Anim */
  acss_anim test_anim("test.xml");

//  for (int count = 0; count < numOfPUs; count++) {
//      test_anim.update_pu_icon(puNodes.Get(count)->GetId());
//  }
//
//  for (int count = 0; count < numOfSUs; count++) {
//      test_anim.update_su_icon(suNodes.Get(count)->GetId());
//  }
//
//  for (int count = 0; count < numOfSUs; count++) {
//      test_anim.update_noise_gen_icon(noiseGeneratorNodes.Get(count)->GetId());
//  }

  Simulator::Stop (Seconds (simTime));

  Simulator::Run ();

  Simulator::Destroy ();

}


