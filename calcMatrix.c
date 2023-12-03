
#include <Arduino.h>

// first "calcMatrix.c" will be included
// all variables used in calcMatrix will be declared here
// all other variables can be declared in the main program
int letterCol[110];             // the color of each letter in the actual matrix
int colMatrix, setColMatrix;    // the choosen color of the matrix, setColMatrix was used for setting the color


void setLetCol (int begStr, int lenStr)
// defines which letters of the matrix should be displayed in the selected colour
{
  for (int c=begStr; c<=(begStr+lenStr-1); c=c+1)
  {
    letterCol[c]=colMatrix;  
  }
}


void calcMatrix(int mtx, int valA, int valB)        // Berechnung des Matrix-Arrays
// calculate which matrix will be used and which letters will be displayed in the selected color
// mtx is the matrix, valA means the case to display, valB ist a special value used in some cases to display special things
{   
  if (mtx<99) {mtx=mtx+10;}   // uses the matrixes for the english version started at 10
  
  //******************** matrix for the english version ********************
  // display the time (minutes)
  if (mtx==10)     
  {       
    setLetCol(0,2); setLetCol(3,2);    // "it is"  
    switch (valA)
    {
      case 0: setLetCol(104,6); break;                                          // "o'clock"volle Stunde, wie case 12
      case 1: setLetCol(28,4); setLetCol(44,4); break;                          // "five past"
      case 2: setLetCol(38,3); setLetCol(44,4); break;                          // "ten past"
      case 3: setLetCol(13,7); setLetCol(44,4); break;                          // "quarter past"
      case 4: setLetCol(22,6); setLetCol(44,4); break;                          // "twenty past"
      case 5: setLetCol(22,10); setLetCol(44,4); break;                         // "twentyfive past"   
      case 6: setLetCol(33,4); setLetCol(44,4); break;                          // "half past"
      case 7: setLetCol(22,10); setLetCol(42,2); break;                         // "twentyfive to"
      case 8: setLetCol(22,6); setLetCol(42,2); break;                          // "twenty to"
      case 9: setLetCol(13,7); setLetCol(42,2); break;                          // "quarter to "
      case 10: setLetCol(38,3); setLetCol(42,2); break;                         // "ten to"
      case 11: setLetCol(28,4); setLetCol(42,2); break;                         // "five to"
      case 12: setLetCol(104,6); break;                                         // "o'clock" volle Stunde wie case 0
    }      
    // display the time (hours)             
    switch (valB)
    {
      // case entspricht der Stunde
      case 1: setLetCol(55,3); break;
      case 2: setLetCol(74,3); break;
      case 3: setLetCol(61,5); break;
      case 4: setLetCol(66,4); break;
      case 5: setLetCol(70,4); break;
      case 6: setLetCol(58,3); break;
      case 7: setLetCol(88,5); break;
      case 8: setLetCol(77,5); break;
      case 9: setLetCol(51,4); break;
      case 10: setLetCol(99,3); break;
      case 11: setLetCol(82,6); break;
      case 12: setLetCol(93,6); break;
    }     
  }   
  // display the status of the battery  
  if (mtx==11)   
  {     
    setLetCol(0,3); setLetCol(14,7); setLetCol(22,2);    // "the battery is"
    if (valB==1)     // battery is charging
    {
      setLetCol(96,2); setLetCol(99,8);  // "ist charging"      
    }   
    
// empty 0 
// almost empty  1  
// less than a quarter full  2
// more than a quarter full  3   
// less than half full 4
// half full 5
// more than half full 6
// less than three quarters full 7    
// more than three quarters full 8
// almost full 9
// full  10

    switch (valA)
    {
      case 0: setLetCol(77,5); break;
      case 1: setLetCol(44,6); setLetCol(77,5); break;
      case 2: setLetCol(33,4); setLetCol(38,4); setLetCol(43,1); setLetCol(55,7); setLetCol(73,4); break;
      case 3: setLetCol(26,4); setLetCol(38,4); setLetCol(43,1); setLetCol(55,7); setLetCol(73,4); break;
      case 4: setLetCol(33,4); setLetCol(38,4); setLetCol(66,4); setLetCol(73,4); break;       
      case 5: setLetCol(66,4); setLetCol(73,4); break;       
      case 6: setLetCol(26,4); setLetCol(38,4); setLetCol(66,4); setLetCol(73,4); break;
      case 7: setLetCol(33,4); setLetCol(38,4); setLetCol(49,5); setLetCol(55,7); setLetCol(73,4); break;
      case 8: setLetCol(26,4); setLetCol(38,4); setLetCol(49,5); setLetCol(55,7); setLetCol(73,4); break;
      case 9: setLetCol(44,6); setLetCol(73,4); break; 
      case 10: setLetCol(73,4); break;      
    }       
  }
  
  // display the menu
  if (mtx==12)
  {
    if (valB == 1)
    { 
      setLetCol(0,3); setLetCol(11,4);   // set time
      setLetCol(38,6); setLetCol(50,5);  // change color
      setLetCol(66,5); setLetCol(77,10); // power management  
    }
    if (valB == 0)
    { 
      switch (valA)
      {
      case 1: setLetCol(0,3); setLetCol(11,4); break;   // set time
      case 2: setLetCol(38,6); setLetCol(50,5); break;  // change color
      case 3: setLetCol(66,5); setLetCol(77,10); break; // power management  
      }
    }
  }
  // display the date (day)
  if (mtx==13)  
  {       
    switch (valA)    
    {
      case 1: setLetCol(33,5); break;
      case 2: setLetCol(22,6); break;
      case 3: setLetCol(37,5); break;
      case 4: setLetCol(44,6); break;       
      case 5: setLetCol(50,7); break;       
      case 6: setLetCol(28,5); break;
      case 7: setLetCol(55,7); break;
      case 8: setLetCol(66,6); break;
      case 9: setLetCol(17,5); break; 
      case 10: setLetCol(72,5); break;  
      case 11: setLetCol(77,8); break;
      case 12: setLetCol(88,8); break;
      case 13: setLetCol(37,4); setLetCol(104,6); break;
      case 14: setLetCol(44,4); setLetCol(104,6); break;     
      case 15: setLetCol(50,3); setLetCol(104,6); break;  
      case 16: setLetCol(28,3); setLetCol(104,6); break;
      case 17: setLetCol(55,5); setLetCol(104,6); break;
      case 18: setLetCol(66,5); setLetCol(104,6); break;
      case 19: setLetCol(62,4); setLetCol(104,6); break;
      case 20: setLetCol(5,8); break;
      case 21: setLetCol(5,6); setLetCol(33,5); break;
      case 22: setLetCol(5,8); setLetCol(22,6);break;
      case 23: setLetCol(5,8); setLetCol(37,5); break;
      case 24: setLetCol(5,8); setLetCol(44,6); break;
      case 25: setLetCol(5,8); setLetCol(50,7);break;      
      case 26: setLetCol(5,8); setLetCol(28,5); break;
      case 27: setLetCol(5,8); setLetCol(55,7); break;
      case 28: setLetCol(5,8); setLetCol(66,6); break;
      case 29: setLetCol(5,8); setLetCol(17,5); break;
      case 30: setLetCol(11,6); break;  
      case 31: setLetCol(11,6); setLetCol(33,5); break; 
    }       
  }
  // display the date (month)
  if (mtx==14)  
  {   
    switch (valA)    
    {
      case 1: setLetCol(11,7); break;
      case 2: setLetCol(23,8); break;
      case 3: setLetCol(33,5); break;
      case 4: setLetCol(39,5); break;       
      case 5: setLetCol(44,3); break;       
      case 6: setLetCol(51,4); break;
      case 7: setLetCol(55,4); break;
      case 8: setLetCol(60,6); break;
      case 9: setLetCol(66,9); break; 
      case 10: setLetCol(81,7); break;  
      case 11: setLetCol(88,8); break;
      case 12: setLetCol(99,8); break;
    }   
  }
  // display the date (year)
  if (mtx==15)  
  {    
    setLetCol(16,6);
    switch (valA)    
    {
      case 2020: setLetCol(22,6); break;
      case 2021: setLetCol(22,6); setLetCol(44,3); break;
      case 2022: setLetCol(22,6); setLetCol(67,3); break;
      case 2023: setLetCol(22,6); setLetCol(50,5); break;       
      case 2024: setLetCol(22,6); setLetCol(62,4); break;       
      case 2025: setLetCol(22,6); setLetCol(55,4); break;
      case 2026: setLetCol(22,6); setLetCol(81,3); break;
      case 2027: setLetCol(22,6); setLetCol(88,5); break;
      case 2028: setLetCol(22,6); setLetCol(105,5); break;
      case 2029: setLetCol(22,6); setLetCol(73,4); break; 
      case 2030: setLetCol(33,6); break;  
      case 2031: setLetCol(33,6); setLetCol(44,3); break;
      case 2032: setLetCol(33,6); setLetCol(67,3); break;
      case 2033: setLetCol(33,6); setLetCol(50,5); break;     
      case 2034: setLetCol(33,6); setLetCol(62,4); break;       
      case 2035: setLetCol(33,6); setLetCol(55,4); break;
      case 2036: setLetCol(33,6); setLetCol(81,3); break;
      case 2037: setLetCol(33,6); setLetCol(88,5); break;
      case 2038: setLetCol(33,6); setLetCol(105,5); break;
      case 2039: setLetCol(33,6); setLetCol(73,4); break; 
    }    
  }
  // setting the time (hour)
  if (mtx==16)  
  {    
    setLetCol(104,6); 
    switch (valA)    
    {
      case 0: setLetCol(49,6); setLetCol(6,2); break;
      case 1: setLetCol(11,3); setLetCol(6,2); break;
      case 2: setLetCol(22,3); setLetCol(6,2); break;
      case 3: setLetCol(33,5); setLetCol(6,2); break;
      case 4: setLetCol(44,4); setLetCol(6,2); break;
      case 5: setLetCol(55,4); setLetCol(6,2); break;
      case 6: setLetCol(63,3); setLetCol(6,2); break;
      case 7: setLetCol(72,5); setLetCol(6,2); break;
      case 8: setLetCol(77,5); setLetCol(6,2); break;
      case 9: setLetCol(90,4); setLetCol(6,2); break;
      case 10: setLetCol(18,3); setLetCol(6,2); break;
      case 11: setLetCol(27,6); setLetCol(6,2); break;
      case 12: setLetCol(49,6); setLetCol(9,2); break;
      case 13: setLetCol(11,3); setLetCol(9,2); break;
      case 14: setLetCol(22,3); setLetCol(9,2); break;
      case 15: setLetCol(33,5); setLetCol(9,2); break;
      case 16: setLetCol(44,4); setLetCol(9,2); break;
      case 17: setLetCol(55,4); setLetCol(9,2); break;
      case 18: setLetCol(63,3); setLetCol(9,2); break;
      case 19: setLetCol(72,5); setLetCol(9,2); break;
      case 20: setLetCol(77,5); setLetCol(9,2); break;
      case 21: setLetCol(90,4); setLetCol(9,2); break;
      case 22: setLetCol(18,3); setLetCol(9,2); break;
      case 23: setLetCol(27,6); setLetCol(9,2); break;
    }     
  }
  // setting the time (minute)
  if (mtx==17)  
  {          
    switch (valA)    
    {
      case 0: setLetCol(29,4); break;
      case 1: setLetCol(33,3); break;
      case 2: setLetCol(36,3); break;
      case 3: setLetCol(39,5); break;
      case 4: setLetCol(44,4); break;
      case 5: setLetCol(48,4); break;
      case 6: setLetCol(52,3); break;
      case 7: setLetCol(55,5); break;
      case 8: setLetCol(60,5); break;
      case 9: setLetCol(66,4); break;
      case 10: setLetCol(70,3); break;
      case 11: setLetCol(77,6); break;
      case 12: setLetCol(88,6); break;
      case 13: setLetCol(99,8); break;
      case 14: setLetCol(44,4); setLetCol(103,4); break;
      case 15: setLetCol(22,3); setLetCol(103,4); break;
      case 16: setLetCol(52,3); setLetCol(103,4); break;
      case 17: setLetCol(55,5); setLetCol(103,4); break;
      case 18: setLetCol(60,4); setLetCol(103,4); break;
      case 19: setLetCol(66,4); setLetCol(103,4); break;
      case 20: setLetCol(5,6); break;
      case 21: setLetCol(5,5); setLetCol(33,3); break;
      case 22: setLetCol(5,5); setLetCol(36,3); break;
      case 23: setLetCol(5,5); setLetCol(39,5); break;
      case 24: setLetCol(5,5); setLetCol(44,4); break;
      case 25: setLetCol(5,5); setLetCol(48,4); break;
      case 26: setLetCol(5,5); setLetCol(52,3); break;
      case 27: setLetCol(5,5); setLetCol(55,5); break;
      case 28: setLetCol(5,5); setLetCol(60,5); break;
      case 29: setLetCol(5,5); setLetCol(66,4); break;
      case 30: setLetCol(11,6); break;
      case 31: setLetCol(11,6); setLetCol(33,3); break;
      case 32: setLetCol(11,6); setLetCol(36,3); break;
      case 33: setLetCol(11,6); setLetCol(39,5); break;
      case 34: setLetCol(11,6); setLetCol(44,4); break;
      case 35: setLetCol(11,6); setLetCol(48,4); break;
      case 36: setLetCol(11,6); setLetCol(52,3); break;
      case 37: setLetCol(11,6); setLetCol(55,5); break;
      case 38: setLetCol(11,6); setLetCol(60,5); break;
      case 39: setLetCol(11,6); setLetCol(66,4); break;
      case 40: setLetCol(17,5); break;
      case 41: setLetCol(17,5); setLetCol(33,3); break;
      case 42: setLetCol(17,5); setLetCol(36,3); break;
      case 43: setLetCol(17,5); setLetCol(39,5); break;
      case 44: setLetCol(17,5); setLetCol(44,4); break;
      case 45: setLetCol(17,5); setLetCol(48,4); break;
      case 46: setLetCol(17,5); setLetCol(52,3); break;
      case 47: setLetCol(17,5); setLetCol(55,5); break;
      case 48: setLetCol(17,5); setLetCol(60,5); break;
      case 49: setLetCol(17,5); setLetCol(66,4); break;
      case 50: setLetCol(22,5); break;
      case 51: setLetCol(22,5); setLetCol(33,3); break;
      case 52: setLetCol(22,5); setLetCol(36,3); break;
      case 53: setLetCol(22,5); setLetCol(39,5); break;
      case 54: setLetCol(22,5); setLetCol(44,4); break;
      case 55: setLetCol(22,5); setLetCol(48,4); break;
      case 56: setLetCol(22,5); setLetCol(52,3); break;
      case 57: setLetCol(22,5); setLetCol(55,5); break;
      case 58: setLetCol(22,5); setLetCol(60,5); break;
      case 59: setLetCol(22,5); setLetCol(66,4); break;
      case 60: setLetCol(94,5); break;                    // display "saved"
    }      
  }  
  // Change color 
  // for more information about the colors see "codeColor(int code)" in the main program
  if (mtx==18)  
  {    
    switch (valA)    
    {
      case 0: setLetCol(0,5); break;
      case 1: setLetCol(5,4); break;
      case 2: setLetCol(13,9); break;
      case 3: setLetCol(13,4); setLetCol(22,4); break;
      case 4: setLetCol(26,6); break;
      case 5: setLetCol(33,6); break;
      case 6: setLetCol(39,5); break;
      case 7: setLetCol(51,4); break;
      case 8: setLetCol(13,4); setLetCol(51,4); break;
      case 9: setLetCol(13,4); setLetCol(99,4); break;
      case 10: setLetCol(17,5); break;
      case 11: setLetCol(22,4); break;
      case 12: setLetCol(11,3); break;
      case 13: setLetCol(66,7); break;
      case 14: setLetCol(77,6); break;       
      case 15: setLetCol(82,5); break;     
      case 16: setLetCol(88,6); break;
      case 17: setLetCol(17,5); setLetCol(77,6); break;
      case 18: setLetCol(103,4); break;
      case 19: setLetCol(94,5); break;
      case 20: setLetCol(73,4); break;
      case 21: setLetCol(55,6); break;
      case 22: setLetCol(99,4); break;
      case 23: setLetCol(44,6); break;    
      case 24: setLetCol(107,3); break;     
    }     
  }
  // Power Management
  if (mtx==19)  
  {    
    if (valB==1)
    {
      setLetCol(0,6); setLetCol(7,3); setLetCol(11,9); 
      setLetCol(33,6); setLetCol(40,3); setLetCol(44,6); setLetCol(51,4);
      setLetCol(77,4); setLetCol(82,6); setLetCol(88,3);
    }
    else 
    {
      switch (valA)    
      {
        case 0: setLetCol(0,6); setLetCol(7,2); setLetCol(11,9);  break;
        case 1: setLetCol(33,6); setLetCol(40,3); setLetCol(44,6); setLetCol(51,4); break;
        case 2: setLetCol(77,4); setLetCol(82,6); setLetCol(88,3); break;  
        case 3: setLetCol(66,7); setLetCol(99,5); break;     
      }     
    }
  }

  {    
  if (valB==1)
    {
      setLetCol(11,8); setLetCol(22,7);  
      setLetCol(77,7); setLetCol(88,6);       
    }
    else 
    {
      switch (valA)    
      {
        case 0: setLetCol(77,7); setLetCol(88,6); break;
        case 1: setLetCol(11,8); setLetCol(22,7); break;
        case 2: setLetCol(44,11); setLetCol(55,5); break;          
      }     
    }
  }
}
