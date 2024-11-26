cmake_minimum_required(VERSION 3.22)

project(dis C)
add_executable(${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PRIVATE $<IF:$<BOOL:${MSVC}>,c_std_17,c_std_23>)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /source-charset:utf-8 /J)
  target_compile_options(${PROJECT_NAME} PRIVATE /d1trimfile:${CMAKE_CURRENT_SOURCE_DIR}\\)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -funsigned-char -O3 -Wall -Wextra -Werror -pipe)
endif()

target_sources(${PROJECT_NAME} PRIVATE
  src/analyze.c
  src/analyze2.c
  src/disasm.c
  src/ea.c
  src/eastr.c
  src/etc.c
  src/eval.c
  src/fpconv.c
  src/fpu.c
  src/generate.c
  src/getopt.c
  src/hex.c
  src/human.c
  src/include.c
  src/label.c
  src/labelcheck.c
  src/labelfile.c
  src/main.c
  src/mmu.c
  src/offset.c
  src/opstr.c
  src/option.c
  src/osk.c
  src/output.c
  src/search.c
  src/symbol.c
  src/table.c
  src/version.c
)
