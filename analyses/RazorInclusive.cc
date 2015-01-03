#define RazorAnalyzer_cxx
#include "RazorAnalyzer.h"
#include "JetCorrectorParameters.h"

//C++ includes
#include <sys/stat.h>

//ROOT includes
#include "TH1F.h"

using namespace std;

void RazorAnalyzer::RazorInclusive(string outFileName, bool combineTrees)
{
    //initialization: create one TTree for each analysis box 
    cout << "Initializing..." << endl;
    if (outFileName.empty()){
        cout << "RazorInclusive: Output filename not specified!" << endl << "Using default output name RazorInclusive.root" << endl;
        outFileName = "RazorInclusive.root";
    }
    TFile outFile(outFileName.c_str(), "RECREATE");
    
    //one tree to hold all events
    TTree *razorTree = new TTree("RazorInclusive", "Info on selected razor inclusive events");
    
    //initialize jet energy corrections
    std::vector<JetCorrectorParameters> correctionParameters;
    //get correct directory for JEC files (different for lxplus and t3-higgs)
    struct stat sb;
    string dir;
    if(stat("/afs/cern.ch/work/s/sixie/public", &sb) == 0 && S_ISDIR(sb.st_mode)){ //check if Si's directory exists
        dir = "/afs/cern.ch/work/s/sixie/public/releases/run2/CMSSW_7_2_0/src/RazorAnalyzer/data";
        cout << "Getting JEC parameters from " << dir << endl;
    }
    else{ //we are on t3-higgs (for running locally on your laptop we need a separate solution)
        dir = Form("%s/src/RazorAnalyzer/data/", getenv("CMSSW_BASE"));
        cout << "Getting JEC parameters from " << dir << endl;
    }
    correctionParameters.push_back(JetCorrectorParameters(Form("%sPHYS14_V2_MC_L1FastJet_AK4PFchs.txt", dir.c_str())));
    correctionParameters.push_back(JetCorrectorParameters(Form("%sPHYS14_V2_MC_L2Relative_AK4PFchs.txt", dir.c_str())));
    correctionParameters.push_back(JetCorrectorParameters(Form("%sPHYS14_V2_MC_L3Absolute_AK4PFchs.txt", dir.c_str())));    

    FactorizedJetCorrector *JetCorrector = new FactorizedJetCorrector(correctionParameters);

    //separate trees for individual boxes
    map<string, TTree*> razorBoxes;
    vector<string> boxNames;
    boxNames.push_back("MuEle");
    boxNames.push_back("MuMu");
    boxNames.push_back("EleEle");
    boxNames.push_back("MuMultiJet");
    boxNames.push_back("MuJet");
    boxNames.push_back("EleMultiJet");
    boxNames.push_back("EleJet");
    boxNames.push_back("LooseLeptonMultiJet");
    boxNames.push_back("MultiJet");
    boxNames.push_back("TwoBJet");
    boxNames.push_back("OneBJet");
    boxNames.push_back("ZeroBJet");
    for(size_t i = 0; i < boxNames.size(); i++){
        razorBoxes[boxNames[i]] = new TTree(boxNames[i].c_str(), boxNames[i].c_str());
    }

    //histogram containing total number of processed events (for normalization)
    TH1F *NEvents = new TH1F("NEvents", "NEvents", 1, 1, 2);

    //tree variables
    int nSelectedJets, nBTaggedJets;
    int nLooseMuons, nTightMuons, nLooseElectrons, nTightElectrons, nTightTaus;
    int nVetoMuons, nVetoElectrons, nLooseTaus;
    float theMR;
    float theRsq;
    RazorBox box;

    //set branches on big tree
    if(combineTrees){
        razorTree->Branch("nSelectedJets", &nSelectedJets, "nSelectedJets/I");
        razorTree->Branch("nBTaggedJets", &nBTaggedJets, "nBTaggedJets/I");
        razorTree->Branch("nLooseMuons", &nLooseMuons, "nLooseMuons/I");
        razorTree->Branch("nTightMuons", &nTightMuons, "nTightMuons/I");
        razorTree->Branch("nLooseElectrons", &nLooseElectrons, "nLooseElectrons/I");
        razorTree->Branch("nTightElectrons", &nTightElectrons, "nTightElectrons/I");
        razorTree->Branch("nTightTaus", &nTightTaus, "nTightTaus/I");
        razorTree->Branch("MR", &theMR, "MR/F");
        razorTree->Branch("Rsq", &theRsq, "Rsq/F");
        razorTree->Branch("box", &box, "box/I");
    }
    //set branches on all trees
    else{ 
        for(auto& box : razorBoxes){
            box.second->Branch("nSelectedJets", &nSelectedJets, "nSelectedJets/I");
            box.second->Branch("nBTaggedJets", &nBTaggedJets, "nBTaggedJets/I");
            box.second->Branch("nLooseMuons", &nLooseMuons, "nLooseMuons/I");
            box.second->Branch("nTightMuons", &nTightMuons, "nTightMuons/I");
            box.second->Branch("nLooseElectrons", &nLooseElectrons, "nLooseElectrons/I");
            box.second->Branch("nTightElectrons", &nTightElectrons, "nTightElectrons/I");
            box.second->Branch("nTightTaus", &nTightTaus, "nTightTaus/I");
            box.second->Branch("MR", &theMR, "MR/F");
            box.second->Branch("Rsq", &theRsq, "Rsq/F");
        }
    }

    //begin loop
    if (fChain == 0) return;
    Long64_t nentries = fChain->GetEntriesFast();
    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        //begin event
        if(jentry % 10000 == 0) cout << "Processing entry " << jentry << endl;
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;

        //fill normalization histogram
        NEvents->Fill(1.0);

        //reset tree variables
        nSelectedJets = 0;
        nBTaggedJets = 0;
	nVetoMuons = 0;
        nLooseMuons = 0;
        nTightMuons = 0;
	nVetoElectrons = 0;
        nLooseElectrons = 0;
        nTightElectrons = 0;
	nLooseTaus = 0;
        nTightTaus = 0;
        theMR = -1;
        theRsq = -1;
        if(combineTrees) box = NONE;

        //TODO: triggers!
        bool passedLeptonicTrigger = true;
        bool passedHadronicTrigger= true;
        if(!(passedLeptonicTrigger || passedHadronicTrigger)) continue; //ensure event passed a trigger
        
        vector<TLorentzVector> GoodLeptons; //leptons used to compute hemispheres
        for(int i = 0; i < nMuons; i++){

            if(muonPt[i] < 5) continue;
            if(abs(muonEta[i]) > 2.4) continue;

	    if(isVetoMuon(i)) nVetoMuons++;
            if(isLooseMuon(i) && muonPt[i] >= 10 ) nLooseMuons++;
            if(isTightMuon(i) && muonPt[i] >= 10) nTightMuons++;

	    if(!isVetoMuon(i)) continue;  
	    TLorentzVector thisMuon = makeTLorentzVector(muonPt[i], muonEta[i], muonPhi[i], muonE[i]); 
            GoodLeptons.push_back(thisMuon);           
        }
        for(int i = 0; i < nElectrons; i++){
            if(elePt[i] < 5) continue;
	    if(fabs(eleEta[i]) > 2.5) continue;
	    if(isMVANonTrigVetoElectron(i)) nVetoElectrons++;
	    if(isLooseElectron(i) && elePt[i] > 10 ) nLooseElectrons++;
            if(isTightElectron(i) && elePt[i] > 10 ) nTightElectrons++;

            if(!isMVANonTrigVetoElectron(i)) continue; 
	    TLorentzVector thisElectron = makeTLorentzVector(elePt[i], eleEta[i], elePhi[i], eleE[i]);
            GoodLeptons.push_back(thisElectron);            
        }
        for(int i = 0; i < nTaus; i++){
          if(isLooseTau(i)) nLooseTaus++;
	  if(isTightTau(i)) nTightTaus++;
        }
        
        vector<TLorentzVector> GoodJets;
        int numJetsAbove80GeV = 0;
        for(int i = 0; i < nJets; i++){

	    double JEC = JetEnergyCorrectionFactor(jetPt[i], jetEta[i], jetPhi[i], jetE[i], 
						   fixedGridRhoFastjetAll, jetJetArea[i], 
						   JetCorrector);   
	    double jetCorrPt = jetPt[i]*JEC;
	    double jetCorrE = jetE[i]*JEC;

	    if(jetCorrPt < 40) continue;
    	    if(fabs(jetEta[i]) > 3.0) continue;

            //exclude selected muons and electrons from the jet collection
            double deltaR = -1;
            TLorentzVector thisJet = makeTLorentzVector(jetCorrPt, jetEta[i], jetPhi[i], jetCorrE);
            for(auto& lep : GoodLeptons){
                double thisDR = thisJet.DeltaR(lep);
                if(deltaR < 0 || thisDR < deltaR) deltaR = thisDR;
            }
            if(deltaR > 0 && deltaR < 0.4) continue; //jet matches a selected lepton
            
            if(jetCorrPt > 80) numJetsAbove80GeV++;
            GoodJets.push_back(thisJet);
            nSelectedJets++;

            if(isCSVM(i)){ 
                nBTaggedJets++;
            }
        }

        if(numJetsAbove80GeV < 2) continue; //event fails to have two 80 GeV jets

        //Compute the razor variables using the selected jets and possibly leptons
        vector<TLorentzVector> GoodPFObjects;
        for(auto& jet : GoodJets) GoodPFObjects.push_back(jet);
        if(passedLeptonicTrigger) for(auto& lep : GoodLeptons) GoodPFObjects.push_back(lep);
        TLorentzVector PFMET = makeTLorentzVectorPtEtaPhiM(metPt, 0, metPhi, 0);

        vector<TLorentzVector> hemispheres = getHemispheres(GoodPFObjects);
        theMR = computeMR(hemispheres[0], hemispheres[1]); 
        theRsq = computeRsq(hemispheres[0], hemispheres[1], PFMET);

        //MuEle Box
        if(passedLeptonicTrigger && nTightElectrons > 0 && nLooseMuons > 0 && nBTaggedJets > 0){
            if(passesLeptonicRazorBaseline(theMR, theRsq)){ 
                if(combineTrees){
                    box = MuEle;
                    razorTree->Fill();
                }
                else razorBoxes["MuEle"]->Fill();
            }
        }
        //MuMu Box
        else if(passedLeptonicTrigger && nTightMuons > 0 && nLooseMuons > 1 && nBTaggedJets > 0){
            if(passesLeptonicRazorBaseline(theMR, theRsq)){ 
                if(combineTrees){
                    box = MuMu;
                    razorTree->Fill();
                }
                else razorBoxes["MuMu"]->Fill();
            }
        }
        //EleEle Box
        else if(passedLeptonicTrigger && nTightElectrons > 0 && nLooseElectrons > 1 && nBTaggedJets > 0){
            if(passesLeptonicRazorBaseline(theMR, theRsq)){ 
                if(combineTrees){
                    box = EleEle;
                    razorTree->Fill();
                }
                else razorBoxes["EleEle"]->Fill();
            }
        }
        //MuMultiJet Box
        else if(passedLeptonicTrigger && nTightMuons > 0 && nBTaggedJets > 0 && nSelectedJets > 3){
            if(passesLeptonicRazorBaseline(theMR, theRsq)){ 
                if(combineTrees){
                    box = MuMultiJet;
                    razorTree->Fill();
                }
                else razorBoxes["MuMultiJet"]->Fill();
            }
        }
        //MuJet Box
        else if(passedLeptonicTrigger && nTightMuons > 0 && nBTaggedJets > 0){
            if(passesLeptonicRazorBaseline(theMR, theRsq)){ 
                if(combineTrees){
                    box = MuJet;
                    razorTree->Fill();
                }
                else razorBoxes["MuJet"]->Fill();
            }
        }
        //EleMultiJet Box
        else if(passedLeptonicTrigger && nTightElectrons > 0 && nBTaggedJets > 0 && nSelectedJets > 3){
            if(passesLeptonicRazorBaseline(theMR, theRsq)){ 
                if(combineTrees){
                    box = EleMultiJet;
                    razorTree->Fill();
                }
                else razorBoxes["EleMultiJet"]->Fill();
            }
        }
        //EleJet Box
        else if(passedLeptonicTrigger && nTightElectrons > 0 && nBTaggedJets > 0){
            if(passesLeptonicRazorBaseline(theMR, theRsq)){ 
                if(combineTrees){
                    box = EleJet;
                    razorTree->Fill();
                }
                else razorBoxes["EleJet"]->Fill();
            }
        }
	//Soft Lepton + MultiJet Box
        else if(passedHadronicTrigger && nLooseTaus + nVetoElectrons + nVetoMuons > 0 && nBTaggedJets > 0 && nSelectedJets > 3){
            if(passesHadronicRazorBaseline(theMR, theRsq)){  
                if(combineTrees){
                    box = LooseLeptonMultiJet;
                    razorTree->Fill();
                }
                else razorBoxes["LooseLeptonMultiJet"]->Fill();
            }
        }
        //MultiJet Box
        else if(passedHadronicTrigger && nBTaggedJets > 0 && nSelectedJets > 3){
            if(passesHadronicRazorBaseline(theMR, theRsq)){  
                if(combineTrees){
                    box = MultiJet;
                    razorTree->Fill();
                }
                else { razorBoxes["MultiJet"]->Fill(); }
            }
        }
        //TwoBJet Box
        else if(passedHadronicTrigger && nBTaggedJets > 1){
            if(passesHadronicRazorBaseline(theMR, theRsq)){ 
                if(combineTrees){
                    box = TwoBJet;
                    razorTree->Fill();
                }
                else razorBoxes["TwoBJet"]->Fill();
            }
        }
        //OneBJet Box
        else if(passedHadronicTrigger && nBTaggedJets > 0){
            if(passesHadronicRazorBaseline(theMR, theRsq)){ 
                if(combineTrees){
                    box = OneBJet;
                    razorTree->Fill();
                }
                else razorBoxes["OneBJet"]->Fill();
            }
        }
        //ZeroBJetBox
        else if(passedHadronicTrigger){
            if(passesHadronicRazorBaseline(theMR, theRsq)){ 
                if(combineTrees){
                    box = ZeroBJet;
                    razorTree->Fill();
                }
                else razorBoxes["ZeroBJet"]->Fill();
            }
        }
    }//end of event loop

    cout << "Writing output trees..." << endl;
    if(combineTrees) razorTree->Write();
    else for(auto& box : razorBoxes) box.second->Write();
    NEvents->Write();

    outFile.Close();
}