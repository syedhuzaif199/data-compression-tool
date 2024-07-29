# data-compression-tool

A tool for compressing data files using Huffman coding and Lempel-Ziv-Welch algorithm.

## How to use

1. Compile huff_encode.c and huff_decode.c

2. To compress a file (say abc.txt), write the following command in the terminal

```
./huff_encode abc.txt
```

or simply run `huff_encode` and the program will ask for a file name.

3. To decompress, run

```
./huff_decode abc.txt.huff
```

or simply run `huff_decode` and the program will ask for a file name.
