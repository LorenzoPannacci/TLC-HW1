#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Network Topology
//
//               10.0.1.0/30           10.0.2.0/30
//          n1-----------------n3-----------------------n6
//          |                                      ____/ |
//  n0      |       n2                10.0.4.0/30 /      |       n7      n8
//  |       |       |                            /       |       |       |
//  =================             n4-----------n5        =================
//   192.138.1.0/24                 10.0.3.0/30           192.138.2.0/24

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Task_1_Team_46");

int
main(int argc, char* argv[])
{
    // Gestione argomenti funzione
    uint32_t configuration = 0; // default

    CommandLine cmd(__FILE__);
    cmd.AddValue("configuration", "Configuration value", configuration);
    cmd.Parse(argc, argv);

    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);

    // Creazione rete
    NS_LOG_INFO("Create nodes.");

    uint32_t nNodes = 9;
    NodeContainer nodes;
    nodes.Create(nNodes);

    NodeContainer csma1Nodes;
    csma1Nodes.Add(nodes.Get(0));
    csma1Nodes.Add(nodes.Get(1));
    csma1Nodes.Add(nodes.Get(2));

    NodeContainer csma2Nodes;
    csma2Nodes.Add(nodes.Get(6));
    csma2Nodes.Add(nodes.Get(7));
    csma2Nodes.Add(nodes.Get(8));

    NodeContainer l0Nodes;
    l0Nodes.Add(nodes.Get(1));
    l0Nodes.Add(nodes.Get(3));

    NodeContainer l1Nodes;
    l1Nodes.Add(nodes.Get(3));
    l1Nodes.Add(nodes.Get(6));

    NodeContainer l2Nodes;
    l2Nodes.Add(nodes.Get(4));
    l2Nodes.Add(nodes.Get(5));

    NodeContainer l3Nodes;
    l3Nodes.Add(nodes.Get(5));
    l3Nodes.Add(nodes.Get(6));

    NS_LOG_INFO("Create and install channels.");

    CsmaHelper csma1Helper;
    csma1Helper.SetChannelAttribute("DataRate", StringValue("25Mbps"));
    csma1Helper.SetChannelAttribute("Delay", StringValue("10us"));
    NetDeviceContainer csma1Device = csma1Helper.Install(csma1Nodes);

    CsmaHelper csma2Helper;
    csma2Helper.SetChannelAttribute("DataRate", StringValue("30Mbps"));
    csma2Helper.SetChannelAttribute("Delay", StringValue("20us"));
    NetDeviceContainer csma2Device = csma2Helper.Install(csma2Nodes);

    PointToPointHelper l0Helper;
    l0Helper.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    l0Helper.SetChannelAttribute("Delay", StringValue("5us"));
    NetDeviceContainer l0Device = l0Helper.Install(l0Nodes);

    PointToPointHelper l1Helper;
    l1Helper.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    l1Helper.SetChannelAttribute("Delay", StringValue("5us"));
    NetDeviceContainer l1Device = l1Helper.Install(l1Nodes);

    PointToPointHelper l2Helper;
    l2Helper.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    l2Helper.SetChannelAttribute("Delay", StringValue("5us"));
    NetDeviceContainer l2Device = l2Helper.Install(l2Nodes);

    PointToPointHelper l3Helper;
    l3Helper.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    l3Helper.SetChannelAttribute("Delay", StringValue("5us"));
    NetDeviceContainer l3Device = l3Helper.Install(l3Nodes);

    InternetStackHelper internet;
    internet.Install(nodes);

    NS_LOG_INFO("Assign IP Addresses.");

    Ipv4AddressHelper address;

    address.SetBase("192.138.1.0", "/24");
    Ipv4InterfaceContainer csma1Interface = address.Assign(csma1Device);

    address.SetBase("192.138.2.0", "/24");
    Ipv4InterfaceContainer csma2Interface = address.Assign(csma2Device);

    address.SetBase("10.0.1.0", "/30");
    Ipv4InterfaceContainer l0Interface = address.Assign(l0Device);

    address.SetBase("10.0.2.0", "/30");
    Ipv4InterfaceContainer l1Interface = address.Assign(l1Device);

    address.SetBase("10.0.3.0", "/30");
    Ipv4InterfaceContainer l2Interface = address.Assign(l2Device);

    address.SetBase("10.0.4.0", "/30");
    Ipv4InterfaceContainer l3Interface = address.Assign(l3Device);

    NS_LOG_INFO("Create applications.");

    if (configuration == 0)
    {
        // n2 (TCP Sink)
        uint16_t port2 = 2400;
        Address n2Address(InetSocketAddress(csma1Interface.GetAddress(2), port2));
        PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", n2Address);
        ApplicationContainer n2App = packetSinkHelper.Install(nodes.Get(2));

        n2App.Start(Seconds(2.0)); // Server aperto 1 secondo prima del client
        n2App.Stop(Seconds(16.0)); // e chiuso un secondo dopo il client

        // n4 (TCP OnOff Client)
        uint32_t packetSize = 1500;
        OnOffHelper onOffHelper("ns3::TcpSocketFactory", n2Address);
        onOffHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
        onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        ApplicationContainer n4App = onOffHelper.Install(nodes.Get(4));

        n4App.Start(Seconds(3.0));
        n4App.Stop(Seconds(15.0));
    }

    else if (configuration == 1)
    {
        // n0 (TCP Sink)
        uint16_t port0 = 7777;
        Address n0Address(InetSocketAddress(csma1Interface.GetAddress(0), port0));
        PacketSinkHelper packetSinkHelper0("ns3::TcpSocketFactory", n0Address);
        ApplicationContainer n0App = packetSinkHelper0.Install(nodes.Get(0));

        n0App.Start(Seconds(4.0)); // Server aperto 1 secondo prima del client
        n0App.Stop(Seconds(16.0)); // e chiuso un secondo dopo il client

        // n2 (TCP Sink)
        uint16_t port2 = 2400;
        Address n2Address(InetSocketAddress(csma1Interface.GetAddress(2), port2));
        PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", n2Address);
        ApplicationContainer n2App = packetSinkHelper.Install(nodes.Get(2));

        n2App.Start(Seconds(1.0)); // Server aperto 1 secondo prima del client
        n2App.Stop(Seconds(10.0)); // e chiuso un secondo dopo il client

        // n4 (TCP OnOff Client)
        uint32_t packetSize4 = 2500;
        OnOffHelper onOffHelper4("ns3::TcpSocketFactory", n0Address);
        onOffHelper4.SetAttribute("PacketSize", UintegerValue(packetSize4));
        onOffHelper4.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelper4.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        ApplicationContainer n4App = onOffHelper4.Install(nodes.Get(4));

        n4App.Start(Seconds(5.0));
        n4App.Stop(Seconds(15.0));

        // n8 (TCP OnOff Client)
        uint32_t packetSize8 = 4500;
        OnOffHelper onOffHelper8("ns3::TcpSocketFactory", n2Address);
        onOffHelper8.SetAttribute("PacketSize", UintegerValue(packetSize8));
        onOffHelper8.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelper8.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        ApplicationContainer n8App = onOffHelper8.Install(nodes.Get(8));

        n8App.Start(Seconds(2.0));
        n8App.Stop(Seconds(9.0));
    }

    else if (configuration == 2)
    {
        // n2 (UDP ECHO Server)
        uint16_t portEcho2 = 63;
        Address n2EchoAddress(InetSocketAddress(csma1Interface.GetAddress(2), portEcho2));
        UdpEchoServerHelper n2Server(portEcho2);
        ApplicationContainer n2EchoApp = n2Server.Install(nodes.Get(2));

        n2EchoApp.Start(Seconds(2.0)); // Server aperto 1 secondo prima del client
        n2EchoApp.Stop(Seconds(13.0)); // e chiuso un secondo dopo il client

        // n8 (UDP ECHO Client)
        uint32_t packetSize8 = 2560;
        uint32_t maxPacketCount = 5;
        Time interPacketInterval = Seconds(2);
        UdpEchoClientHelper n8Client(n2EchoAddress, portEcho2);
        n8Client.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
        n8Client.SetAttribute("Interval", TimeValue(interPacketInterval));
        n8Client.SetAttribute("PacketSize", UintegerValue(packetSize8));
        ApplicationContainer n8App = n8Client.Install(nodes.Get(8));

        n8App.Start(Seconds(3.0));
        n8App.Stop(Seconds(12.0));

        int* messaggio = (int*)calloc(packetSize8, sizeof(char));
        *messaggio = 1948926 + 1948942 + 1915940 + 1883939;
        n8Client.SetFill(n8App.Get(0), (uint8_t*)messaggio, packetSize8, packetSize8);

        // n2 (TCP Sink)
        uint16_t portSink2 = 2600;
        Address n2SinkAddress(InetSocketAddress(csma1Interface.GetAddress(2), portSink2));
        PacketSinkHelper packetSinkHelper2("ns3::TcpSocketFactory", n2SinkAddress);
        ApplicationContainer n2SinkApp = packetSinkHelper2.Install(nodes.Get(2));

        n2SinkApp.Start(Seconds(2.0)); // Server aperto 1 secondo prima del client
        n2SinkApp.Stop(Seconds(10.0)); // e chiuso un secondo dopo il client

        // n4 (TCP OnOff Client)
        uint32_t packetSize4 = 3000;
        OnOffHelper onOffHelper4("ns3::TcpSocketFactory", n2SinkAddress);
        onOffHelper4.SetAttribute("PacketSize", UintegerValue(packetSize4));
        onOffHelper4.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelper4.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        ApplicationContainer n4App = onOffHelper4.Install(nodes.Get(4));

        n4App.Start(Seconds(3.0));
        n4App.Stop(Seconds(9.0));

        // n0 (UDP Sink)
        uint16_t port0 = 2500;
        Address n0Address(InetSocketAddress(csma1Interface.GetAddress(0), port0));
        PacketSinkHelper packetSinkHelper0("ns3::UdpSocketFactory", n0Address);
        ApplicationContainer n0App = packetSinkHelper0.Install(nodes.Get(0));

        n0App.Start(Seconds(4.0)); // Server aperto 1 secondo prima del client
        n0App.Stop(Seconds(16.0)); // e chiuso un secondo dopo il client

        // n7 (UDP OnOff Client)
        uint32_t packetSize7 = 3000;
        OnOffHelper onOffHelper7("ns3::UdpSocketFactory", n0Address);
        onOffHelper7.SetAttribute("PacketSize", UintegerValue(packetSize7));
        onOffHelper7.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelper7.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        ApplicationContainer n7App = onOffHelper7.Install(nodes.Get(7));

        n7App.Start(Seconds(5.0));
        n7App.Stop(Seconds(15.0));
    }

    else
    {
        fprintf(stderr, "Configurazione invalida.\n");
        exit(0);
    }

    NS_LOG_INFO("Enable static global routing.");
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    NS_LOG_INFO("Enable tracing.");

    // PCAP Tracing
    // Nella nomenclatura originale il primo numero nel nome generato dei file PCAP è il numero del
    // nodo, mentre il secondo è detto "devicename", ed è un numero relativo alla connessione,
    // questo numero viene assegnato in base a quando quella connessione è stata installata sul
    // nodo.

    if (configuration == 0)
    {
        l0Helper.EnablePcap("task1-0-n3.pcap", l0Device.Get(1), false, true);
        l2Helper.EnablePcap("task1-0-n5.pcap", l2Device.Get(1), false, true);
        csma2Helper.EnablePcap("task1-0-n6.pcap", csma2Device.Get(0), false, true);
    }

    else if (configuration == 1)
    {
        l0Helper.EnablePcap("task1-1-n3.pcap", l0Device.Get(1), false, true);
        l2Helper.EnablePcap("task1-1-n5.pcap", l2Device.Get(1), false, true);
        csma2Helper.EnablePcap("task1-1-n6.pcap", csma2Device.Get(0), false, true);
    }

    else if (configuration == 2)
    {
        l0Helper.EnablePcap("task1-2-n3.pcap", l0Device.Get(1), false, true);
        l2Helper.EnablePcap("task1-2-n5.pcap", l2Device.Get(1), false, true);
        csma2Helper.EnablePcap("task1-2-n6.pcap", csma2Device.Get(0), false, true);
    }

    // ASCII Tracing
    AsciiTraceHelper ascii;

    if (configuration == 0)
    {
        NodeContainer n2Node;
        n2Node.Add(nodes.Get(2));
        csma1Helper.EnableAscii(ascii.CreateFileStream("task1-0-n2.tr"), n2Node);

        NodeContainer n4Node;
        n4Node.Add(nodes.Get(4));
        l2Helper.EnableAscii(ascii.CreateFileStream("task1-0-n4.tr"), n4Node);
    }

    else if (configuration == 1)
    {
        NodeContainer n2Node;
        n2Node.Add(nodes.Get(2));
        csma1Helper.EnableAscii(ascii.CreateFileStream("task1-1-n2.tr"), n2Node);

        NodeContainer n0Node;
        n0Node.Add(nodes.Get(0));
        csma1Helper.EnableAscii(ascii.CreateFileStream("task1-1-n0.tr"), n0Node);

        NodeContainer n4Node;
        n4Node.Add(nodes.Get(4));
        l2Helper.EnableAscii(ascii.CreateFileStream("task1-1-n4.tr"), n4Node);

        NodeContainer n8Node;
        n8Node.Add(nodes.Get(8));
        csma2Helper.EnableAscii(ascii.CreateFileStream("task1-1-n8.tr"), n8Node);
    }

    else if (configuration == 2)
    {
        NodeContainer n2Node;
        n2Node.Add(nodes.Get(2));
        csma1Helper.EnableAscii(ascii.CreateFileStream("task1-2-n2.tr"), n2Node);

        NodeContainer n8Node;
        n8Node.Add(nodes.Get(8));
        csma2Helper.EnableAscii(ascii.CreateFileStream("task1-2-n8.tr"), n8Node);

        NodeContainer n0Node;
        n0Node.Add(nodes.Get(0));
        csma1Helper.EnableAscii(ascii.CreateFileStream("task1-2-n0.tr"), n0Node);

        NodeContainer n4Node;
        n4Node.Add(nodes.Get(4));
        l2Helper.EnableAscii(ascii.CreateFileStream("task1-2-n4.tr"), n4Node);

        NodeContainer n7Node;
        n7Node.Add(nodes.Get(7));
        csma2Helper.EnableAscii(ascii.CreateFileStream("task1-2-n7.tr"), n7Node);
    }

    NS_LOG_INFO("Run Simulation.");
    Simulator::Stop(Seconds(20));
    Simulator::Run();
    Simulator::Destroy();

    NS_LOG_INFO("Done.");
    return 0;
}