# CMake file for the Main component

idf_component_register( SRCS "main.c"
		INCLUDE_DIRS "."
		PRIV_REQUIRES f_measurement wifi_drv nvs_flash)

