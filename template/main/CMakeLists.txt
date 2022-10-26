# By default source file(s) are kept in the "main" component of the project
# This is the CMake file for this component

idf_component_register( SRCS "main.c"
                        INCLUDE_DIRS "."
                        PRIV_REQUIRES 
		                    test_driver)
