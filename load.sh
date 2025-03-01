fdisk -l
read -p "Select drive [example sdb,sda]: " drive
read -p "Do you want rewrite drive with BosyOS(y/n)?" yn
if [ "$yn" == "y" ]; then 
    dd if=/dev/zero of='/dev/'$drive bs=512 count=211
    dd if=BosyOS.img of='/dev/'$drive bs=512
fi