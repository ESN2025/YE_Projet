#include <system.h>
#include <sys/alt_stdio.h>
#include <unistd.h>
#include "opencores_i2c.h"
#include <alt_types.h>
#include <stdio.h>
#include <altera_avalon_pio_regs.h>
#include <math.h>
#include <sys/alt_irq.h>
#include <alt_types.h>
#include <altera_avalon_timer_regs.h>
#include <altera_avalon_timer.h>


//Registers
#define ADXL345_address 0x1d
#define ADXL345_OFSX  	0x1E
#define ADXL345_OFSY  	0x1F
#define ADXL345_OFSZ  	0x20
#define ADXL345_DATAX0  0x32
#define ADXL345_DATAX1  0x33
#define ADXL345_DATAY0  0x34
#define ADXL345_DATAY1  0x35
#define ADXL345_DATAZ0  0x36
#define ADXL345_DATAZ1  0x37

#define POWER_CTL  0x2D                                 // registre Power Control
#define DATA_FORMAT  0x31                               // registre Data Format

int chip_adress = -1;

//	I2C
int i2c_data = 0;

float mg_LSB = 3.9;						//Typique pour la gamme par défaut : + ou - 2g

int a ;

//	7 segments init
int c0 = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0;
int sev_seg = 0;

//	Timer IRQ
int timer_irq_flag = 0;

//	Offsets
int X_offset = 0 ;
int Y_offset = 3 ;
int Z_offset = 1;



void write_byte(int reg, int data){
	I2C_start(OPENCORES_I2C_0_BASE,ADXL345_address,0);      //Bit de départ + adresse de l'esclave + bit d'écriture
	I2C_write(OPENCORES_I2C_0_BASE,reg,0);       			//S'inscrire pour écrire
	I2C_write(OPENCORES_I2C_0_BASE,data,1);       			//Données d'écriture + bit d'arrêt
}

unsigned int read_byte(int reg){
	I2C_start(OPENCORES_I2C_0_BASE,ADXL345_address,0);
	I2C_write(OPENCORES_I2C_0_BASE,reg,0);
	I2C_start(OPENCORES_I2C_0_BASE,ADXL345_address,1);      //Démarrage + adresse de l'esclave + bit de lecture
	i2c_data =  I2C_read(OPENCORES_I2C_0_BASE,1);           //Collecte des dernières données (bit d'arrêt)

	return i2c_data;
}

void g_data(int add0,int add1){
    int data0,data1;
    int value;
    unsigned int value1;
	data0=read_byte(add0);                                   //8bits
	data1=read_byte(add1);                                   //8bits
    value1 = data0+(data1<<8);                               //16bits


    if(value1 > 0x8000){
    c5 = 0x0;                                                //Si le MSB est 1 (nombre négatif)
	value = -(((~value1) & 0xFFFF) + 1);                     //2-complément sur 16 bits
}
else {
	c5 = 0xf;
	value = value1;
}
	if (value<0){
		c5 = 0x0;                                            //Correspond à « - » dans le VHDL (pour les nombres négatifs)
		value = -value;
}
	else{
		c5 = 0xf;                                            //pour les nombres positifs

	}
	value = round( mg_LSB * value );
		c4 = value /10000;
		c3 = (value / 1000) % 10;
		c2 = (value /100) % 10;
		c1 = (value/10) % 10;
		c0 = value % 10;
		//Numéro à envoyer au PIO à 7 segments
		sev_seg = (c5 << 20) + (c4 << 16) +(c3 << 12) + (c2 << 8) + (c1 <<4) + c0;

		//Écrire le nombre sur le segment 7
		IOWR_ALTERA_AVALON_PIO_DATA(COUNTER_BASE,sev_seg);

}
//comme lab1,lab2
static void timer_IRQ (void * context, alt_u32 id)
{
    //Drapeau irq de la minuterie levé
    timer_irq_flag = 1;

	//Effacer l'interruption
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0x00);
}

void timer_IRQ_init(){

	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0x00);

	//Paramètres de réglage : comptage continu, demande d'IRQ + Démarrage
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE , ALTERA_AVALON_TIMER_CONTROL_CONT_MSK | ALTERA_AVALON_TIMER_CONTROL_START_MSK | ALTERA_AVALON_TIMER_CONTROL_ITO_MSK);

	//Enregistrer l'ISR dans l'interruption correspondante
	alt_irq_register (TIMER_0_IRQ , NULL, (void*) timer_IRQ);
}


void INIT_ADXL345(void){
	I2C_init(OPENCORES_I2C_0_BASE,ALT_CPU_FREQ,100000);
	write_byte(POWER_CTL, 0x08);   //measure
	write_byte( DATA_FORMAT, 0x0B); //+ - 16g 13
	write_byte( 0x2C, 0x08);    //v 12.5

    alt_printf("read POWER_CTL ADXL345  x: 0x%x\n", read_byte(POWER_CTL));
    alt_printf("read DATA_FORMAT ADXL345  x: 0x%x\n", read_byte(DATA_FORMAT));

	write_byte( ADXL345_OFSX, X_offset);    //offset x
	write_byte(ADXL345_OFSY, Y_offset);    //offset y
	write_byte(ADXL345_OFSZ, Z_offset);    //offset z
}


int main(int argc, char *argv[])
{
	int key1=1;
	int data=1;
	int read_address=0;
	//Initialisation I2C
    I2C_init(OPENCORES_I2C_0_BASE,ALT_CPU_FREQ,100000);

    read_address = read_byte(0x00);
    alt_printf("read address is : 0x%x\n", read_address);

	//Rechercher ADXL345
    chip_adress = I2C_start(OPENCORES_I2C_0_BASE,ADXL345_address,0);
    if ( chip_adress == 0){
        alt_printf("ADXL345 found at the address : 0x%x\n", ADXL345_address);
    }
    else if ( chip_adress == 1){
        alt_printf("ADXL345 not found\n");
    }
    else {
        alt_printf("Communication pb\n");
    }

int flag=0;
    while(1){
    	usleep(200);
    	INIT_ADXL345();

		key1 = IORD_ALTERA_AVALON_PIO_DATA(KEY1_BASE);
		if (key1 == 0 && flag==0){
	    	usleep(20);
			flag=1;
		if(data == 3)
			data =1;
		else {
			data++;
		}
		}
		else if(key1 == 1)
		{flag=0;}
		else {

		}

    	usleep(2000);

			//Impression sur les 7 segments
			if (data == 1){
				g_data(ADXL345_DATAX0,ADXL345_DATAX1);
		        alt_printf("x: 0x%x\n", sev_seg);
			}
			else if (data == 2){
				g_data(ADXL345_DATAY0,ADXL345_DATAY1);
		        alt_printf("y: 0x%x\n", sev_seg);
			}
			else {
				g_data(ADXL345_DATAZ0,ADXL345_DATAZ1);
		        alt_printf("z: 0x%x\n", sev_seg);
			}

	}

    return 0;
}
