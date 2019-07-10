rm -rf output_sam2p
./afl-fuzz -o output_sam2p -d  -b "sam2p0-49-4" -u 10.0.0.16:27017 ./sam2p @@ EPS: /dev/null
