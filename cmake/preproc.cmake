

# Function to run the C preprocessor, lifted from
# https://stackoverflow.com/questions/3562483/only-run-c-preprocessor-in-cmake

function(add_c_preprocessor_command)
    # Add custom command to run C preprocessor.
    #
    # Arguments
    #   OUTPUT          output file
    #   SOURCE          input file
    #   TARGET          CMake target to inherit compile definitions, include directories, and compile options
    #   EXTRA_C_FLAGS   extra compiler flags added after all flags inherited from the TARGET

    set(one_value_args TARGET SOURCE OUTPUT)
    set(multi_value_args EXTRA_C_FLAGS)
    cmake_parse_arguments(CPP "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    string(TOUPPER ${CMAKE_BUILD_TYPE} build_type)
    string(REPLACE " " ";" c_flags "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${build_type}}")

    add_custom_command(
            OUTPUT ${CPP_OUTPUT}
            COMMAND ${CMAKE_C_COMPILER}
            "-D$<JOIN:$<TARGET_PROPERTY:${CPP_TARGET},COMPILE_DEFINITIONS>,;-D>"
            "-I$<JOIN:$<TARGET_PROPERTY:${CPP_TARGET},INCLUDE_DIRECTORIES>,;-I>"
            ${c_flags}
            $<TARGET_PROPERTY:${CPP_TARGET},COMPILE_OPTIONS>
            ${CPP_EXTRA_C_FLAGS}
            -E -P -x c ${CPP_SOURCE} -o ${CPP_OUTPUT}
            COMMAND_EXPAND_LISTS VERBATIM
            IMPLICIT_DEPENDS C ${CPP_SOURCE}
            DEPENDS ${CPP_SOURCE})
endfunction()