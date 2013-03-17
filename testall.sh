#make -j clean all

pushd test/TestCdma
rm *.out
rm *.diff
rm *.tick.txt
for t in {1..6}
do
../../bin/TestCdma < test$t.txt
for s in {2..4}
do
diff *Station.$s.in *Station.$s.out > $t.$s.diff
done
done
ls -l *.diff
popd

