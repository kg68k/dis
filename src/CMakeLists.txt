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
  analyze.c
  analyze2.c
  disasm.c
  ea.c
  eastr.c
  etc.c
  eval.c
  fpconv.c
  fpu.c
  generate.c
  getopt.c
  hex.c
  human.c
  include.c
  label.c
  labelcheck.c
  labelfile.c
  main.c
  mmu.c
  offset.c
  opstr.c
  option.c
  osk.c
  output.c
  search.c
  symbol.c
  table.c
  version.c
)
