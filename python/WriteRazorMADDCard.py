import sys,os
import argparse
import ROOT as rt

#local imports
from macro.razorAnalysis import xbinsSignal, colsSignal
from macro.razorMacros import unrollAndStitch
from macro.razorWeights import loadScaleFactorHists
from RunCombine import exec_me
from DustinTuples2DataCard import uncorrelate, uncorrelateSFs, writeDataCard_th1
import macro.macro as macro
from SidebandMacro import SAMPLES, LUMI, config
from CheckSignalContamination import checkSignalContamination
from framework import Config

SIGNAL_DIR = "root://eoscms:///eos/cms/store/group/phys_susy/razor/Run2Analysis/FullRazorInclusive/V1p24_ForMoriond20160124/combined"
BACKGROUND_DIR = "root://eoscms:///eos/cms/store/group/phys_susy/razor/Run2Analysis/RazorMADD2015"
LIMIT_DIR = "root://eoscms:///eos/cms/store/group/phys_susy/razor/Run2Analysis/RazorMADD2015"

if __name__ == "__main__":
    rt.gROOT.SetBatch()

    #parse args
    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--verbose", help="display detailed output messages",
                                action="store_true")
    parser.add_argument("-d", "--debug", help="display excruciatingly detailed output messages",
                                action="store_true")
    parser.add_argument('--box', help="choose a box")
    parser.add_argument('--dir', help="output directory", default="SignalRegionPlots", dest='outDir')
    parser.add_argument('--no-sys',dest="noSys",default=False,action='store_true', help="no systematic templates")
    parser.add_argument('--no-stat',dest="noStat",default=False,action='store_true', help="no MC statistics uncertainty")
    #sigal options
    parser.add_argument('-m','--model', default="T1bbbb", help="signal model name")
    parser.add_argument('--mGluino',default=-1,type=int, help="mass of gluino")
    parser.add_argument('--mStop',default=-1,type=int, help="mass of stop")
    parser.add_argument('--mLSP',default=-1,type=int, help="mass of LSP")
    parser.add_argument('--fit-sys', dest="addMCVsFit", action='store_true', help="add MC vs fit systematic")
    parser.add_argument('--no-limit', dest='noCombine', action='store_true', help='do not call combine, make template histograms only')
    parser.add_argument('--signif', action='store_true', help='compute significance rather than limit')
    parser.add_argument('--contamination', action='store_true', help='add uncertainty for signal contamination')

    args = parser.parse_args()
    debugLevel = args.verbose + 2*args.debug
    outDir = args.outDir

    if args.box is None:
        print "Please choose an analysis box with --box"
        sys.exit()
    box = args.box
    boxList = box.split('_') #interpret box1_box2_... as a list of boxes to combine

    #make output directory
    os.system('mkdir -p '+outDir)

    for curBox in boxList:
        #get configuration for this box
        samples = SAMPLES[curBox]
        unrollBins = [(xbinsSignal[curBox][str(btags)+'B'], colsSignal[curBox][str(btags)+'B']) for btags in range(4)]

        #assess signal contamination
        contamHists = None
        sfNames = { "TTJets1L":"TTJets", "TTJets2L":"TTJets", "ZInv":"GJetsInv" }
        sfHists = loadScaleFactorHists("data/ScaleFactors/RazorMADD2015/RazorScaleFactors_Inclusive_CorrectedToMultiJet.root", processNames=["TTJets1L","TTJets2L","WJets","ZInv"], scaleFactorNames=sfNames, debugLevel=debugLevel)
        if args.contamination:
            #get level of contamination in control regions
            print "Computing signal contamination level in control regions"
            ttContamHist = checkSignalContamination("config/run2_20151229_ControlRegion.config",
                   outDir=outDir, lumi=LUMI, box="TTJetsSingleLeptonControlRegion", model=args.model,
                   mLSP=args.mLSP, mGluino=args.mGluino, mStop=args.mStop, mergeBins=True)
            wContamHist = checkSignalContamination("config/run2_20151229_ControlRegion.config",
                   outDir=outDir, lumi=LUMI, box="WJetControlRegion", model=args.model,
                   mLSP=args.mLSP, mGluino=args.mGluino, mStop=args.mStop, mergeBins=True)
            contamHists = { "TTJets1L":ttContamHist, "TTJets2L":ttContamHist, "WJets":wContamHist }

            #scale contamination level by expected signal strength exclusion
            expExclusion = macro.getExcludedSignalStrength(LIMIT_DIR, args.model, mGluino=args.mGluino,
                    mStop=args.mStop, mLSP=args.mLSP, debugLevel=debugLevel)
            print "Expected limit is",expExclusion,"-- scaling signal contamination by this amount"
            contamHists["WJets"].Scale(expExclusion)
            contamHists["TTJets1L"].Scale(expExclusion)

        #make combined unrolled histograms for background
        backgroundHists = unrollAndStitch(curBox, samples=samples, inDir=BACKGROUND_DIR, outDir=outDir, unrollBins=unrollBins, noSys=args.noSys, addStatUnc=(not args.noStat), addMCVsFit=args.addMCVsFit, debugLevel=debugLevel, signalContaminationHists=contamHists, sfHistsForSignalContamination=sfHists)

        if args.noCombine: continue #if not setting a limit, we are done

        #treat appropriate uncertainties as uncorrelated bin to bin
        toUncorrelate = ['ttcrosscheck','zllcrosscheck','btagcrosscheckmr','btagcrosscheckrsq','btaginvcrosscheck','vetolepetacrosscheck','vetotauetacrosscheck','vetolepptcrosscheck','vetotauptcrosscheck']
        toUncorrelate += ['stat'+curBox+sample for sample in samples]

        for sys in toUncorrelate:
            if 'stat' in sys:
                if args.noStat: continue
                suppressLevel = 0.1
            else:
                if args.noSys: continue
                suppressLevel = 0.0
            uncorrelate(backgroundHists, sys, suppressLevel=suppressLevel)

        #scale factor uncertainties are correlated according to the bin they are in
        toUncorrelateSF = ['sfstatttjets','sfstatwjets','sfstatzinv']
        cfg = Config.Config(config)
        for sys in toUncorrelateSF:
            uncorrelateSFs(backgroundHists, sys, sfHists, cfg, curBox, unrollBins=unrollBins)

        #call SMS template maker
        if 'T1' in args.model or 'T5' in args.model:
            modelName = 'SMS-'+args.model+'_'+str(args.mGluino)+'_'+str(args.mLSP)
        else:
            modelName = 'SMS-'+args.model+'_'+str(args.mStop)+'_'+str(args.mLSP)
        signalFilename=SIGNAL_DIR+'/'+modelName+'.root'

        #to modify branching ratios in T1ttbb sample
        brString = ""
        if 'T1x' in args.model:
            xBR = float(args.model[args.model.find('x')+1:args.model.find('y')].replace('p','.'))
            yBR = float(args.model[args.model.find('y')+1:].replace('p','.'))
            brString = '--xBR %.2f --yBR %.2f'%(xBR,yBR)
            signalFilename = SIGNAL_DIR+'/SMS-T1ttbb_'+str(args.mGluino)+'_'+str(args.mLSP)+'.root'

        exec_me('python python/SMSTemplates.py --merge-bins -c %s -d %s --lumi %d --box %s %s %s %s'%(config, outDir, LUMI, curBox, ((args.noSys)*('--no-signal-sys')), signalFilename, brString), False) 
        #load SMS template histograms
        signalHistFilename = '%s/%s_lumi-%.3f_0-3btag_%s.root'%(outDir,modelName,LUMI*1.0/1000,curBox)
        signalHists = macro.importHists(signalHistFilename)
        #update with correct names
        for x,h in signalHists.items():
            h.SetName(h.GetName().replace(curBox+'_'+args.model,modelName))
            signalHists[h.GetName()] = signalHists.pop(x)
        #combine signal and background dictionaries
        hists = backgroundHists.copy()
        hists.update(signalHists)

        #write histograms to ROOT file
        outFileName = outDir+'/RazorInclusiveMADD_lumi-%.1f_%s.root'%(LUMI*1.0/1000.,curBox)
        outFile = rt.TFile(outFileName, 'recreate')
        for x,h in hists.iteritems():
            h.Write()
        outFile.Close()
        #write combine card
        cardName = outFileName.replace('.root','.txt')
        writeDataCard_th1(curBox,modelName,cardName,hists,samples)

    if args.noCombine: 
        sys.exit()

    #run combine
    if args.signif:
        combineMethod = 'ProfileLikelihood'
        combineFlags = '--signif -t -1 --toysFreq'
    else:
        combineMethod = 'Asymptotic'
        combineFlags = ''
    if len(boxList) == 1:
        #get card name
        cardName = outDir+'/RazorInclusiveMADD_lumi-%.1f_%s.txt'%(LUMI*1.0/1000.,boxList[0])
        combineName = 'MADD_'+boxList[0]+'_'+modelName
        exec_me('combine -M %s %s -n %s %s'%(combineMethod,cardName,combineName,combineFlags), False)
        exec_me('mv higgsCombine%s.%s.mH120.root %s/'%(combineName,combineMethod,outDir),False)
    elif len(boxList) > 1:
        #get card names
        cardNames = []
        combineName = 'MADD_'+('_'.join(boxList))+'_'+modelName
        for curBox in boxList:
            cardName = 'RazorInclusiveMADD_lumi-%.1f_%s.txt'%(LUMI*1.0/1000.,curBox)
            cardNames.append(cardName)
        #combine cards
        combinedCardName = 'RazorInclusiveMADD_'+('_'.join(boxList))+'.txt'
        exec_me('cd '+outDir+'; combineCards.py '+(' '.join(cardNames))+' > '+combinedCardName+'; cd ..', False)
        #call combine
        exec_me('combine -M '+combineMethod+' '+outDir+'/'+combinedCardName+' -n '+combineName+' '+combineFlags, False)
        #store output in output directory
        exec_me('mv higgsCombine%s.%s.mH120.root %s/'%(combineName,combineMethod,outDir),False)