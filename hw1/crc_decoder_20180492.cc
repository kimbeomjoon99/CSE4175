#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <string>
#include <cmath>
#include <queue>
#include <stack>
#include <deque>
#include <fstream>
#include <bitset>
using namespace std;
// g++ -o crc_decoder_20180492 crc_decoder_20180492.cc
// ./crc_decoder_20180492 input_file output_file result_file generator dataword_size

ifstream input;
FILE* output;
FILE* result;
int dataword_size, pad_num, codeword_size, err_cnt, codeword_cnt;
string codeword, generator_str, decoded_bit;
vector<int> hexbit;

void read_padding(){
    string padding = codeword.substr(0, 8);
    codeword.erase(0, 8);
    bitset<8> pad_bit(padding);
    int pad_size = pad_bit.to_ulong();
    codeword.erase(0, pad_size);
}

void read_input(){
    input.seekg(0, input.end);
    int size = (int)input.tellg();
    input.seekg(0, input.beg);
    unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char) * size);
    input.read((char*)buffer, size);
    for (int i = 0; i < size; i++){
        bitset<8> bit(buffer[i]);
        codeword += bit.to_string();
    }
    free(buffer);
}

string mod_2(string remainder, string divisor, int len){
    for (int j = 0; j < remainder.size() - (len); j++){
        if (remainder[j] == '0') continue;
        for (int k = 0; k < divisor.size(); k++){
            if (divisor[k] == '1'){
                if (remainder[j + k] == '1') remainder[j + k] = '0';
                else if (remainder[j + k] == '0') remainder[j + k] = '1';
            }
            else if (divisor[k] == '0'){
                if (remainder[j + k] == '1') remainder[j + k] = '1';
                else if (remainder[j + k] == '0') remainder[j + k] = '0';
            }
        }
    }
    remainder.erase(0, remainder.size() - (len));
    return remainder;
}

void redundancy_check(){
    decoded_bit = "";
    while(!codeword.empty()){
        string tmp = codeword.substr(0, codeword_size);
        codeword.erase(0, codeword_size);
        string syndrome = mod_2(tmp, generator_str, generator_str.size() - 1);
        int flag = 0;
        for (int i = 0; i < syndrome.size(); i++){
            if (syndrome[i] != '0'){
                flag = 1;
                break;
            }
        }
        if (flag) err_cnt++;
        codeword_cnt++;
        decoded_bit += tmp.substr(0, dataword_size);
    }
}

void decode(){
    while(!decoded_bit.empty()){
        string tmp = decoded_bit.substr(0, 8);
        decoded_bit.erase(0, 8);
        bitset<8> bit(tmp);
        int ascii = bit.to_ulong();
        fprintf(output, "%c", (char)ascii);
    }
    fprintf(result, "%d %d", codeword_cnt, err_cnt);
}

int main(int argc, char* argv[]){
    ios_base::sync_with_stdio(0);
    cin.tie(0), cout.tie(0);

    if (argc != 6) {
        cout << "usage: ./crc_decoder input_file output_file result_file generator dataword_size\n";
        return 1;
    }

    input.open(argv[1], ios::binary);
    if (!input){
        cout << "input file open error.\n";
        return 1;
    }
    
    output = fopen(argv[2], "w");
    if (!output){
        cout << "output file open error.\n";
        return 1;
    }
    result = fopen(argv[3], "w");
    if (!result){
        cout << "result file open error.\n";
        return 1;
    }
    
    dataword_size = argv[5][0] - '0';
    if (dataword_size != 4 && dataword_size != 8){
        cout << "dataword size must be 4 or 8\n";
        return 1;
    }

    generator_str = argv[4];
    codeword_size = dataword_size + generator_str.size() - 1;

    read_input();
    read_padding();
    input.close();
    redundancy_check();
    decode();
    fclose(output);
    fclose(result);

    return 0;
}