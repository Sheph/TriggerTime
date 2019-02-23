include_directories(${CMAKE_CURRENT_BINARY_DIR} dynasm src src/host)

add_executable(minilua src/host/minilua.c)
if (NOT WIN32)
    target_link_libraries(minilua m dl)
endif ()

# Dynasm
set(DASM ${CMAKE_CURRENT_SOURCE_DIR}/dynasm/dynasm.lua)
set(DASM_T ${CMAKE_CURRENT_BINARY_DIR}/buildvm_arch.h)

# 2DO: Proper detection of flags
set(DASM_VER "")
set(DASM_FLAGS -D FPU -D HFABI)
set(DASM_ARCH x86)

# Windows is ... special
if (WIN32)
    list(APPEND DASM_FLAGS -D WIN)
endif ()

# 32bit vs 64bit
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    list(APPEND DASM_FLAGS -D P64)
else ()
    list(APPEND DASM_FLAGS -D SSE)
endif ()

list(APPEND DASM_FLAGS -D JIT)
list(APPEND DASM_FLAGS -D FFI)
list(APPEND DASM_FLAGS -D VER=${DASM_VER})

string(REPLACE ";" " " DASM_FLAGS_STR "${DASM_FLAGS}")

set(DASM_DASC ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_${DASM_ARCH}.dasc)

# Generate buildvm arch header
add_custom_command(OUTPUT ${DASM_T}
    COMMAND minilua ${DASM} ${DASM_FLAGS} -o ${DASM_T} ${DASM_DASC}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dynasm
    DEPENDS minilua
)

# Buildvm
file(GLOB SRC_BUILDVM src/host/buildvm*.c)
add_executable(buildvm ${SRC_BUILDVM} ${DASM_T})

macro(add_buildvm_target _target _mode)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_target}
        COMMAND buildvm ARGS -m ${_mode} -o ${CMAKE_CURRENT_BINARY_DIR}/${_target} ${ARGN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS buildvm ${ARGN}
    )
endmacro(add_buildvm_target)

if (WIN32)
    add_buildvm_target(lj_vm.obj peobj)
    set(LJ_VM_SRC ${CMAKE_CURRENT_BINARY_DIR}/lj_vm.obj)
else ()
    add_buildvm_target(lj_vm.s elfasm)
    set(LJ_VM_SRC ${CMAKE_CURRENT_BINARY_DIR}/lj_vm.s)
endif ()

set(LJLIB_C
    src/lib_base.c
    src/lib_math.c
    src/lib_bit.c
    src/lib_string.c
    src/lib_table.c
    src/lib_io.c
    src/lib_os.c
    src/lib_debug.c
    src/lib_jit.c
    src/lib_ffi.c
    src/lib_package.c
)

add_buildvm_target(lj_ffdef.h ffdef ${LJLIB_C})
add_buildvm_target(lj_bcdef.h bcdef ${LJLIB_C})
add_buildvm_target(lj_folddef.h folddef src/lj_opt_fold.c)
add_buildvm_target(lj_recdef.h recdef ${LJLIB_C})
add_buildvm_target(lj_libdef.h libdef ${LJLIB_C})
add_buildvm_target(jit/vmdef.lua libvm ${LJLIB_C})

set(DEPS
    ${LJ_VM_SRC}
    ${CMAKE_CURRENT_BINARY_DIR}/lj_ffdef.h
    ${CMAKE_CURRENT_BINARY_DIR}/lj_bcdef.h
    ${CMAKE_CURRENT_BINARY_DIR}/lj_libdef.h
    ${CMAKE_CURRENT_BINARY_DIR}/lj_recdef.h
    ${CMAKE_CURRENT_BINARY_DIR}/lj_folddef.h
)

## LuaJIT Library

set(LJCORE_C
    src/lj_gc.c
    src/lj_err.c
    src/lj_char.c
    src/lj_bc.c
    src/lj_obj.c
    src/lj_str.c
    src/lj_tab.c
    src/lj_func.c
    src/lj_udata.c
    src/lj_meta.c
    src/lj_debug.c
    src/lj_state.c
    src/lj_dispatch.c
    src/lj_vmevent.c
    src/lj_vmmath.c
    src/lj_strscan.c
    src/lj_api.c
    src/lj_lex.c
    src/lj_parse.c
    src/lj_bcread.c
    src/lj_bcwrite.c
    src/lj_load.c
    src/lj_ir.c
    src/lj_opt_mem.c
    src/lj_opt_fold.c
    src/lj_opt_narrow.c
    src/lj_opt_dce.c
    src/lj_opt_loop.c
    src/lj_opt_split.c
    src/lj_opt_sink.c
    src/lj_mcode.c
    src/lj_snap.c
    src/lj_record.c
    src/lj_crecord.c
    src/lj_ffrecord.c
    src/lj_asm.c
    src/lj_trace.c
    src/lj_gdbjit.c
    src/lj_ctype.c
    src/lj_cdata.c
    src/lj_cconv.c
    src/lj_ccall.c
    src/lj_ccallback.c
    src/lj_carith.c
    src/lj_clib.c
    src/lj_cparse.c
    src/lj_lib.c
    src/lj_alloc.c
    src/lib_aux.c
    src/lib_init.c
    ${LJLIB_C}
)

add_library(lua STATIC ${LJCORE_C} ${DEPS})

## LuaJIT Executable
add_executable(luac src/luajit.c)
target_link_libraries(luac lua)
if (NOT WIN32)
    target_link_libraries(luac m dl)
endif ()
