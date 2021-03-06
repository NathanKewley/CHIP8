//include required c/c++ stuffz
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

//include the chip-8 cpu core
#include "chip-8-cpu.h"

//because I always use this as typig std::::::::: is lame
using namespace std;

//create an instance of the chip-8 cpu
chip8 c8;

//function to draw the screen
void updateScreen(chip8 &c8, sf::RenderWindow &window){
	//define the square pixel
	sf::RectangleShape pixel(sf::Vector2f(10, 10));

	//clear the window
	window.clear();

	//for each pixel on the screen decide to draw or not
	for(int i=0;i<64;i++){
		for(int ii=0;ii<32;ii++){
			if(c8.screen[(ii*64)+i] == 1){
				//set the position of the pixel appropriatly
				pixel.setPosition(i*10, ii*10);
		
				//draw to the screen
				window.draw(pixel);
			}
		}
	} 
	
	//display the window
	window.display();
	
	//set the draw flag back to 0
	c8.drawFlag = 0;
}

//main function and entry point of the program
int main(){

    //Define the screen
    sf::RenderWindow window(sf::VideoMode(640, 320), "C8");

	//limit the frames to 60 using SFML... this saves implementing the timer counter in the c8 emulator
	window.setFramerateLimit(60);

    //set up the system
    c8.init();

    //load a rom
    c8.load("tetris.c8");

    //infinite emulation loop
    while(1){
		/* KEY MAPPING
		ORIGINAL        EMULATED
		+-+-+-+-+       +-+-+-+-+
		|1|2|3|C|       |1|2|3|4|
		+-+-+-+-+       +-+-+-+-+
		|4|5|6|D|       |Q|W|E|R|
		+-+-+-+-+       +-+-+-+-+
		|7|8|9|E|       |A|S|D|F|
		+-+-+-+-+       +-+-+-+-+
		|A|0|B|F|       |Z|X|C|V|
		+-+-+-+-+       +-+-+-+-+
		*/

		//key[0] --- 1
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)){
        	c8.key[0] = 1;
        }else{
        	c8.key[0] = 0;
        }
        
		//key[1] --- 2
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)){
        	c8.key[1] = 1;
        }else{
        	c8.key[1] = 0;
        }
        
		//key[2] --- 3
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)){
        	c8.key[2] = 1;
        }else{
        	c8.key[2] = 0;
        }
        
		//key[3] --- C
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)){
        	c8.key[3] = 1;
        }else{
        	c8.key[3] = 0;
        }        
        
		//key[4] --- 4
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
        	c8.key[4] = 1;
        }else{
        	c8.key[4] = 0;
        }
        
		//key[5] --- 5
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
        	c8.key[5] = 1;
        }else{
        	c8.key[5] = 0;
        }
        
		//key[6] --- 6
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
        	c8.key[6] = 1;
        }else{
        	c8.key[6] = 0;
        }
        
		//key[7] --- D
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
        	c8.key[7] = 1;
        }else{
        	c8.key[7] = 0;
        }       
        
		//key[8] --- 7
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
        	c8.key[8] = 1;
        }else{
        	c8.key[8] = 0;
        }
        
		//key[9] --- 8
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
        	c8.key[9] = 1;
        }else{
        	c8.key[9] = 0;
        }
        
		//key[10] --- 9
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
        	c8.key[10] = 1;
        }else{
        	c8.key[10] = 0;
        }
        
		//key[11] --- E
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)){
        	c8.key[11] = 1;
        }else{
        	c8.key[11] = 0;
        }         
        
		//key[12] --- A
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)){
        	c8.key[12] = 1;
        }else{
        	c8.key[12] = 0;
        }
        
		//key[13] --- 0
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)){
        	c8.key[13] = 1;
        }else{
        	c8.key[13] = 0;
        }
        
		//key[14] --- B
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)){
        	c8.key[14] = 1;
        }else{
        	c8.key[14] = 0;
        }
        
		//key[15] --- F
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)){
        	c8.key[15] = 1;
        }else{
        	c8.key[15] = 0;
        }                                
 
        //emulate a cycle of the cpu
        c8.cycle();

        //update the screen if required
        if(c8.drawFlag == 1){updateScreen(c8, window);}        
        
    }

    //return and end the program
    return 0;
}
