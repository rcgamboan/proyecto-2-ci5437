make
for i in 1 2 3 4
    do
    timeout 3600s stdbuf -oL ./main ${i} > outputs/algoritmo${i}.txt   
    timeout 3600s stdbuf -oL ./main ${i} true > outputs/algoritmo${i}TT.txt 
    done

