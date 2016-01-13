//
//  SSL/TLS Project
//  handshakeConstants.c
//
//  Created on 07/01/15.
//  Copyright © 2015 Mello, Darka. All rights reserved.
//

#include "handshakeConstants.h"


const int RSA_IDS_NUM = 28;
const uint16_t RSA_IDS[] = {0x0001, 0x0002, 0x0004, 0x0005, 0x0007, 0x0009, 0x000A, 0x002E, 0x002F, 0x0035, 0x003B, 0x003C, 0x003D, 0x0041, 0x0084, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x009C, 0x009D, 0x00AC, 0x00AD, 0x00B6, 0x00B7, 0x00B8, 0x00B9
};

const int DH_DSS_IDS_NUM = 12;
const uint16_t DH_DSS_IDS[] = {0x000B, 0x000C, 0x000D, 0x0030, 0x0036, 0x003E, 0x0042, 0x0068, 0x0085, 0x0097, 0x00A4, 0x00A5
};

const int DH_RSA_IDS_NUM = 12;
const uint16_t DH_RSA_IDS[] = {0x000E, 0x000F, 0x0010, 0x0031, 0x0037, 0x003F, 0x0043, 0x0069, 0x0086, 0x0098, 0x00A0, 0x00A1
};

const int SHA_IDS_NUM = 16 + 15;
const uint16_t SHA_IDS[] = {0x000B, 0x000C, 0x000D, 0x0030, 0x0036, 0x0042, 0x0085, 0x0097, 0x000E, 0x000F, 0x0010, 0x0031, 0x0037, 0x0043, 0x0086, 0x0098,
    0x0002, 0x0005, 0x0007, 0x0009, 0x000A, 0x002E, 0x002F, 0x0035, 0x0041, 0x0084, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096
};

const int SHA256_IDS_NUM = 6+7;
const uint16_t SHA256_IDS[]={0x00003E, 0x0068, 0x00A4, 0x003F, 0x0069, 0x00A0, 0x003B, 0x003C, 0x003D, 0x009C, 0x00AC, 0x00B6, 0x00B8
};

const int SHA384_IDS_NUM = 6;
const uint16_t SHA384_IDS[] = {0x00A5, 0x00A1, 0x009D, 0x00AD, 0x00B7, 0x00B9
};

const int MD5_IDS_NUM = 2;
const uint16_t MD5_IDS[] = {0x0001, 0x0004
};

key_exchange_algorithm get_kx_algorithm(uint16_t cipher_suite_Id){
    for(int i=0;i<DH_RSA_IDS_NUM;i++)
        if(DH_RSA_IDS[i]==cipher_suite_Id)
            return DH_RSA_KX;
    
    for(int i=0;i<RSA_IDS_NUM;i++)
        if(RSA_IDS[i]==cipher_suite_Id)
            return RSA_KX;
    
    for(int i=0;i<DH_DSS_IDS_NUM;i++)
        if(DH_DSS_IDS[i]==cipher_suite_Id)
            return DH_DSS_KX;
    
    return 0;
}

const EVP_MD *get_hash_function(uint16_t cipher_suite_Id){
    for(int i=0;i<SHA_IDS_NUM;i++)
        if(SHA_IDS[i]==cipher_suite_Id)
            return EVP_sha();
    
    for(int i=0;i<SHA256_IDS_NUM;i++)
        if(SHA256_IDS[i]==cipher_suite_Id)
            return EVP_sha256();
    
    for(int i=0;i<SHA384_IDS_NUM;i++)
        if(SHA384_IDS[i]==cipher_suite_Id)
            return EVP_sha384();
    
    for(int i=0;i<MD5_IDS_NUM;i++)
        if(MD5_IDS[i]==cipher_suite_Id)
            return EVP_md5();
    return NULL;
}