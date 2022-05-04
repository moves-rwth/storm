# This modules defines
#  SPHINX_EXECUTABLE
#  SPHINX_FOUND

find_program(SPHINX_EXECUTABLE
  NAMES sphinx-build sphinx-build2
  HINTS $ENV{SPHINX_DIR}
  PATHS
    /usr/bin
    /usr/local/bin
    /opt/local/bin
  DOC "Sphinx documentation generator"
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Sphinx DEFAULT_MSG SPHINX_EXECUTABLE)

option( SPHINX_HTML_OUTPUT "Build a single HTML with the whole content." ON )
option( SPHINX_EPUB_OUTPUT "Build HTML pages with additional information for building a documentation collection in epub." OFF )
option( SPHINX_LATEX_OUTPUT "Build LaTeX sources that can be compiled to a PDF document using pdflatex." OFF )
option( SPHINX_MAN_OUTPUT "Build manual pages in groff format for UNIX systems." OFF )
option( SPHINX_TEXT_OUTPUT "Build plain text files." OFF )


mark_as_advanced(
  SPHINX_EXECUTABLE
  SPHINX_HTML_OUTPUT
  SPHINX_EPUB_OUTPUT
  SPHINX_LATEX_OUTPUT
  SPHINX_MAN_OUTPUT
  SPHINX_TEXT_OUTPUT
)

function( Sphinx_add_target target_name builder conf source destination )
  add_custom_target( ${target_name} ALL
    COMMAND ${SPHINX_EXECUTABLE} -b ${builder}
    -c ${conf}
    ${source}
    ${destination}
    COMMENT "Generating sphinx documentation: ${builder}"
    )

  set_property(
    DIRECTORY APPEND PROPERTY
    ADDITIONAL_MAKE_CLEAN_FILES
    ${destination}
    )
endfunction()

# Target dependencies can be optionally listed at the end.
function( Sphinx_add_targets target_base_name conf source base_destination )
  if( ${SPHINX_HTML_OUTPUT} )
    Sphinx_add_target( ${target_base_name}_html html ${conf} ${source} ${base_destination}/html )
  endif()

  if( ${SPHINX_EPUB_OUTPUT} )
    Sphinx_add_target( ${target_base_name}_epub epub ${conf} ${source} ${base_destination}/epub )
  endif()

  if( ${SPHINX_LATEX_OUTPUT} )
    Sphinx_add_target( ${target_base_name}_latex latex ${conf} ${source} ${base_destination}/latex )
  endif()

  if( ${SPHINX_MAN_OUTPUT} )
    Sphinx_add_target( ${target_base_name}_man man ${conf} ${source} ${base_destination}/man )
  endif()

  if( ${SPHINX_TEXT_OUTPUT} )
    Sphinx_add_target( ${target_base_name}_text text ${conf} ${source} ${base_destination}/text )
  endif()
endfunction()
