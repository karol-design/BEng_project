# CMake file for the Main component

idf_component_register( SRCS "main.c"
		INCLUDE_DIRS "."
		PRIV_REQUIRES 
			interrupts_drv
			common)

