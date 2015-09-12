#ifndef CHIP8CPU_H_INCLUDED
#define CHIP8CPU_H_INCLUDED

//because I always use this as typig std::::::::: is lame even though should prob be using just printf....
using namespace std;

//define the fontset for the chip-8
unsigned char chip8Font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

//define the chip-8 class
class chip8 {
    //public variables and functions
    public:
        //Empty constructor and destrucotors
        chip8();
        ~chip8();

        //the draw flag so the program can determine wether anything needs to be drawn to the screen
        bool drawFlag;

        //main CPU emulation function that will read, decode and execute the opcodes. This function will also
        //handle timers and limit execution speed
        void cycle();

        //funcition to clear the display
        void clearDisplay();

        //function to load a rom into the emulaotrs memory
        void load(const char *filename);

        //array to hold each pixel of the screen for drawing, The screen is 64x32px making a total of 2048 pixels
        unsigned char screen[2048];

        //variable to hold the state of the 16 possible inputs
        unsigned char key[16];

        //declare the init() function to set things up when the chip-8 CPU object is first created
        void init();

    //private varibles and funtions
    private:
        //define the program counter
        unsigned short pc;

        //define current opcode
        unsigned short opcode;

        //define the indes register
        unsigned short indexReg;

        //define the stack pointer
        unsigned short stackPoint;

        //define all 16 registers V0 - VF
        unsigned char v[16];

        //define the cpu stack
        unsigned short stack[16];

        //define the MASSIVE!!! 4k of memory
        unsigned char memory[4096];

        //define the delay and sound timers
        unsigned char delayTimer;
        unsigned char soundTimer;

        //define a int to hold the result of math sums
        int sumValue;
        
        //declare variables for drawing sprites
        int x, y, height;
        
        //declare private functions
        void drys(int posX, int posY, int height);
};

//YAY! for empty constructors and destructors
chip8::chip8(){}
chip8::~chip8(){}

//function to clear the display
void chip8::clearDisplay(){
    //for each pixel on the screen
    for(int i=0;i<2048;i++){
        //set the pixel to not draw
        screen[i] = 0;
    }

    //set the draw flag so the screen is re-drawn
    drawFlag = 1;
}

//function to draw a sprite to the display
void chip8::drys(int posX, int posY, int height){	
	//set register v[f] to 0
	v[15] = 0;
	
	//for loop to draw the sprites to the correct height
	for(int y=0;y<height;y++){
		//char to hold the 'sprite'
		unsigned short sprite = memory[indexReg + y];
		
		//nested for loop for the x of the sprite
		for(int x=0;x<8;x++){
			//if a pixel should be drawn...
			if((sprite & (0x80 >> x)) != 0){
				//if a pixel is already drawn at the current spot
				if(screen[(posX + x + ((posY + y) * 64))] == 1){
					//v[f] is set to 1
					v[15] = 1;
				}
				
				//XOR '^' is used to draw to the screen ... or erase from the screen
				screen[posX + x + ((posY + y) * 64)] ^= 1;
			
			}
		}
	}

	//set the draw flag to 1
	drawFlag = 1;
}

//create the init function for the chip-8 cpu
void chip8::init(){
    //Initial setup of the chip-8 CPU

    //we set the program counter to 0x200 at the start of the emulater since that is where the chip-8 expects the application to be loaded.
    pc = 0x200;

    //the opcode is reset to 0
    opcode = 0;

    //the index register is reset to 0
    indexReg = 0;

    //the stack pointer is reset to 0
    stackPoint = 0;

    //clear disply - dont worry about this till graphics implementation
    clearDisplay();

    //clear stack, keys and registers
    for(int i=0;i<16;i++){stack[i] = key[i] = v[i] = 0;}

    //clear memory
    for(int i=0;i<4096;i++){memory[i] = 0;}

    //load the chip-8 fontset. The fontset needs to be loaded into the memory space 0x05 == 80 onwards.
    for(int i=0;i<80;i++){
        memory[i] = chip8Font[i];
    }

    //reset timers to 0
    delayTimer = 0;
    soundTimer = 0;

    //set int sumValue to 0
    sumValue = 0;
}

//load the rom into memory
void chip8::load(const char *filename){
    //the rom is loaded into memory in binary using fopen. The rom is stored in memory
    FILE * pFile = fopen (filename,"rb");

    //load the rom into memory
    fread( &memory[0x200], 0xfff, 1, pFile);

    //tell me that the rom has been loaded
    cout << "Rom Loaded" << endl;

    //close the file and buffer
	fclose(pFile);
}

//emulate a cycle of the cpu
void chip8::cycle(){
    //fetch Opcode - to do this we need to get 2 bytes from the memory where the program counter is currenty pointing,
    //to do this we need to use the bitwise 'OR operation' bitwiser operations reference: https://en.wikipedia.org/wiki/Bitwise_operation

    //The first half of this statment takes the 8 bits from memory[pc] and shifts them 8 bits to the right adding 8 zeros, All of this
    //is then stored in the opcode as 16 bits (The original 8 + the 8 zero's from the shift). The 8 bits in memory[pc +1] are then added
    //to the 16 bits already stored in the opcode variable using the OR operator. This puts 2 bytes/18 bits of data from the memory into opcode.
    opcode = memory[pc] << 8 | memory[pc + 1];

    //Decode and execute opcode - We use a switch statment to test against all possible opcodes and detect the one that needs to be executed.
    //The detected opcode is then executed. The program counter (pc) is increased by 2 each time since each opcode is 2 bytes.
    switch(opcode & 0xF000){
        //all possible opcodes go here..............

        //if the opcode is a 0xxx series
        case 0x0000:
            //Switch by only the last unit in the opcode
            switch(opcode & 0x000F){
                //detect: 00E0 - CLS (Clear the Screen)
                case 0x0000:
                    //Code to clear the screen here
                    clearDisplay();

                    //increase the pc counter by 2
                    pc += 2;

                    //output the detected opcode to me
                    cout << "00E0 - CLS detected: " << opcode << endl;
                break;

                //detect: 00EE - RTS (Return From Subroutine Call)
                case 0x000E:
                    //decrement the stack to the previous subroutine recored in the stack
                    stackPoint--;

                    //move to the previous point in the stack
                    pc = stack[stackPoint];

                    //increment the program counter to the next instruciton
                    pc += 2;

                    //output the detected opcode to me
                    cout << "00EE - RTS detected: " << opcode << endl;
                break;

                //if the code detected is not implemented and not part of the chip-8 spec output it to me
                default:
                    printf("Unknown opcode 0x%X\n", opcode);
                break;
            }
        break;

        //if the opcode is a 1xxx series - being only one: JMP (Jump to Address XXX)
        case 0x1000:
            //set the program counter to XXX of the JMP command
            pc = (opcode & 0x0FFF);

            //output the detected opcode to me
            cout << "1xxx - JMP detected: " << opcode << endl;
        break;

        //jsr
        case 0x2000:
            //set the current space in memory to the current spot in the stack
            stack[stackPoint] = pc;

            //increment the current position in the stack
            stackPoint++;

            //jump to the subrouting at xxx where xxx is (0x0'xxx')
            pc = (opcode & 0x0FFF);

            //output the detected opcode to me
            cout << "2xxx - JSR detected: " << opcode << endl;
        break;

        //if the opcode is a 3xxx serios - being only one: 3rxx: SKEQ V[r], xx.
        case 0x3000:
            //opcode is 3rxx. holding 2 parameters, 'r' and 'xx'. if register v[r] == xx then skip next instruction. else dont.
            //to compare register r we must get its value as the only significant number in the opcode so it sits between 0 and F (0 and 15) - v[0] to v[15]
            //to do this we use "((opcode & 0x0F00) >> 8)" which shifts the result 0r00 to 000r making r a value of 1 to 15 addressing the correct register
            if(v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
                //skip the next instruction
                pc+=4;
            }

            //if they are not equal the next instruciton is executes as per normal
            else{
                //move on to the next instruciton
                pc+=2;
            }

            //output the detected opcode to me
            cout << "3rxx - SKEQ detected: " << opcode << endl;
        break;

        //if the opcode is a 4xxx serios - being only one: 4rxx: SKNE V[r], xx.
        case 0x4000:
            //opcode is 4rxx. holding 2 parameters, 'r' and 'xx'. if register v[r] != xx then skip next instruction. else dont.
            if(v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
                //skip the next instruction
                pc+=4;
            }

            //if they are equal dont skip next instruciton
            else{
                //move on to the next instruciton
                pc+=2;
            }

            //output the detected opcode to me
            cout << "4rxx - SKNE detected: " << opcode << endl;
        break;

        //if the opcode is a 0x5xxx series - being only one: 5ry0 SKEQ v[r], v[y]
        case 0x5000:
            //opode is 5ry0. holding 2 parameters 'r' and 'y'. if registers v[r] == v[y] then skip next instruction
            if(v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0) >> 8]){
                //skip the next instruction
                pc+=4;
            }

            //if they are not equal dont skip next instruciton
            else{
                //move to next instruction
                pc+=2;
            }

            //tell me the detected opcode
            cout << "5ry0 - SKEQ detected: " << opcode << endl;

        break;

        //if the opcode is a 0x6xxx series - being only one: 6rxx MOV v[r], xx
        case 0x6000:
            //opcode is 6rxx. moves the value xx into register v[r]
            //perform the move
            v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);

            //move to next instruction
            pc+=2;

            //tell me opcode detected
            cout << "6rxx - MOV detected: " << opcode << endl;

        break;

        //if the opcode is a 0x7xxx series - being only one: 7rxx add v[r], xx
        case 0x7000:
            //opcode is 7rxx. adds the value xx to register v[r]
            //perform the add
            v[(opcode & 0x0F00) >> 8] = (v[(opcode & 0x0F00) >> 8] + (opcode & 0x00FF));
            
            //move to next instruction
            pc+=2;

            //tell me opcode detected
            cout << "7rxx - ADD detected: " << opcode << endl;

        break;

        //if the opcode is a 0x8xxx series... we need to determine which one it is
        case 0x8000:
            //run a switch statment to find the last value in the opcode as that is unique for all 8x value
            switch(opcode & 0x000F){
                //if the instruction is 8ry0
                case 0x0000:
                    //move register v[y] into v[r]
                    v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];

                    //move to next instruction
                    pc+=2;

                    //tell me opcode detected
                    cout << "8ry0 detected: " << opcode << endl;
                break;

                //if the instruction is 8ry1
                case 0x0001:
                    //v[r] = v[r] OR v[y]
                    // '|' is bitwise OR operator
                    v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];

                    //move to next instruction
                    pc+=2;

                    //tell me opcode detected
                    cout << "8ry1 detected: " << opcode << endl;
                break;

                //if the instruction is 8ry2
                case 0x0002:
                    //v[r] = v[r] AND v[y]
                    v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];


                    //move to next instruction
                    pc+=2;

                    //tell me opcode detected
                    cout << "8ry2 detected: " << opcode << endl;
                break;

                //if the instruction is 8ry3
                case 0x0003:
                    //v[r] = v[r] XOR v[y]
                    v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];

                    //move to next instruction
                    pc+=2;

                    //tell me opcode detected
                    cout << "8ry3 detected: " << opcode << endl;
                break;

                //if the instruction is 8ry4 - Set Vx = Vx + Vy, set VF = carry.
                case 0x0004:
                    //work out the total value of the addition
                    sumValue = static_cast<int>(v[(opcode & 0x0F00) >> 8]) + static_cast<int>(v[(opcode & 0x00F0) >> 4]);

                    //if the value is > 255 we have a carry so v[F] is set to 1
                    if(sumValue > 255){v[15] = 1;}

                    //if there is no carry v[F] is set to 0
                    else{v[15] = 0;}

                    //now that the carry is set we can perform the addition
                    v[(opcode & 0x0F00) >> 8] = sumValue;

                    //move to next instruction
                    pc+=2;

                    //tell me opcode detected
                    cout << "8ry4 detected: " << opcode << endl;
                break;

                //if the instruction is 8xy5 - Set Vx = Vx - Vy
                case 0x0005:
                    //see if a carry is needed v[x] > v[y] if so sett v[f] to 1
                    if(v[(opcode & 0x0F00) >> 8] > v[(opcode & 0x00F0) >> 4]){v[15] = 1;}

                    //else set v[f] to 0
                    else{v[15] = 0;}

                    //perform the subtraciton
                    v[(opcode & 0x0F00) >> 8] =  v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];

                    //move to next instruction
                    pc+=2;

                    //tell me opcode detected
                    cout << "8ry5 detected: " << opcode << endl;
                break;

                //if the instruction is 8x06 - basically divides v[x] by 2
                case 0x0006:
                    //set v[F] to the carry
                    v[15] = v[(opcode & 0x0F00) >> 8] & 0x1;

                    //shift v[x] right by one
                    v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] >>= 1;

                    //move to next instruction
                    pc+=2;

                    //tell me opcode detected
                    cout << "8ry6 detected: " << opcode << endl;
                break;

                //if the instruction is 8xy7
                case 0x0007:
                    //check if there is a borrow
                    if(v[(opcode & 0x00F0) >> 4] > v[(opcode & 0x0F00) >> 8]){v[15] = 1;}

                    //else v[15] = 0
                    v[15] = 0;

                    //perform oprtation: v[x] = v[y] - v[x]
                    v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];

                    //move to next instruction
                    pc+=2;

                    //tell me opcode detected
                    cout << "8ry7 detected: " << opcode << endl;
                break;

                //if the instruction is 8xye
                case 0x000e:
                    //set v[F] to the carry
                    v[15] = (v[(opcode & 0x0F00) >> 8] >> 7) & 0x1;

                    //shift v[x] Left by one
                    v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] <<= 1;

                    //move to next instruction
                    pc+=2;

                    //tell me opcode detected
                    cout << "8rye detected: " << opcode << endl;
                break;

                //if the opcode is not caught
                default:
                    //tell me the unknown opcode
                    printf("Unknown opcode 0x%X\n", opcode);
                    exit(1);
                break;
            }
        break;

		//9xy0 - SNE Vx, Vy
		case 0x9000:
			//Skip next instruction if V[x] != V[y]
			if(v[(opcode & 0x0F00) >> 8] != v[(opcode & 0x00F0) >> 4]){
				pc+=4;
			}
			
			//otherwise run next instruction
			else{
				pc+=2;
			}
		break;

        //if the opcode is 0xAnnn then indexReg = nnn
        case 0xA000:
            //set the indexReg to nnn
            indexReg = (opcode & 0x0FFF);

            //move to the next instruction
            pc+=2;

            //tell me opcode detected
            cout << "Annn - MVI detected: " << opcode << endl;
        break;
		
		//Bnnn - JP V0, addr
        case 0xB000:
        	//set pc
        	pc = (opcode & 0x0FFF) + v[0];
        break;

        //if the opcode is 0xCrxx then we generate a random number
        case 0xC000:
            v[(opcode & 0x0F00) >> 8] = ((rand() % 255) & (opcode & 0x00FF));

            //move to the next instruction
            pc+=2;

            //tell me opcode detected
            cout << "Crxx - rand detected: " << opcode << endl;
        break;


        //if the opcode is 0xDnnn then we draw stuff to the screen later
        case 0xD000:
        	//get the required parameters
			x = v[(opcode & 0x0F00) >> 8];
			y = v[(opcode & 0x00F0) >> 4];
			height = (opcode & 0x000F);
			
			//run function to update values in the screen
			drys(x, y, height);
        
            //move to the next instruction
            pc+=2;

            //tell me opcode detected
            cout << "Dnnn - Dxyn detected: " << opcode << endl;
        break;

        //if the opcode is 0xExxx series we need to then work out which one it is
        case 0xE000:
            //switch to determine which of the Exxx series the instruciton is
            switch(opcode & 0x00FF){
                //if command ek9e: skip if key pressed
                case 0x009E:
                	//skip next instruction of key k is pressed
                	if(key[v[(opcode & 0x0F00) >> 8]] == 1){
                		pc+=4;
                	}else{
	                    pc+=2;
	                }

                    //tell me opcode detected
                    cout << "EK9E - skpr detected: " << opcode << endl;

                break;

                //if command is eka1
                case 0x00A1:
                	//skip next instruction if key K is not pressed
                	if(key[v[(opcode & 0x0F00) >> 8]] == 0){
                		pc+=4;
                	}else{
	                    pc+=2;
	                }

                    //tell me opcode detected
                    cout << "eka1 - skup detected: " << opcode << endl;

                break;

            }

        break;

        //if the opcode is 0xFxxx series we need to then work out which one it is
        case 0xF000:
            //switch to determine which of the Fxxx series the instruciton is
            switch(opcode & 0x00FF){

                //if opcode is fr07
                case 0x0007:
                    v[(opcode & 0x0F00) >> 8] = delayTimer;

                    //increment pc counter
                    pc+=2;

                    //tell me opcode detected
                    cout << "Fr15 - sdelay detected: " << opcode << endl;
                break;

                //if opcode is fr0a
                case 0x000A:
                    //increment pc counter
                    pc+=2;

                    //tell me opcode detected
                    cout << "Fr0A - key detected: " << opcode << endl;
                break;

                //if opcode is fr15
                case 0x0015:
                    delayTimer = v[(opcode & 0x0F00) >> 8];

                    //increment pc counter
                    pc+=2;

                    //tell me opcode detected
                    cout << "Fr15 - sdelay detected: " << opcode << endl;
                break;

                //if opcode is fr18
                case 0x0018:
                    soundTimer = v[(opcode & 0x0F00) >> 8];

                    //increment pc counter
                    pc+=2;

                    //tell me opcode detected
                    cout << "Fr18 - ssound detected: " << opcode << endl;
                break;

                //if opcode is fr1e
                case 0x001e:
                    //indexRegister += v[r]
                    indexReg += v[(opcode & 0x0F00) >> 8];

                    //increment pc counter
                    pc+=2;

                    //tell me opcode detected
                    cout << "Fr1e - adi detected: " << opcode << endl;
                break;

                //if opcode is fr29
                case 0x0029:
                    indexReg = v[(opcode & 0x0F00) >> 8] * 0x5;

                    //increment pc counter
                    pc+=2;

                    //tell me opcode detected
                    cout << "Fr29 - LDR detected: " << opcode << endl;
                break;

                //if opcode is fr33
                case 0x0033:
                    //FX33 - store BCD of VX in [I], [I+1], [I+2]. using division and modulo
                    memory[indexReg] = v[(opcode & 0x00FF) >> 8] / 100;
                    memory[indexReg+1] = (v[(opcode & 0x00FF) >> 8] / 10) % 10;
                    memory[indexReg+2] = (v[(opcode & 0x00FF) >> 8] % 100) %10;

                    //increment pc counter
                    pc+=2;

                    //tell me opcode detected
                    cout << "Fr33 - BCD detected: " << opcode << endl;
                break;

				//Fx55 - LD [I], Vx
                case 0x0055:
                    //for i = 0 to v[x]
                    for(int i=0;i<=(v[(opcode & 0x0F00)] >> 8);i++){
                    	memory[i] = v[i];
                    }
                    
                    //increase pc cpunter
                    pc+=2;
                break;

                //if opcode is fx65
                case 0x0065:
                    //for i = 0 to v[x]
                    for(int i=0;i<=(v[(opcode & 0x0F00)] >> 8);i++){
                        v[i] = memory[indexReg+i];
                    }

                    //increment pc counter
                    pc+=2;

                    //tell me opcode detected
                    cout << "Fx65 - LDR detected: " << opcode << endl;
                break;

                //if the opcode is not caught
                default:
                    //tell me the unknown opcode
                    printf("Unknown opcode 0x%X\n", opcode);
                    exit(1);
                break;
            }

        break;

        //if the opcode is not caught
        default:
            //tell me the unknown opcode
            printf("Unknown opcode 0x%X\n", opcode);
            exit(1);
        break;
    }

    //Update timers - Timers should slow down the system to allow execution of only 60 opcodes per second
    //update the delayTimer
    if(delayTimer > 0){delayTimer--;}

    //update the sound timer
    if(soundTimer > 0){
        //check if sound timer is == 1, if so the computer beeps... well it outputs that It would beep
        if(soundTimer == 1){
        	cout << "beep..." << endl;
        	cout << '\a';
        }

        //decrement the sound timer
        soundTimer--;
    }
}


#endif





























