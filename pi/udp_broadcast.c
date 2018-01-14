#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <memory.h>
#include <stdio.h>

#include "udp_broadcast.h"


/*
  example:  send SSDP discovery broadcast message via UDP
  sendBroadcastPacket(
    "239.255.255.250", 
    1900, 
    "M-SEARCH * HTTP/1.1\r\nHOST:239.255.255.250:1900\r\nMAN:\"ssdp:discover\"\r\nST:ssdp:all\r\nMX:1\r\n\r\n"
  );
*/


void sendBroadcastPacket(char* ip_addr, unsigned int port, char* message) {
    // Open a socket
    int sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sd<=0) {
        printf("Error: Could not open socket");
        return;
    }
    
    // Set socket options
    int broadcastEnable = 1;
    int ret = setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret) {
        printf("Error: Could not open set socket to broadcast mode");
        close(sd);
        return;
    }
    
    // Since we don't call bind() here, the system decides on the source port for us, which is what we want.    
    
    // Configure the destination port and ip we want to send to
    struct sockaddr_in broadcastAddr; // Make an endpoint
    memset(&broadcastAddr, 0, sizeof broadcastAddr);
    broadcastAddr.sin_family = AF_INET;

    inet_pton(AF_INET, ip_addr, &broadcastAddr.sin_addr);
    broadcastAddr.sin_port = htons(port);
    
    // Send the broadcast request, ie "Any upnp devices out there?"
    ret = sendto(sd, message, strlen(message), 0, (struct sockaddr*)&broadcastAddr, sizeof broadcastAddr);
    if (ret<0) {
        printf("Error: Could not open send broadcast");
        close(sd);
        return;        
    }
    
    // Get responses here using recvfrom if you want...
    close(sd);
}


