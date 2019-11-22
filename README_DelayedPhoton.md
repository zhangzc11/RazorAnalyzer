RazorAnalyzer for Delayed photon analysis (2016)
=============

Setup
-------------

    cmsrel CMSSW_9_4_9
    cd CMSSW_9_4_9/src
    git clone -b 94X_DelayedPhoton_2016 git@github.com:zhangzc11/RazorAnalyzer.git
    cd RazorAnalyzer
    make

Run
-------------

    ./RazorRun_T2 <list of input files> <name of your analyzer> <options>    

Example: to process the DY sample, do:
	
     ./RazorRun_T2 lists/Run2/razorNtuplerV4p1/MC_Summer16_reMINIAOD/YJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.caltech.txt DelayedPhotonAnalyzer -d=no -f=DelayedPhoton_DY.root

(we use V4p1 razorNtupler for 2016 paper; they are all in: lists/Run2/razorNtuplerV4p1/)

submit condor jobs to run all samples 
-------------

     cd scripts_condor
     source submit_delayedphoton_signal.sh

(might need to update the scripts since Caltech T2 condor configs keep changing...)


The razoranalyzer ntupler we use for the 2016 paper are in: /mnt/hadoop/store/group/phys_susy/razor/Run2Analysis/DelayedPhotonAnalysis/2016/orderByPt/withcut

