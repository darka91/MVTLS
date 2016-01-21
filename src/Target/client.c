//
//  client.c
//  SSLXcodeProject
//
//  Created by Darka on 12/01/16.
//  Copyright © 2016 Darka. All rights reserved.
//

#include <stdio.h>
#include "TLS.h"

void onPacketReceive(channel *ch, packet_basic *p);

void print_master_secret();
void print_random();

TLS_parameters TLS_param;

int main() {
	// Setup the channel
	char *fileName = "SSLchannel.txt";
	char *channelFrom = "Client";
	char *channelTo = "Server";
	channel *client2server = create_channel(fileName, channelFrom, channelTo, CLIENT);
	set_on_receive(client2server, &onPacketReceive);
	
	TLS_param.previous_state = 0x0000;
	printf("*** TLS client is started ***\n\n");
	
	// Make ClientHello
	printf(">>> Client hello\n");
	handshake *client_hello = make_client_hello(TLS_param.client_random);
	print_handshake(client_hello);
	send_handshake(client2server, client_hello);
	backup_handshake(&TLS_param,client_hello);
	free_handshake(client_hello);
	
	// Start channel and listener for new messages
	start_listener(client2server);
	wait_channel(client2server);
	
	free(client2server);
    
    
    free(TLS_param.handshake_messages);
    free(TLS_param.master_secret);
    X509_free(TLS_param.server_certificate);
    //free openssl resources
    CRYPTO_cleanup_all_ex_data();
}

/*
 * Function called from basic protocol
 * when a message is received
 */
void onPacketReceive(channel *client2server, packet_basic *p){
	
	// Get record and print
	record *r = deserialize_record(p->message, p->messageLen);
	if(r->type == CHANGE_CIPHER_SPEC){
		printf("\n<<< Change cipher spec\n");
		print_record(r);
		
		free_record(r);
		free_packet(p);
	}
	else if(r->type == HANDSHAKE){
		handshake *h = deserialize_handshake(r->message, r->lenght);
		
		free_record(r);
		free_packet(p);
		
		switch (h->type) {
				
			case SERVER_HELLO:
				if(TLS_param.previous_state == 0x0000){
					TLS_param.previous_state = SERVER_HELLO;
					backup_handshake(&TLS_param,h);
					handshake_hello *server_hello = deserialize_client_server_hello(h->message, h->length, SERVER_MODE);

					printf("\n<<< Server Hello\n");
					print_handshake(h);
					
					// Extract data for next steps
                    TLS_param.cipher_suite = *server_hello->cipher_suites;
					TLS_param.tls_version = server_hello->TLS_version;
					
					// Backup server random
					memcpy(TLS_param.server_random,&(server_hello->random.UNIX_time), 4);
					memcpy(TLS_param.server_random+4, server_hello->random.random_bytes, 28);
					
					printf("\nCipher suite :%s\n",TLS_param.cipher_suite.name);
					print_random();
					
					free_hello(server_hello);
				}
				break;
				
			case CERTIFICATE:
				
				if(TLS_param.previous_state == SERVER_HELLO){
					
					TLS_param.previous_state = CERTIFICATE;
					
					backup_handshake(&TLS_param, h);
					printf("\n<<< Certificate\n");
					print_handshake(h);
					
					certificate_message *certificate_m = deserialize_certificate_message(h->message, h->length);
                    TLS_param.server_certificate = certificate_m->X509_certificate;
                    TLS_param.server_certificate->references+=1;
                    
                    printf("\nCertificate details: %s\n", TLS_param.server_certificate->name);

					free_certificate_message(certificate_m);
				}
				break;
				
			case SERVER_KEY_EXCHANGE:
				
				if(TLS_param.previous_state == CERTIFICATE){
					TLS_param.previous_state = SERVER_KEY_EXCHANGE;
					
					//save the server key exchange parameters
					TLS_param.server_key_ex = deserialize_server_key_exchange(h->length, h->message, TLS_param.cipher_suite.kx);
				}
				break;
				
			case SERVER_DONE:
				
				if((TLS_param.previous_state == CERTIFICATE || TLS_param.previous_state == SERVER_KEY_EXCHANGE)){
					backup_handshake(&TLS_param,h);
					printf("<<< Server Hello Done\n");
					print_handshake(h);
					
					//make Client Key Exchange Message
                    handshake * client_key_exchange = make_client_key_exchange(&TLS_param, TLS_param.cipher_suite.kx);
                    backup_handshake(&TLS_param, client_key_exchange);
                    send_handshake(client2server, client_key_exchange);
                    printf("\n>>> Client Key Exchange\n");
                    print_handshake(client_key_exchange);
                    free_handshake(client_key_exchange);
                    
                    //send_RSA_client_key_exchange(client2server, &TLS_param);
                    print_master_secret();

					
					printf("\n>>> Change cipher spec\n");
					record* change_cipher_spec = make_change_cipher_spec();
					send_record(client2server, change_cipher_spec);
					print_record(change_cipher_spec);
					free_record(change_cipher_spec);
					
					printf("\n>>> Finished\n");
					handshake *finished = make_finished_message(&TLS_param);
					send_handshake(client2server, finished);
					print_handshake(finished);
					free_handshake(finished);
				}
				break;
				
			case FINISHED:
				
				printf("\n<<< Finished\n");
				print_handshake(h);
				free_handshake(h);
				stop_channel(client2server);
				break;
				
			default:
				break;
		}
		free_handshake(h);
	}
}

void print_master_secret() {
	printf("\nMaster secret:\n");
	for(int i=0;i<48;i++)
		printf("%02x ",TLS_param.master_secret[i]);
	printf("\n");
}

void print_random() {
	//print server and client random
	printf("Server random :\n");
	for(int i=0;i<32;i++)
		printf("%02x ",TLS_param.server_random[i]);
	printf("\nClient random :\n");
	for(int i=0;i<32;i++)
		printf("%02x ",TLS_param.client_random[i]);
}
