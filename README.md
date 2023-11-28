# UFRN - DCA

# Vítor Yeso Fidelis Freitas

# compressor-parallel
Parallel implementation of some compressing algorithms

# Running
```bash
    sudo apt-get update -y
    sudo apt-get install build-essentials python3 -y
    git clone <repo>
    make all

    pip install numpy
    python scripts/generate_normal_random_files.py

    # Compressing random_normal_2048.npy file
    ./bin/compress -a huffman -i test_files/random_normal_2048MB.npy -o compressed_files/random_normal_compressed_2048MB.npy

    # Benchmarks [TODO]
    ./bin/benchmark -a huffman 
```
