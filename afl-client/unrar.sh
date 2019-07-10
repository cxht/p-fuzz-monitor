rm -rf output_unrar
./afl-fuzz -o output_unrar -b "unrar5-4-2" -u 10.0.0.16:27017 ./unrar x @@
