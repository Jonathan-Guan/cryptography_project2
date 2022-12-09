#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>

//THIS IS COPY PASTE FROM TUTORIAL CHANGE IN THE FUTURE
void generate_keys(){
  //generate a keyset
  const int minimum_lambda = 110;
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



//Converts an input string to an array of int16_t in the same order as the string
std::vector<int16_t> string_to_bitarray(const std::string& s) {
	int n = s.size();
	std::vector<int16_t> binarystr;
	std::stack<int16_t> reverse;
	for (int i = n - 1; i >= 0; i--) {
		if ((n - i) % 2 == 1) {
			reverse.push((int16_t)s[i]);	//Every other character, push onto the stack
		}
		else {
			int16_t temp = reverse.top();
			std::cout << ((int16_t)s[i] << 8) << " ";
			reverse.pop();                              //Every other character, add the character shifted 1 char size (8bits)
			reverse.push(((int16_t)s[i] << 8) + (int16_t)temp);    //left to the existing value on the stack
		}

	}
	while (!reverse.empty()) {
		binarystr.push_back(reverse.top());	//Reverse the order
		reverse.pop();
	}
	return binarystr;
}


int main() {

	std::vector<int16_t> v = string_to_bitarray("abcd");
	for (int i = 0; i < v.size(); i++) {
		std::cout << v[i] << " ";
	}
	return 1;
}
