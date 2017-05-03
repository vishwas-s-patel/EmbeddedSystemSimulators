/*  On  my  honor,  I  have  neither  given  nor  received
unauthorized aid on this assignment */

/* -----------------------------------------------------------
 * File name   : MIPSsim.cpp
 * Description : Constructed a colored token-based Petri Net for 
                 an in-order execution MIPS Processor. Modeled 
                 the MIPS Pipeline, register file and data Memory. 
                 Logged state of the MIPS Processor after every cycle. 
 * Author      : Vishwas Satish Patel
 * -----------------------------------------------------------
*/

/*
 * -----------------------------------------------------------
 * Include section
 * -----------------------------------------------------------
 */

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <stdlib.h>
#include <bitset>
#include <climits>
#include <string>

using namespace std;

/*
 * -----------------------------------------------------------
 * MACRO (define) section
 * -----------------------------------------------------------
 */
 
#define PRINT_REF 0

#if PRINT_REF
    string print_ref_string = "Code: \n";
#endif // PRINT_REF


//////////////////////////////////////////////////

/*
 * -----------------------------------------------------------
 * Type definition section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Global prototypes section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Local prototypes section
 * -----------------------------------------------------------
 */
 
struct rle_data
{
    string data;
    int count;
};

/*
 * -----------------------------------------------------------
 * Global data section
 * -----------------------------------------------------------
 */

struct rle_data rle_temp = {"", 0};

//////////////////////////////////////////////////

string in_compressed_string;
vector<string> input_dictionary;
string data_for_rle;
vector<string> decompress_output;
vector<string> input_vector;
vector<string> temp_vector;
std::map<string, int> input_hash_map;
vector<string> final_dictionary;
string output_string = "";


const string RLE_CONST              = "001";
const string DIRECT_MATCH_CONST     = "111";
const string BITS_1_MISMATCH        = "011";
const string BITS_2_MISMATCH        = "100";
const string BITS_4_MISMATCH        = "101";
const string BITS_2_MISMATCH_ANY    = "110";
const string ORIGINAL               = "000";
const string BITMASK_BASED          = "010";

 /*
 * -----------------------------------------------------------
 * Local (static) data section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Local (static) and inline functions section
 * -----------------------------------------------------------
 */

 
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
/********* Read from the Input file *****************/

void read_file_for_compress()
{
    ifstream infile;
    string input;
    std::map<string,int>::iterator it;

    infile.open("original.txt");
    getline(infile, input);

    while(!infile.eof())
    {
        temp_vector.push_back(input);
        input_vector.push_back(input);

        it = input_hash_map.find(input);

        if (it == input_hash_map.end())
        {
            input_hash_map[input] = 1;
        }
        else
        {
            it->second = it->second + 1;
        }

        getline(infile, input);
    }
    infile.close();
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
/********* Build the Dictionary ********************/

void create_dictionary(vector<string>& comp)
{
    std::vector< string >::iterator prev;
    std::vector< string >::iterator curr;
    unsigned int i = 0;
    int curr_index;
    int final_index;

    while(final_dictionary.size() < 16 && !comp.empty())
    {
        curr = find(temp_vector.begin(), temp_vector.end(), comp.at(0));
        curr_index = std::distance(temp_vector.begin(), curr);
        final_index = curr_index;

        for(i=1; i<comp.size(); i++)
        {
            curr = find(temp_vector.begin(), temp_vector.end(), comp.at(i));
            if(curr != temp_vector.end())
            {
                curr_index = std::distance(temp_vector.begin(), curr);
                if(curr_index < final_index)
                    final_index = curr_index;
            }
        }

        final_dictionary.push_back(temp_vector.at(final_index));

        comp.erase(std::remove(comp.begin(), comp.end(), temp_vector.at(final_index)), comp.end());
        temp_vector.erase(std::remove(temp_vector.begin(), temp_vector.end(), temp_vector.at(final_index)), temp_vector.end());
    }
}

void dictionary()
{
    vector<string> comp;
    while((final_dictionary.size() < 16) && !input_hash_map.empty())
    {
        auto x = std::max_element(input_hash_map.begin(), input_hash_map.end(),[](const pair<string, int>& p1, const pair<string, int>& p2) {return p1.second < p2.second; });

        for (auto it = input_hash_map.begin(); it != input_hash_map.end(); ++it )
        {
            if (it->second == x->second)
            {
                comp.push_back(it->first);
                input_hash_map.erase(it->first);
            }
        }

        create_dictionary(comp);
    }
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
/**************** RLE Check ************************/

int rle_check(string in)
{
    const string rle_count_string[] = {"000", "001", "010", "011", "100", "101", "110", "111"};

    if(in.compare(rle_temp.data) == 0)
    {
        if(rle_temp.count >= 1 && rle_temp.count < 9)
        {
            rle_temp.count += 1;
            if(rle_temp.count == 9)
            {
                output_string = output_string + RLE_CONST + rle_count_string[(rle_temp.count - 2)];
#if PRINT_REF
                print_ref_string = print_ref_string + RLE_CONST +  " " + rle_count_string[rle_temp.count - 2] + "\n";
#endif
                rle_temp.data = "";
                rle_temp.count = 0;
            }
            else
            {
#if PRINT_REF
                print_ref_string = print_ref_string + "\n";
#endif
            }
        }
        return 1;
    }
    else if(rle_temp.count > 1)
    {
        output_string = output_string + RLE_CONST + rle_count_string[(rle_temp.count - 2)];
#if PRINT_REF
        print_ref_string = print_ref_string + RLE_CONST + " " + rle_count_string[rle_temp.count - 2] + "\n";
#endif
    }
    return 0;
}

/************** Direct Match ********************/

int direct_match(string in)
{

    const string direct_match_string[] = {  "0000", "0001", "0010", "0011",
                                            "0100", "0101", "0110", "0111",
                                            "1000", "1001", "1010", "1011",
                                            "1100", "1101", "1110", "1111"};

    for(unsigned int i = 0; i < final_dictionary.size(); i++)
    {
        if(in.compare(final_dictionary.at(i)) == 0)
        {
            output_string = output_string + DIRECT_MATCH_CONST + direct_match_string[i];
#if PRINT_REF
            print_ref_string = print_ref_string + DIRECT_MATCH_CONST + " " + direct_match_string[i] + "\n";
#endif
            rle_temp.data = in;
            rle_temp.count = 1;
            return 1;
        }
    }
    return 0;
}

/************** consecutive MisMatch ************/

size_t popcount(size_t n) {
    std::bitset<sizeof(size_t) * CHAR_BIT> b(n);
    return b.count();
}

unsigned int first_mismatch(string a, string b)
{
    for(unsigned int i=0; i < 32; i++)
    {
        if(a.at(i) != b.at(i))
            return i;
    }
}

unsigned int last_mismatch(string a, string b)
{
    for(unsigned int i=31; i >=0; i--)
    {
        if(a.at(i) != b.at(i))
            return i;
    }
}

int mis_match(string in, unsigned int mask, unsigned int MASK_CONST, const string BITS_MISMATCH)
{
    unsigned int bits_dict;
    unsigned int bits_in;
    unsigned int i;
    unsigned int temp;
    unsigned int no_of_mis;
    unsigned int last_mis;
    unsigned int first_mis;

    bits_in = stoul(in.c_str(), nullptr, 2);

    for(i = 0; i < final_dictionary.size(); i++)
    {
        bits_dict = stoul(final_dictionary.at(i).c_str(), nullptr, 2);

        temp = bits_in ^ bits_dict;

        no_of_mis = popcount(temp);
        first_mis = first_mismatch(in, final_dictionary.at(i));
        last_mis = last_mismatch(in, final_dictionary.at(i));

        if((no_of_mis == (MASK_CONST+1)) && ((last_mis - first_mis) == MASK_CONST))
        {
            if(first_mis > 28)
                first_mis = 28;

            output_string = output_string + BITS_MISMATCH + std::bitset< 5 >( first_mis).to_string() +  std::bitset< 4 >(i).to_string();
#if PRINT_REF
            print_ref_string = print_ref_string + BITS_MISMATCH + " " + std::bitset< 5 >( first_mis).to_string() +  " " + std::bitset< 4 >(i).to_string() + "\n";
#endif
            rle_temp.data = in;
            rle_temp.count = 1;
            return 1;
        }
    }
    return 0;
}


/************** consecutive MisMatch ************/

int consecutive_mis_match(string in)
{
    if(mis_match(in, 0x80000000, 0, BITS_1_MISMATCH))
        return 1;
    else if(mis_match(in, 0xC0000000, 1, BITS_2_MISMATCH))
        return 1;
    else if(mis_match(in, 0xF0000000, 3, BITS_4_MISMATCH))
        return 1;
    else
        return 0;
}


/************ 4-bit bitmask based mismatch************/

int bitmask_based_mis_match(string in)
{
    unsigned int bits_dict;
    unsigned int bits_in;
    unsigned int i;
    unsigned int temp;
    unsigned int no_of_mis;
    unsigned int last_mis;
    unsigned int first_mis;
    unsigned int bitmask;

    bits_in = stoul(in.c_str(), nullptr, 2);

    for(i = 0; i < final_dictionary.size(); i++)
    {
        bits_dict = stoul(final_dictionary.at(i).c_str(), nullptr, 2);

        temp = bits_in ^ bits_dict;

        no_of_mis = popcount(temp);
        first_mis = first_mismatch(in, final_dictionary.at(i));
        last_mis = last_mismatch(in, final_dictionary.at(i));

        if((no_of_mis < 4) && ((last_mis - first_mis) < 4))
        {
            if(first_mis > 28)
                first_mis = 28;

            bitmask = (temp >> (32 - first_mis - 4)) & 0xf;

            output_string = output_string + BITMASK_BASED + std::bitset< 5 >( first_mis).to_string() +  std::bitset< 4 >( bitmask).to_string() + std::bitset< 4 >(i).to_string();
#if PRINT_REF
            print_ref_string = print_ref_string + BITMASK_BASED + " " + std::bitset< 5 >( first_mis).to_string() +  " " +std::bitset< 4 >( bitmask).to_string() + " " + std::bitset< 4 >(i).to_string() + "\n";
#endif
            rle_temp.data = in;
            rle_temp.count = 1;
            return 1;
        }
    }
    return 0;
}


/************ 2-bit MisMatch anywhere************/

int mismatch_any_2(string in)
{
    unsigned int bits_dict;
    unsigned int bits_in;
    unsigned int i;
    unsigned int temp;
    unsigned int no_of_mis;
    unsigned int last_mis;
    unsigned int first_mis;

    bits_in = stoul(in.c_str(), nullptr, 2);

    for(i = 0; i < final_dictionary.size(); i++)
    {
        bits_dict = stoul(final_dictionary.at(i).c_str(), nullptr, 2);

        temp = bits_in ^ bits_dict;

        no_of_mis = popcount(temp);
        first_mis = first_mismatch(in, final_dictionary.at(i));
        last_mis = last_mismatch(in, final_dictionary.at(i));

        if(no_of_mis == 2)
        {
            output_string = output_string + BITS_2_MISMATCH_ANY + std::bitset< 5 >( first_mis).to_string() +  std::bitset< 5 >( last_mis).to_string() + std::bitset< 4 >(i).to_string();
#if PRINT_REF
            print_ref_string = print_ref_string + BITS_2_MISMATCH_ANY + " " + std::bitset< 5 >( first_mis).to_string() +  " " +std::bitset< 5 >( last_mis).to_string() + " " + std::bitset< 4 >(i).to_string() + "\n";
#endif
            rle_temp.data = in;
            rle_temp.count = 1;
            return 1;
        }
    }
    return 0;
}

/******************* Original *******************/

void original(string in)
{
    output_string = output_string + ORIGINAL + in.substr(0,32);
#if PRINT_REF
    print_ref_string = print_ref_string + ORIGINAL + " " + in + "\n";
#endif

}

//////////////////////////////////////////////////////
void choose_compression_type(string in)
{
#if PRINT_REF
    print_ref_string = print_ref_string + in + " --> ";
#endif

    if(rle_check(in))
        return;
    if(direct_match(in))
        return;
    if(consecutive_mis_match(in))
        return;
    if(bitmask_based_mis_match(in))
        return;
    if(mismatch_any_2(in))
        return;
    original(in);

    rle_temp.data = in;
    rle_temp.count = 1;

#if PRINT_REF
    print_ref_string = print_ref_string + "\n";
#endif

}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

void simulate_compression()
{
    choose_compression_type(input_vector.at(0));

    rle_temp.count = 1;
    rle_temp.data = input_vector.at(0);

    for(unsigned int i = 1; i < input_vector.size(); i++)
    {
        choose_compression_type(input_vector.at(i));
    }
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
/*************** Write to a file ****************/

void write_to_file_compressed()
{
    ofstream ofs;

    ofs.open ("cout.txt", ofstream::out);

    for(unsigned int i=0; i< output_string.length(); i++)
    {
        if(i%32 == 0 && (i > 0))
            ofs << "\n";

        ofs << output_string.at(i);
    }

    for(unsigned int i=0; i< (32 - (output_string.length()%32)); i++)
    {
        ofs << "0";
    }

    ofs << "\nxxxx\n";

    for(unsigned int i=0; i<final_dictionary.size(); i++)
    {
        ofs << final_dictionary.at(i) << endl;
    }

    ofs.close();

}
////////////////////////////////////////////////////////////////////////
void compress()
{
    read_file_for_compress();

    dictionary();

    simulate_compression();

    write_to_file_compressed();
}

////////////////////////////////////////////////////////////////////////
/********* Read from the Input file *****************/

void read_file_for_decompress()
{
    ifstream infile;
    string input;

    infile.open("compressed.txt");
    getline(infile, input);

    while(!infile.eof())
    {
        if(input.compare(0, 4, "xxxx") == 0)
        {
            break;
        }
        in_compressed_string.append(input, 0, 32);
        getline(infile, input);
    }

    getline(infile, input);
    while(!infile.eof())
    {
        input_dictionary.push_back(input);
        getline(infile, input);
    }
    input_dictionary.push_back(input);

    infile.close();
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

void get_direct_match_data()
{
    unsigned int dict_index;

    string temp = in_compressed_string.substr(0, 4);
    in_compressed_string.erase(0, 4);
    dict_index = std::stoul(temp, nullptr, 2);

    decompress_output.push_back(input_dictionary.at(dict_index));

    data_for_rle = input_dictionary.at(dict_index);
}
////////////////////////////////////////////////////////////////////////
typedef enum
{
    MISMATCH_BITS_1 = 0,
    MISMATCH_BITS_2 = 1,
    MISMATCH_BITS_4 = 2,
    MISMATCH_ANY_2_BITS = 3,
    MISMATCH_BITMASK_BASED = 4
}bitmask_types;

void get_bitmasked_based_data(unsigned int mask, bitmask_types bitmask_kind)
{
    unsigned int dict_index;
    unsigned int start_loc, start_loc1, start_loc2;
    string temp;
    unsigned int decoded_value;
    unsigned int bit_mask;

    switch(bitmask_kind)
    {
        case MISMATCH_BITS_1:
        case MISMATCH_BITS_2:
        case MISMATCH_BITS_4:

            temp = in_compressed_string.substr(0, 5);
            in_compressed_string.erase(0, 5);
            start_loc = std::stoul(temp, nullptr, 2);

            temp = in_compressed_string.substr(0, 4);
            in_compressed_string.erase(0, 4);
            dict_index = std::stoul(temp, nullptr, 2);

            decoded_value = (mask >> start_loc) ^ stoul(input_dictionary.at(dict_index), nullptr, 2);

            decompress_output.push_back(std::bitset< 32 >(decoded_value).to_string());
            data_for_rle = std::bitset< 32 >(decoded_value).to_string();

            break;
        case MISMATCH_ANY_2_BITS:
            temp = in_compressed_string.substr(0, 5);
            in_compressed_string.erase(0, 5);
            start_loc1 = std::stoul(temp, nullptr, 2);

            temp = in_compressed_string.substr(0, 5);
            in_compressed_string.erase(0, 5);
            start_loc2 = std::stoul(temp, nullptr, 2);

            temp = in_compressed_string.substr(0, 4);
            in_compressed_string.erase(0, 4);
            dict_index = std::stoul(temp, nullptr, 2);

            decoded_value = (mask >> start_loc2) ^ (mask >> start_loc1) ^ stoul(input_dictionary.at(dict_index), nullptr, 2);

            decompress_output.push_back(std::bitset< 32 >(decoded_value).to_string());
            data_for_rle = std::bitset< 32 >(decoded_value).to_string();

            break;
        case MISMATCH_BITMASK_BASED:
            temp = in_compressed_string.substr(0, 5);
            in_compressed_string.erase(0, 5);
            start_loc = std::stoul(temp, nullptr, 2);

            temp = in_compressed_string.substr(0, 4);
            in_compressed_string.erase(0, 4);
            bit_mask = std::stoul(temp, nullptr, 2);
            bit_mask = bit_mask << 28;

            temp = in_compressed_string.substr(0, 4);
            in_compressed_string.erase(0, 4);
            dict_index = std::stoul(temp, nullptr, 2);

            decoded_value = (bit_mask >> start_loc) ^ stoul(input_dictionary.at(dict_index), nullptr, 2);

            decompress_output.push_back(std::bitset< 32 >(decoded_value).to_string());

            data_for_rle = std::bitset< 32 >(decoded_value).to_string();

            break;
    }
}
////////////////////////////////////////////////////////////////////////
int get_original_data()
{
    string temp;

    temp = in_compressed_string.substr(0, 32);

    if(temp.size() != 32)
        return 0;
    else
    {
        in_compressed_string.erase(0, 32);
        decompress_output.push_back(temp);

        data_for_rle = temp;
        return 1;
    }
}
////////////////////////////////////////////////////////////////////////
void get_rle_data()
{
    string temp;
    unsigned int rle_encoding;

    temp = in_compressed_string.substr(0, 3);
    in_compressed_string.erase(0, 3);
    rle_encoding = std::stoul(temp, nullptr, 2);

    for(unsigned int i=0; i<=rle_encoding;i++)
    {
        decompress_output.push_back(data_for_rle);
    }
}

void simulate_decompression()
{
    string format;

    while(!in_compressed_string.empty())
    {
        if(in_compressed_string.length() > 3)
        {
            format = in_compressed_string.substr(0, 3);
            in_compressed_string.erase(0, 3);

            if(format.compare(RLE_CONST) == 0)
            {
                get_rle_data();
            }
            else if(format.compare(DIRECT_MATCH_CONST) == 0)
            {
                get_direct_match_data();
            }
            else if(format.compare(BITS_1_MISMATCH) == 0)
            {
                get_bitmasked_based_data(0x80000000, MISMATCH_BITS_1);
            }
            else if(format.compare(BITS_2_MISMATCH) == 0)
            {
                get_bitmasked_based_data(0xC0000000, MISMATCH_BITS_2);
            }
            else if(format.compare(BITS_4_MISMATCH) == 0)
            {
                get_bitmasked_based_data(0xF0000000, MISMATCH_BITS_4);
            }
            else if(format.compare(BITS_2_MISMATCH_ANY) == 0)
            {
                get_bitmasked_based_data(0x80000000, MISMATCH_ANY_2_BITS);
            }
            else if(format.compare(ORIGINAL) == 0)
            {
                if(!get_original_data())
                    break;
            }
            else if(format.compare(BITMASK_BASED) == 0)
            {
                get_bitmasked_based_data(0xF0000000, MISMATCH_BITMASK_BASED);
            }

        }
    }
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
/*********** Write to a file decompressed data****************/

void write_to_file_decompressed()
{
    ofstream ofs;

    ofs.open ("dout.txt", ofstream::out);

    for(unsigned int i=0; i<decompress_output.size(); i++)
    {
        ofs << decompress_output.at(i) << endl;
    }

    ofs.close();
}

//////////////////////////////////////////////////////////////////////////
void decompress()
{
    read_file_for_decompress();

    simulate_decompression();
    
    write_to_file_decompressed();
}

//////////////////////////////////////////////////////////////////////////
int main(int argc , char *argv[])
{
	unsigned int com_decom = stoi(argv[1],nullptr);

	if(com_decom == 1)
        compress();
    else if(com_decom == 2)
        decompress();
    else
        cout << "Provide the Correct Input" << endl;

#if PRINT_REF
    ofstream ofs;

    ofs.open ("ref.txt", ofstream::out);

    ofs << print_ref_string << endl;

    ofs.close();
#endif // PRINT_REF

    return 0;
}
