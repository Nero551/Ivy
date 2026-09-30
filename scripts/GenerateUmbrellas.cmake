function(generate_umbrella_header DIR SUBSYSTEM)
    set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
    set(SUBSYSTEM_DIR "${ROOT_DIR}/${DIR}")
    set(OUTPUT "${SUBSYSTEM_DIR}/${SUBSYSTEM}.hpp")

    file(GLOB_RECURSE HEADERS
            CONFIGURE_DEPENDS
            "${SUBSYSTEM_DIR}/*.hpp"
    )

    list(FILTER HEADERS EXCLUDE REGEX "/${SUBSYSTEM}\\.hpp$")

    list(SORT HEADERS)

    file(WRITE "${OUTPUT}" "#pragma once\n\n")

    foreach (HEADER IN LISTS HEADERS)
        file(RELATIVE_PATH RELATIVE_HEADER
                "${SUBSYSTEM_DIR}"
                "${HEADER}"
        )

        file(APPEND "${OUTPUT}"
                "#include \"${RELATIVE_HEADER}\"\n"
        )
    endforeach ()
endfunction()

function(generate_nova_umbrella)
    set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
    set(OUTPUT "${ROOT_DIR}/Nova.hpp")

    file(GLOB SUBSYSTEM_DIRS
            CONFIGURE_DEPENDS
            LIST_DIRECTORIES true
            "${ROOT_DIR}/*"
    )

    file(GLOB MODULE_HEADERS
            CONFIGURE_DEPENDS
            "${ROOT_DIR}/Modules/*/Header.hpp"
    )

    list(SORT SUBSYSTEM_DIRS)
    list(SORT MODULE_HEADERS)

    file(WRITE "${OUTPUT}" "#pragma once\n\n")

    foreach (DIR IN LISTS SUBSYSTEM_DIRS)
        if (NOT IS_DIRECTORY "${DIR}")
            continue()
        endif ()

        get_filename_component(SUBSYSTEM "${DIR}" NAME)

        if (EXISTS "${DIR}/${SUBSYSTEM}.hpp")
            file(APPEND "${OUTPUT}"
                    "#include <${SUBSYSTEM}/${SUBSYSTEM}.hpp>\n"
            )
        endif ()
    endforeach ()

    foreach (HEADER IN LISTS MODULE_HEADERS)
        get_filename_component(MODULE_DIR "${HEADER}" DIRECTORY)
        get_filename_component(MODULE "${MODULE_DIR}" NAME)

        file(APPEND "${OUTPUT}"
                "#include <Modules/${MODULE}/Header.hpp>\n"
        )
    endforeach ()
endfunction()