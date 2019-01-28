all:
	sed 's/ //g' xtea_test_vec.txt > test_vec0.txt
	tr -d '\n' < test_vec0.txt > test_vec1.txt
	hexdump -v -e '"0x" 8 "%c" ", "' test_vec1.txt > test_vec2.txt
	hexdump -v -e '"{" 96 "%c" "aaa" "},\n"' test_vec2.txt > test_vec3.txt
	sed 's/, aaa//g' test_vec3.txt > test_vec4.txt
clean:
	rm test_vec0.txt test_vec1.txt test_vec2.txt test_vec3.txt test_vec4.txt
