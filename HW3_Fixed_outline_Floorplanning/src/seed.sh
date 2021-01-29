#!/bin/bash
make clean
make
echo choose input file : 
read  filename
echo dead space ratio : 
read  dsr
if [[ "${filename}" == "1" ]]; then
    for round in {1..10}
    do
        ../bin/hw3 ../testcase/n100.hardblocks ../testcase/n100.nets ../testcase/n100.pl "${dsr}"
    done    
elif [[ "${filename}" == "2" ]]; then
    for round in {1..10}
    do
        ../bin/hw3 ../testcase/n200.hardblocks ../testcase/n200.nets ../testcase/n200.pl "${dsr}"
    done
elif [[ "${filename}" == "3" ]]; then
    for round in {1..10}
    do
        ../bin/hw3 ../testcase/n300.hardblocks ../testcase/n300.nets ../testcase/n300.pl "${dsr}" 
    done
fi