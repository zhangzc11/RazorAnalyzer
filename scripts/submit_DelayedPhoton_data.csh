#!/bin/tcsh
foreach sample( \
DoubleEG_2016B_ver1_25Setp2017 \
DoubleEG_2016B_ver2_25Setp2017 \
DoubleEG_2016C_25Setp2017 \
DoubleEG_2016D_25Setp2017 \
DoubleEG_2016E_25Setp2017 \
DoubleEG_2016F_25Setp2017 \
DoubleEG_2016G_25Setp2017 \
DoubleEG_2016H_25Setp2017 \
)
  echo "Sample " $sample
  set inputfilelist="/afs/cern.ch/work/z/zhicaiz/public/release/CMSSW_9_4_9/src/RazorAnalyzer/lists/Run2/razorNtuplerV4p1/Data_2016_reMINIAOD/${sample}.cern.txt"

  set filesPerJob = 5
  set nfiles = `cat $inputfilelist | wc | awk '{print $1}' `
  set maxjob=`python -c "print int($nfiles.0/$filesPerJob)-1"`
  mkdir -p ../submission

  foreach jobnumber(`seq 0 1 $maxjob`)
    echo "job " $jobnumber " out of " $maxjob
    bsub -q 1nd -o /afs/cern.ch/work/z/zhicaiz/public/release/CMSSW_9_4_9/src/RazorAnalyzer/submission/DelayedPhoton_${sample}_${jobnumber}.out -J DelayedPhoton_${sample}_${jobnumber} runRazorJob_CERN_EOS_zhicai.csh  DelayedPhotonAnalyzer $inputfilelist yes 10 $filesPerJob $jobnumber DelayedPhoton_${sample}.Job${jobnumber}Of${maxjob}.root /eos/cms/store/group/phys_susy/razor/Run2Analysis/DelayedPhotonAnalysis/2016/V4p1_private_REMINIAOD/jobs_cmscaf1nh_withcut/
    sleep 0.1
  end

end

