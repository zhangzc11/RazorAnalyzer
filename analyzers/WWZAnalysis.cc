//LOCAL INCLUDES
#include "WWZAnalysis.h"
#include "RazorHelper.h"
#include "JetCorrectorParameters.h"
#include "JetCorrectionUncertainty.h"
#include "BTagCalibrationStandalone.h"
#include "EnergyScaleCorrection_class.hh"
//C++ INCLUDES
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <assert.h> 
//ROOT INCLUDES
#include <TH1F.h>
#include <TH2D.h>
#include "TRandom3.h"

using namespace std;


const double JET_CUT = 30.;
const int NUM_PDF_WEIGHTS = 60;

//Testing branching and merging
void WWZAnalysis::Analyze(bool isData, int option, string outFileName, string label)
{
  //*****************************************************************************
  //Settings
  //*****************************************************************************
  TRandom3 random(3003);

  string analysisTag = "Razor2016_80X";
  if ( label != "") analysisTag = label;

  std::cout << "[INFO]: option = " << option << std::endl;
  std::cout << "[INFO]: analysisTag --> " << analysisTag << std::endl;

  
  if ( outFileName.empty() )
    {
      std::cout << "WWZAnalysis: Output filename not specified!" << endl << "Using default output name WWZAnalysis.root" << std::endl;
      outFileName = "WWZAnalysis.root";
    }
  TFile* outFile = new TFile( outFileName.c_str(), "RECREATE" );
  //---------------------------
  //one tree to hold all events
  //---------------------------
  TTree *outputTree = new TTree("WWZAnalysis", "Info on selected razor inclusive events");
  
  //Get CMSSW Directory
  char* cmsswPath;
  cmsswPath = getenv("CMSSW_BASE");

  //--------------------------------
  //Initialize helper
  //--------------------------------
  RazorHelper *helper = 0;
  if (analysisTag == "Razor2015_76X") helper = new RazorHelper("Razor2015_76X", isData, false);
  else if (analysisTag == "Razor2016_80X") helper = new RazorHelper("Razor2016_80X", isData, false);
  else helper = new RazorHelper(analysisTag, isData, false);
  

  //--------------------------------
  //Including Jet Energy Corrections
  //--------------------------------
  std::vector<FactorizedJetCorrector*> JetCorrector = helper->getJetCorrector();
  std::vector<std::pair<int,int> > JetCorrectorIOV = helper->getJetCorrectionsIOV();


  //----------
  //pu histo
  //----------
  TH1D* puhisto = new TH1D("pileup", "", 50, 0, 50);
  
  //histogram containing total number of processed events (for normalization)
  TH1F *histNPV = new TH1F("NPV", "NPV", 2, -0.5, 1.5);
  TH1F *NEvents = new TH1F("NEvents", "NEvents", 1, 1, 2);
  TH1F *SumWeights = new TH1F("SumWeights", "SumWeights", 1, 0.5, 1.5);
  TH1F *SumScaleWeights = new TH1F("SumScaleWeights", "SumScaleWeights", 6, -0.5, 5.5);
  TH1F *SumPdfWeights = new TH1F("SumPdfWeights", "SumPdfWeights", NUM_PDF_WEIGHTS, -0.5, NUM_PDF_WEIGHTS-0.5);

  //--------------
  //tree variables
  //--------------
  float weight;
  float pileupWeight, pileupWeightUp, pileupWeightDown;
  float triggerEffWeight;
  float triggerEffSFWeight;
  int lep1Id;
  float lep1Pt, lep1Eta, lep1Phi;
  int lep2Id;
  float lep2Pt, lep2Eta, lep2Phi;
  int lep3Id;
  float lep3Pt, lep3Eta, lep3Phi;
  int lep4Id;
  float lep4Pt, lep4Eta, lep4Phi;

  //PDF SF
  std::vector<float> sf_pdf;
  
  int NPU;
  float ZMass, ZPt;
  float lep3MT, lep4MT;
  float lep34MT;

  int NJet20;
  int NJet30;
  int NBJet20;
  int NBJet30;
  float minDRJetToLep3;
  float minDRJetToLep4;

  float MET, MET_JESUp, MET_JESDown;  
  unsigned int run, lumi, event;
  
  //------------------------
  //set branches on big tree
  //------------------------

  outputTree->Branch("weight", &weight, "weight/F");
  outputTree->Branch("pileupWeight", &pileupWeight, "pileupWeight/F");
  outputTree->Branch("pileupWeightUp", &pileupWeightUp, "pileupWeightUp/F");
  outputTree->Branch("pileupWeightDown", &pileupWeightDown, "pileupWeightDown/F");
  outputTree->Branch("triggerEffWeight", &triggerEffWeight, "triggerEffWeight/F");
  outputTree->Branch("triggerEffSFWeight", &triggerEffSFWeight, "triggerEffSFWeight/F");
     
  outputTree->Branch("run", &run, "run/i");
  outputTree->Branch("lumi", &lumi, "lumi/i");
  outputTree->Branch("event", &event, "event/i");
  outputTree->Branch("NPU", &NPU, "npu/i");
  outputTree->Branch("nPV", &nPV, "nPV/i");
  outputTree->Branch("MET", &MET, "MET/F");
  outputTree->Branch("MET_JESUp", &MET_JESUp, "MET_JESUp/F");
  outputTree->Branch("MET_JESDown", &MET_JESDown, "MET_JESDown/F");
  outputTree->Branch("NJet20", &NJet20, "NJet20/I");
  outputTree->Branch("NJet30", &NJet30, "NJet30/I");
  outputTree->Branch("NBJet20", &NBJet20, "NBJet20/I");
  outputTree->Branch("NBJet30", &NBJet30, "NBJet30/I");
  outputTree->Branch("lep1Id", &lep1Id, "lep1Id/I");
  outputTree->Branch("lep1Pt", &lep1Pt, "lep1Pt/F");
  outputTree->Branch("lep1Eta", &lep1Eta, "lep1Eta/F");
  outputTree->Branch("lep1Phi", &lep1Phi, "lep1Phi/F");
  outputTree->Branch("lep2Id", &lep2Id, "lep2Id/I");
  outputTree->Branch("lep2Pt", &lep2Pt, "lep2Pt/F");
  outputTree->Branch("lep2Eta", &lep2Eta, "lep2Eta/F");
  outputTree->Branch("lep2Phi", &lep2Phi, "lep2Phi/F");
  outputTree->Branch("lep3Id", &lep3Id, "lep3Id/I");
  outputTree->Branch("lep3Pt", &lep3Pt, "lep3Pt/F");
  outputTree->Branch("lep3Eta", &lep3Eta, "lep3Eta/F");
  outputTree->Branch("lep3Phi", &lep3Phi, "lep3Phi/F");
  outputTree->Branch("lep4Id", &lep4Id, "lep4Id/I");
  outputTree->Branch("lep4Pt", &lep4Pt, "lep4Pt/F");
  outputTree->Branch("lep4Eta", &lep4Eta, "lep4Eta/F");
  outputTree->Branch("lep4Phi", &lep4Phi, "lep4Phi/F");
  outputTree->Branch("ZMass", &ZMass, "ZMass/F");
  outputTree->Branch("ZPt", &ZPt, "ZPt/F");
  outputTree->Branch("lep3MT", &lep3MT, "lep3MT/F");
  outputTree->Branch("lep4MT", &lep4MT, "lep4MT/F");
  outputTree->Branch("lep34MT", &lep34MT, "lep34MT/F");
  outputTree->Branch("minDRJetToLep3", &minDRJetToLep3, "minDRJetToLep3/F");
  outputTree->Branch("minDRJetToLep4", &minDRJetToLep4, "minDRJetToLep4/F");


  //begin loop
  if ( fChain == 0 ) return;
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  std::cout << "[INFO]: Total Entries = " << fChain->GetEntries() << "\n";
  for ( Long64_t jentry=0; jentry < nentries; jentry++ ) {
      //begin event
      if( jentry % 10000 == 0 ) std::cout << "[INFO]: Processing entry " << jentry << std::endl;
      Long64_t ientry = LoadTree( jentry );
      if ( ientry < 0 ) break;
      nb = fChain->GetEntry(jentry);
      nbytes += nb;
    
      //fill normalization histogram    
      NEvents->SetBinContent( 1, NEvents->GetBinContent(1) + genWeight);
      weight = genWeight;
      SumWeights->Fill(1.0, weight);
      
      //reset tree variables
      pileupWeight      = 1.0;
      pileupWeightUp    = 1.0;
      pileupWeightDown  = 1.0;
      triggerEffWeight  = 1.0;
      triggerEffSFWeight  = 1.0;
       
      //lepton variables
      lep1Id = 0;
      lep1Pt = -999;
      lep1Eta = -999;
      lep1Phi = -999;  
      lep2Id = 0;
      lep2Pt = -999;
      lep2Eta = -999;
      lep2Phi = -999;  
      lep3Id = 0;
      lep3Pt = -999;
      lep3Eta = -999;
      lep3Phi = -999;  
      lep4Id = 0;
      lep4Pt = -999;
      lep4Eta = -999;
      lep4Phi = -999;   
      ZMass = -999;
      ZPt = -999;
      lep3MT = -999;
      lep4MT = -999;      
      MET = -999;
      MET_JESUp = -999;
      MET_JESDown = -999;
      NJet20 = 0;
      NJet30 = 0;
      NBJet20 = 0;
      NBJet30 = 0;     
      minDRJetToLep3 = 9999;
      minDRJetToLep4 = 9999;

      //------------------
      //Pileup reweighting
      //------------------
      pileupWeight = 1.0;
      if( !isData ) {
	//Get number of PU interactions
	for (int i = 0; i < nBunchXing; i++) {
	  if (BunchXing[i] == 0) {
	    NPU = nPUmean[i];
	  }
	}
	puhisto->Fill(NPU);
	pileupWeight = helper->getPileupWeight(NPU);
	pileupWeightUp = helper->getPileupWeightUp(NPU) / pileupWeight;
	pileupWeightDown = helper->getPileupWeightDown(NPU) / pileupWeight;	
      }
      
      /////////////////////////////////
      //Scale and PDF variations
      /////////////////////////////////

      if ( (*scaleWeights).size() >= 9 ) 
	{
	  // sf_facScaleUp      = (*scaleWeights)[1]/genWeight;
	  // sf_facScaleDown    = (*scaleWeights)[2]/genWeight;
	  // sf_renScaleUp      = (*scaleWeights)[3]/genWeight;
	  // sf_renScaleDown    = (*scaleWeights)[6]/genWeight;
	  // sf_facRenScaleUp   = (*scaleWeights)[4]/genWeight;
	  // sf_facRenScaleDown = (*scaleWeights)[8]/genWeight;

	  
	  SumScaleWeights->Fill(0.0, (*scaleWeights)[1]);
	  SumScaleWeights->Fill(1.0, (*scaleWeights)[2]);
	  SumScaleWeights->Fill(2.0, (*scaleWeights)[3]);
	  SumScaleWeights->Fill(3.0, (*scaleWeights)[6]);
	  SumScaleWeights->Fill(4.0, (*scaleWeights)[4]);
	  SumScaleWeights->Fill(5.0, (*scaleWeights)[8]);
	}
      
      sf_pdf.erase( sf_pdf.begin(), sf_pdf.end() );
      for ( unsigned int iwgt = 0; iwgt < pdfWeights->size(); ++iwgt ) 
	{
	  sf_pdf.push_back( pdfWeights->at(iwgt)/genWeight );
	  SumPdfWeights->Fill(double(iwgt),(*pdfWeights)[iwgt]);
	}
      
 
      //*************************************************************************
      //Start Object Selection
      //*************************************************************************
      vector<TLorentzVector> Leptons;
      vector<int> LeptonsId;
      vector<int> LeptonsIndex;

      //-------------------------------
      //Muons
      //-------------------------------
      TLorentzVector ZCandidate;
      for( int i = 0; i < nMuons; i++ )	{
	  if(!isMuonPOGLooseMuon(i)) continue;  
	  if(muonPt[i] < 10) continue;
	  if(abs(muonEta[i]) > 2.4) continue;
	
	  //remove overlaps
	  bool overlap = false;
	  for(auto& lep : Leptons){
	    if (RazorAnalyzer::deltaR(muonEta[i],muonPhi[i],lep.Eta(),lep.Phi()) < 0.3) overlap = true;
	  }
	  if(overlap) continue;
	  
	  TLorentzVector tmpMuon;
	  tmpMuon.SetPtEtaPhiM(muonPt[i],muonEta[i], muonPhi[i],0.1057);
	  Leptons.push_back(tmpMuon);
	  LeptonsId.push_back(13 * -1 * muonCharge[i]);	  
	  LeptonsIndex.push_back(i);
      }

      //-------------------------------
      //Electrons
      //-------------------------------
      for( int i = 0; i < nElectrons; i++ )	{
	if(!isEGammaPOGVetoElectron(i)) continue;  
	if(elePt[i] < 10) continue;
	if(abs(eleEta[i]) > 2.4) continue;
	
	//remove overlaps
	bool overlap = false;
	for(auto& lep : Leptons){
	  if (RazorAnalyzer::deltaR(eleEta[i],elePhi[i],lep.Eta(),lep.Phi()) < 0.3) overlap = true;
	}
	if(overlap) continue;
	
	TLorentzVector tmpElectron;
	tmpElectron.SetPtEtaPhiM(elePt[i],eleEta[i], elePhi[i],0.000511);	
	Leptons.push_back(tmpElectron);
	LeptonsId.push_back(11 * -1 * eleCharge[i]);	  	
	LeptonsIndex.push_back(i);
   }
      

      //*************************************************************************
      //Find Z Candidate
      //*************************************************************************
      ZMass = -999; ZPt = -999;
      double tmpDistToZPole = 9999;      
      pair<uint,uint> ZCandidateLeptonIndex;
      bool foundZ = false;
      for( uint i = 0; i < Leptons.size(); i++ )	{
	for( uint j = 0; j < Leptons.size(); j++ ) {
	  if (!( LeptonsId[i] == -1*LeptonsId[j] )) continue;
	  double tmpMass = (Leptons[i]+Leptons[j]).M();

	  //select the pair closest to Z pole mass
	  if ( fabs( tmpMass - 91.2) < tmpDistToZPole) {
	    tmpDistToZPole = tmpMass;
	    if (LeptonsId[i] > 0) {
	      ZCandidateLeptonIndex = pair<int,int>(i,j);
	    } else {
	      ZCandidateLeptonIndex = pair<int,int>(j,i);
	    }
	    ZMass = tmpMass;
	    ZPt = (Leptons[i]+Leptons[j]).Pt();
	    foundZ = true;
	  }
	}
      }
      
      if (foundZ) {
	lep1Id = LeptonsId[ZCandidateLeptonIndex.first];
	lep1Pt = Leptons[ZCandidateLeptonIndex.first].Pt();
	lep1Eta = Leptons[ZCandidateLeptonIndex.first].Eta();
	lep1Phi = Leptons[ZCandidateLeptonIndex.first].Phi();
	lep2Id = LeptonsId[ZCandidateLeptonIndex.second];
	lep2Pt = Leptons[ZCandidateLeptonIndex.second].Pt();
	lep2Eta = Leptons[ZCandidateLeptonIndex.second].Eta();
	lep2Phi = Leptons[ZCandidateLeptonIndex.second].Phi();
      }
      
      //*************************************************************************
      //Save W leptons
      //*************************************************************************
      int lep3Index = -1;
      int lep4Index = -1;
      for( uint i = 0; i < Leptons.size(); i++ )	{
	if (foundZ && ( i == ZCandidateLeptonIndex.first 
			|| i ==  ZCandidateLeptonIndex.second)
	    ) continue;
	if (Leptons[i].Pt() > lep3Pt) {
	  lep4Id = lep3Id;
	  lep4Pt = lep3Pt;
	  lep4Eta = lep3Eta;
	  lep4Phi = lep3Phi;
	  lep4Index = lep3Index;
	  
	  lep3Id = LeptonsId[i];
	  lep3Pt = Leptons[i].Pt();
	  lep3Eta = Leptons[i].Eta();
	  lep3Phi = Leptons[i].Phi();
	  lep3Index = i;	  
	} else if (Leptons[i].Pt() > lep4Pt) {
	  lep4Id = LeptonsId[i];
	  lep4Pt = Leptons[i].Pt();
	  lep4Eta = Leptons[i].Eta();
	  lep4Phi = Leptons[i].Phi();
	  lep4Index = i;
	}
      }
      
    

      //***********************************************
      //Select Jets
      //***********************************************
      for(int i = 0; i < nJets; i++){
	
	//*****************************************************************
	//exclude selected muons and electrons from the jet collection
	//*****************************************************************
	double deltaR = -1;
	for(auto& lep : Leptons){
	  double thisDR = RazorAnalyzer::deltaR(jetEta[i],jetPhi[i],lep.Eta(),lep.Phi());  
	  if(deltaR < 0 || thisDR < deltaR) deltaR = thisDR;
	}
	if(deltaR > 0 && deltaR < 0.4) continue; //jet matches a selected lepton
	

	//*****************************************************************
	//Apply Jet Energy and Resolution Corrections
	//*****************************************************************
	double JEC = JetEnergyCorrectionFactor( jetPt[i], jetEta[i], jetPhi[i], jetE[i],
						fixedGridRhoAll, jetJetArea[i], runNum,
						JetCorrectorIOV,JetCorrector);
	
	TLorentzVector thisJet = makeTLorentzVector( jetPt[i]*JEC, jetEta[i], jetPhi[i], jetE[i]*JEC );
	
	//cout << "jet: " << thisJet.Pt() << " " << jetPassIDLoose[i] << " " << isCSVL(i) << "\n";

	if( thisJet.Pt() < 20 ) continue;//According to the April 1st 2015 AN
	if( fabs( thisJet.Eta() ) >= 3.0 ) continue;
	if ( !jetPassIDLoose[i] ) continue;

	NJet20++;
	if (thisJet.Pt() > 30) {
	  NJet30++;
	  if (lep3Index >= 0) {
	    double dRJetToLep3 = RazorAnalyzer::deltaR(thisJet.Eta(),thisJet.Phi(),Leptons[lep3Index].Eta(),Leptons[lep3Index].Phi());
	    if (dRJetToLep3 < minDRJetToLep3) minDRJetToLep3 = dRJetToLep3;
	  }
	  if (lep4Index >= 0) {
	    double dRJetToLep4 = RazorAnalyzer::deltaR(thisJet.Eta(),thisJet.Phi(),Leptons[lep4Index].Eta(),Leptons[lep4Index].Phi());
	    if (dRJetToLep4 < minDRJetToLep4) minDRJetToLep4 = dRJetToLep4;
	  }
	}

	if (isCSVL(i)) NBJet20++;
    	if (isCSVL(i) && thisJet.Pt() > 30) NBJet30++;
	

      }
      
      //cout << "NJet: " << NJet20 << " " << NJet30 << " " << NBJet20 << " " << NBJet30 << "\n";
      
      //*************************************************************************
      //Other kinematic variables
      //*************************************************************************
      double PFMetCustomType1CorrectedX = metType1Pt*cos(metType1Phi);
      double PFMetCustomType1CorrectedY = metType1Pt*sin(metType1Phi);
      TLorentzVector PFMETCustomType1Corrected; 
      PFMETCustomType1Corrected.SetPxPyPzE(PFMetCustomType1CorrectedX, PFMetCustomType1CorrectedY, 0, 
					   sqrt( pow(PFMetCustomType1CorrectedX,2) + pow(PFMetCustomType1CorrectedY,2)));      
      TLorentzVector MyMET = PFMETCustomType1Corrected; //This is the MET that will be used below.
      MET = MyMET.Pt();

      TLorentzVector DileptonWW;
      if (lep3Index >= 0) {
	lep3MT = sqrt(2*lep3Pt*MyMET.Pt()*( 1.0 - cos(  Leptons[lep3Index].DeltaPhi(MyMET) ) ) ); 
	if (lep4Index >= 0) {
	  lep4MT = sqrt(2*lep4Pt*MyMET.Pt()*( 1.0 - cos(  Leptons[lep4Index].DeltaPhi(MyMET) ) ) ); 
	  DileptonWW = Leptons[lep3Index] + Leptons[lep4Index];	  
	  lep34MT = sqrt(2*DileptonWW.Pt()*MyMET.Pt()*( 1.0 - cos(  DileptonWW.DeltaPhi(MyMET) ) ) ); 
	}
      }


      //******************************************************
      //compute trigger efficiency weights for MC
      //******************************************************
      triggerEffWeight = 1.0;
      triggerEffSFWeight = 1.0;
   
    
      //Fill Event
      outputTree->Fill();

      //end of event loop
  }
  
  std::cout << "[INFO]: Number of events processed: " << NEvents->Integral() << std::endl;
  
  std::cout << "[INFO]: Writing output trees..." << std::endl;    
  outFile->cd();
  outputTree->Write();
  NEvents->Write();
  SumWeights->Write();
  SumScaleWeights->Write();
  SumPdfWeights->Write();
  histNPV->Write();
  puhisto->Write();
  
  outFile->Close();
  delete helper;

}
