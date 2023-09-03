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
// g++ -o crc_encoder_20180492 crc_encoder_20180492.cc
// ./crc_encoder_20180492 input_file output_file generator dataword_size

ifstream input;
FILE* output;

string codeword;
vector<string> dataword;
int dataword_size, generator;
vector<unsigned char> bitwise_codeword;

void mod_2(string divisor, int len){
    for (int i = 0; i < dataword.size(); i++){
        string remainder = dataword[i];
        string tmp = remainder;
        for (int j = 0; j < len; j++) remainder += "0";
        
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
        tmp += remainder;
        codeword += tmp;
    }
}

void read_input(int dword, int len){
    int flag = 0;
    while(!input.eof()){
        char c;
        input.get(c);
        if (input.eof()) break;
        if (dword == 4){
            unsigned int bit = c;
            unsigned int backward = bit % 16;
            bit = bit >> 4;
            bitset<4> first(bit);
            bitset<4> second(backward);
            dataword.push_back(first.to_string());
            dataword.push_back(second.to_string());
        }
        else if (dword == 8){
            bitset<8> bitstr(c);
            dataword.push_back(bitstr.to_string());
        }
    }
}

void str_to_bit(){
    while(!codeword.empty()){
        string bit8 = codeword.substr(0, 8);
        codeword.erase(0, 8);
        unsigned char bitdata = 0;
        for (int i = 0; i < 8; i++){
            if (bit8[7 - i] == '1'){
                bitdata = bitdata | (1 << i);
            }
        }
        bitwise_codeword.push_back(bitdata);
    }
}

void write_output(){
    int padding_num = codeword.size() % 8;
    padding_num = 8 - padding_num;
    bitset<8> padding(padding_num);
    string pad = padding.to_string();
    //for (int i = 0; i < 8; i++) fprintf(output, "%c", pad[i]);
    //fprintf(output, " ");
    if (padding_num){
        for (int i = 0; i < padding_num; i++)
            codeword = "0" + codeword;
    }
    codeword = pad + codeword;
    /*
    for (int i = 1; i < codeword.size() + 1; i++){
        fprintf(output, "%c", codeword[i - 1]);
        if (!(i % 8) && i != codeword.size()) fprintf(output, " ");
    }
    fprintf(output, "\n");
    */
    str_to_bit();
    for (int i = 0; i < bitwise_codeword.size(); i++){
        fputc(bitwise_codeword[i], output);
    }
}

int main(int argc, char* argv[]){
    ios_base::sync_with_stdio(0);
    cin.tie(0), cout.tie(0);

    if (argc != 5) {
        cout << "usage: ./crc_encoder input_file output_file generator dataword_size\n";
        return 1;
    }

    input.open(argv[1]);
    if (!input){
        cout << "input file open error.\n";
        return 1;
    }
    output = fopen(argv[2], "wb");
    if (!output){
        cout << "output file open error.\n";
        return 1;
    }
    dataword_size = argv[4][0] - '0';
    if (dataword_size != 4 && dataword_size != 8){
        cout << "dataword size must be 4 or 8\n";
        return 1;
    }
    
    string generator_str = argv[3];
    for (int i = 1; i <= generator_str.size(); i++){
        if (generator_str[generator_str.size() - i] == '1'){
            unsigned int tmp = 1;
            for (int j = 0; j < i - 1; j++) tmp = tmp << 1;
            generator += tmp;
        }
    }
    read_input(dataword_size, generator_str.size());
    input.close();
    mod_2(generator_str, generator_str.size() - 1);
    write_output();
    fclose(output);

    return 0;
}