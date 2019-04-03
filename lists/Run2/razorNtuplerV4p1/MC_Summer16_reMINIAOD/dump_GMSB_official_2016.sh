#!/bin/bash

baseDir=/mnt/hadoop/store/group/phys_susy/razor/run2/Run2RazorNtupleV4.1/MC/MC2016ReMiniAODOfficial/v1/zhicaiz/

for Lambda in 100 150 200 250 300 350 400
do
	for CTau in 0_1 10 200 400 600 800 1000 1200 
	do
		echo 'xxxxxxxxxx' > GMSB_L${Lambda}TeV_Ctau${CTau}cm_13TeV-pythia8.caltech.txt
		subBase1=${baseDir}GMSB_L-${Lambda}TeV_Ctau-${CTau}cm_13TeV-pythia8/crab_prod_Run2RazorNtuplerV4p1_MC2016_GMSB_L${Lambda}TeV_Ctau-${CTau}cm_25Mar2019/

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
					echo "${subBase3}${ifile}" >> GMSB_L${Lambda}TeV_Ctau${CTau}cm_13TeV-pythia8.caltech.txt
				done
			done
		done
		sed -i '/xxxxxxxxxx/d' GMSB_L${Lambda}TeV_Ctau${CTau}cm_13TeV-pythia8.caltech.txt
		
		if [ -s GMSB_L${Lambda}TeV_Ctau${CTau}cm_13TeV-pythia8.caltech.txt ]
                then
			#rm GMSB_L${Lambda}TeV_Ctau${CTau}cm_13TeV-pythia8.caltech.txt
			echo "GMSB_L${Lambda}TeV_Ctau${CTau}cm_13TeV-pythia8.caltech.txt is generated"
		else
			
			rm GMSB_L${Lambda}TeV_Ctau${CTau}cm_13TeV-pythia8.caltech.txt
		fi

	done
done
