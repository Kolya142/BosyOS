set -e

make kernel_super_real_fast
make compile
aplay non-kernel\ files/beep.wav