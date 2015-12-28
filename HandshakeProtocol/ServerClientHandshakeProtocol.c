//
//  SSL/TLS Project
//  ServerClientHandshakeProtocol.h
//
//  Created on 24/12/15.
//  Copyright © 2015 Mello, Darka. All rights reserved.
//

#include "ServerClientHandshakeProtocol.h"

int send_handshake(channel *ch, handshake *h){
    unsigned char *message = NULL;
    uint32_t messageLen;
    serialize_handshake(h, &message, &messageLen);
    
    //make record
    record *to_send = malloc(sizeof(record));
    to_send->type = HANDSHAKE;
    to_send->version = SSL3_0;
    to_send->lenght = messageLen;
    to_send->message = message;
    
    int result = send_record(ch, to_send);
    free(message);
    free(to_send);
    return result;
}

void serialize_handshake(handshake *h, unsigned char **stream, uint32_t *streamLen){
    unsigned char *buff = malloc(h->length+6);
    *stream = buff;
    *buff = h->type;
    buff++;
    
    uint32_t len = REV32(h->length)>>8;
    memcpy(buff, &len, 3);
    buff+=3;
    
    h->TLS_version = REV16(h->TLS_version);
    memcpy(buff, &(h->TLS_version),2);
    buff+=2;
    
    memcpy(buff, h->message, h->length);
    

    *streamLen = h->length+6;
}

handshake *deserialize_handshake(unsigned char *message, uint32_t messageLen){
    handshake *h = malloc(sizeof(handshake));
    h->type = *message;
    message++;
    
    uint32_t len;
    memcpy(&len, message, 3);
    len = REV32(len)>>8;
    h->length = len;
    message+=3;
    
    memcpy(&(h->TLS_version) , message, 2);
    h->TLS_version = REV16(h->TLS_version);
    message+=2;
    h->message = message;
    return h;
}

void print_handshake(handshake *h){
    printf("\n***Handshake***\n");
    printf("Type : %d\n", h->type);
    printf("Version : %d\n",h->TLS_version);
    printf("Length : %d\n", h->length);
    printf("Message : \n");
    for(int i =0 ; i<h->length;i++){
        printf("%02x ",*(h->message+i));
    }
}