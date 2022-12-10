#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "tfhe.h"
#include "tfhe_io.h"

using namespace std;

// generate cloud keys and put them in appropriate files, will use same key for every 16 bits to encrypt
void generate_keys(const int minimum_lambda) {
    TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);

    //generate a random key
    uint32_t seed[] = { 314, 1592, 657 };
    tfhe_random_generator_setSeed(seed,3);
    TFheGateBootstrappingSecretKeySet* key = new_random_gate_bootstrapping_secret_keyset(params);

    //export the secret key to file for later use
    FILE* secret_key = fopen("secret.key","wb");
    export_tfheGateBootstrappingSecretKeySet_toFile(secret_key, key);
    fclose(secret_key);

    //export the cloud key to a file (for the cloud)
    FILE* cloud_key = fopen("cloud.key","wb");
    export_tfheGateBootstrappingCloudKeySet_toFile(cloud_key, &key->cloud);
    fclose(cloud_key);

    //clean up all pointers
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

}

//encrypt the 16 bits of input
LweSample* encrypt16(int16_t plaintext, const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key) {
    LweSample* ciphertext = new_gate_bootstrapping_ciphertext_array(16, params);
    for (int i=0; i<16; i++) {
        bootsSymEncrypt(&ciphertext[i], (plaintext>>i)&1, key);
    }
    // delete_gate_bootstrapping_ciphertext_array(16, ciphertext);
    return ciphertext;
}

void export_ciphertext(LweSample* ciphertext, const TFheGateBootstrappingParameterSet* params) {
    //export the 2x16 ciphertexts to a file (for the cloud)
    FILE* cloud_data = fopen("cloud.data","wb");
    for (int i=0; i<16; i++) {
        export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext[i], params);
    }
    fclose(cloud_data);

}


int main() {
    const int minimum_lambda = 110;
    generate_keys(minimum_lambda);
    int bits [] = {10, 20, 30};
    int buffer = sizeof(bits);
    int arr_index = 0;

    FILE* secret_key = fopen("secret.key","rb");
    TFheGateBootstrappingSecretKeySet* key = new_tfheGateBootstrappingSecretKeySet_fromFile(secret_key);
    fclose(secret_key);

    FILE* cloud_key = fopen("cloud.key","rb");
    TFheGateBootstrappingCloudKeySet* bk = new_tfheGateBootstrappingCloudKeySet_fromFile(cloud_key);
    fclose(cloud_key);

    const TFheGateBootstrappingParameterSet* params = key->params;

    for (int i=0; i<buffer; i++) {
        export_ciphertext(encrypt16(int16_t(bits[arr_index]), params, key), params);
    }

    // LweSample* cipher = new_gate_bootstrapping_ciphertext_array(16, key->params);
    // FILE* cloud_data = fopen("cloud.data","rb");
    // for (int i=0; i<16; i++) 
    //     import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &cipher[i], params);
    // fclose(cloud_data);


    
}
