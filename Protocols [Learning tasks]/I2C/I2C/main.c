/*
 * main.c
 *
 * Created: 1/9/2022 3:46:45 PM
 *  Author: advai
 */ 
#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>



void I2C_Master_Init(int addr){
	TWAR=addr<<1;
	TWCR=1<<TWEN;
	TWSR=0x00;
	TWBR=0x0C; 
}

void I2C_Start(){
	TWCR=1<<TWSTA|1<<TWEN|1<<TWINT;
	while(!(TWCR & (1<<TWINT)));
}
void I2C_SlaveWrite(unsigned int slavAdr){

	TWDR=slavAdr<<1 |0<<0;
	TWCR=1<<TWEN|1<<TWINT|1<<TWEA;
	while(!(TWCR & (1<<TWINT)));
	
	
}

void I2C_SendMessage(unsigned char data){
	TWDR=data;
	TWCR=1<<TWEN|1<<TWINT;
	while(!(TWCR & (1<<TWINT)));
	
}

void I2C_SlaveRead(unsigned int slavAdr){
	
	TWDR=slavAdr<<1|1<<0;
	TWCR=(1<<TWEN)|(1<<TWINT);
	while(!(TWCR & (1<<TWINT)));
	
}
void I2C_Stop(){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void I2C_MasterTransmit(){
	I2C_Start();
	while(1){
		if(((TWSR& 0xF8)==0x10 )|((TWSR& 0xF8)==0x08))
		{
			I2C_SlaveWrite(2<<0);
		}
		if((TWSR& 0xF8)==0x18){
			I2C_SendMessage('d');
		}
		if((TWSR & 0xF8)==0x28){
			if(TWDR=='#'){
				break;
			}
			I2C_Stop();
	}
	I2C_Stop();
}
}
int main(void)
{
	I2C_Master_Init(1<<0);
	I2C_MasterTransmit();
}