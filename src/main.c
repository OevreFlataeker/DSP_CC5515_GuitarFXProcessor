/*****************************************************************************/
/*                                                                           */
/* Audioeffects for DSP Lab                                                  */
/* 	 main.c                                                                  */
/*                                                                           */
/*****************************************************************************/ 


#include "stdio.h"
#include "usbstk5515.h"
#include "aic3204.h"
#include "PLL.h"
#include "stereo.h"
#include "math.h"
#include "pushbuttons.h"
#include "oled.h"
#include "sine.h"
#include "math.h"

Int16 left_input;
Int16 right_input;
Int16 left_output;
Int16 right_output;
Int16 mono_input, mono_output;

unsigned int modulateDelay(unsigned long fixeddelta, signed long depth, unsigned int lfo_freq);
Int16 _delayElement(unsigned int);
Int16 _delayElementEffect(unsigned int);
Int16 echo(Int16 in, unsigned int);
Int16 echoInfi(Int16, unsigned int);
Int16 phaser(Int16);
Int16 clean(Int16);
Int16 chorus(Int16);
Int16 tremolo(Int16);

Int32 idx = 0;
#define SAMPLES_PER_SECOND 24000
#define GAIN 0
unsigned long int i = 0;
#define N 24000


signed Int16 cleanbuffer[N];
signed Int16 effectbuffer[N];  

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  main( )                                                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */
 void init_array() {
 	int idx = 0;
 	for (idx=0; idx<N; idx++) {
 		cleanbuffer[idx]=0;
 		effectbuffer[idx]=0;
 	}
 }
 
 void createsine() {
 
 	// Creates a sine table with N/4 precalculated values of a 1Hz sine
 	#define PI 3.141592
 	double t = 0;
 	int idx = 0;
 	double val = 0.0;
 	while (idx<6000) {
 		 
 		 val = sin(2*PI*t);
 	   samples[idx]= val;
 	   t+=1.0/24000.0;
 	   idx++;
 	}
}

void main( void ) 
{
    int flag = 1;
    unsigned int btn = 0;
    unsigned int btnOld = 0;
    /* Initialize BSL */
    USBSTK5515_init();
	
	/* Initialize PLL */
	pll_frequency_setup(100);

    /* Initialise hardware interface and I2C for code */
    aic3204_hardware_init();
    
    /* Initialise the AIC3204 codec */
	aic3204_init(); 

	/* Setup sampling frequency and 30dB gain for microphone */
    set_sampling_frequency_and_gain(SAMPLES_PER_SECOND, GAIN);	
  
    oled_init();
    SAR_init();
    oled_display_message("Guitar        ", "    Effects       ");
    asm(" bclr XF");
    init_array();
    createsine();
    
 	for ( i = 0  ; i < SAMPLES_PER_SECOND * 600L  ;i++  )
 	{

     aic3204_codec_read(&left_input, &right_input); // Configured for one interrupt per two channels.
     mono_input = stereo_to_mono(left_input, right_input);
   
     btn = pushbuttons_read(6);
     
 	 if (idx>=N) idx=0;
	 cleanbuffer[idx] = mono_input;
	 
	 if (btn>5) btn=1;
 	 if (btn==1) {
     	mono_output = clean(mono_input);
     	if (btn!=btnOld)
     		  oled_display_message("Guitar        ", "    Clean       ");
 	 } else if (btn == 2) {
 	 	mono_output = chorus(mono_input);
 	 	if (btn!=btnOld)
     		  oled_display_message("Guitar        ", "    Chorus      ");
 	 } else if (btn == 3) {
 	 	mono_output = phaser(mono_input);
 	 	if (btn!=btnOld)
     		  oled_display_message("Guitar        ", "    Phaser      ");
 	 }  else if (btn == 4) {
 	 	mono_output = tremolo(mono_input);
 	 	if (btn!=btnOld)
     		  oled_display_message("Guitar        ", "    Tremolo     ");
 	 }else if (btn == 5) {
 	 	mono_output = echo(mono_input, 400);
 	 	if (btn!=btnOld)
     		  oled_display_message("Guitar        ", "    Echo        ");
 	 }else if (btn == 6) {
 	 	mono_output = echoInfi(mono_input, 400);
 	 	if (btn!=btnOld)
     		  oled_display_message("Guitar        ", "    EchoInfi    ");
 	 }

     idx++;
     btnOld = btn;

     aic3204_codec_write(mono_output,mono_output);
 	}

    /* Disable I2S and put codec into reset */ 
    aic3204_disable();

    printf( "\n***Program has Terminated***\n" );
    SW_BREAKPOINT;
}

Int16 clean(Int16 in) {
	return in;
}

Int16 chorus(Int16 inputSample) {
signed long voice1, voice2, voice3, temp;
Int16 delay1 = 0, delay2 = 0, delay3 = 0;
	
	delay1 = modulateDelay(300,100,1);
	delay2 = modulateDelay(100,20,1);
	delay3 = modulateDelay(240,200,1);
		
    voice1 = _delayElement(delay1);
    voice1 = (((Int16)(0.9*voice1))>>1) + (inputSample>>1);    
    
    voice2 = _delayElement(delay2);
    voice2 = (((Int16)(0.8*voice2))>>1) + (inputSample>>1);
    
    voice3 = _delayElement(delay3);
    voice3 = (((Int16)(0.85*voice3))>>1) + (inputSample>>1);
    
    temp = voice1+inputSample;
    
    temp = temp+voice2+voice3;
        
    while (temp>32767 || temp<-32767) {
    	temp>>=1;	
    }
    return temp;    
}

unsigned int modulateDelay(unsigned long fixeddelta, signed long depth, unsigned int lfo_freq) {
double sinevalue = 0;
long tmp = 0;
int sineidx = 0; 
    
    /*
    tmp = lfo_freq*idx;
	sineidx = tmp % 6000;

	if (idx>=0 && idx<=5999) { 
	  	sinevalue = samples[sineidx];
	} else if (idx>=6000 && idx<=11999) {
		sinevalue = samples[6000-sineidx];
	} else if (idx>=12000 && idx<=17999) {
		sinevalue = -samples[sineidx];
	} else if (idx>=18000 && idx<=23999) {
		sinevalue = -samples[6000-sineidx];
	}
	*/
	if (idx>=0 && idx<=5999) { 
	  	sinevalue = samples[idx];
	} else if (idx>=6000 && idx<=11999) {
		sinevalue = samples[6000-(idx-6000)];
	} else if (idx>=12000 && idx<=17999) {
		sinevalue = -samples[sineidx-12000];
	} else if (idx>=18000 && idx<=23999) {
		sinevalue = -samples[18000-(idx-6000)];
	}
	
	return fixeddelta + depth*sinevalue;				
}

Int16 tremolo(Int16 inputSample) {
signed long tmp;
signed long factor = 0;
	factor = (signed long) modulateDelay(90,5,1);
		
	tmp = (factor * inputSample)/100;
		
	return (Int16) tmp;	
}

Int16 phaser(Int16 inputSample) {
static unsigned int delay = 0; 
Int16 delayedElement = 0;
signed long outputSample = 0;
signed long feedback = 0;

	delay = modulateDelay(240,100,1);
	
		
	delayedElement = _delayElementEffect(delay);
	
	// Build effect
	feedback = delayedElement + inputSample;
	// 
	feedback *= 0.9; // alpha gain
	
	while (feedback<-32767 || feedback>32767) feedback >>=1;
	
	effectbuffer[idx] = ((Int16) -1*feedback)+inputSample;
	outputSample = feedback+delayedElement;
	while (outputSample<-32767 || outputSample>32767) outputSample>>=1;
		
	return (Int16) outputSample;		
}

Int16 echo(Int16 in, unsigned int delay) {
signed long temp=0;
	Int16 echoelement;
	echoelement =  _delayElement(delay);
	echoelement >>= 1;
    temp = echoelement + in;
    //temp >>= 1;
    if (temp<-32767) temp = -32767;
    if (temp>32767) temp = 32767;
	return (Int16) temp;
}

Int16 echoInfi(Int16 in, unsigned int delay) {
signed long temp=0;
	Int16 echoelement;
	echoelement =  _delayElementEffect(delay);
	echoelement >>= 1;
    temp = echoelement + in;
   // temp >>= 1;
    if (temp<-32767) temp = -32767;
    if (temp>32767) temp = 32767;
	effectbuffer[idx] = temp;

	return (Int16) temp;
}

Int16 _delayElement(unsigned int delay) {
  Int32 tmp = idx - delay;
  if (tmp<0) tmp = N+tmp;
  return cleanbuffer[tmp];
}

Int16 _delayElementEffect(unsigned int delay) {
  Int32 tmp = idx - delay;
  if (tmp<0) tmp = N+tmp;
  return effectbuffer[tmp];
}
