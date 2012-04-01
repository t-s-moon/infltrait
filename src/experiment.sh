#! /bin/bash -x

## output terms: CORPUS_BTYPE
## CORPUS = mini/full: which version of nyt was used
## BTYPE = as/aio: baseline type, as=all singletons, aio=all in one

## experiments for baseline
# ./driver.dbg -d  ../data/nyt/smaller  -c ../data/emw.cd -o ../results/mini_as -l 1
# ./driver.dbg -d  ../data/nyt/smaller  -c ../data/emw.cd -o ../results/mini_aio -l 2

# ./driver.dbg -d  ../data/nyt  -c ../data/emw.cd -o ../results/full_as -l 1
# ./driver.dbg -d  ../data/nyt  -c ../data/emw.cd -o ../results/full_aio -l 2

# Baseline experiments for first k overlapping strings

# for i in {1..30}; do
#     ./driver.dbg -d  ../data/nyt/smaller  -c ../data/gold/emw.cd -o trash -k $i
# done
# for i in {1..20}; do
#     ./driver.dbg -d  ../data/nyt  -c ../data/gold/emw.cd -o trash -k $i | tee nyt_dump >> nyt_dump
# done
# echo > nyt_dump
# for i in {1..20}; do
#     ./driver.dbg -d  ../data/n1sorted  -c ../data/gold/emw.cd -o trash -k $i >> nyt_dump
# done

# for i in {1..20}; do
#     ./driver.dbg -e -d ../data/uspanteko  -c ../data/gold/umw.cd -o trash -k $i -f 3 >> usp_dump
# done
#./driver.dbg -d  ../data/nyt/smaller  -c ../data/emw.cd -o ../results/mini_k_3 -k 3
# ./driver.dbg -d  ../data/nyt/smaller  -c ../data/emw.cd -o ../results/mini_k_4 -k 4
# ./driver.dbg -d  ../data/nyt/smaller  -c ../data/emw.cd -o ../results/mini_k_5 -k 5
# ./driver.dbg -d  ../data/nyt/smaller  -c ../data/emw.cd -o ../results/mini_k_6 -k 6

# ./driver.dbg -d  ../data/nyt  -c ../data/emw.cd -o ../results/full_k_3 -k 3
# ./driver.dbg -d  ../data/nyt  -c ../data/emw.cd -o ../results/full_k_4 -k 4
# ./driver.dbg -d  ../data/nyt  -c ../data/emw.cd -o ../results/full_k_5 -k 5
# ./driver.dbg -d  ../data/nyt  -c ../data/emw.cd -o ../results/full_k_6 -k 6

## output terms: CORPUS_SS_X2_CS
## CORPUS = mini/full: which version of nyt was used
## SS = ds/gs: search strategy. document boundary aware (ds) and global (gs)
## X2 = 0/1/2: chi square threshold, 0=95%, 1=99%, 2=99.8%
## CS = dc/gc: clustering strategy. document boundary aware (dc) and boundary agnostic (gc)

# ## experiments for mini dataset
# ./driver.dbg -d  ../data/nyt/smaller  -c ../data/emw.cd -o ../results/mini_ds_0_gc -g
# ./driver.dbg -d  ../data/nyt/smaller  -c ../data/emw.cd -o ../results/mini_ds_0_dc
# ./driver.dbg -d  ../data/n2  -c ../data/emw.cd -o ../results/mini_gs_0_gc
# ./driver.dbg -d ../data/nyt/smaller -b ../data/n2  -c ../data/emw.cd -o ../results/mini_gs_0_dc

# ## experiments for full dataset
# ./driver.dbg -d  ../data/nyt  -c ../data/emw.cd -o ../results/full_ds_0_gc -g
# ./driver.dbg -d  ../data/nyt  -c ../data/emw.cd -o ../results/full_ds_0_dc
# ./driver.dbg -d  ../data/n1  -c ../data/emw.cd -o ../results/full_gs_0_gc
# ./driver.dbg -d ../data/nyt -b ../data/n1  -c ../data/emw.cd -o ../results/full_gs_0_dc

# ## experiments for mini dataset with strengthened significance
# ./driver.dbg -d  ../data/n2  -c ../data/emw.cd -o ../results/mini_gs_2_gc -t 2
# ./driver.dbg -d ../data/nyt/smaller -b ../data/n2  -c ../data/emw.cd -o ../results/mini_gs_2_dc -t 2
# ./driver.dbg -d  ../data/nyt/smaller  -c ../data/emw.cd -o ../results/mini_ds_2_dc -t 2
# ./driver.dbg -d  ../data/nyt/smaller  -c ../data/emw.cd -o ../results/mini_ds_2_gc -g -t 2

# ## experiments for full dataset with strengthened significance
# ./driver.dbg -d  ../data/n1  -c ../data/emw.cd -o ../results/full_gs_2_gc -t 2
# ./driver.dbg -d ../data/nyt -b ../data/n1  -c ../data/emw.cd -o ../results/full_gs_2_dc -t 2
# ./driver.dbg -d  ../data/nyt  -c ../data/emw.cd -o ../results/full_ds_2_dc -t 2
# ./driver.dbg -d  ../data/nyt  -c ../data/emw.cd -o ../results/full_ds_2_gc -g -t 2

# ## experiments for uspanteko
# ./driver.dbg -f 2 -d  ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_gs_0_gc
# ./driver.dbg -f 2 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_ds_0_dc

# ## experiments for uspanteko with strengthened significance
# ./driver.dbg -f 2 -d  ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_gs_2_gc -t 2
# ./driver.dbg -f 2 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_ds_2_dc -t 2

# ## experiments for uspanteko prefixes
#./driver.dbg -f 3 -d  ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_pre_gs_0_gc -e
#./driver.dbg -f 3 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_pre_ds_0_dc -e
#./driver.dbg -f 3 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_pre_ds_0_dc -e
./driver.dbg -f 3 -d  ../data/uspanteko -b ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_pre_gs_0_dc -e
./driver.dbg -f 3 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_pre_ds_0_gc -g -e

## experiments for uspanteko suffixes
#./driver.dbg -f 4 -d  ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_suf_gs_0_gc -e # -m 0
#./driver.dbg -f 4 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_suf_ds_0_dc -e  #-m 0
# ./driver.dbg -f 4 -d  ../data/uspanteko -b ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_suf_gs_0_dc -e -m 0
# ./driver.dbg -f 4 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_suf_ds_0_gc -g -e -m 0

## experiments for uspanteko with pref/suffix
#./driver.dbg  -f 2 -d  ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_gs_0_gc -e -p
#./driver.dbg  -f 2 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_ds_0_dc -e -p
# ./driver.dbg -f 4 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_suf_ds_0_dc -e -m 0
# ./driver.dbg -f 4 -d  ../data/uspanteko -b ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_suf_gs_0_dc -e -m 0
# ./driver.dbg -f 4 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_suf_ds_0_gc -g -e -m 0
