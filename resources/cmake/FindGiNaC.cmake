set( GINAC_FOUND FALSE )

find_path( GINAC_INCLUDE_DIR ginac.h
           /usr/include/ginac
           /usr/local/include/ginac
           /opt/local/include/ginac
           $ENV{UNITTESTXX_PATH}/src
           $ENV{UNITTESTXX_INCLUDE_PATH} )

find_library( GINAC_LIBRARIES NAMES ginac PATHS 
              /usr/lib 
              /usr/local/lib 
              /opt/local/lib 
              $ENV{UNITTESTXX_PATH} 
              ENV{UNITTESTXX_LIBRARY_PATH} )

if( GINAC_INCLUDE_DIR AND GINAC_LIBRARIES )
        SET( GINAC_FOUND TRUE )
ENDIF (GINAC_INCLUDE_DIR AND GINAC_LIBRARIES)


MARK_AS_ADVANCED (	GINAC_INCLUDE_DIR
					GINAC_LIBRARIES
				 )
