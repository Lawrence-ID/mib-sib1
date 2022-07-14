3 folders 
1. executeables
    main() function, read data file and decode mib/sib1.
2. openair1
    phy code
3. cmake_targets
    cmake files and data files(input file for test)
4. asn1_decoder
    asn1 decode

HOW TO BUILD
1. use script
cd cmake_targets
./build_oai

2. cmake
cd cmake_targets
mkdir build
cd build
cmake ..
make -j32


HOW TO RUN
1. rename the data file to rxsig_2frame.dat
2. put rxsig_2frame.dat in the same folder as nr-uesoftmodem file
3. run nr-uesoftmodem.Rel15
