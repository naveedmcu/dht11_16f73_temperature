#define TC_CLK  PIN_A0
#define TC_DATA PIN_A1
#define TC_CS   PIN_A2
int1 thermocouple_error;         //a handy dandy global error flag to tell you if a thermocouple is connected or not

void init_TC(int CS) 
{ 
   output_low(TC_CLK); 
   output_low(TC_DATA); 
   output_high(CS);            //if we idle high, the chip keeps doing conversions. Change this if you like 
} 

int16 read_TC(int CS)               //It takes 200ms (ish) for the MAX6675 to perform a conversion 
{ 
   int8 i; 
   int16 data; 

   output_low(CS);            //stop any conversion processes 
   delay_ms(1);               //and give it some time to power up (not very much, admittedly) 

   for (i=0;i<16;i++){ 
      shift_left(&data,2,input(TC_DATA));      //reads in 2 bytes to data from the pin TC_DATA 
      output_high(TC_CLK); 
      delay_us(1); //Descomentar si usa crystal mayor a 10MHz
      output_low(TC_CLK); 
      delay_us(1);
   } 

   thermocouple_error=bit_test(data,2);      //this is the thermocouple status bit 
   delay_ms(1);     
   output_high(CS); 
   return(data); 
} 

int16 sortout(int16 raw) 
{ 
    return(0x0FFF & (raw>>3));      //returns only the bits converning temperature 
} 

float toFloat_TC(int16 tmp) 
{ 
   //return((((float)tmp/4.9127+459)*9/5)+32);
   return((float)tmp/2.021142857-25);      //adjusts data to floating point format, and accounts for the decimal point 
   //return((float)tmp/2.021142857-30); 
   //return((float)tmp);
} 

float do_everything(int CS) 
{ 
   init_TC(CS); 
   delay_ms(200);               //200ms is a long time to be doing nothing. use a timer interrupt to avoid wasting time here 
   return(toFloat_TC(sortout(read_TC(CS)))); 
} 
