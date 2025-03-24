fdisk -l
read -p "Select drive [example sdb,sda]: " drive
read -p "Do you want rewrite drive with BosyOS(y/n)?" yn
if [ "$yn" == "y" ]; then 
    dd if=grub/bosyos.iso of='/dev/'$drive bs=512 status=progress && sync
fi