rm -rf output_mp3
./afl-fuzz -o output_mp3 -d -b "mp3gain1-6-2" -u 10.0.0.16:27017 ./mp3gain @@ /dev/null
