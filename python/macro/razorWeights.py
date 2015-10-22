##Weight and trigger utilities for inclusive razor analysis

import ROOT as rt
import sys

#local imports
import macro, razorAnalysis

#####################################
### WEIGHT AND TRIGGER INFO
#####################################

def passTrigger(event, triggerNumList):
    """Checks if the event passed any trigger in the list"""
    passes = False
    for i in triggerNumList:
        if event.HLTDecision[i]:
            passes = True
            break
    return passes

def passSingleLeptonTrigger(event, isData=False, debugLevel=0):
    if isData:
        trigNums = singleLeptonTriggerNumsData
    else:
        trigNums = singleLeptonTriggerNumsMC
    if debugLevel > 1: print("Event passes single lepton trigger")
    return passTrigger(event, trigNums)

def passDileptonTrigger(event, isData=False, debugLevel=0):
    if debugLevel > 1: print("Note: dilepton trigger requirement is a pass-through right now")
    return True

def passHadronicTrigger(event, isData=False, debugLevel=0):
    if debugLevel > 1: print("Note: hadronic trigger requirement is a pass-through right now")
    return True

def pileupWeight(event, wHists, puBranch="NPV", debugLevel=0):
    if "pileup" in wHists:
        if not hasattr(event, puBranch):
            print "Error in pileupWeight: tree does not have a branch for",puBranch
            sys.exit()
        pileupWeight = wHists["pileup"].GetBinContent(wHists["pileup"].GetXaxis().FindFixBin(getattr(event, puBranch)))
        if debugLevel > 1: print puBranch," weight:",pileupWeight,"(",puBranch,"=",getattr(event, puBranch),")"
        return pileupWeight
    else:
        print "Error in pileupWeight: pileup reweighting histogram not found!"
        sys.exit()

def leptonWeight(event, wHists, doLep2=False, debugLevel=0):
    """Weights using leading lepton pt and eta.  If doLep2 is True, weights using subleading lepton too."""
    #check for histograms
    if not ("ele" in wHists):
        print "Error in leptonWeight: electron scale factor histogram not found!"
        sys.exit()
    if not ("muon" in wHists):
        print "Error in leptonWeight: muon scale factor histogram not found!"
        sys.exit()

    leptonWeight = 1.0
    #leading muon 
    if abs(event.lep1Type) == 13 and event.lep1.Pt() > 0:
        leptonWeight *= wHists["muon"].GetBinContent(
                wHists["muon"].GetXaxis().FindFixBin(max(min(event.lep1.Pt(), 199.9),15.01)),
                #wHists["muon"].GetXaxis().FindFixBin(min(event.lep1.Pt(), wHists["muon"].GetXaxis().GetXmax()-1.0)),
                wHists["muon"].GetYaxis().FindFixBin(abs(event.lep1.Eta())))
    #leading electron
    elif abs(event.lep1Type) == 11 and event.lep1.Pt() > 0:
        leptonWeight *= wHists["ele"].GetBinContent(
                wHists["ele"].GetXaxis().FindFixBin(max(min(event.lep1.Pt(), 199.9),15.01)),
                #wHists["ele"].GetXaxis().FindFixBin(min(event.lep1.Pt(), wHists["ele"].GetXaxis().GetXmax()-1.0)),
                wHists["ele"].GetYaxis().FindFixBin(abs(event.lep1.Eta())))
    if doLep2:
        #subleading muon 
        if abs(event.lep2Type) == 13 and event.lep2.Pt() > 0:
            leptonWeight *= wHists["muon"].GetBinContent(
                    wHists["muon"].GetXaxis().FindFixBin(max(min(event.lep2.Pt(), 199.9),15.01)),
                    #wHists["muon"].GetXaxis().FindFixBin(min(event.lep2.Pt(), wHists["muon"].GetXaxis().GetXmax()-1.0)),
                    wHists["muon"].GetYaxis().FindFixBin(abs(event.lep2.Eta())))
        #subleading electron
        elif abs(event.lep2Type) == 11 and event.lep2.Pt() > 0:
            leptonWeight *= wHists["ele"].GetBinContent(
                    wHists["ele"].GetXaxis().FindFixBin(max(min(event.lep2.Pt(), 199.9),15.01)),
                    #wHists["ele"].GetXaxis().FindFixBin(min(event.lep2.Pt(), wHists["ele"].GetXaxis().GetXmax()-1.0)),
                    wHists["ele"].GetYaxis().FindFixBin(abs(event.lep2.Eta())))
    if debugLevel > 1: 
        print "lepton weight:",leptonWeight
    return leptonWeight

def leptonTriggerWeight(event, wHists, doLep2=False, debugLevel=0):
    if not ("eletrig" in wHists):
        print "Error in leptonTriggerWeight: electron trigger scale factor histogram not found!"
        sys.exit()
    if not ("muontrig" in wHists):
        print "Error in leptonTriggerWeight: muon trigger scale factor histogram not found!"
        sys.exit()
    trigWeight = 1.0
    #leading muon 
    if abs(event.lep1Type) == 13 and event.lep1.Pt() > 0:
        trigWeight *= wHists["muontrig"].GetBinContent(
                wHists["muontrig"].GetXaxis().FindFixBin(max(min(event.lep1.Pt(), 199.9),15.01)),
                #wHists["muontrig"].GetXaxis().FindFixBin(min(event.lep1.Pt(), wHists["muontrig"].GetXaxis().GetXmax()-1.0)),
                wHists["muontrig"].GetYaxis().FindFixBin(abs(event.lep1.Eta())))
    #leading electron
    elif abs(event.lep1Type) == 11 and event.lep1.Pt() > 0:
        trigWeight *= wHists["eletrig"].GetBinContent(
                wHists["eletrig"].GetXaxis().FindFixBin(max(min(event.lep1.Pt(), 199.9),15.01)),
                #wHists["eletrig"].GetXaxis().FindFixBin(min(event.lep1.Pt(), wHists["eletrig"].GetXaxis().GetXmax()-1.0)),
                wHists["eletrig"].GetYaxis().FindFixBin(abs(event.lep1.Eta())))
    if doLep2: 
        #subleading muon 
        if abs(event.lep2Type) == 13 and event.lep2.Pt() > 0:
            trigWeight *= wHists["muontrig"].GetBinContent(
                    wHists["muontrig"].GetXaxis().FindFixBin(max(min(event.lep2.Pt(), 199.9),15.01)),
                    #wHists["muontrig"].GetXaxis().FindFixBin(min(event.lep2.Pt(), wHists["muontrig"].GetXaxis().GetXmax()-1.0)),
                    wHists["muontrig"].GetYaxis().FindFixBin(abs(event.lep2.Eta())))
        #subleading electron
        elif abs(event.lep2Type) == 11 and event.lep2.Pt() > 0:
            trigWeight *= wHists["eletrig"].GetBinContent(
                    wHists["eletrig"].GetXaxis().FindFixBin(max(min(event.lep2.Pt(), 199.9),15.01)),
                    #wHists["eletrig"].GetXaxis().FindFixBin(min(event.lep2.Pt(), wHists["eletrig"].GetXaxis().GetXmax()-1.0)),
                    wHists["eletrig"].GetYaxis().FindFixBin(abs(event.lep2.Eta())))
    if debugLevel > 1: 
        if not doLep2:
            print "1-lepton trigger weight:",trigWeight
        else: 
            print "2-lepton trigger weight:",trigWeight
    return trigWeight

def weight_mc(event, wHists, scale=1.0, weightOpts=["doNPVWeights", "doLep1Weights", "do1LepTrigWeights"], errorOpt=None, debugLevel=0):
    """Apply pileup weights and other known MC correction factors -- for razor control regions"""
    lweightOpts = map(str.lower, weightOpts)

    eventWeight = event.weight*scale
    if debugLevel > 1: 
        print "Weight from ntuple:",event.weight
        print "Scale by:",scale

    #pileup reweighting
    if str.lower("doNPVWeights") in lweightOpts:
        eventWeight *= pileupWeight(event, wHists, debugLevel=debugLevel)
    elif str.lower("doNVtxWeights") in lweightOpts:
        eventWeight *= pileupWeight(event, wHists, puBranch="nVtx", debugLevel=debugLevel)

    #lepton scale factors
    if str.lower("doLep1Weights") in lweightOpts:
        doLep2 = (str.lower("doLep2Weights") in lweightOpts)
        eventWeight *= leptonWeight(event, wHists, doLep2, debugLevel=debugLevel)

    #trigger scale factors
    if str.lower("do1LepTrigWeights") in lweightOpts:
        doLep2Trig = (str.lower("doLep2TrigWeights") in lweightOpts)
        eventWeight *= leptonTriggerWeight(event, wHists, doLep2Trig, debugLevel=debugLevel)

    #up/down corrections for systematics
    if errorOpt == "muoneffUp":
        eventWeight *= event.sf_muonEffUp
        if debugLevel > 1: print "MuonEffUp scale factor:",event.sf_muonEffUp
    if errorOpt == "muoneffDown":
        eventWeight *= event.sf_muonEffDown
        if debugLevel > 1: print "MuonEffDown scale factor:",event.sf_muonEffDown
    if errorOpt == "eleeffUp":
        eventWeight *= event.sf_eleEffUp
        if debugLevel > 1: print "EleEffUp scale factor:",event.sf_eleEffUp
    if errorOpt == "eleeffDown":
        eventWeight *= event.sf_eleEffDown
        if debugLevel > 1: print "EleEffDown scale factor:",event.sf_eleEffDown
    if errorOpt == "btagUp":
        eventWeight *= event.sf_btagUp
        if debugLevel > 1: print "BTagUp scale factor:",event.sf_btagUp
    if errorOpt == "btagDown":
        eventWeight *= event.sf_btagDown
        if debugLevel > 1: print "BTagDown scale factor:",event.sf_btagDown

    if debugLevel > 1: 
        print "event weight:",eventWeight
    return eventWeight

def weight_data(event, wHists, scale=1.0, weightOpts=[], errorOpt=None, debugLevel=0):
    lweightOpts = map(str.lower, weightOpts)
    eventWeight = scale
    if debugLevel > 1: print("Applying a weight of "+str(eventWeight))
    return eventWeight

def loadWeightHists(filenames={}, histnames={}, debugLevel=0):
    """Returns a dict with necessary reweighting histograms"""
    if "pileup" not in filenames:
        filenames["pileup"] = "data/ScaleFactors/Placeholders/DummyRun2PileupWeights.root"
        histnames["pileup"] = "PUWeight_Run2"
    if "muon" not in filenames:
        filenames["muon"] = "data/ScaleFactors/Placeholders/DummyRun2MuonWeights.root" #dummy file
        histnames["muon"] = "MuonWeight_Run2_Tight" 
    if "ele" not in filenames:
        filenames["ele"] = "data/ScaleFactors/Placeholders/DummyRun2EleWeights.root" #dummy file
        histnames["ele"] = "EleWeight_Run2_Tight" #razor tag&probe
    wHists = {}
    wFiles = {}
    for name in filenames:
        if debugLevel > 0: print "Opening",name,"weight file",filenames[name]
        wFiles[name] = rt.TFile.Open(filenames[name])
        if debugLevel > 0: print "Getting histogram",histnames[name]
        wHists[name] = wFiles[name].Get(histnames[name])
        wHists[name].SetDirectory(0)
        assert wHists[name]
    for name in wFiles:
        wFiles[name].Close()
    return wHists

def loadScaleFactorHists(sfFilename="RazorScaleFactors.root", processNames=[], debugLevel=0):
    """Returns a dict with available scale factor histograms"""
    sfFile = rt.TFile.Open(sfFilename)
    sfHists = {}
    for name in processNames:
        if debugLevel > 0: print "Looking for scale factor histogram for",name,"...",
        tmp = sfFile.Get(name+"ScaleFactors")
        if tmp: 
            sfHists[name] = tmp
            sfHists[name].SetDirectory(0)
            if debugLevel > 0: print "Found!"
        else:
            if debugLevel > 0: print ""
    sfFile.Close()
    return sfHists
