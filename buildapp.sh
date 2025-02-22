cd non-kernel\ files/
set -e
python3 bosybuild.py exampleapp.asm N
python3 file_to_c.py exampleapp.bsf