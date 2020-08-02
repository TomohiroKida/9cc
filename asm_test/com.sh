asm=$1
gcc -o asm $asm
./asm
echo $?
