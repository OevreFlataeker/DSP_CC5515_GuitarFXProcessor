################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
src/%.obj: ../src/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C5500 Compiler'
	"D:/ti/ccs1010/ccs/tools/compiler/c5500_4.4.1/bin/cl55" -v5505 --memory_model=huge -g --include_path="D:/ti/ccs1010/ccs/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/daubsi/workspace_v10/DemoApp1/include" --define=vc5505 --diag_warning=225 --sat_reassoc=off --ptrdiff_size=32 --fp_reassoc=off --asm_source=mnemonic --preproc_with_compile --preproc_dependency="src/$(basename $(<F)).d_raw" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


