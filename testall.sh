
#make -j clean all

pushd test/TestCdma
rm *.out
rm *.diff
rm *.tick.txt
for v in {1..2}
do
    if [ $v = 2 ]
    then
        opts='-f -o'
    fi

    for t in {1..6}
    do
        ../../bin/TestCdma $opts < test$t.txt > /dev/null
        for s in {2..5}
        do
            mv MobileStation.$s.tick.txt MobileStation.$t.$s.tick.$v.txt
            diff *Station.$s.in *Station.$s.out > $t.$s.diff
        done
    done
done
ls -l *.diff
tail *.tick.*
popd
