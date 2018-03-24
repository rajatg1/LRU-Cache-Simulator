#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <fstream>

// Input defined
#define address_length 32
#define offset_bits 4
#define index_bits 17
#define tag_bits 11
#define total_sets 131072
#define associativity 2

// Output Variables
int total_traces = 0;
int reads = 0;
int writes = 0;
int cache_hits = 0;
int cache_misses = 0;
int Cache[total_sets][associativity] = {{0}};
int MRU_Valid[total_sets] = {0};
int LRU_Valid[total_sets] = {0};

using namespace std;

// My functions
void getIndexAndTag(int arr[address_length], int index[index_bits], int tag[tag_bits]){

	for(int i=0; i< tag_bits; i++){
			tag[i] = arr[i];
	}
	int j=0;
	for(int i = tag_bits; i < address_length-offset_bits; i++){
		index[j] = arr[i];
		j++;
	}

}

void printArray(int* a, int n){
	cout << "\nPrinting 1D array: \t";
	for(int i=0; i<n; i++){
		cout << a[i];
	}

	cout << endl;
}

void print2DArray(int a[total_sets][associativity]){
	cout << "\nPrinting 2D array: \t";
	for(int i=0; i<total_sets; i++){
		for(int j=0; j<associativity; j++){
			cout << a[i][j] <<"\t";
		}
		cout << endl;
	}

	cout << endl;
}

int checkValidBits(int MRU_Val, int LRU_Val){
	// 00 - 0, 01 - 1, 10 - 2, 11 - 3
	int k = 0;

	if(MRU_Val == 1){
		if(LRU_Val == 1)
			k = 3;
		else
			k = 2;
	}
	else{
		if(LRU_Val==1)
			k = 1;
		else
			k = 0;
	}

	return k;
}

int fromBinaryArrToDecimal(int* arr1, int n){

	int sum = 0;
	int power = 0;
	int result = 0;

	for(int i=0; i<n; i++){
		power = (int) pow(2.0, n-1-i);
		result = arr1[i]*power;
		sum = sum + result;
	}

	return sum;
}

void HexaCharToBinaryArr(char ch, int* a){
	switch(ch){
				// MSB   						LSB
	case '0' :	a[0] = 0; a[1] = 0; a[2] = 0; a[3] = 0;
						break;
	case '1' :	a[0] = 0; a[1] = 0; a[2] = 0; a[3] = 1;
						break;
	case '2' :	a[0] = 0; a[1] = 0; a[2] = 1; a[3] = 0;
						break;
	case '3' :	a[0] = 0; a[1] = 0; a[2] = 1; a[3] = 1;
						break;
	case '4' :	a[0] = 0; a[1] = 1; a[2] = 0; a[3] = 0;
						break;
	case '5' :	a[0] = 0; a[1] = 1; a[2] = 0; a[3] = 1;
						break;
	case '6' :	a[0] = 0; a[1] = 1; a[2] = 1; a[3] = 0;
						break;
	case '7' :	a[0] = 0; a[1] = 1; a[2] = 1; a[3] = 1;
						break;
	case '8' :	a[0] = 1; a[1] = 0; a[2] = 0; a[3] = 0;
						break;
	case '9' :	a[0] = 1; a[1] = 0; a[2] = 0; a[3] = 1;
						break;
	case 'a' :	a[0] = 1; a[1] = 0; a[2] = 1; a[3] = 0;
						break;
	case 'b' :	a[0] = 1; a[1] = 0; a[2] = 1; a[3] = 1;
						break;
	case 'c' :	a[0] = 1; a[1] = 1; a[2] = 0; a[3] = 0;
						break;
	case 'd' :	a[0] = 1; a[1] = 1; a[2] = 0; a[3] = 1;
						break;
	case 'e' :	a[0] = 1; a[1] = 1; a[2] = 1; a[3] = 0;
						break;
	case 'f' :	a[0] = 1; a[1] = 1; a[2] = 1; a[3] = 1;
						break;

	}
}

void HexaDecimalStringToBinary(string hexastr, int arr[address_length]){

	int length = hexastr.length();
	int count = (address_length)-4;

	for(int i=length-1; i>=0; i--){
		int temp[4] = {0};
		HexaCharToBinaryArr(char(hexastr[i]), temp);
		for(int j=0; j<4; j++){
			arr[count+j] = temp[j];
		}

		count -= 4;

		if(count < 0){
			break;
		}

	}


}

void extractHexaRWFromInput(string input_str, string &hexastr, char &ch){
	int offset = input_str.find(' ');
	ch = input_str[offset+1];
	hexastr = input_str.substr(0,offset);
}

void countIncrease(char ch){
	if(ch == 'R' || ch == 'r')
		reads++;
	else if(ch == 'W' || ch == 'w')
		writes++;
}

int checkInCache(int index_val, int tag_val){

	// checkValidBits
	int k = checkValidBits(MRU_Valid[index_val], LRU_Valid[index_val]);
	if(k == 0){
		//00
		cache_misses++;
		// becomes 10
		Cache[index_val][0] = tag_val;
		MRU_Valid[index_val] = 1;
		LRU_Valid[index_val] = 0;
	}
	else if(k == 2){
		//10
		if(tag_val == Cache[index_val][0]){
			cache_hits++;
		}
		else{
			// becomes 11
			cache_misses++;
			Cache[index_val][1] = Cache[index_val][0];
			Cache[index_val][0] = tag_val;
			MRU_Valid[index_val] = 1;
			LRU_Valid[index_val] = 1;
		}
	}
	else if(k == 3){
		// 11
		if(tag_val == Cache[index_val][0] || tag_val == Cache[index_val][1]){
			cache_hits++;

			if(tag_val == Cache[index_val][1]){
				Cache[index_val][1] = Cache[index_val][0];
				Cache[index_val][0] = tag_val;
				MRU_Valid[index_val] = 1;
				LRU_Valid[index_val] = 1;
			}
		}
		else{
			// again becomes 11
			cache_misses++;
			Cache[index_val][1] = Cache[index_val][0];
			Cache[index_val][0] = tag_val;
			MRU_Valid[index_val] = 1;
			LRU_Valid[index_val] = 1;
		}
	}

	// No case of k == 2 (01)
	return 0;
}

void printFinalOutput(){
	total_traces = reads + writes;
	cout <<"\n\nTotal traces: " << total_traces;
	cout <<"\nReads: " << reads;
	cout <<"\nWrites: " << writes;
	cout <<"\nCache hits: " << cache_hits;
	cout <<"\nCache misses: " << cache_misses << endl << endl;
}

int main(int argc, char* argv[]) {

	// Read reading input line by line
	string fileName = argv[1];

	// My variables
	string line = "";
	string hexastring = "";
	char ch;
	int index_value = 0;
	int tag_value = 0;


	ifstream file;
	file.open(fileName.c_str(), ios::in);

	if(file.is_open()){
		while(getline(file, line)){

			// everytime initialization
			int arr[address_length] = {0};
			int index[index_bits] = {0};
			int tag[tag_bits] = {0};


			// extract hexastring and ch from the line
			extractHexaRWFromInput(line, hexastring, ch);

			// increase read or write count
			countIncrease(ch);

			// extract hexastring into binary array of 32 fields
			HexaDecimalStringToBinary(hexastring, arr);

			//extract index and tag
			getIndexAndTag(arr, index, tag);

			//calculate index location and tag
			index_value = fromBinaryArrToDecimal(index, index_bits);
			tag_value = fromBinaryArrToDecimal(tag, tag_bits);

			//checkInCache
			checkInCache(index_value, tag_value);

		}
	}


	file.close();

	printFinalOutput();
	//print2DArray(Cache);
	return 0;
}
