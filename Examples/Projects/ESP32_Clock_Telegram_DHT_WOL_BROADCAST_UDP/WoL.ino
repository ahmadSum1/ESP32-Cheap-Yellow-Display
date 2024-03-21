#include <WiFiUdp.h>

#include <WakeOnLan.h>

WiFiUDP UDP;
WakeOnLan WOL(UDP);


void wakeMyPC() {
    const char *MACAddress = WOL_MAC;   //MOVED TO "arduino_secrets.h"
  
    WOL.sendMagicPacket(MACAddress); // Send Wake On Lan packet with the above MAC address. Default to port 9.
    // WOL.sendMagicPacket(MACAddress, 7); // Change the port number
}


void setup_WoL()
{
    WOL.setRepeat(3, 100); // Optional, repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.


    WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); // Optional  => To calculate the broadcast address, otherwise 255.255.255.255 is used (which is denied in some networks).
    
    wakeMyPC();
}