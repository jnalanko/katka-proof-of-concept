mkdir -p temp

make MEMs
make brute_MEMs

./brute_MEMs test_data/random_ref.fna test_data/random_query.fna > temp/correct.txt
./MEMs test_data/random_ref.fna test_data/random_query.fna > temp/ours.txt

head temp/*

sha1sum temp/correct.txt temp/ours.txt


