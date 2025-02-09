fdisk -l
read -p "Select drive [example sdb,sda]: " drive
read -p "Do you want rewrite drive with BosyOS" yn
if [ "$yn" == "y" ]; then 
    dd if=drive of=/dev/$drive bs=512
fi