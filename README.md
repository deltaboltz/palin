# palin

run make clean first to clean up any files

use ./master -h for usage info and help

to run use ./master [any argument] infile
  infile being the name of the file you wish to use to test the palindromes
  
  
  problems:
  
  Occasional floating point exceptions are thrown
  possible crashes
  MUST USE ipcrm -a to clean memory since freeshm does not seem to want to work...
