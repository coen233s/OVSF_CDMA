
#make -j clean all

pushd test/TestCdma
rm *.out
rm *.diff*
rm *.tick.*
mode=variable_dynamic
for v in {1..2}
do
    if [ $v = 2 ]
    then
        opts='-f -o'
        mode=fixed_once
    fi

    for t in {1..6}
    do
        echo "../../bin/TestCdma $opts < test$t.txt"
        ../../bin/TestCdma $opts < test$t.txt > /dev/null
        for s in {2..5}
        do
            [ -f MobileStation.$s.tick.txt ] && mv MobileStation.$s.tick.txt MobileStation.$t.$s.tick.$mode.txt
            [ -f *Station.$s.out ] && diff *Station.$s.in *Station.$s.out > $t.$s.$mode.diff
        done
    done
done
ls -l *.diff
tail *.tick.*
echo ""
popd
