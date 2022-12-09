#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>

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
LweSample* encrypt16(int16_t plaintext) {
    LweSample* ciphertext = new_gate_bootstrapping_ciphertext_array(16, params);
    for (int i=0; i<16; i++) {
        bootsSymEncrypt(&ciphertext[i], (plaintext>>i)&1, key);
    }
    delete_gate_bootstrapping_ciphertext_array(16, ciphertext);
}

void export_ciphertext(LweSample* &ciphertext) {
    //export the 2x16 ciphertexts to a file (for the cloud)
    FILE* cloud_data = fopen("cloud.data","wb");
    for (int i=0; i<16; i++) {
        export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext[i], params);
    }
    fclose(cloud_data);

}

/// duh duh duh 
void end ( LweSample* result , const LweSample* c, const TFheGateBootstrappingCloudKeySet* bk)
{
    int l= sizeof(c)/sizeof(c[0]);
    // LweSample* result = new_gate_bootstrapping_ciphertext_array(l-1, bk->params);
    LweSample* r= new_gate_bootstrapping_ciphertext(bk->params);  //uh not sure how to make it one variable 
    while( l>1){
        
        for (i =0; i<l-1; i++)

        {
        
            // # "And" the current bit and the next bit, and add the result
            // # to the resulting string
            bootsAND(r, c[i], c[i+1])
            result[i]=r
        
        }
        
        c=result;
        l= sizeof(c)/sizeof(c[0]);
        
    }

}

void compare_strings(LweSample* result, const LweSample* a, const LweSample* b, int n, const TFheGateBootstrappingCloudKeySet* bk)
{ 
    int len = sizeof(a)/sizeof(a[0]);

    if(len != n)
    {
        return 0 ;
    }

    LweSample* tmps = new_gate_bootstrapping_ciphertext_array(n, bk->params);   

    // not sure what exactly the above thing gives but hopefully an array of len equal to n 

    for (int i =0; i< n; i++)
        {
            bootsXNOR(&tmps[i], &a[i], &b[i], bk); 
            //    # the xnor to check equality   for every bit and storing the result in     tmps  
           
        }

    // LweSample* result = new_gate_bootstrapping_ciphertext_array(n-1, bk->params);   
    end (result, tmps, bk)

}

//// du duh duh 



int main() {
    generate_keys(110);
    char string_of_bits = '110010101010101001010111100101001010111010101101';
    int buffer = string_of_bits.length();
    int string_index = 0;
    while (True) {
        if (buffer - string_index >= 16) {
            //encrypt next 16 bits
            export_ciphertext(encrypt16(int16_t(string_of_bits[string_index:string_index+16])));
            string_index = string_index + 16
        }
        else if (buffer - string_index < 16){
            //encrypt remaining bits
            export_ciphertext(encrypt16(int16_t(string_of_bits[string_index:buffer])));
            break;
        }
    }
    
 LweSample* ciphertexts = new_gate_bootstrapping_ciphertext_array(n, bk->params);  
 import_gate_bootstrapping_ciphertext_fromFile(FILE* F, LweSample* sample, const TFheGateBootstrappingParameterSet* params);
 LweSample* result= new_gate_bootstrapping_ciphertext(bk->params) ; 

 int count= 0
 for (i =0 ; i<n ; i++)
 {
    compare_strings(result, ciphertexts[i], keyword[i], bk);
    if(result[0]==1)
    {
        count++; 
    }
 }
 

}
