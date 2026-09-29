function(generate_umbrella_header SUBSYSTEM)
    set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
    set(SUBSYSTEM_DIR "${ROOT_DIR}/${SUBSYSTEM}")
    set(OUTPUT "${SUBSYSTEM_DIR}/${SUBSYSTEM}.hpp")

    file(GLOB_RECURSE HEADERS
            CONFIGURE_DEPENDS
            "${SUBSYSTEM_DIR}/*.hpp"
    )

    list(FILTER HEADERS EXCLUDE REGEX "/${SUBSYSTEM}\\.hpp$")

    list(SORT HEADERS)

    file(WRITE "${OUTPUT}" "#pragma once\n\n")

    foreach(HEADER IN LISTS HEADERS)
        file(RELATIVE_PATH RELATIVE_HEADER
                "${SUBSYSTEM_DIR}"
                "${HEADER}"
        )

        file(APPEND "${OUTPUT}"
                "#include \"${RELATIVE_HEADER}\"\n"
        )
    endforeach()
endfunction()


function(generate_nova_umbrella)
    set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
    set(OUTPUT "${ROOT_DIR}/Nova.hpp")

    file(GLOB SUBSYSTEM_DIRS
            LIST_DIRECTORIES true
            "${ROOT_DIR}/*"
    )

    file(WRITE "${OUTPUT}" "#pragma once\n\n")

    foreach(DIR IN LISTS SUBSYSTEM_DIRS)
        if(IS_DIRECTORY "${DIR}")
            get_filename_component(SUBSYSTEM "${DIR}" NAME)

            if(EXISTS "${DIR}/${SUBSYSTEM}.hpp")
                file(APPEND "${OUTPUT}"
                        "#include <${SUBSYSTEM}/${SUBSYSTEM}.hpp>\n"
                )
            endif()
        endif()
    endforeach()
endfunction()