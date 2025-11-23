#Userlib directories
USERLIB = ./userlib

# List of all the Userlib files
USERSRC =  $(USERLIB)/src/max7219.c 
          
# Required include directories
USERINC =  $(USERLIB) \
           $(USERLIB)/include 

# Shared variables
ALLCSRC += $(USERSRC)
ALLINC  += $(USERINC)