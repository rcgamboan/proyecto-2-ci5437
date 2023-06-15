make
for i in 1 2 3 4
do
    ./main ${i} > algoritmo${i}.txt
    ./main ${i} true > algoritmo${i}TT.txt
done
