fdisk -l
read -p "Select drive [example sdb,sda]: " drive
read -p "Do you want rewrite drive with BosyOS(y/n)?" yn
if [ "$yn" == "y" ]; then 
    dd if=/dev/zero of='/dev/'$drive bs=1 count=33100
    dd if=drive of='/dev/'$drive bs=512
fi