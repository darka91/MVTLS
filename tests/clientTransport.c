//
//  SSL/TLS Project
//
//  clientBasic.c
//  Client for testing the basic protocol layer.
//
//  Created on 22/12/15.
//  Copyright © 2015 Alessandro Melloni, Andrea Francesco Vinci. All rights reserved.
//  

#include <stdio.h>
#include <stdlib.h>

#include "ServerClientTransportProtocol.h"

void onPacketReceive(channel_t *ch, packet_transport_t *p);

int main(int argc, char **argv){
    char *fileName = "channel.txt";
    char *clientName = "Client";
    channel_t *client = create_channel(fileName, clientName, "Server");
    set_on_receive(client, &onPacketReceive);
    start_listener(client);
    printf("*** Client is start ***\n");
    
    //sending packet
    unsigned char *message = malloc(sizeof(unsigned char)*2);
    *message = '1';
    char to[] = "Server\0";
    printf("Client send: %c \n",*message);
    
    packet_transport_t *p = create_packet(NULL, to, message, 1);
    free(message);
    send_packet(client, p);
    free_packet(p);
    wait_channel(client);
    free(client);
}

void onPacketReceive(channel_t *ch, packet_transport_t *p){
    
    //print received message
    printf("message from: %s\n", p->source);
    printf("message len: %d\n", p->length);
    printf("message:\n%.*s\n\n",p->length, p->message);
    
    //prepare new packet to be send
    //if the 'from' field is NULL it will be autofill
    
    if(*(p->message)<'8'){
        (*(p->message))++;
        packet_transport_t *packet = create_packet(NULL, p->source, p->message, 1);
        
        if(send_packet(ch, packet))
            printf("\nPacket sent correctly\n\n");
        else printf("\nError in sendPacket\n\n");
		free_packet(packet);
        if(*(p->message)=='7'){
			free_packet(p);
        	stop_channel(ch);
        }
    }
	free_packet(p);
}