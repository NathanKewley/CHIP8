//define the chip 8 class/function... js is wierd
function chip8(){
	//--FONTSET--//
	this.font = [
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
	];	

	//--EMU VARIABLES--//
	//just a simple var so we know if the emu is still running
	this.running = true;
	
	//the draw flag so the program can determine wether anything needs to be drawn to the screen
	this.drawFlag = 0;
	
	//screen holds state either 1 or 0. we dont specify a size sine arrays are dynamic is js, but size will be 2048 as that is the amount of pixels the chip 8 has.
	this.screen = [];
	
	//holds the states of all 16 keys, either presses (1) or not pressed (0)
	this.key = [];
	
	//define the cpu registers.. there are 16 of these
	this.v = [];
	
	//define the stack... again 16
	this.stack = [];
	
	//define the memory.. 4096 bytes
	var memory = new ArrayBuffer(4096);
    this.memory = new Uint8Array(memory);
	
	//define program counter to keep track of where we are in the program
	this.pc = 0;
	
	//create a varibale for the current opcode
	this.opcode;
	
	//defince the index register
	this.indexReg;
	
	//define stack pointer
	this.stackPointer;
	
	//define timers
	this.delayTimer;
	this.soundTimer;
	
	//--EMU FUNCTIONS--//
	//initiate the emulator...
	this.init = c8Init;
	
    //main CPU emulation function that will read, decode and execute the opcodes. This function will also
    //handle timers and limit execution speed
	this.cycle = c8Cycle;
	
	//function to clear the display
	this.clearDisplay = c8ClearDisplay;
	
	//load specified rom from local storage
	this.load = c8Load;
	
	//link/define functions for opcodes
	this.zerox = c80x;
	this.jmp = c8jmp;
	this.skeq = c8skeq;
	this.skne = c8skne;
	this.se = c8se;
	this.mov = c8mov;
	this.add = c8add;
	this.eightx = c88x;
	this.mvi = c8mvi;
	this.sne = c8sne;
	this.rand = c8rand;
	this.drys = c8drys;
	this.jp = c8jp;
	this.ex = c8ex;
	this.fx = c8fx;
	this.jsr = c8jsr;
}

//initiates the emulator
function c8Init(){
	//some boring output...
	console.log("initiating");
	
	//set running to true
	this.running = true;
	
	//set the pc to the starting position of 0x200
	this.pc = 0x0200;
	
	//reset the rest of the variables
	this.opcode = 0;
	this.indexReg = 0;
	this.stackPoint = 0;
	this.delayTimer = 0;
	this.soundTimer = 0;
	this.clearDisplay();
	
    //clear stack, keys and registers
    for(var i=0;i<16;i++){this.stack[i] = this.key[i] = this.v[i] = 0;}

    //clear memory
    for(i=0;i<4096;i++){this.memory[i] = 0;}	
    
    //load the font set into 0 - 0x200 range of memory
    for(i=0;i<80;i++){
        this.memory[i] = this.font[i];
    }
}

//function to clear the display
function c8ClearDisplay(){
    //for each pixel on the screen
    for(var i=0;i<2048;i++){
        //set the pixel to not draw
        this.screen[i] = 0;
    }

    //set the draw flag so the screen is re-drawn
    this.drawFlag = 1;
}

//function to load a specified rom from local storage
function c8Load(){
	//load program into memory
	for(var i = 0; i < buffer.length; i++){
		this.memory[i + 0x200] = buffer[i];
	}
}

//0x0000 series
function c80x(opcode){
	//Switch by only the last unit in the opcode
    switch(opcode & 0x000F){
    	//detect: 00E0 - CLS (Clear the Screen)
        case 0x0000:
        	//clear the display
        	this.clearDisplay();
        	
        	//increase pc by 2
        	this.pc += 2;
        break;
        
        //detect: 00EE - RTS (Return From Subroutine Call)
        case 0x000E:
        	//decrement the stack to the previous subroutine recored in the stack
            this.stackPoint--;

            //move to the previous point in the stack
            this.pc = this.stack[this.stackPoint];

            //increment the program counter to the next instruciton
            this.pc += 2;
        break;

        //if the code detected is not implemented and not part of the chip-8 spec output it to me
        default:
        	console.log("unknown opcode: " + opcode);
        break;
    }
}

//0x1xxx jmp
function c8jmp(opcode){
	//jump to the specified location
	this.pc = (opcode & 0x0FFF);
	
	//general logging
	console.log("jmp - 0x1000");
}

//0x2000 jsr
function c8jsr(opcode){
	//set the current space in memory to the current spot in the stack
    this.stack[this.stackPoint] = this.pc;

    //increment the current position in the stack
    this.stackPoint++;

    //jump to the subrouting at xxx where xxx is (0x0'xxx')
    this.pc = (opcode & 0x0FFF);

    //output the detected opcode to me
	console.log("jsr - 0x2000");
}

//0x3000 skeq
function c8skeq(opcode){
	//opcode is 3rxx. holding 2 parameters, 'r' and 'xx'. if register v[r] == xx then skip next instruction. else dont.
    //to compare register r we must get its value as the only significant number in the opcode so it sits between 0 and F (0 and 15) - v[0] to v[15]
   	//to do this we use "((opcode & 0x0F00) >> 8)" which shifts the result 0r00 to 000r making r a value of 1 to 15 addressing the correct register
    if(this.v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
    	//skip the next instruction
        this.pc+=4;
    }

    //if they are not equal the next instruciton is executes as per normal
    else{
    	//move on to the next instruciton
        this.pc+=2;
    }

    //general logging
    console.log("skeq - 0x3000");
}

//0x4000 skne
function c8skne(opcode){
	//opcode is 4rxx. holding 2 parameters, 'r' and 'xx'. if register v[r] != xx then skip next instruction. else dont.
	if(this.v[(opcode & 0x0F00) >> 8] != (this.opcode & 0x00FF)){
    	//skip the next instruction
        this.pc+=4;
    }

    //if they are equal dont skip next instruciton
    else{
    	//move on to the next instruciton
        this.pc+=2;
    }
}

//0x5000 se
function c8se(opcode){
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
}

//0x6rxx mov
function c8mov(opcode){
	//move the value xx into register r
    this.v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);

	//increase the pc counter
   	this.pc+=2;
   	
   	//general logging
	console.log("mov - 0x6000");
}

//0x7rxx add
function c8add(opcode){
    //perform the add
    this.v[(opcode & 0x0F00) >> 8] = (this.v[(opcode & 0x0F00) >> 8] + (opcode & 0x00FF));

	//increase pc
	this.pc += 2;
	
	//general logging
	console.log("add - 0x7000");
}

//0x8xxx 8series
function c88x(opcode){
	//run a switch statment to find the last value in the opcode as that is unique for all 8x value
    switch(opcode & 0x000F){
    	//if the instruction is 8ry0
        case 0x0000:
        	//move register v[y] into v[r]
            this.v[(opcode & 0x0F00) >> 8] = this.v[(opcode & 0x00F0) >> 4];

            //move to next instruction
           	this.pc+=2;
        break;

        //if the instruction is 8ry1
        case 0x0001:
        	//v[r] = v[r] OR v[y]
        	this.v[(opcode & 0x0F00) >> 8] |= this.v[(opcode & 0x00F0) >> 4];

            //move to next instruction
            this.pc+=2;
        break;

        //if the instruction is 8ry2
        case 0x0002:
        	//v[r] = v[r] AND v[y]
            this.v[(this.opcode & 0x0F00) >> 8] &= this.v[(this.opcode & 0x00F0) >> 4];

            //move to next instruction
        	this.pc+=2;
        break;
        
     	//if the instruction is 8ry3
        case 0x0003:
        	//v[r] = v[r] XOR v[y]
            this.v[(opcode & 0x0F00) >> 8] = (this.v[(opcode & 0x0F00) >> 8] ^ this.v[(opcode & 0x00F0) >> 4]);

            //move to next instruction
            this.pc+=2;
	    break;
        
        //if the instruction is 8ry4 - Set Vx = Vx + Vy, set VF = carry.
        case 0x0004:
        	//work out the total value of the addition
            var sumValue = (this.v[(opcode & 0x0F00) >> 8]) + (this.v[(opcode & 0x00F0) >> 4]);

            //if the value is > 255 we have a carry so v[F] is set to 1
            if(sumValue > 255){this.v[15] = 1;}

            	//if there is no carry v[F] is set to 0
                else{this.v[15] = 0;}

                //now that the carry is set we can perform the addition
                this.v[(this.opcode & 0x0F00) >> 8] = sumValue;

                //move to next instruction
                this.pc+=2;
		break;
		
        //if the instruction is 8xy5 - Set Vx = Vx - Vy
        case 0x0005:
        	//see if a carry is needed v[x] > v[y] if so sett v[f] to 1
            if(this.v[(opcode & 0x0F00) >> 8] > this.v[(opcode & 0x00F0) >> 4]){this.v[15] = 1;}

            //else set v[f] to 0
            else{this.v[15] = 0;}

            //perform the subtraciton
            this.v[(opcode & 0x0F00) >> 8] =  this.v[(opcode & 0x0F00) >> 8] -= this.v[(opcode & 0x00F0) >> 4];

            //move to next instruction
            this.pc+=2;
        break;		
        
        //if the instruction is 8x06 - basically divides v[x] by 2
        case 0x0006:
        	//set v[F] to the carry
            this.v[15] = this.v[(opcode & 0x0F00) >> 8] & 0x1;

            //shift v[x] right by one
            this.v[(opcode & 0x0F00) >> 8] = this.v[(opcode & 0x0F00) >> 8] >> 1;

            //move to next instruction
            this.pc+=2;
        break;        
        
        //if the instruction is 8xye
        case 0x000e:
        	//set v[F] to the carry
            this.v[15] = (this.v[(opcode & 0x0F00) >> 8] >> 7) & 0x1;

            //shift v[x] Left by one
            this.v[(opcode & 0x0F00) >> 8] = this.v[(opcode & 0x0F00) >> 8] << 1;

            //move to next instruction
            this.pc+=2;
        break;
        
		//if opcode is not found output it and end program        
        default:
        	console.log("Unknown opcode: 0x" + (this.opcode.toString(16)));
        	this.running = false;
        break;
    }
              	
    //general logging
    console.log("8xxx - 8ry0");
}

//9xy0 - SNE Vx, Vy
function c8sne(opcode){
	//Skip next instruction if V[x] != V[y]
	if(this.v[(opcode & 0x0F00) >> 8] != this.v[(opcode & 0x00F0) >> 4]){
		this.pc+=4;
	}
			
	//otherwise run next instruction
	else{
		this.pc+=2;
	}
}

//0xannn mvi
function c8mvi(opcode){
	//set the index register
	this.indexReg = (opcode & 0x0FFF);	

	//increase program counter
	this.pc += 2;
	
	//general logging
	console.log("mvi - 0xannn");
}

//0xB000 JP
function c8jp(opcode){
	this.pc = (opcode & 0x0FFF) + this.v[0];

	//general logging
	console.log("jp - 0x8xxx");
}

//0xCrxx rand
function c8rand(opcode){
	//generate a random number and & it with xx
	this.v[(this.opcode & 0x0F00) >> 8] = ((Math.floor((Math.random() * 255) + 1)) & (this.opcode & 0x00FF)); //((rand() % 255) & (opcode & 0x00FF));	
 
 	//increase program counter
	this.pc += 2;
	
	//general logging
	console.log("rnd - 0xCrxx");
}

//0xDnnn drys
function c8drys(opcode){
	//retrieve the required variables
	var posX = this.v[(opcode & 0x0F00) >> 8];
	var posY = this.v[(opcode & 0x00F0) >> 4];
	var height = (opcode & 0x000F);
	
	//set register v[f] to 0
	this.v[15] = 0;
	
	//for loop to draw the sprites to the correct height
	for(var y=0;y<height;y++){
		//char to hold the 'sprite'
		var sprite = this.memory[this.indexReg + y];
		
		//nested for loop for the x of the sprite
		for(var x=0;x<8;x++){
			//if a pixel should be drawn...
			if((sprite & (0x80 >> x)) != 0){
				//if a pixel is already drawn at the current spot
				if(this.screen[(posX + x + ((posY + y) * 64))] == 1){
					//v[f] is set to 1
					this.v[15] = 1;
				}
				
				//XOR '^' is used to draw to the screen ... or erase from the screen
				this.screen[posX + x + ((posY + y) * 64)] ^= 1;
			
			}
		}
	}

	//set the draw flag to 1
	this.drawFlag = 1;
	
	//increase the pc
	this.pc += 2;
	
	//general logging
	console.log("drys - 0xD000");
}

//0xExxx eseries
function c8ex(opcode){
	//switch to determine which of the Exxx series the instruciton is
    switch(opcode & 0x00FF){
    	//if command ek9e: skip if key pressed
        case 0x009E:
        	//skip next instruction of key k is pressed
            if(this.key[this.v[(opcode & 0x0F00) >> 8]] == 1){
            	this.pc+=4;
            }else{
	        	this.pc+=2;
	        }
		break;

        //if command is eka1
        case 0x00A1:
        	//skip next instruction if key K is not pressed
            if(this.key[this.v[(opcode & 0x0F00) >> 8]] == 0){
            	this.pc+=4;
            }else{
	        	this.pc+=2;
	        }
        break;
        
		//if opcode is not found output it and end program        
        default:
        	console.log("Unknown opcode: 0x" + (this.opcode.toString(16)));
        	this.running = false;
        break;
	}

    //tell me opcode detected
    console.log("skup - 0xE000");

}

//0xFxxx fSeries
function c8fx(opcode){
	//switch to determine which of the Fxxx series the instruciton is
    switch(opcode & 0x00FF){
    	//if opcode is fr07
        case 0x0007:
        	this.v[(opcode & 0x0F00) >> 8] = this.delayTimer;

            //increment pc counter
           	this.pc+=2;
           	
           	//debug log
           	console.log("07");
        break;

        //if opcode is fr0a
        case 0x000A:
        	//increment pc counter
            this.pc+=2;
		break;

        //if opcode is fr15
       	case 0x0015:
        	this.delayTimer = this.v[(opcode & 0x0F00) >> 8];

            //increment pc counter
            this.pc+=2;
           	//debug log
           	console.log("015");
       	break;
       	
        //if opcode is fr18
        case 0x0018:
        	this.soundTimer = this.v[(opcode & 0x0F00) >> 8];

            //increment pc counter
            this.pc+=2;
        break;

    	//if opcode is fr1e
        case 0x001e:
        	//indexRegister += v[r]
        	this.indexReg += this.v[(opcode & 0x0F00) >> 8];	
        	
        	//increase the pc
        	this.pc+=2;
           	//debug log
           	console.log("1e");
        break;

        //if opcode is fr29
        case 0x0029:
        	this.indexReg = this.v[(opcode & 0x0F00) >> 8] * 5;

            //increment pc counter
            this.pc+=2;

           	//debug log
           	console.log("29");
        break;

        //if opcode is fr33
        case 0x0033:
        	//FX33 - store BCD of VX in [I], [I+1], [I+2]. using division and modulo
            this.memory[this.indexReg] = this.v[(opcode & 0x00FF) >> 8] / 100;
            this.memory[this.indexReg+1] = (this.v[(opcode & 0x00FF) >> 8] / 10) % 10;
            this.memory[this.indexReg+2] = (this.v[(opcode & 0x00FF) >> 8] % 100) %10;

            //increment pc counter
            this.pc+=2;
            
           	//debug log
           	console.log("33");
        break;
        
		//Fx55 - LD [I], Vx
        case 0x0055:
        	//for i = 0 to v[x]
            for(var i=0;i<=(this.v[(opcode & 0x0F00)] >> 8);i++){
            	this.memory[i] = this.v[i];
            }
                    
            //increase pc cpunter
            this.pc+=2;
        break;        
        
        //if opcode is fx65
        case 0x0065:
        	//for i = 0 to v[x]
            for(var i=0;i<=(this.v[(opcode & 0x0F00)] >> 8);i++){
            	this.v[i] = this.memory[this.indexReg+i];
            }

            //increment pc counter
            this.pc+=2;
            
           	//debug log
           	console.log("65");
        break;
                
		//if opcode is not found output it and end program        
        default:
        	console.log("Unknown opcode: 0x" + (this.opcode.toString(16)));
        	this.running = false;
        break;
    }
    
    //produce general logging
    console.log("Fxxx - " + this.opcode.toString(16));
}

//main CPU cycle function to handle all our opcodes
function c8Cycle(){
    //fetch Opcode - to do this we need to get 2 bytes from the memory where the program counter is currenty pointing,
    //to do this we need to use the bitwise 'OR operation' bitwiser operations reference: https://en.wikipedia.org/wiki/Bitwise_operation

    //The first half of this statment takes the 8 bits from memory[pc] and shifts them 8 bits to the right adding 8 zeros, All of this
    //is then stored in the opcode as 16 bits (The original 8 + the 8 zero's from the shift). The 8 bits in memory[pc +1] are then added
    //to the 16 bits already stored in the opcode variable using the OR operator. This puts 2 bytes/18 bits of data from the memory into opcode.
    this.opcode = this.memory[this.pc] << 8 | this.memory[this.pc + 1];
    
    //Decode and execute opcode - We use a switch statment to test against all possible opcodes and detect the one that needs to be executed.
    //The detected opcode is then executed. The program counter (pc) is increased by 2 each time since each opcode is 2 bytes.
    switch(this.opcode & 0xF000){
		//if the opcode is a 0xxx series
        case 0x0000:
			this.zerox(this.opcode);
		break;    
    
		//if the opcode is a 1xxx series
        case 0x1000:
			this.jmp(this.opcode);
		break;

        //if the opcode is a 2xxx series
        case 0x2000:
        	this.jsr(this.opcode);
        break;
		
        //if the opcode is 3xxx: 3rxx: SKEQ V[r], xx.
        case 0x3000:
        	this.skeq(this.opcode);
        break;
        
        //if the opcode is 4xxx: skne
        case 0x4000:
        	this.skne(this.opcode);
        break;
        
        //if the opcode is a 0x5xxx - se
        case 0x5000:
        	this.se(this.opcode);
        break;
		
		//mov. moves the value xx into register v[r]
        case 0x6000:
			this.mov(this.opcode);
		break;
		
		//opcode is 7rxx. adds the value xx to register v[r]        
		case 0x7000:
            this.add(this.opcode);
        break;
        
		//opcode is 8rxx send the opcode to the function 8x to determine and execute opcode        
		case 0x8000:
            this.eightx(this.opcode);
        break;        
		
		//opcode is 9xy0 - sne        
		case 0x9000:
            this.sne(this.opcode);
        break;        		
		
		//if the opcode is 0xAnnn then indexReg = nnn
        case 0xA000:
			this.mvi(this.opcode);
		break;
		
		//Bnnn - JP V0, addr
        case 0xB000:
        	this.jp(opcode);
        break;
        
		//if the opcode is 0xCrxx then indexReg = nnn
        case 0xC000:
			this.rand(this.opcode);
		break;		
		
        //if the opcode is 0xDnnn then we draw stuff to the screen
        case 0xD000:
        	//run the function to draw to the screen, it works out its own paramerts based on the opcode
        	this.drys(this.opcode);
        break;	

        //if the opcode is 0xE000 series we send the opcode to the function to work out which variation and execute
        case 0xE000:
            this.ex(this.opcode);
        break;	
        
        //if the opcode is 0xF000 series we send the opcode to the function to work out which variation and execute
        case 0xF000:
            this.fx(this.opcode);
        break;	
		
		//tell me if the opcode is not found
		default:
		    console.log("Unknown opcode: 0x" + (this.opcode.toString(16)));
    		this.running = false;
		break;
	}    

    //update the delayTimer
    if(this.delayTimer > 0){this.delayTimer-=1;}

    //update the sound timer
    if(this.soundTimer > 0){
        //check if sound timer is == 1, if so the computer beeps... well it outputs that It would beep
        if(this.soundTimer == 1){
			console.log("beep...");
        }

        //decrement the sound timer
        this.soundTimer-=1;
    }
}



















