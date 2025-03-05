mkdir -p userdir
sudo mount -o loop ./userdata userdir
sudo chown -R sony:sony userdir
echo Mounted. Press enter to exit
read
sudo umount -l userdir
rm -rf userdir