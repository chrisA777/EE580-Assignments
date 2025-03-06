################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccs1271/ccs/tools/compiler/c6000_7.3.23/bin/cl6x" -mv6740 -g --define=c6748 --include_path="C:/ti/c6748/bsl/inc" --include_path="C:/Users/ringl/workspace_v12/Lab3" --include_path="C:/ti/ccs1271/ccs/tools/compiler/c6000_7.3.23/include" --include_path="C:/Users/ringl/workspace_v12/Lab3/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-947886559:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-947886559-inproc

build-947886559-inproc: ../hello.tcf
	@echo 'Building file: "$<"'
	@echo 'Invoking: TConf'
	"C:/ti/bios_5_42_01_09/xdctools/tconf" -b -Dconfig.importPath="C:/ti/bios_5_42_01_09/packages;" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

hellocfg.cmd: build-947886559 ../hello.tcf
hellocfg.s??: build-947886559
hellocfg_c.c: build-947886559
hellocfg.h: build-947886559
hellocfg.h??: build-947886559
hello.cdb: build-947886559

%.obj: ./%.s?? $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccs1271/ccs/tools/compiler/c6000_7.3.23/bin/cl6x" -mv6740 -g --define=c6748 --include_path="C:/ti/c6748/bsl/inc" --include_path="C:/Users/ringl/workspace_v12/Lab3" --include_path="C:/ti/ccs1271/ccs/tools/compiler/c6000_7.3.23/include" --include_path="C:/Users/ringl/workspace_v12/Lab3/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccs1271/ccs/tools/compiler/c6000_7.3.23/bin/cl6x" -mv6740 -g --define=c6748 --include_path="C:/ti/c6748/bsl/inc" --include_path="C:/Users/ringl/workspace_v12/Lab3" --include_path="C:/ti/ccs1271/ccs/tools/compiler/c6000_7.3.23/include" --include_path="C:/Users/ringl/workspace_v12/Lab3/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


