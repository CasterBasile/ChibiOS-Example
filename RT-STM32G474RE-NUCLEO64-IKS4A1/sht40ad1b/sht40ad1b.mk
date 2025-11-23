# sht40ad1b PATH
SHT40AD1BPATH = ./sht40ad1b

# List of all the SHT40AD1B device files.
SHT40AD1BSRC := $(SHT40AD1BPATH)/sht40ad1b.c

# Required include directories
SHT40AD1BINC := $(SHT40AD1BPATH)

# Shared variables
ALLCSRC += $(SHT40AD1BSRC)
ALLINC  += $(SHT40AD1BINC)