mkdir -p temp

make MEMs
make brute_MEMs

./brute_MEMs test_data/random_ref.fna test_data/random_query.fna --lex-ranges > temp/lex_correct.txt
./MEMs test_data/random_ref.fna test_data/random_query.fna --lex-ranges > temp/lex_ours.txt

./brute_MEMs test_data/random_ref.fna test_data/random_query.fna --doc-ranges > temp/doc_correct.txt
./MEMs test_data/random_ref.fna test_data/random_query.fna --doc-ranges > temp/doc_ours.txt

head temp/lex_correct.txt temp/lex_ours.txt
head temp/doc_correct.txt temp/doc_ours.txt

sha1sum temp/lex_correct.txt temp/lex_ours.txt
sha1sum temp/doc_correct.txt temp/doc_ours.txt


