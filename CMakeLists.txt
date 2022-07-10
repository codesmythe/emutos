
cmake_minimum_required(VERSION 3.10)

include("cmake/preproc.cmake")

set(CMAKE_USER_MAKE_RULES_OVERRIDE "./cmake/overrides.cmake")

project(emutos LANGUAGES C ASM VERSION 1.1.1)

enable_language(ASM)

option(COLDFIRE "Compile for a ColdFire target." OFF)
option(WITH_AES "Enable AES support." ON)
option(WITH_CLI "Enable CLI support." ON)

set(UNIQUE us)

if (WITH_AES)
    message("AES support enabled.")
endif()

if (WITH_CLI)
    message("CLI support enabled.")
endif()

execute_process(
        COMMAND date +%Y%m%d
        OUTPUT_VARIABLE BUILD_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(UNOFFICIAL 1)
execute_process(
        COMMAND awk -f ${CMAKE_SOURCE_DIR}/tools/mkheader.awk ${UNIQUE} ${PROJECT_VERSION_MAJOR}
            ${PROJECT_VERSION_MINOR} ${PROJECT_VERSION_PATCH} ${UNOFFICIAL}
        OUTPUT_VARIABLE HEADER_CONTENTS
)

file(WRITE ${CMAKE_BINARY_DIR}/header.h ${HEADER_CONTENTS})

configure_file(version.c.in version.c)


# Compile flags

set(BUILD_TOOLS_OPTFLAGS -O)
set(CPUFLAGS -m68000)
set(OTHERFLAGS -fomit-frame-pointer -fno-common)
set(WARNFLAGS
        -Wall -Werror=undef -Werror=missing-prototypes -Werror=strict-prototypes -Werror=implicit-function-declaration
        -Werror=format -Werror=redundant-decls -Werror=format-extra-args  -Werror=old-style-definition
        -Werror=type-limits
        # -Werror=unused-function -Wshadow -Werror
)

set(NATIVECC gcc -ansi -pedantic ${WARNFLAGS} -W ${BUILD_TOOLS_OPTFLAGS})

#
# source code in bios/
#

set(bios_src
        # The source below must be the first to be linked
        bios/startup.S
        # These sources will be placed in ST-RAM by the linked script
        bios/lowstram.c
        # Other BIOS sources can be put in any order
        bios/memory.S bios/processor.S bios/vectors.S bios/aciavecs.S bios/bios.c bios/xbios.c bios/acsi.c
        bios/biosmem.c bios/blkdev.c bios/chardev.c bios/clock.c bios/conout.c bios/country.c
        bios/disk.c bios/dma.c bios/dmasound.c bios/floppy.c bios/font.c bios/ide.c bios/ikbd.c bios/initinfo.c
        bios/kprint.c bios/kprintasm.S bios/linea.S bios/lineainit.c bios/lineavars.S bios/machine.c
        bios/mfp.c bios/midi.c bios/mouse.c bios/natfeat.S bios/natfeats.c bios/nvram.c bios/panicasm.S
        bios/parport.c bios/screen.c bios/serport.c bios/sound.c bios/videl.c bios/vt52.c bios/xhdi.c
        bios/pmmu030.c bios/68040_pmmu.S bios/amiga.c bios/amiga2.S bios/spi_vamp.c
        bios/lisa.c bios/lisa2.S bios/delay.c bios/delayasm.S bios/sd.c bios/memory2.c bios/bootparams.c
        bios/scsi.c bios/nova.c bios/dsp.c bios/dsp2.S
)

if (COLDFIRE)
    set(bios_src ${bios_src} bios/codefile.c bios/coldfile2.S spi_cf.c)
endif()

#
# source code in bdos/
#

set(bdos_src bdos/bdosmain.c bdos/console.c bdos/fsbuf.c bdos/fsdir.c bdos/fsdrive.c bdos/fsfat.c bdos/fsglob.c
        bdos/fshand.c bdos/fsio.c bdos/fsmain.c bdos/fsopnclo.c bdos/iumem.c bdos/kpgmld.c bdos/osmem.c
        bdos/proc.c bdos/rwa.S bdos/time.c bdos/umem.c
)

#
# source code in util/.  Generated file langs.c handled separately
#

set(util_src util/cookie.c util/doprintf.c util/intmath.c util/memmove.S util/memset.S util/miscasm.S
        util/nls.c util/nlsasm.S util/setjmp.S util/string.c util/lisautil.S util/miscutil.c
)

# The functions in the following modules are used by the AES and EmuDesk
if (WITH_AES)
    set(util_src ${util_src} util/gemdos.c util/optimize.c util/optimopt.S util/rectfunc.c util/stringasm.S)
endif()

#
# font code not in the font lib
#

set(font_src bios/fnt_off_6x6.c bios/fnt_off_8x8.c)

#
# source code in vdi/
#

set(vdi_src vdi/vdi_asm.S vdi/vdi_bezier.c vdi/vdi_col.c vdi/vdi_control.c vdi/vdi_esc.c
        vdi/vdi_fill.c vdi/vdi_gdp.c vdi/vdi_input.c vdi/vdi_line.c vdi/vdi_main.c
        vdi/vdi_marker.c vdi/vdi_misc.c vdi/vdi_mouse.c vdi/vdi_raster.c vdi/vdi_text.c
        vdi/vdi_textblit.c
)

if (COLDFIRE)
    set(vdi_src ${vdi_src} vdi/vdi_tblit_cf.S)
else()
    set(vdi_src ${vdi_src} vdi/vdi_blit.S vdi/vdi_tblit.S)
endif()

# The source below must be the last VDI one
set(vdi_src ${vdi_src} vdi/endvdi.S)

#
# source code in aes/. Generated files handled separately.
#

set(aes_src aes/gemasm.S aes/gemstart.S aes/gemdosif.S aes/gemaplib.c aes/gemasync.c aes/gemctrl.c
        aes/gemdisp.c aes/gemevlib.c aes/gemflag.c aes/gemfmalt.c aes/gemfmlib.c
        aes/gemfslib.c aes/gemgraf.c aes/gemgrlib.c aes/gemgsxif.c aes/geminit.c aes/geminput.c
        aes/gemmnext.c aes/gemmnlib.c aes/gemobed.c aes/gemobjop.c aes/gemoblib.c aes/gempd.c aes/gemqueue.c
        aes/gemrslib.c aes/gemsclib.c aes/gemshlib.c aes/gemsuper.c aes/gemwmlib.c aes/gemwrect.c
        aes/gsx2.c)

#
# source code in desk/. Generated files handled separately.
#

set(desk_src desk/deskstart.S desk/deskmain.c desk/gembind.c desk/deskact.c desk/deskapp.c desk/deskdir.c
        desk/deskfpd.c desk/deskfun.c desk/deskglob.c desk/deskinf.c desk/deskins.c desk/deskobj.c
        desk/deskpro.c desk/deskrez.c desk/deskrsrc.c desk/desksupp.c desk/deskwin.c)

#
# source code in cli/ for EmuTOS console EmuCON
#

set(cli_src cli/cmdasm.S cli/cmdmain.c cli/cmdedit.c cli/cmdexec.c cli/cmdint.c cli/cmdparse.c cli/cmdutil.c)

set(source_files ${bios_src} ${bdos_src} ${util_src} ${font_src} ${vdi_src} ${CMAKE_CURRENT_BINARY_DIR}/version.c)
set(option_definitions "-DWITH_CMAKE")

if (WITH_AES)
    set(source_files ${source_files} ${aes_src} ${desk_src})
    set(option_definitions ${option_definitions} "-DWITH_AES=1")
else()
    set(option_definitions ${option_definitions} "-DWITH_AES=0")
endif()

if (WITH_CLI)
    set(source_files ${source_files} ${cli_src})
    set(option_definitions ${option_definitions} "-DWITH_CLI=1")
else()
    set(option_definitions ${option_definitions} "-DWITH_CLI=0")
endif()

# Create the font library.

add_library(font
        bios/fnt_st_6x6.c bios/fnt_st_8x8.c bios/fnt_st_8x16.c
        bios/fnt_l2_6x6.c bios/fnt_l2_8x8.c bios/fnt_l2_8x16.c
        bios/fnt_gr_6x6.c bios/fnt_gr_8x8.c bios/fnt_gr_8x16.c
        bios/fnt_ru_6x6.c bios/fnt_ru_8x8.c bios/fnt_ru_8x16.c
        bios/fnt_tr_6x6.c bios/fnt_tr_8x8.c bios/fnt_tr_8x16.c
)
target_include_directories(font SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/include)

#--------
# Due to CMake limitations, to place endrom.c at the end of the link we need to artificially put in a library.
#--------

add_library(endrom bios/endrom.c)
target_include_directories(endrom SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(endrom PUBLIC ${compile_definitions} ${option_definitions})

# Build the various native tools we need.

add_custom_target(localise
        COMMAND ${NATIVECC} ${CMAKE_SOURCE_DIR}/tools/localise.c -o localise
        DEPENDS ${CMAKE_SOURCE_DIR}/tools/localise.c
)

add_custom_target(mkrom
        COMMAND ${NATIVECC} ${CMAKE_SOURCE_DIR}/tools/mkrom.c -o mkrom
        DEPENDS ${CMAKE_SOURCE_DIR}/tools/mkrom.c
)

add_custom_target(bug
        COMMAND ${NATIVECC} ${CMAKE_SOURCE_DIR}/tools/bug.c -o bug
        DEPENDS ${CMAKE_SOURCE_DIR}/tools/bug.c
)

add_custom_target(draft
        COMMAND ${NATIVECC} ${option_definitions} ${CMAKE_SOURCE_DIR}/tools/draft.c ${CMAKE_SOURCE_DIR}/tools/draftexc.c -o draft
        DEPENDS ${CMAKE_SOURCE_DIR}/tools/draft.c ${CMAKE_SOURCE_DIR}/tools/draftexc.c
        )

add_custom_target(erd
        COMMAND ${NATIVECC} ${CMAKE_SOURCE_DIR}/tools/erd.c -o erd
        DEPENDS ${CMAKE_SOURCE_DIR}/tools/erd.c
        )

add_custom_target(grd
        COMMAND ${NATIVECC} -DGEM_RSC ${CMAKE_SOURCE_DIR}/tools/erd.c -o grd
        DEPENDS ${CMAKE_SOURCE_DIR}/tools/erd.c
)

add_custom_target(ird
        COMMAND ${NATIVECC} -DICON_RSC ${CMAKE_SOURCE_DIR}/tools/erd.c -o ird
        DEPENDS ${CMAKE_SOURCE_DIR}/tools/erd.c
)

add_custom_target(mrd
        COMMAND ${NATIVECC} -DMFORM_RSC ${CMAKE_SOURCE_DIR}/tools/erd.c -o mrd
        DEPENDS ${CMAKE_SOURCE_DIR}/tools/erd.c
)


set(POFILES po/cs.po po/de.po po/es.po po/fi.po po/fr.po po/gr.po po/hu.po po/it.po po/nl.po po/pl.po po/ru.po po/tr.po)

function(configure_target target_name target_define rom_size optflags unique)
    set(target ${target_name}.img)
    set(target_build_dir ${CMAKE_CURRENT_BINARY_DIR}/${target_name})
    file(MAKE_DIRECTORY ${target_build_dir}/util  ${target_build_dir}/po ${target_build_dir}/aes ${target_build_dir}/desk)
    file(COPY ${POFILES} po/LINGUAS DESTINATION ${target_build_dir}/po)
    set(linker_file ${target_build_dir}/${target_name}pp.ld)
    target_sources(${target} PUBLIC ${target_build_dir}/util/langs.c ${target_build_dir}/aes/gem_rsc.c
            ${target_build_dir}/desk/desk_rsc.c ${target_build_dir}/desk/icons.c ${target_build_dir}/aes/mforms.c)
    target_compile_options(${target} PUBLIC ${CPUFLAGS} -mshort ${optflags} ${OTHERFLAGS} ${WARNFLAGS})
    target_compile_definitions(${target} PUBLIC ${compile_definitions} ${option_definitions} ${target_define})
    target_include_directories(${target} SYSTEM PUBLIC ${target_build_dir}/aes ${target_build_dir}/desk ${target_build_dir}
            ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/aes ${CMAKE_SOURCE_DIR}/bios
            ${CMAKE_SOURCE_DIR}/util )
    target_link_libraries(${target} PUBLIC font gcc endrom)
    target_link_options(${target} BEFORE PUBLIC ${CPUFLAGS} -mshort -nostartfiles -nostdlib)
    target_link_options(${target} PUBLIC LINKER:-T,${linker_file} LINKER:-Map=${target_build_dir}/${target_name}.map)
    add_dependencies(${target} bug draft erd grd ird mrd ${target_name}_header ${target_name}_i18nconf ${target_name}pp.ld)
    set_target_properties(${target} PROPERTIES LINK_DEPENDS ${linker_file})

    message("Incoming UNIQUE is '${unique}'")
    set(localise_unique "")
    if (unique)
        set(localise_unique "-u${unique}")
        message("localise unique is '${localise_unique}")
    endif()

    add_c_preprocessor_command(
            OUTPUT ${target_name}/${target_name}pp.ld
            SOURCE ${CMAKE_SOURCE_DIR}/emutos.ld
            TARGET ${target}
            EXTRA_C_FLAGS
            "-DLOWSTRAM_OBJ=CMakeFiles/${target}.dir/bios/lowstram.c.o"
            "-DLINEAVARS_OBJ=CMakeFiles/${target}.dir/bios/lineavars.S.o"
    )
    add_custom_target(${target_name}pp.ld DEPENDS ${linker_file})

    add_custom_target(${target_name}.bin ALL
            COMMAND ./mkrom pad ${rom_size}k ${target} ${target_name}.bin
            DEPENDS ${target} mkrom)

    add_custom_command(OUTPUT ${target_build_dir}/header.h
            COMMAND ./localise ${localise_unique} ${CMAKE_SOURCE_DIR}/localise.ctl ${target_build_dir}/ctables.h /dev/null
            DEPENDS localise localise.ctl
    )
    add_custom_target(${target_name}_header DEPENDS ${target_build_dir}/header.h)

    add_custom_command(OUTPUT ${target_build_dir}/i18nconf.h
            COMMAND ./localise ${localise_unique} ${CMAKE_SOURCE_DIR}/localise.ctl /dev/null ${target_build_dir}/i18nconf.h
            DEPENDS localise localise.ctl
            )
    add_custom_target(${target_name}_i18nconf DEPENDS ${target_build_dir}/i18nconf.h)

    add_custom_command(OUTPUT ${target_build_dir}/po/messages.pot
            COMMAND ${CMAKE_BINARY_DIR}/bug -m ${target_build_dir}/po/messages.pot xgettext
            DEPENDS bug ${CMAKE_SOURCE_DIR}/po/POTFILES.in
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_custom_command(OUTPUT ${target_build_dir}/util/langs.c
            COMMAND ${CMAKE_BINARY_DIR}/bug make
            DEPENDS bug ${POFILES} po/LINGUAS ${target_build_dir}/po/messages.pot
            WORKING_DIRECTORY ${target_build_dir}
            )

    add_custom_command(OUTPUT ${target_build_dir}/aes/gem_rsc.c ${target_build_dir}/aes/gem_rsc.h
            COMMAND ${CMAKE_BINARY_DIR}/grd ${CMAKE_SOURCE_DIR}/aes/gem ${target_build_dir}/aes/gem_rsc
            DEPENDS grd ${CMAKE_SOURCE_DIR}/aes/gem.rsc
    )

    add_custom_command(OUTPUT ${target_build_dir}/desk/desk_rsc.c ${target_build_dir}/desk/desk_rsc.h
            COMMAND ${CMAKE_BINARY_DIR}/draft ${CMAKE_SOURCE_DIR}/desk/desktop ${target_build_dir}/draft.tmp
            COMMAND ${CMAKE_BINARY_DIR}/erd -pdesk ${target_build_dir}/draft.tmp ${target_build_dir}/desk/desk_rsc
            DEPENDS draft erd ${CMAKE_SOURCE_DIR}/desk/desktop.rsc
    )

    add_custom_command(OUTPUT ${target_build_dir}/aes/mforms.c ${target_build_dir}/aes/mforms.h
            COMMAND ${CMAKE_BINARY_DIR}/mrd -pmform ${CMAKE_SOURCE_DIR}/aes/mform ${target_build_dir}/aes/mforms
            DEPENDS mrd ${CMAKE_SOURCE_DIR}/aes/mform.rsc
    )

    add_custom_command(OUTPUT ${target_build_dir}/desk/icons.c ${target_build_dir}/desk/icons.h
            COMMAND ${CMAKE_BINARY_DIR}/ird -picon ${CMAKE_SOURCE_DIR}/desk/icon ${target_build_dir}/desk/icons
            DEPENDS ird ${CMAKE_SOURCE_DIR}/desk/icon.rsc
    )

endfunction()

#add_executable(etos192us.img ${source_files})
add_executable(etos256us.img ${source_files})
add_executable(etos512us.img ${source_files})
add_executable(etos1024k.img ${source_files})

#configure_target(etos192us TARGET_192 192)
configure_target(etos256us TARGET_256   256 "-Os" ${UNIQUE})
configure_target(etos512us TARGET_512   512 "-O2" ${UNIQUE})
configure_target(etos1024k TARGET_1024 1024 "-O2" "")



