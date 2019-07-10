rm -rf output_jhead
./afl-fuzz -o output_jhead -b jhead3-03 -u 10.0.0.16:27017 ./jhead  @@
