import os
import sys

def run_command(command):
    result = os.system(command)
    if result:
        print(f'failed: {command}')
        sys.exit(1)
        os._exit(1)
        exit

build_dir = "build"
os.makedirs(build_dir, exist_ok=True)
os.system("rm build/*")

print("Build kernel")
project_root = os.path.abspath(".")
cc = f"gcc \"-I{project_root}/include\" -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -m32 -Wall -Wextra -c"
print(f"Build command: {cc}")
for root, _, files in os.walk("."):
    for file in files:
        if (file != 'KMain.c' and root != 'kernel') and file.endswith(".c"):
            source_file = os.path.join(root, file)
            output_file = os.path.join(build_dir, file + ".o")
            run_command(f"{cc} {source_file} -o {output_file}")
        if file.endswith('.s'):
            source_file = os.path.join(root, file)
            output_file = os.path.join(build_dir, file + ".o")
            run_command(f"nasm -elf {source_file} {output_file}")
run_command(f"{cc} kernel/KMain.c -o build/KMain.o")
obj_files = " ".join(
    [os.path.join(build_dir, file) for file in os.listdir(build_dir) if file.endswith(".o") and file != 'KMain.o']
)

print("Link") # evil ld bug fixing      VVVVVVVVVVVVV
run_command(f"ld -m elf_i386 -T link.ld build/KMain.o {obj_files} -o kernel.b --verbose")
