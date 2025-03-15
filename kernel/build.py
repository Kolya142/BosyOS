import os
import subprocess
import sys
import multiprocessing
from pathlib import Path

plat = 1 if sys.platform.startswith("linux") else 2

def run_command(command):
    result = os.system(command)
    if result:
        print(f'failed: {command}')
        sys.exit(1)

project_root = Path(".").resolve()
build_dir = project_root / "build"
build_dir.mkdir(exist_ok=True)

print("Build kernel")
cc = f"ccache {"gcc" if plat == 1 else "i686-elf-gcc"} \"-I{project_root}/include\" -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -m32 -mhard-float -mfpmath=387 -c" # -Wall -Wextra -c"
print(f"Build command: {cc}")
tasks = []
for root, _, files in os.walk("."):
    if "build" in root:
        continue
    for file in files:
        source_file = Path(root, file)
        output_file = Path(build_dir, (root.replace('/', '_') + '_' + file + '.o'))
        
        if (file != 'KMain.c' and root != 'kernel') and file.endswith(".c"):
            tasks.append(f"{cc} {source_file} -o {output_file}")
        if file.endswith('.s'):
            tasks.append(f"nasm -f elf32 {source_file} -o {output_file}")

# with multiprocessing.Pool(processes=multiprocessing.cpu_count()) as pool:
#     pool.map(run_command, tasks)
for t in tasks:
    run_command(t)

run_command(f"{cc} kernel/KMain.c -o build/KMain.o")
obj_files = " ".join(
    [str(file) for file in build_dir.glob("*.o") if file.name != 'KMain.o']
)

print("Link")
run_command(f"{"ld -m elf_i386" if plat == 1 else "i686-elf-ld"} -T link.elf.ld build/KMain.o {obj_files} -o kernel.elf") # --verbose")
run_command(f"{"ld -m elf_i386" if plat == 1 else "i686-elf-ld"} -T link.ld build/KMain.o {obj_files} -o kernel.b") # --verbose")


dump_result = subprocess.run(["objdump" if plat == 1 else "i686-elf-objdump", "-t", "kernel.elf"], stdout=subprocess.PIPE)
functions = []

for line in dump_result.stdout.decode().split("\n"):
    if " F " in line:
        sp = line.split()
        functions.append(f'F {sp[-1]} - 0x{sp[0]}')

with open("func-dump.txt", 'w') as f:
    f.write('\n'.join(functions))