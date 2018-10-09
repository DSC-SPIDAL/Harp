## Make target on multiple platforms

ARCHS = knl hsw skl nec
ARCH ?= hsw

COMPILERs = icc gnu ncc
COMPILER ?= gnu

$(if $(filter $(COMPILERs),$(COMPILER)),,$(error COMPILER must be one of $(COMPILERs)))
$(if $(filter $(ARCHS),$(ARCH)),,$(error ARCH must be one of $(ARCHS)))

COMPILER_is_$(COMPILER) := yes
ARCH_is_$(ARCH)         := yes

## for openmp
-omp := $(if $(COMPILER_is_icc), -qopenmp, -fopenmp)
## for intel avx
-avx := $(if $(COMPILER_is_icc), $(if $(ARCH_is_knl), -xMIC-AVX512, -xCORE-AVX512), )
-rpt := $(if $(COMPILER_is_icc), -qopt-report=5, )
## for nec mpi version
-nccflag := $(if $(COMPILER_is_ncc), -I${NMPI_ROOT} -DSC_NEC,)
## for knl MCDRAM
-knlflag := $(if $(ARCH_is_knl), -DSC_MEMKIND -I./memkind/include -L./memkind/lib -lmemkind, )

## mpi compiler
mpi_compl=$(if $(COMPILER_is_ncc), mpinc++, $(if $(COMPILER_is_icc), mpiicc, mpicxx))


## start compile codes
CXXFLAGS_BASIC= -std=c++1y -MD -MP
CXXFLAGS_NOVEC=-Wall -O3 $(-nccflag) $(-knlflag) $(-omp)
CXXFLAGS_VEC=-Wall -O3 $(-nccflag) $(-knlflag) $(-omp) $(-avx) $(-rpt) 
-include $(SRCS:%.cpp=%.d)

SRCS = $(wildcard *.cpp)

all: prog-novec-$(ARCH)-$(COMPILER) prog-vec-$(ARCH)-$(COMPILER)

prog-novec-$(ARCH)-$(COMPILER): $(SRCS:%.cpp=%.o) 
	$(mpi_compl) $(CXXFLAGS_BASIC) $(CXXFLAGS_NOVEC) -DSC_NOVEC -o $@ $^ 
        
prog-vec-$(ARCH)-$(COMPILER): $(SRCS:%.cpp=%.o)
	$(mpi_compl) $(CXXFLAGS_BASIC) ${CXXFLAGS_VEC} -o $@ $^

clean:
	rm *.o *.optrpt

cleanall:
	rm *.o *.optrpt prog*


