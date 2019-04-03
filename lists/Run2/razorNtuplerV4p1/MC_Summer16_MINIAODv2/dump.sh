#!/bin/bash

baseDir=/mnt/hadoop/store/group/phys_susy/razor/run2/Run2RazorNtupleV4.1/MC/MC2016MiniAODv2/v1/zhicaiz/



for datasetName in TGJets_TuneCUETP8M1_13TeV_amcatnlo_madspin_pythia8 \
TGGJets_leptonDecays_13TeV_MadGraph_madspin_pythia8 \
TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8 \
ZGGJets_ZToHadOrNu_5f_LO_madgraph_pythia8 \
WZG_TuneCUETP8M1_13TeV-amcatnlo-pythia8 \
WGGJets_TuneCUETP8M1_13TeV_madgraphMLM_pythia8 \
WWG_TuneCUETP8M1_13TeV-amcatnlo-pythia8 \
DiPhotonJetsBox_M40_80-Sherpa \
DiPhotonJetsBox_MGG-80toInf_13TeV-Sherpa 

do
	subBase0=${baseDir}${datasetName}/

	rm ${datasetName}.caltech.txt

	for sub0 in `ls $subBase0 | grep 25Mar2019`
	do
		subBase1=${subBase0}${sub0}/
		echo ${subBase1}
		for sub1 in `ls ${subBase1}`
		do
			subBase2=${subBase1}${sub1}/
			echo ${subBase2}
			for sub2 in `ls ${subBase2}`
			do
				subBase3=${subBase2}${sub2}/
				echo ${subBase3}
				for ifile in `ls ${subBase3} | grep root`
				do
					echo "${subBase3}${ifile}" >> ${datasetName}.caltech.txt
				done
			done
		done
	done
done

#TTJets

datasetName=TTJets_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8

subBase0=${baseDir}${datasetName}/

rm ${datasetName}.caltech.txt

for sub0 in `ls $subBase0 | grep 25Mar2019`
do
	subBase1=${subBase0}${sub0}/
        echo ${subBase1}
	for sub1 in `ls ${subBase1}`
        do
		subBase2=${subBase1}${sub1}/
                echo ${subBase2}
                for sub2 in `ls ${subBase2}`
                do
			subBase3=${subBase2}${sub2}/
                        echo ${subBase3}
                        for ifile in `ls ${subBase3} | grep root`
                        do
				echo "${subBase3}${ifile}" >> ${datasetName}.caltech.txt
			done
		done
	done
done

#WJets

for HT in 70To100 100To200 200To400 400To600 600To800 800To1200 1200To2500 2500ToInf
do
	datasetName=WJetsToLNu_HT-${HT}_TuneCUETP8M1_13TeV-madgraphMLM-pythia8
	
	subBase0=${baseDir}${datasetName}/

	rm ${datasetName}.caltech.txt

	for sub0 in `ls $subBase0 | grep 25Mar2019`
	do
		subBase1=${subBase0}${sub0}/
		echo ${subBase1}
		for sub1 in `ls ${subBase1}`
		do
			subBase2=${subBase1}${sub1}/
			echo ${subBase2}
			for sub2 in `ls ${subBase2}`
			do
				subBase3=${subBase2}${sub2}/
				echo ${subBase3}
				for ifile in `ls ${subBase3} | grep root`
				do
					echo "${subBase3}${ifile}" >> ${datasetName}.caltech.txt
				done
			done
		done
	done
done
