import datetime
import hashlib
import os
import sys
kernel_hash = hashlib.sha256(open("kernel.bin", 'rb').read()).hexdigest()
os.system("rm kernel/build -rf")
print("(0) branch night")
print("(1) branch stable")
print("(2) branch test")
print("(3) branch user-friendly")
branch = input("select brach: ")

name = None

if branch == '0':
    name = f"night-{kernel_hash[:16]}"
    branch = "night"
elif branch == '1':
    name = f"stable-{kernel_hash[:16]}"
    branch = "stable"
elif branch == '2':
    name = f"test-{kernel_hash[:16]}"
    branch = "test"
elif branch == '3':
    name = f"user-friendly-{kernel_hash[:16]}"
    branch = "user-friendly"
else:
    print("invalid brach")
    sys.exit(1)

os.system(f"cp grub/bosyos.iso release/{branch}/{name}")
os.system(f"cp grub/bosyos.iso release/current")
os.system(f"mv grub/bosyos.iso release/all/{name}")
os.system("rm kernel.bin kernel/kernel.b initrom kernel/kernel.elf")

import tarfile

tar_name = f"release/release.tar.gz"

with tarfile.open(tar_name, "w:gz") as tar:
    tar.add(f"release/{branch}/{name}", arcname=name+'_'+datetime.datetime.now().strftime("%d.%m.%Y_%H-%M-%S"))
    tar.add("CHANGELOG.mez", arcname="changelog.mez")
    tar.add("non-kernel files/mez", arcname="mez")

os.system(f"gh release create {name} {tar_name} --title \"kernel {name}\" --notes 'Automated build from {branch}'")