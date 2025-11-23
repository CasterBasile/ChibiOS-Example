# lsm6dsv16x PATH
LSM6DSV16XPATH = ./lsm6dsv16x

# List of all the LSM6DSV16X device files.
LSM6DSV16XSRC := $(LSM6DSV16XPATH)/lsm6dsv16x.c

# Required include directories
LSM6DSV16XINC := $(LSM6DSV16XPATH)

# Shared variables
ALLCSRC += $(LSM6DSV16XSRC)
ALLINC  += $(LSM6DSV16XINC)