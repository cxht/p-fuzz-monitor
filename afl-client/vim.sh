rm -rf output_unrar
./afl-fuzz -o output_vim -b "vim8-1-1622" -u 10.0.0.16:27017 ./vim  @@
