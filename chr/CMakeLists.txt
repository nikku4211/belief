find_package(Python3 REQUIRED)

set(CONVERTER ${CMAKE_CURRENT_SOURCE_DIR}/pilbmp2nes.py)

set(BIN2H ${CMAKE_CURRENT_SOURCE_DIR}/bin2h.py)

# Create commands to convert BG PNGs to CHRs
set(CHR_FILES "")
file(GLOB PNG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/testbg.png)
foreach(PNG_FILE ${PNG_FILES})
    get_filename_component(PNG_NAME ${PNG_FILE} NAME_WE)
    set(CHR_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PNG_NAME}.chr)

    add_custom_command(
        OUTPUT ${CHR_FILE}
        COMMAND ${Python3_EXECUTABLE} ${CONVERTER} -i ${PNG_FILE} --planes "0;1" -o ${CHR_FILE}
        DEPENDS ${CONVERTER} ${PNG_FILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Converting ${PNG_NAME}.png to ${PNG_NAME}.chr"
    )

    list(APPEND CHR_FILES ${CHR_FILE})
endforeach()

add_custom_target(convert_chrs DEPENDS ${CHR_FILES})

# Create commands to convert player PNGs to CHRs
file(GLOB PNGSPR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/playerspr.png ${CMAKE_CURRENT_SOURCE_DIR}/enemyninjspr.png ${CMAKE_CURRENT_SOURCE_DIR}/shurikspr.png)
foreach(PNGSPR_FILE ${PNGSPR_FILES})
    get_filename_component(PNGSPR_NAME ${PNGSPR_FILE} NAME_WE)
    set(CHR_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PNGSPR_NAME}.chr)

    add_custom_command(
        OUTPUT ${CHR_FILE}
        COMMAND ${Python3_EXECUTABLE} ${CONVERTER} -i ${PNGSPR_FILE} --planes "0;1" -W 8 -H 16 -o ${CHR_FILE}
        DEPENDS ${CONVERTER} ${PNGSPR_FILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Converting ${PNGSPR_NAME}.png to ${PNGSPR_NAME}.chr but sprite"
    )

    list(APPEND CHR_FILES ${CHR_FILE})
endforeach()

add_custom_target(convert_chrsprs DEPENDS ${CHR_FILES})

# Create commands to convert CHRs to Hs
set(H_FILES "")
foreach(CHR_FILE ${CHR_FILES})
    get_filename_component(CHR_NAME ${CHR_FILE} NAME_WE)
    set(H_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${CHR_NAME}.h)

    add_custom_command(
        OUTPUT ${H_FILE}
        COMMAND ${Python3_EXECUTABLE} ${BIN2H} -o ${H_FILE} ${CHR_FILE}
        DEPENDS ${BIN2H} ${CHR_FILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Converting ${CHR_NAME}.chr to ${CHR_NAME}.h"
    )

    list(APPEND H_FILES ${H_FILE})
endforeach()

add_custom_target(convert_hs DEPENDS ${H_FILES})

# tell cmake that this `chr_data` will add these generated header files to the build
add_library(chr_data ${H_FILES})
set_target_properties(chr_data PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(chr_data convert_hs)
add_dependencies(convert_hs convert_chrs)

# Allow `.incbin` to find the .chr file
target_include_directories(chr_data PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)