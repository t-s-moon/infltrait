
#file soft_tfidf.dbg
#b main
#r de.lower dump.txt
#r /corpora/data/en-de/001000/de.lower dump.txt

#file jw.dbg
#b main
#r

#file pm.dbg
#b main
#r ed.actual.ti.final de.actual.ti.final de.trn.trg.vcb alignments.mp

#file ss.dbg
#b main
#r de.verbs probs dump.txt 1

#file na.dbg
#b main
#r data/de.verbs

#file cl.dbg
#b main
#r data/ed.1000.final data/de.1000.final data/de.1000.dict data/de.1000.stopwords cluster.txt ../data/en-de/gmw.cd

# file ptest.dbg
# b main
# r words

#file cl.dbg
#b main
#r -t ../data/008000/mkcls_lemma_pos/de.actual.ti.final -s ../data/008000/mkcls_lemma_pos/ed.actual.ti.final -p ../data/008000/mkcls_lemma_pos/pos_dict -c ../data/celex_table -i 0.98

# file cl_baseline.dbg
# b main
# r -p ../data/001000/mkcls_lemma_pos/pos_dict -c ../data/celex_table # -o baseline_cluster

file driver.dbg
b main
#r -f 2 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_ds_0_dc
#r  -f 4 -d  ../data/uspanteko  -c ../data/gold/umw.cd -o ../results/usp_suf_ds_0_dc -e
#r  -f 4 -d  ../data/uspanteko/fuck  -c ../data/gold/umw.cd -o ../results/usp_suf_ds_0_dc -e
#r  -f 4 -d  ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_suf_gs_0_gc -e -m 0
#r  -d  ../data/nyt/smaller  -c ../data/gold/emw.cd -o ../results/mini_ds_0_dc
#r  -d  ../data/n2  -c ../data/gold/emw.cd -o ../results/mini_gs_0_gc
#r  -f 2 -d  ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_suf_gs_0_gc -e -m 0 -p
#r  -f 3 -d  ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_pre_gs_0_gc -e
#r   -f 2 -d  ../data/uspanteko/all  -c ../data/gold/umw.cd -o ../results/usp_gs_0_gc -e -p
#r  -d  ../data/nyt/smaller  -c ../data/emw.cd -o trash -k 1
#r -e  -d ../data/uspanteko  -c ../data/gold/umw.cd -o trash -k 2 -f 4 >> usp_dump
#r  -e -d ../data/uspanteko  -c ../data/gold/umw.cd -o trash -k 2 -f 3 >> usp_dump
r  -d  ../data/n1sorted  -c ../data/gold/emw.cd -o trash -k 3