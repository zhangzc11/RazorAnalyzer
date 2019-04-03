#!/bin/bash

baseDir=/mnt/hadoop/store/group/phys_susy/razor/run2/Run2RazorNtupleV4.1/MC/MC2016ReMiniAODOfficial/v1/zhicaiz/



for datasetName in \
QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
GJets_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
GJets_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
GJets_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
GJets_HT-40To100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
GJets_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
DiPhotonJetsBox_M40_80-Sherpa \
DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 

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

