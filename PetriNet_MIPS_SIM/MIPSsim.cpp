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
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstring>

/*
 * -----------------------------------------------------------
 * MACRO (define) section
 * -----------------------------------------------------------
 */
 
#define MAX_INSTRUCTIONS		16
#define MAX_REGISTERS			8
#define MAX_DATA_MEM			8

#define	OPCODE_ADD				1
#define	OPCODE_SUB				2
#define	OPCODE_AND				3
#define	OPCODE_OR				4
#define	OPCODE_LD				5

#define REG_0					0
#define REG_1					1
#define REG_2					2
#define REG_3					3
#define REG_4					4
#define REG_5					5
#define REG_6					6
#define REG_7					7

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
 
struct inst_val_t
{
	char inst_name[4];
	char opcode;
	char out_reg;
	char in1_value;
	char in2_value;
	char buff_empty;
};

struct instructions_t
{
	char inst_name[4];
	char opcode;
	char out_reg;
	char in_reg1;
	char in_reg2;
};

struct out_buffer_t
{
    char reg_index;
    char reg_value;
    char buff_empty;
};

struct result_buffer_t
{
    char reg_index;
    char reg_value;
    char buff_empty;
};

/*
 * -----------------------------------------------------------
 * Global data section
 * -----------------------------------------------------------
 */

instructions_t  instructions[16];
unsigned char   registers[8];
unsigned char   data_memory[8];

inst_val_t      instruction_buffer;
inst_val_t      load_instruction_buffer;
inst_val_t      arithmetic_instruction_buffer;
result_buffer_t result_buffer[2];
out_buffer_t    address_buffer;

int             no_of_insts;
int             no_of_regs;
int             no_of_mems;

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


using namespace std;

/*
 * -----------------------------------------------------------
 * Function     : init_mips_petrinet
 * Description  : 
 * Input        : 
 *
 * Output       : 
 * -----------------------------------------------------------
 */
 
void init_mips_petrinet(void)
{
	ifstream infile, in_regfile, in_datafile;

	string input, opcode, regs;

    char file_ins[20];
	const char *cstr;
    const char *input_char;

	size_t pos;

	no_of_insts = 0;
	no_of_regs = 0;
	no_of_mems = 0;

    input_char = &file_ins[0];
	/********************************************************/
	infile.open("instructions.txt");
	getline(infile, input);

	while (!infile.eof())
	{
		char *input_char = new char [input.length()+1];
        strcpy (input_char, input.c_str());

		opcode = input.substr(1, 3);
		if (opcode.compare("ADD") == 0)
		{
			instructions[no_of_insts].opcode = OPCODE_ADD;
			strcpy(instructions[no_of_insts].inst_name, "ADD");

			instructions[no_of_insts].out_reg = atoi(&input_char[6]);
			instructions[no_of_insts].in_reg1 = atoi(&input_char[9]);
			instructions[no_of_insts].in_reg2 = atoi(&input_char[12]);
		}
		else if (opcode.compare("SUB") == 0)
		{
			instructions[no_of_insts].opcode = OPCODE_SUB;
			strcpy(instructions[no_of_insts].inst_name, "SUB");

			instructions[no_of_insts].out_reg = atoi(&input_char[6]);
			instructions[no_of_insts].in_reg1 = atoi(&input_char[9]);
			instructions[no_of_insts].in_reg2 = atoi(&input_char[12]);
		}
		else if (opcode.compare("AND") == 0)
		{
			instructions[no_of_insts].opcode = OPCODE_AND;
			strcpy(instructions[no_of_insts].inst_name, "AND");


			instructions[no_of_insts].out_reg = atoi(&input_char[6]);
			instructions[no_of_insts].in_reg1 = atoi(&input_char[9]);
			instructions[no_of_insts].in_reg2 = atoi(&input_char[12]);
		}
		else if (opcode.compare("OR,") == 0)
		{
			instructions[no_of_insts].opcode = OPCODE_OR;
			strcpy(instructions[no_of_insts].inst_name, "OR");

			instructions[no_of_insts].out_reg = atoi(&input_char[5]);
			instructions[no_of_insts].in_reg1 = atoi(&input_char[8]);
			instructions[no_of_insts].in_reg2 = atoi(&input_char[11]);
		}
		else
		{
			instructions[no_of_insts].opcode = OPCODE_LD;
			strcpy(instructions[no_of_insts].inst_name, "LD");

			instructions[no_of_insts].out_reg = atoi(&input_char[5]);
			instructions[no_of_insts].in_reg1 = atoi(&input_char[8]);
			instructions[no_of_insts].in_reg2 = atoi(&input_char[11]);
		}

		no_of_insts++;
        getline(infile, input);
	}

	infile.close();

	/********************************************************/
	in_regfile.open("registers.txt");
    getline(in_regfile, input);

	while (!in_regfile.eof())
	{
		int reg_no;
		int reg_value;


		regs = input.substr(2, 1);
		input = input.substr(4);
		cstr = regs.c_str();
		reg_no = atoi(cstr);

		pos = input.find(">");
		regs = input.substr(0, pos);
		cstr = regs.c_str();
		reg_value = atoi(cstr);

		registers[reg_no] = reg_value;

		no_of_regs++;
        getline(in_regfile, input);
	}

	in_regfile.close();

	/********************************************************/
	in_datafile.open("datamemory.txt");
    getline(in_datafile, input);
    
	while (!in_datafile.eof())
	{
		int mem_no;
		int mem_value;


		regs = input.substr(1, 1);
		input = input.substr(3);
		cstr = regs.c_str();
		mem_no = atoi(cstr);

		pos = input.find(">");
		regs = input.substr(0, pos);
		cstr = regs.c_str();
		mem_value = atoi(cstr);

		data_memory[mem_no] = mem_value;

		no_of_mems++;
        getline(in_datafile, input);
	}

	in_datafile.close();

	/********************************************************/

    instruction_buffer.buff_empty = true;
    load_instruction_buffer.buff_empty = true;
    arithmetic_instruction_buffer.buff_empty = true;
    result_buffer[0].buff_empty = true;
    result_buffer[1].buff_empty = true;
    address_buffer.buff_empty = true;
}
/*
 * -----------------------------------------------------------
 * Function     : print_simulations
 * Description  : 
 * Input        : 
 *
 * Output       : 
 * -----------------------------------------------------------
 */
void print_simulations(ofstream *ofs, int curr_inst)
{
    *ofs << "STEP " << curr_inst << ":" << endl;
    /////////////////////////////////////////////
    *ofs << "INM:";
    int i;
    if(curr_inst < no_of_insts)
    {
        for (i = curr_inst; i < no_of_insts-1; i++)
        {
            *ofs << "<" << instructions[i].inst_name << ",R" << (int) instructions[i].out_reg;
            *ofs << ",R" << (int) instructions[i].in_reg1 << ",R" << (int) instructions[i].in_reg2 << ">,";
        }
        *ofs << "<" << instructions[i].inst_name << ",R" << (int) instructions[i].out_reg;
        *ofs << ",R" << (int) instructions[i].in_reg1 << ",R" << (int) instructions[i].in_reg2 << ">";
    }
    *ofs << endl;

    ///////////////////////////////////////////////
    *ofs << "INB:";
    if(!instruction_buffer.buff_empty)
    {
        *ofs << "<" << instruction_buffer.inst_name << ",R" << (int) instruction_buffer.out_reg;
        *ofs << "," << (int) instruction_buffer.in1_value << "," << (int) instruction_buffer.in2_value<< ">";
    }
    *ofs << endl;

    ///////////////////////////////////////////////
    *ofs << "AIB:";
    if(!arithmetic_instruction_buffer.buff_empty)
    {
        *ofs << "<" << arithmetic_instruction_buffer.inst_name << ",R" << (int) arithmetic_instruction_buffer.out_reg;
        *ofs << "," << (int) arithmetic_instruction_buffer.in1_value << "," << (int) arithmetic_instruction_buffer.in2_value<< ">";
    }
    *ofs << endl;

    ///////////////////////////////////////////////
    *ofs << "LIB:";
    if(!load_instruction_buffer.buff_empty)
    {
        *ofs << "<" << load_instruction_buffer.inst_name << ",R" << (int) load_instruction_buffer.out_reg;
        *ofs << "," << (int) load_instruction_buffer.in1_value << "," << (int) load_instruction_buffer.in2_value<< ">";
    }
    *ofs << endl;

    ///////////////////////////////////////////////
    *ofs << "ADB:";
    if(!address_buffer.buff_empty)
    {
        *ofs << "<R" << (int) address_buffer.reg_index << "," << (int) address_buffer.reg_value << ">";
    }
    *ofs << endl;

    ///////////////////////////////////////////////
    *ofs << "REB:";
    if(!result_buffer[0].buff_empty)
    {
        *ofs << "<R" << (int) result_buffer[0].reg_index << "," << (int) result_buffer[0].reg_value << ">";
    }

    if(!result_buffer[1].buff_empty)
    {
        *ofs << ",<R" << (int) result_buffer[1].reg_index << "," << (int) result_buffer[1].reg_value << ">";
    }
    *ofs << endl;
    ///////////////////////////////////////////////

    *ofs << "RGF:";
    int j;
    for(j=0; j<7; j++)
    {
        *ofs << "<R" << j << "," << (int) registers[j] << ">,";
    }
    *ofs << "<R" << j << "," << (int) registers[j] << ">";
    *ofs << endl;
    ///////////////////////////////////////////////

    *ofs << "DAM:";
    int k;
    for(k=0; k<7; k++)
    {
        *ofs << "<" << k << "," << (int) data_memory[k] << ">,";
    }
    *ofs << "<" << k << "," << (int) data_memory[k] << ">";
    *ofs << endl;

    ///////////////////////////////////////////////
    *ofs << endl;
}

/*
 * -----------------------------------------------------------
 * Function     : decode
 * Description  : 
 * Input        : 
 *
 * Output       : 
 * -----------------------------------------------------------
 */
void decode(int inst_index)
{
	if (inst_index < no_of_insts)
	{
        strcpy(instruction_buffer.inst_name, instructions[inst_index].inst_name);
        instruction_buffer.opcode = instructions[inst_index].opcode;
        instruction_buffer.out_reg = instructions[inst_index].out_reg;
        instruction_buffer.in1_value = registers[instructions[inst_index].in_reg1];
        instruction_buffer.in2_value = registers[instructions[inst_index].in_reg2];
        instruction_buffer.buff_empty = false;
	}
}

/*
 * -----------------------------------------------------------
 * Function     : issue1
 * Description  : 
 * Input        : 
 *
 * Output       : 
 * -----------------------------------------------------------
 */
void issue1()
{
    if(!instruction_buffer.buff_empty && (strcmp(instruction_buffer.inst_name, "LD") != 0))
    {
        strcpy(arithmetic_instruction_buffer.inst_name, instruction_buffer.inst_name);
        arithmetic_instruction_buffer.opcode = instruction_buffer.opcode;
        arithmetic_instruction_buffer.out_reg = instruction_buffer.out_reg;
        arithmetic_instruction_buffer.in1_value = instruction_buffer.in1_value;
        arithmetic_instruction_buffer.in2_value = instruction_buffer.in2_value;
        arithmetic_instruction_buffer.buff_empty = false;

        instruction_buffer.buff_empty = true;
    }
}
/*
 * -----------------------------------------------------------
 * Function     : issue2
 * Description  : 
 * Input        : 
 *
 * Output       : 
 * -----------------------------------------------------------
 */
void issue2()
{
    if(!instruction_buffer.buff_empty && (strcmp(instruction_buffer.inst_name, "LD") == 0))
    {
        strcpy(load_instruction_buffer.inst_name, instruction_buffer.inst_name);
        load_instruction_buffer.opcode = instruction_buffer.opcode;
        load_instruction_buffer.out_reg = instruction_buffer.out_reg;
        load_instruction_buffer.in1_value = instruction_buffer.in1_value;
        load_instruction_buffer.in2_value = instruction_buffer.in2_value;
        load_instruction_buffer.buff_empty = false;

        instruction_buffer.buff_empty = true;
    }
}
/*
 * -----------------------------------------------------------
 * Function     : addr
 * Description  : 
 * Input        : 
 *
 * Output       : 
 * -----------------------------------------------------------
 */
void addr()
{
    if(!load_instruction_buffer.buff_empty)
    {
        address_buffer.reg_index = load_instruction_buffer.out_reg;
        address_buffer.reg_value = load_instruction_buffer.in1_value + load_instruction_buffer.in2_value;
        address_buffer.buff_empty = false;

        load_instruction_buffer.buff_empty = true;
    }
}
/*
 * -----------------------------------------------------------
 * Function     : load
 * Description  : 
 * Input        : 
 *
 * Output       : 
 * -----------------------------------------------------------
 */
void load()
{
    if(!address_buffer.buff_empty && (result_buffer[0].buff_empty))
    {
        result_buffer[0].reg_index = address_buffer.reg_index;
        result_buffer[0].reg_value = data_memory[address_buffer.reg_value];
        result_buffer[0].buff_empty = false;

        address_buffer.buff_empty = true;
    }
    else if(!address_buffer.buff_empty && (result_buffer[1].buff_empty))
    {
        result_buffer[1].reg_index = address_buffer.reg_index;
        result_buffer[1].reg_value = data_memory[address_buffer.reg_value];
        result_buffer[1].buff_empty = false;

        address_buffer.buff_empty = true;
    }
}
/*
 * -----------------------------------------------------------
 * Function     : alu
 * Description  : 
 * Input        : 
 *
 * Output       : 
 * -----------------------------------------------------------
 */
void alu()
{
    if(!arithmetic_instruction_buffer.buff_empty && (result_buffer[0].buff_empty))
    {
        switch(arithmetic_instruction_buffer.opcode)
        {
            case OPCODE_ADD:
                result_buffer[0].reg_index = arithmetic_instruction_buffer.out_reg;
                result_buffer[0].reg_value = arithmetic_instruction_buffer.in1_value + arithmetic_instruction_buffer.in2_value;
                result_buffer[0].buff_empty = false;
                    break;
            case OPCODE_SUB:
                result_buffer[0].reg_index = arithmetic_instruction_buffer.out_reg;
                result_buffer[0].reg_value = arithmetic_instruction_buffer.in1_value - arithmetic_instruction_buffer.in2_value;
                result_buffer[0].buff_empty = false;
                    break;
            case OPCODE_OR:
                result_buffer[0].reg_index = arithmetic_instruction_buffer.out_reg;
                result_buffer[0].reg_value = arithmetic_instruction_buffer.in1_value | arithmetic_instruction_buffer.in2_value;
                result_buffer[0].buff_empty = false;
                    break;
            case OPCODE_AND:
                result_buffer[0].reg_index = arithmetic_instruction_buffer.out_reg;
                result_buffer[0].reg_value = arithmetic_instruction_buffer.in1_value & arithmetic_instruction_buffer.in2_value;
                result_buffer[0].buff_empty = false;
                    break;
        }
        arithmetic_instruction_buffer.buff_empty = true;
    }
    else if(!arithmetic_instruction_buffer.buff_empty && (result_buffer[1].buff_empty))
    {
        switch(arithmetic_instruction_buffer.opcode)
        {
            case OPCODE_ADD:
                result_buffer[1].reg_index = arithmetic_instruction_buffer.out_reg;
                result_buffer[1].reg_value = arithmetic_instruction_buffer.in1_value + arithmetic_instruction_buffer.in2_value;
                result_buffer[1].buff_empty = false;
                    break;
            case OPCODE_SUB:
                result_buffer[1].reg_index = arithmetic_instruction_buffer.out_reg;
                result_buffer[1].reg_value = arithmetic_instruction_buffer.in1_value - arithmetic_instruction_buffer.in2_value;
                result_buffer[1].buff_empty = false;
                    break;
            case OPCODE_OR:
                result_buffer[1].reg_index = arithmetic_instruction_buffer.out_reg;
                result_buffer[1].reg_value = arithmetic_instruction_buffer.in1_value | arithmetic_instruction_buffer.in2_value;
                result_buffer[1].buff_empty = false;
                    break;
            case OPCODE_AND:
                result_buffer[1].reg_index = arithmetic_instruction_buffer.out_reg;
                result_buffer[1].reg_value = arithmetic_instruction_buffer.in1_value & arithmetic_instruction_buffer.in2_value;
                result_buffer[1].buff_empty = false;
                    break;
        }
        arithmetic_instruction_buffer.buff_empty = true;
    }
}
/*
 * -----------------------------------------------------------
 * Function     : write
 * Description  : 
 * Input        : 
 *
 * Output       : 
 * -----------------------------------------------------------
 */
void write()
{
    if(!result_buffer[0].buff_empty && !result_buffer[1].buff_empty)
    {
        int index = result_buffer[0].reg_index;
        int value = result_buffer[0].reg_value;
        registers[index] = value;

        result_buffer[0].buff_empty = result_buffer[1].buff_empty;
        result_buffer[0].reg_index = result_buffer[1].reg_index;
        result_buffer[0].reg_value = result_buffer[1].reg_value;
        result_buffer[1].buff_empty = true;
    }
    else if(!result_buffer[0].buff_empty)
    {
        int index = result_buffer[0].reg_index;
        int value = result_buffer[0].reg_value;

        registers[index] = value;
        result_buffer[0].buff_empty = true;
    }
}
/*
 * -----------------------------------------------------------
 * Function     : main
 * Description  : 
 * Input        : 
 *
 * Output       : 
 * -----------------------------------------------------------
 */
int main()
{
	int curr_inst;
    ofstream ofs;

    ofs.open ("simulation.txt", ofstream::out);

    curr_inst = 0;

    init_mips_petrinet();

    print_simulations(&ofs, curr_inst);

	while (curr_inst < no_of_insts || !result_buffer[0].buff_empty || !result_buffer[1].buff_empty || !instruction_buffer.buff_empty
        || !load_instruction_buffer.buff_empty || !address_buffer.buff_empty || !arithmetic_instruction_buffer.buff_empty)
	{
        write();

        load();

        alu();

        addr();

        issue1();

        issue2();

        decode(curr_inst);

        curr_inst++;

        print_simulations(&ofs, curr_inst);
	}

	ofs.close();

	return 0;
}
/*
 * -----------------------------------------------------------
 * End of File
 * -----------------------------------------------------------
 */