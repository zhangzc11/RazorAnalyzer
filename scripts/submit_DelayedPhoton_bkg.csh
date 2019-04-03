#!/bin/tcsh
foreach sample( \
GJets_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
GJets_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
GJets_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
GJets_HT-40To100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
GJets_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8 \
)
  echo "Sample " $sample
  set inputfilelist="/afs/cern.ch/work/z/zhicaiz/public/release/CMSSW_9_4_9/src/RazorAnalyzer/lists/Run2/razorNtuplerV4p1/MC_Summer16_reMINIAOD/${sample}.cern.txt"

  set filesPerJob = 20
  set nfiles = `cat $inputfilelist | wc | awk '{print $1}' `
  set maxjob=`python -c "print int($nfiles.0/$filesPerJob)-1"`
  mkdir -p ../submission

  foreach jobnumber(`seq 0 1 $maxjob`)
    echo "job " $jobnumber " out of " $maxjob
    bsub -q 1nh -o /afs/cern.ch/work/z/zhicaiz/public/release/CMSSW_9_4_9/src/RazorAnalyzer/submission/DelayedPhoton_${sample}_${jobnumber}.out -J DelayedPhoton_${sample}_${jobnumber} runRazorJob_CERN_EOS_zhicai.csh  DelayedPhotonAnalyzer $inputfilelist no 10 $filesPerJob $jobnumber DelayedPhoton_${sample}.Job${jobnumber}Of${maxjob}.root /eos/cms/store/group/phys_susy/razor/Run2Analysis/DelayedPhotonAnalysis/2016/V4p1_private_REMINIAOD/jobs_cmscaf1nh_withcut/
    sleep 0.1
  end

end

