.PHONY: x, clean
.SUFFIXES:


## ---------------------------------------------------------------------
## configurable parameters
##

target_base    := mapviewer
units          := MapViewer
opt_dbg_flag   := -O3
exit_first     := -Wfatal-errors
warn_all       := -Wall
cppver         := -std=c++11

## end configurable parameters
## -----------------------------------------------------------------------------

target     := $(target_base)_exe
units_cpp  := $(addsuffix .cpp, $(units))
units_o    := $(addsuffix .o, $(units))
headers    := $(wildcard *.h)

uname:=$(shell uname -s)

ifeq ($(uname),Darwin)
   os           := OSX
   lib_gl       := -framework OpenGL
   lib_aux      :=
   comp         := clang++ 
   lib_glfw     := -lglfw
   lib_atb      := -lAntTweakBar
   lib_path_cmd :=
   glfw_include :=
   ## lib_path_cmd := export DYLD_LIBRARY_PATH=$(atb_lib_dir) ;
   ## extra_inc_dir := -I /opt/local/include
else
   os           := LINUX
   lib_gl       := -lGL
   lib_aux      := -lGLEW $(lib_glu)
   comp         := g++
	atb_dir      := $(HOME)/svn-ct/cuarepo.svn/src/pruebas-anttweakbar/AntTweakBar_116
   lib_atb      := -L$(atb_dir)/lib -lAntTweakBar
   atb_include  := -I$(atb_dir)/include
   lib_glfw     := -lglfw
   lib_path_cmd := export LD_LIBRARY_PATH=$(atb_dir)/lib ;
endif

## linker flags
ld_flags  := $(lib_dir_loc)
ld_libs   := $(lib_atb) $(lib_aux) $(lib_glfw) $(lib_gl)

## compiler flags
c_flags  := $(cppver) -I. $(atb_include) -D$(os) $(opt_dbg_flag) $(exit_first) $(warn_all)

# execute target
x: $(target)
	 $(lib_path_cmd) ./$<

## remove intermediate files
clean:
	rm -f *.o *_exe pol.h *.blob *.zip

## create executable target (link)
$(target): $(units_o)  makefile
	$(comp) $(ld_flags) -o $@  $(units_o) $(ld_libs)

## compile an unit file
%.o : %.cpp $(headers) makefile
	$(comp) -c $(c_flags) $<

## create a zip with source files:
zip:
	rm -f source-code.zip
	zip source-code.zip *.h* *.c* makefile README.md
