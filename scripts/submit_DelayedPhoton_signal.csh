#!/bin/tcsh
foreach sample( \
GMSB_L100TeV_Ctau1000cm_13TeV-pythia8 \
GMSB_L100TeV_Ctau10cm_13TeV-pythia8 \
GMSB_L100TeV_Ctau1200cm_13TeV-pythia8 \
GMSB_L100TeV_Ctau20000cm_13TeV-pythia8 \
GMSB_L100TeV_Ctau4000cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau1000cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau100cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau10cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau1200cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau20000cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau200cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau4000cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau400cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau50cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau5cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau600cm_13TeV-pythia8 \
GMSB_L150TeV_Ctau800cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau0p01cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau0p1cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau1000cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau100cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau10cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau1200cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau20000cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau200cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau400cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau50cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau5cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau600cm_13TeV-pythia8 \
GMSB_L200TeV_Ctau800cm_13TeV-pythia8 \
GMSB_L250TeV_Ctau0p01cm_13TeV-pythia8 \
GMSB_L250TeV_Ctau0p1cm_13TeV-pythia8 \
GMSB_L250TeV_Ctau100cm_13TeV-pythia8 \
GMSB_L250TeV_Ctau10cm_13TeV-pythia8 \
GMSB_L250TeV_Ctau200cm_13TeV-pythia8 \
GMSB_L250TeV_Ctau400cm_13TeV-pythia8 \
GMSB_L250TeV_Ctau50cm_13TeV-pythia8 \
GMSB_L250TeV_Ctau5cm_13TeV-pythia8 \
GMSB_L250TeV_Ctau600cm_13TeV-pythia8 \
GMSB_L300TeV_Ctau0p01cm_13TeV-pythia8 \
GMSB_L300TeV_Ctau0p1cm_13TeV-pythia8 \
GMSB_L300TeV_Ctau100cm_13TeV-pythia8 \
GMSB_L300TeV_Ctau10cm_13TeV-pythia8 \
GMSB_L300TeV_Ctau50cm_13TeV-pythia8 \
GMSB_L300TeV_Ctau5cm_13TeV-pythia8 \
GMSB_L300TeV_Ctau600cm_13TeV-pythia8 \
GMSB_L350TeV_Ctau0p1cm_13TeV-pythia8 \
GMSB_L350TeV_Ctau200cm_13TeV-pythia8 \
GMSB_L400TeV_Ctau0p01cm_13TeV-pythia8 \
GMSB_L400TeV_Ctau0p1cm_13TeV-pythia8 \
GMSB_L400TeV_Ctau10cm_13TeV-pythia8 \
GMSB_L400TeV_Ctau800cm_13TeV-pythia8 \
)
  echo "Sample " $sample
  set inputfilelist="/afs/cern.ch/work/z/zhicaiz/public/release/CMSSW_9_4_9/src/RazorAnalyzer/lists/Run2/razorNtuplerV4p1/MC_Summer16_reMINIAOD/${sample}.cern.txt"

  set filesPerJob = 15
  set nfiles = `cat $inputfilelist | wc | awk '{print $1}' `
  set maxjob=`python -c "print int($nfiles.0/$filesPerJob)-1"`
  mkdir -p ../submission

  foreach jobnumber(`seq 0 1 $maxjob`)
    echo "job " $jobnumber " out of " $maxjob
    bsub -q 1nh -o /afs/cern.ch/work/z/zhicaiz/public/release/CMSSW_9_4_9/src/RazorAnalyzer/submission/DelayedPhoton_${sample}_${jobnumber}.out -J DelayedPhoton_${sample}_${jobnumber} runRazorJob_CERN_EOS_zhicai.csh  DelayedPhotonAnalyzer $inputfilelist no 10 $filesPerJob $jobnumber DelayedPhoton_${sample}.Job${jobnumber}Of${maxjob}.root /eos/cms/store/group/phys_susy/razor/Run2Analysis/DelayedPhotonAnalysis/2016/V4p1_private_REMINIAOD/jobs_cmscaf1nh_withcut/
    sleep 0.1
  end

end

