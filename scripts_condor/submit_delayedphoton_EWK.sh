#!/bin/sh

mkdir -p log
mkdir -p submit

cd ../
RazorAnalyzerDir=`pwd`
cd -

job_script=${RazorAnalyzerDir}/scripts_condor/runRazorJob_CaltechT2.sh
filesPerJob=1

for sample in \
DiPhotonJetsBox_M40_80-Sherpa \
DiPhotonJetsBox_MGG-80toInf_13TeV-Sherpa \
TGJets_TuneCUETP8M1_13TeV_amcatnlo_madspin_pythia8 \
TGGJets_leptonDecays_13TeV_MadGraph_madspin_pythia8 \
TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8 \
ZGGJets_ZToHadOrNu_5f_LO_madgraph_pythia8 \
WZG_TuneCUETP8M1_13TeV-amcatnlo-pythia8 \
WGGJets_TuneCUETP8M1_13TeV_madgraphMLM_pythia8 \
WWG_TuneCUETP8M1_13TeV-amcatnlo-pythia8  \
TTJets_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8 \
WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8  \
WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8  \
WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
WJetsToLNu_HT-70To100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 


do
	echo "Sample " ${sample}
	inputfilelist=/src/RazorAnalyzer/lists/Run2/razorNtuplerV4p1/MC_Summer16_MINIAODv2/${sample}.caltech.txt
	nfiles=`cat ${CMSSW_BASE}$inputfilelist | wc | awk '{print $1}' `
	maxjob=`python -c "print int($nfiles.0/$filesPerJob)-1"`
	analyzer=DelayedPhotonAnalyzer
	rm submit/${analyzer}_${sample}_Job*.jdl
	rm log/${analyzer}_${sample}_Job*

	for jobnumber in `seq 0 1 ${maxjob}`
	do
		echo "job " ${jobnumber} " out of " ${maxjob}
		jdl_file=submit/${analyzer}_${sample}_Job${jobnumber}_Of_${maxjob}.jdl
		echo "Universe = vanilla" > ${jdl_file}
		echo "Executable = ${job_script}" >> ${jdl_file}
		echo "Arguments = ${analyzer}_${sample}_Job${jobnumber}_Of_${maxjob} /store/group/phys_susy/razor/Run2Analysis/DelayedPhotonAnalysis/2016/orderByPt/jobs/ ${analyzer} ${inputfilelist} no 10 ${filesPerJob} ${jobnumber} ${sample}_Job${jobnumber}_Of_${maxjob}.root" >> ${jdl_file}
		echo "Log = log/${analyzer}_${sample}_Job${jobnumber}_Of_${maxjob}_PC.log" >> ${jdl_file}
		echo "Output = log/${analyzer}_${sample}_Job${jobnumber}_Of_${maxjob}_\$(Cluster).\$(Process).out" >> ${jdl_file}
		echo "Error = log/${analyzer}_${sample}_Job${jobnumber}_Of_${maxjob}_\$(Cluster).\$(Process).err" >> ${jdl_file}
		echo 'Requirements=TARGET.OpSysAndVer=="CentOS7"' >> ${jdl_file}
		echo "should_transfer_files = YES" >> ${jdl_file}
		echo "RequestMemory = 2000" >> ${jdl_file}
		echo "RequestCpus = 1" >> ${jdl_file}
		echo "when_to_transfer_output = ON_EXIT" >> ${jdl_file}
		echo "Queue 1" >> ${jdl_file}
		echo "condor_submit submit/${analyzer}_${sample}_Job${jobnumber}_Of_${maxjob}.jdl"
		condor_submit submit/${analyzer}_${sample}_Job${jobnumber}_Of_${maxjob}.jdl
	done
done

