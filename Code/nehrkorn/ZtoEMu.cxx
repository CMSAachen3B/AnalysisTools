#include "ZtoEMu.h"
#include "TLorentzVector.h"
#include <cstdlib>
#include <stdlib.h>
#include "HistoConfig.h"
#include <iostream>

#include "Tools.h"
#include "PDG_Var.h"
#include "TauDataFormat/TauNtuple/interface/DataMCType.h"

#include "Parameters.h"
#include "TMath.h"

#include <TFile.h>
#include <sstream>
// for JEC uncertainties
//#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
//#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

ZtoEMu::ZtoEMu(TString Name_, TString id_):
  Selection(Name_,id_)
  ,mu_ptlow(10)
  ,mu_pthigh(20)
  ,e_ptlow(10)
  ,e_pthigh(20)
  ,mu_eta(2.4)
  ,e_eta(2.5)
  ,mmin(20)
  ,jet_pt(18)
  ,jet_eta(4.7)
  ,jet_sum(70)
  ,zmin(88)
  ,zmax(94)
  ,mtmu(50)
  ,ptbalance(20)
  ,csvl(0.244)
  ,csvm(0.679)
  ,csvt(0.898)
{
    //verbose=true;

	/*gRandom->SetSeed(1234);
	eleres=0;
	muonres=0;
	gause = new TF1("gause","TMath::Gaus(x,0.,1.02)/TMath::Sqrt(2*TMath::Pi())/1.02",-5.,5.);
	gausmu = new TF1("gausmu","TMath::Gaus(x,0.,1.006)/TMath::Sqrt(2*TMath::Pi())/1.006",-5.,5.);
	eres = new TH1D("eres","eres",100,-5.,5.);
	mures = new TH1D("mures","mures",100,-5.,5.);
	eres->FillRandom("gause",1000000);
	mures->FillRandom("gausmu",1000000);
	eleres = eres->GetRandom();
	muonres = mures->GetRandom();*/

	doHiggsObjects = false;
	doWWObjects = true;
	useMadgraphZ = false;
	if(useMadgraphZ) mmin = 50;
	if(doHiggsObjects){
		mu_eta = 2.1;
		e_eta = 2.3;
	}
}

ZtoEMu::~ZtoEMu(){
  for(int j=0; j<Npassed.size(); j++){
    std::cout << "ZtoEMu::~ZtoEMu Selection Summary before: " 
	 << Npassed.at(j).GetBinContent(1)     << " +/- " << Npassed.at(j).GetBinError(1)     << " after: "
	 << Npassed.at(j).GetBinContent(NCuts) << " +/- " << Npassed.at(j).GetBinError(NCuts) << std::endl;
  }
  std::cout << "ZtoEMu::~ZtoEMu()" << std::endl;
}

void  ZtoEMu::Configure(){

  // Setup Cut Values
  for(int i=0; i<NCuts;i++){
    cut.push_back(0);
    value.push_back(0);
    pass.push_back(false);
    if(i==TriggerOk)          cut.at(TriggerOk)=1;
    if(i==PrimeVtx)           cut.at(PrimeVtx)=1;
    if(i==NMu)                cut.at(NMu)=1;
    if(i==NE)                 cut.at(NE)=1;
    if(i==ptthreshold)        cut.at(ptthreshold)=1;
    if(i==mll)                cut.at(mll)=mmin;
    if(i==drEMu)              cut.at(drEMu)=0.3;
    if(i==diMuonVeto)         cut.at(diMuonVeto)=0;
    if(i==triLeptonVeto)      cut.at(triLeptonVeto)=0;
    if(i==charge)             cut.at(charge)=0;
    if(i==jetVeto)            cut.at(jetVeto)=jet_sum;
    if(i==MtMu)               cut.at(MtMu)=mtmu;
    if(i==ptBalance)          cut.at(ptBalance)=ptbalance;
    if(i==ZMassmax)           cut.at(ZMassmax)=zmax;
    if(i==ZMassmin)           cut.at(ZMassmin)=zmin;
  }

  TString hlabel;
  TString htitle;
  for(unsigned int i=0; i<NCuts; i++){
    title.push_back("");
    distindx.push_back(false);
    dist.push_back(std::vector<float>());
    Accumdist.push_back(std::vector<TH1D>());
    Nminus1dist.push_back(std::vector<TH1D>());

    TString c="_Cut_";
    if(i<10)c+="0";
    c+=i;
  
    if(i==PrimeVtx){
      title.at(i)="Number of Prime Vertices $(N>=$";
      title.at(i)+=cut.at(PrimeVtx);
      title.at(i)+=")";
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of Prime Vertices";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_PrimeVtx_",htitle,31,-0.5,30.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_PrimeVtx_",htitle,31,-0.5,30.5,hlabel,"Events"));
    }
    else if(i==TriggerOk){
      title.at(i)="Trigger ";
      hlabel="Trigger ";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_TriggerOk_",htitle,17,-0.5,16.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_TriggerOk_",htitle,17,-0.5,16.5,hlabel,"Events"));
    }

    else if(i==NMu){
      title.at(i)="Number $\\mu >=$";
      title.at(i)+=cut.at(NMu);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of #mu";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NMu_",htitle,6,-0.5,5.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NMu_",htitle,6,-0.5,5.5,hlabel,"Events"));
    }
    else if(i==NE){
      title.at(i)="Number $e >=$";
      title.at(i)+=cut.at(NE);
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="Number of e";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_NE_",htitle,6,-0.5,5.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_NE_",htitle,6,-0.5,5.5,hlabel,"Events"));
    }
    else if(i==ptthreshold){
	  title.at(i)="ptthreshold ";
	  hlabel="ptthreshold ";
	  Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_ptthreshold_",htitle,17,-0.5,16.5,hlabel,"Events"));
	  Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_ptthreshold_",htitle,17,-0.5,16.5,hlabel,"Events"));
    }
    else if(i==mll){
	  title.at(i)="$M_{e,\\mu} > $";
	  char buffer[50];
	  sprintf(buffer,"%5.2f",cut.at(mll));
	  title.at(i)+=buffer;
	  title.at(i)+="(GeV)";
	  htitle=title.at(i);
	  htitle.ReplaceAll("$","");
	  htitle.ReplaceAll("\\","#");
	  hlabel="m_{e#mu} / GeV";
	  Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_mll_",htitle,41,19,142,hlabel,"Events"));
	  Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_mll_",htitle,41,19,142,hlabel,"Events"));
      }
    else if(i==drEMu){
      title.at(i)="$\\Delta R(e,\\mu) < $";
      char buffer[50];
      sprintf(buffer,"%5.2f",cut.at(drEMu));
      title.at(i)+=buffer;
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="dR(e,mu)";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_drEMu_",htitle,50,0,5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_drEMu_",htitle,50,0,5,hlabel,"Events"));
    }
    else if(i==ptBalance){
      title.at(i)="$p_{t,e+\\mu} < $";
      title.at(i)+=cut.at(ptBalance);
      title.at(i)+="(GeV)";
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="p_t balance / GeV";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_ptBalance_",htitle,40,0,200,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_ptBalance_",htitle,40,0,200,hlabel,"Events"));
    }
    else if(i==charge){
      title.at(i)="$e-\\mu$ Charge = ";
      title.at(i)+=cut.at(charge);
      title.at(i)+="";
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="e-#mu Charge";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_charge_",htitle,21,-5.5,5.5,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_charge_",htitle,21,-5.5,5.5,hlabel,"Events"));
    }
    else if(i==ZMassmax){
      title.at(i)="$M_{e,\\mu} < $";
      char buffer[50];
      sprintf(buffer,"%5.2f",cut.at(ZMassmax));
      title.at(i)+=buffer;
      title.at(i)+="(GeV)";
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="m_{e#mu} / GeV";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_ZMassmax_",htitle,41,19,142,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_ZMassmax_",htitle,41,19,142,hlabel,"Events"));
    }
    else if(i==ZMassmin){
      title.at(i)="$M_{e,\\mu} > $";
      char buffer[50];
      sprintf(buffer,"%5.2f",cut.at(ZMassmin));
      title.at(i)+=buffer;
      title.at(i)+="(GeV)";
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="m_{e#mu} / GeV";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_ZMassmin_",htitle,41,19,142,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_ZMassmin_",htitle,41,19,142,hlabel,"Events"));
    }
	else if(i==jetVeto){
      title.at(i)="jet veto";
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="jet P_{T} / GeV";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_jetVeto_",htitle,40,0,200,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_jetVeto_",htitle,40,0,200,hlabel,"Events"));
    }
	else if(i==MtMu){
      title.at(i)="$m_{T}^{\\mu,Miss} < $";
      title.at(i)+=cut.at(MtMu);
      title.at(i)+="(GeV)";
      htitle=title.at(i);
      htitle.ReplaceAll("$","");
      htitle.ReplaceAll("\\","#");
      hlabel="m_{T}^{#mu,Miss} / GeV";
      Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_MtMu_",htitle,40,0,200,hlabel,"Events"));
      Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_MtMu_",htitle,40,0,200,hlabel,"Events"));
    }
    else if(i==diMuonVeto){
	  title.at(i)="di-muon veto";
	  htitle=title.at(i);
	  htitle.ReplaceAll("$","");
	  htitle.ReplaceAll("\\","#");
	  hlabel="dimuon veto";
	  Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_diMuonVeto_",htitle,40,0,20,hlabel,"Events"));
	  Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_diMuonVeto_",htitle,40,0,20,hlabel,"Events"));
	}
    else if(i==triLeptonVeto){
  	  title.at(i)="tri-lepton veto";
  	  htitle=title.at(i);
  	  htitle.ReplaceAll("$","");
  	  htitle.ReplaceAll("\\","#");
  	  hlabel="trilepton veto";
  	  Nminus1.push_back(HConfig.GetTH1D(Name+c+"_Nminus1_triLeptonVeto_",htitle,40,0,20,hlabel,"Events"));
  	  Nminus0.push_back(HConfig.GetTH1D(Name+c+"_Nminus0_triLeptonVeto_",htitle,40,0,20,hlabel,"Events"));
  	}
    
    // calling external files (e.g. root files for efficiencies)
    TString base = "";
	if(runtype==GRID){
		base+=std::getenv("PWD");
		base+="/Code/nehrkorn/";
	}
	else if(runtype==Local){
		//base+=Selection::splitString(std::getenv("PWD"),'/',"workdir");
		//base+="/Code/nehrkorn/";
		base+="/net/scratch_cms/institut_3b/nehrkorn/";
	}
	FRFile = new TFile(base+"FakeRates_2012_19ifb_rereco.root");
	EmbEffFile = new TFile(base+"RecHitElectronEfficiencies.root");
	MuIdEffFile = new TFile(base+"MuonEfficiencies_Run2012ReReco_53X.root");
	MuIsoEffFile = new TFile(base+"MuonEfficiencies_ISO_Run_2012ReReco_53X.root");
	ETrigIdEffFile = new TFile(base+"ElectronEfficiencies_Run2012ReReco_53X_Trig.root");
	ENonTrigIdEffFile = new TFile(base+"ElectronEfficiencies_Run2012ReReco_53X_NonTrig.root");
	TriggerEfficiencies = new TFile(base+"TriggerEfficienciesWW_TH1D.root");
	FakeRates = new TFile(base+"FakeRatesWW.root");
	ZptCorrFile = new TFile(base+"zpt_correction_2012_roch.root");
	ENonTrigIdRecoEffFile = new TFile(base+"CombinedMethod_ScaleFactors_RecoIdIsoSip.root");

	ZptCorrection = (TH1D*)(ZptCorrFile->Get("zptratio"));

	ElectronFakeRate = (TH2D*)(FRFile->Get("ElectronFakeRateHist"));
	MuonFakeRate = (TH2D*)(FRFile->Get("MuonFakeRateHist"));
	EmbEff = (TH2D*)(EmbEffFile->Get("hPtEtaSFL"));

	ElectronTrigEff = (TH2D*)(ETrigIdEffFile->Get("electronsDATAMCratio_FO_ID_ISO"));
	ElectronNonTrigEff = (TH2D*)(ENonTrigIdEffFile->Get("h_electronScaleFactor_IdIsoSip"));
	ElectronNonTrigRecoEff = (TH2D*)(ENonTrigIdRecoEffFile->Get("h_electronScaleFactor_RecoIdIsoSip"));
	MuIdEff09 = (TGraphAsymmErrors*)(MuIdEffFile->Get("DATA_over_MC_Tight_pt_abseta<0.9"));
	MuIdEff12 = (TGraphAsymmErrors*)(MuIdEffFile->Get("DATA_over_MC_Tight_pt_abseta0.9-1.2"));
	MuIdEff21 = (TGraphAsymmErrors*)(MuIdEffFile->Get("DATA_over_MC_Tight_pt_abseta1.2-2.1"));
	MuIdEff24 = (TGraphAsymmErrors*)(MuIdEffFile->Get("DATA_over_MC_Tight_pt_abseta2.1-2.4"));
	MuIsoEff09 = (TGraphAsymmErrors*)(MuIsoEffFile->Get("DATA_over_MC_combRelIsoPF04dBeta<012_Tight_pt_abseta<0.9"));
	MuIsoEff12 = (TGraphAsymmErrors*)(MuIsoEffFile->Get("DATA_over_MC_combRelIsoPF04dBeta<012_Tight_pt_abseta0.9-1.2"));
	MuIsoEff21 = (TGraphAsymmErrors*)(MuIsoEffFile->Get("DATA_over_MC_combRelIsoPF04dBeta<012_Tight_pt_abseta1.2-2.1"));
	MuIsoEff24 = (TGraphAsymmErrors*)(MuIsoEffFile->Get("DATA_over_MC_combRelIsoPF04dBeta<012_Tight_pt_abseta2.1-2.4"));

	SingleEle15 = (TH1D*)(TriggerEfficiencies->Get("SingleEle15"));
	SingleEle25 = (TH1D*)(TriggerEfficiencies->Get("SingleEle25"));
	DoubleEleLead15 = (TH1D*)(TriggerEfficiencies->Get("DoubleEleLead15"));
	DoubleEleLead25 = (TH1D*)(TriggerEfficiencies->Get("DoubleEleLead25"));
	DoubleEleTrail15 = (TH1D*)(TriggerEfficiencies->Get("DoubleEleTrail15"));
	DoubleEleTrail25 = (TH1D*)(TriggerEfficiencies->Get("DoubleEleTrail25"));
	SingleMu08 = (TH1D*)(TriggerEfficiencies->Get("SingleMu08"));
	SingleMu12 = (TH1D*)(TriggerEfficiencies->Get("SingleMu12"));
	SingleMu21 = (TH1D*)(TriggerEfficiencies->Get("SingleMu21"));
	SingleMu25 = (TH1D*)(TriggerEfficiencies->Get("SingleMu25"));
	DoubleMuLead12 = (TH1D*)(TriggerEfficiencies->Get("DoubleMuLead12"));
	DoubleMuLead21 = (TH1D*)(TriggerEfficiencies->Get("DoubleMuLead21"));
	DoubleMuLead25 = (TH1D*)(TriggerEfficiencies->Get("DoubleMuLead25"));
	DoubleMuTrail12 = (TH1D*)(TriggerEfficiencies->Get("DoubleMuTrail12"));
	DoubleMuTrail21 = (TH1D*)(TriggerEfficiencies->Get("DoubleMuTrail21"));
	DoubleMuTrail25 = (TH1D*)(TriggerEfficiencies->Get("DoubleMuTrail25"));

	EleFake1 = (TGraphAsymmErrors*)(FakeRates->Get("EleFake1"));
	EleFake15 = (TGraphAsymmErrors*)(FakeRates->Get("EleFake15"));
	EleFake2 = (TGraphAsymmErrors*)(FakeRates->Get("EleFake2"));
	EleFake25 = (TGraphAsymmErrors*)(FakeRates->Get("EleFake25"));
	MuFake1 = (TGraphAsymmErrors*)(FakeRates->Get("MuFake1"));
	MuFake15 = (TGraphAsymmErrors*)(FakeRates->Get("MuFake15"));
	MuFake2 = (TGraphAsymmErrors*)(FakeRates->Get("MuFake2"));
	MuFake25 = (TGraphAsymmErrors*)(FakeRates->Get("MuFake25"));

    //-----------
  }
  // Setup NPassed Histogams
  Npassed=HConfig.GetTH1D(Name+"_NPass","Cut Flow",NCuts+1,-1,NCuts,"Number of Accumulative Cuts Passed","Events");
  // Setup Extra Histograms
  RelIsoE=HConfig.GetTH1D(Name+"_RelIsoE","RelIsoE",20,0.,1.,"Relative Isolation of Electron");
  RelIsoMu=HConfig.GetTH1D(Name+"_RelIsoMu","RelIsoMu",20,0.,1.,"Relative Isolation of Muon");
  EPt=HConfig.GetTH1D(Name+"_PtE","PtE",40,0.,100.,"p_{T}^{e} / GeV");
  EEt=HConfig.GetTH1D(Name+"_EtE","EtE",40,0.,100.,"E_{T}^{e} / GeV");
  MuPt=HConfig.GetTH1D(Name+"_PtMu","PtMu",40,0.,100.,"p_{T}^{#mu} / GeV");
  mtMu=HConfig.GetTH1D(Name+"_mtMu","mtMu",40,0.,200.,"m_{t}^{#mu} / GeV");
  mtE=HConfig.GetTH1D(Name+"_mtE","mtE",40,0.,200.,"m_{t}^{e} / GeV");
  NJets=HConfig.GetTH1D(Name+"_NJets","NJets",20,0,20,"number of jets");
  NJetsLoose=HConfig.GetTH1D(Name+"_NJetsLoose","NJetsLoose",20,0,20,"number of jets (loose wp)");
  NJetsMedium=HConfig.GetTH1D(Name+"_NJetsMedium","NJetsMedium",20,0,20,"number of jets (medium wp)");
  NJetsTight=HConfig.GetTH1D(Name+"_NJetsTight","NJetsTight",20,0,20,"number of jets (tight wp)");
  PUJetId=HConfig.GetTH1D(Name+"_PUJetId","PUJetId",50,-1.,1.,"pu jet id discr.");
  etaMu=HConfig.GetTH1D(Name+"_etaMu","etaMu",20,-2.5,2.5,"#eta_{#mu}");
  etaE=HConfig.GetTH1D(Name+"_etaE","etaE",20,-2.5,2.5,"#eta_{e}");
  jetsum=HConfig.GetTH1D(Name+"_jetsum","jetsum",80,40.,440,"p_{T}^{leading jet}+p_{T}^{subleading jet} / GeV");
  chargesum=HConfig.GetTH1D(Name+"_chargesum","chargesum",21,-5.5,5.5,"charge sum");
  drmue=HConfig.GetTH1D(Name+"_drmue","drmue",50,0.,5.,"dR(e,#mu)");
  deltaphi=HConfig.GetTH1D(Name+"_deltaphi","deltaphi",40,0.,2,"#phi_{e#mu}");
  ptbal=HConfig.GetTH1D(Name+"_ptbal","ptbal",40,0.,200.,"p_{T}^{e#mu} / GeV");
  chargesumsigned=HConfig.GetTH1D(Name+"_chargesumsigned","chargesumsigned",21,-5.5,5.5,"charge sum");
  FirstJetPt=HConfig.GetTH1D(Name+"_FirstJetPt","FirstJetPt",40,0.,200.,"p_{T}^{hardest jet} / GeV");
  SecondJetPt=HConfig.GetTH1D(Name+"_SecondJetPt","SecondJetPt",40,0.,200.,"p_{T}^{2nd jet} / GeV");
  
  invmass_zmass=HConfig.GetTH1D(Name+"_invmass_zmass","invmass_zmass",41,19,142,"m_{e#mu} / GeV");
  invmass_ptbalance=HConfig.GetTH1D(Name+"_invmass_ptbalance","invmass_ptbalance",41,19,142,"m_{e#mu} / GeV");
  invmass_mtmu=HConfig.GetTH1D(Name+"_invmass_mtmu","invmass_mtmu",41,19,142,"m_{e#mu} / GeV");
  invmass_jetveto=HConfig.GetTH1D(Name+"_invmass_jetveto","invmass_jetveto",41,19,142,"m_{e#mu} / GeV");
  invmass_vetos=HConfig.GetTH1D(Name+"_invmass_vetos","invmass_vetos",41,19,142,"m_{e#mu} / GeV");
  invmass_only_object_id=HConfig.GetTH1D(Name+"_invmass_only_object_id","invmass_only_object_id",41,19,142,"m_{e#mu} / GeV");

  invmass_zmass_m=HConfig.GetTH1D(Name+"_invmass_zmass_m","invmass_zmass_m_m",20,60,120,"m_{e#mu} / GeV");
  invmass_ptbalance_m=HConfig.GetTH1D(Name+"_invmass_ptbalance_m","invmass_ptbalance_m",20,60,120,"m_{e#mu} / GeV");
  invmass_mtmu_m=HConfig.GetTH1D(Name+"_invmass_mtmu_m","invmass_mtmu_m",20,60,120,"m_{e#mu} / GeV");
  invmass_jetveto_m=HConfig.GetTH1D(Name+"_invmass_jetveto_m","invmass_jetveto_m",20,60,120,"m_{e#mu} / GeV");
  invmass_vetos_m=HConfig.GetTH1D(Name+"_invmass_vetos_m","invmass_vetos_m",20,60,120,"m_{e#mu} / GeV");
  invmass_only_object_id_m=HConfig.GetTH1D(Name+"_invmass_only_object_id_m","invmass_only_object_id_m",20,60,120,"m_{e#mu} / GeV");

  invmass_dremu_only=HConfig.GetTH1D(Name+"_invmass_dremu_only","invmass_dremu_only",41,19,142,"m_{e#mu} / GeV");
  invmass_dimuon_only=HConfig.GetTH1D(Name+"_invmass_dimuon_only","invmass_dimuon_only",41,19,142,"m_{e#mu} / GeV");
  invmass_trilepton_only=HConfig.GetTH1D(Name+"_invmass_trilepton_only","invmass_trilepton_only",41,19,142,"m_{e#mu} / GeV");
  invmass_charge_only=HConfig.GetTH1D(Name+"_invmass_charge_only","invmass_charge_only",41,19,142,"m_{e#mu} / GeV");
  invmass_jetveto_only=HConfig.GetTH1D(Name+"_invmass_jetveto_only","invmass_jetveto_only",41,19,142,"m_{e#mu} / GeV");
  invmass_mtmu_only=HConfig.GetTH1D(Name+"_invmass_mtmu_only","invmass_mtmu_only",41,19,142,"m_{e#mu} / GeV");
  invmass_ptbal_only=HConfig.GetTH1D(Name+"_invmass_ptbal_only","invmass_ptbal_only",41,19,142,"m_{e#mu} / GeV");

  nm0_met=HConfig.GetTH1D(Name+"_nm0_met","nm0_met",30,0.,150.,"MET / GeV");
  nm0_jetsum=HConfig.GetTH1D(Name+"_nm0_jetsum","nm0_jetsum",80,0.,400,"p_{T}^{leading jet}+p_{T}^{subleading jet} / GeV");
  nm0_onejet=HConfig.GetTH1D(Name+"_nm0_onejet","nm0_onejet",40,0.,200.,"p_{T}^{jet} / GeV");
  nm0_mtmu=HConfig.GetTH1D(Name+"_nm0_mtmu","nm0_mtmu",40,0.,160.,"m_{t}^{#mu} / GeV");
  nm0_ptbalance=HConfig.GetTH1D(Name+"_nm0_ptbalance","nm0_ptbalance",40,0.,200.,"p_{T}^{e#mu} / GeV");
  
  NPV=HConfig.GetTH1D(Name+"_NPV","NPV",60,0.,60.,"number of vertices");
  NPV3d=HConfig.GetTH1D(Name+"_NPV3d","NPV3d",60,0.,60.,"number of vertices");
  NPVfine=HConfig.GetTH1D(Name+"_NPVfine","NPVfine",60,0.,60.,"number of vertices");
  evtweight=HConfig.GetTH1D(Name+"_evtweight","evtweight",100,-0.1,2.1,"puweight");
  
  met=HConfig.GetTH1D(Name+"_met","met",30,0.,150.,"MET / GeV");
  met_uncorr=HConfig.GetTH1D(Name+"_met_uncorr","met_uncorr",30,0.,150.,"uncorrected MET / GeV");
  onejet=HConfig.GetTH1D(Name+"_onejet","onejet",40,0.,200.,"p_{T}^{jet} / GeV");
  mte_mtmu=HConfig.GetTH1D(Name+"_mte_mtmu","mte_mtmu",40,0.,200.,"m_{t}^{e} / GeV");
  NbJets=HConfig.GetTH1D(Name+"_NbJets","NbJets",20,0,20,"number of b jets");
  NbJetsVtxL=HConfig.GetTH1D(Name+"_NbJetsVtxL","NbJetsVtxL",20,0,20,"number of b jets from vtx loose");
  NbJetsVtxM=HConfig.GetTH1D(Name+"_NbJetsVtxM","NbJetsVtxM",20,0,20,"number of b jets from vtx medium");
  NbJetsVtxT=HConfig.GetTH1D(Name+"_NbJetsVtxT","NbJetsVtxT",20,0,20,"number of b jets from vtx tight");
  
  zpt=HConfig.GetTH1D(Name+"_zpt","zpt",12,0.,30.,"p_{t}^{Z}");
  zeta=HConfig.GetTH1D(Name+"_zeta","zeta",40,-5.,5.,"#eta_{Z}");
  zmass=HConfig.GetTH1D(Name+"_zmass","zmass",20,60.,120.,"m_{Z}");
  leadingjet_pt=HConfig.GetTH1D(Name+"_leadingjet_pt","leadingjet_pt",40,0.,200.,"p_{T}^{leading jet} / GeV");
  subleadingjet_pt=HConfig.GetTH1D(Name+"_subleadingjet_pt","subleadingjet_pt",40,0.,200.,"p_{T}^{subleading jet} / GeV");
  leadingjet_eta=HConfig.GetTH1D(Name+"_leadingjet_eta","leadingjet_eta",40,-5.,5.,"#eta_{leading jet}");
  subleadingjet_eta=HConfig.GetTH1D(Name+"_subleadingjet_eta","subleadingjet_eta",40,-5.,5.,"#eta_{subleading jet}");
  jetsumcustom=HConfig.GetTH1D(Name+"_jetsumcustom","jetsumcustom",80,0.,400,"p_{T}^{leading jet}+p_{T}^{subleading jet} / GeV");

  ptbal_chargepass=HConfig.GetTH1D(Name+"_ptbal_chargepass","ptbal_chargepass",40,0.,200.,"p^{e,#mu}_{T} / GeV");
  ptbal_chargefail=HConfig.GetTH1D(Name+"_ptbal_chargefail","ptbal_chargefail",40,0.,200.,"p^{e,#mu}_{T} / GeV");

  double exbins[11] = {-2.5,-2.,-1.566,-1.442,-0.8,0.,0.8,1.442,1.566,2.,2.5};
  etaE_offBins=HConfig.GetTH1D(Name+"_etaE_offBins","etaE_offBins",10,exbins,"#eta_{e}");
  double muxbins[9] = {-2.4,-2.1,-1.2,-0.8,0.,0.8,1.2,2.1,2.4};
  etaMu_offBins=HConfig.GetTH1D(Name+"_etaMu_offBins","etaMu_offBins",8,muxbins,"#eta_{#mu}");

  etaE_manyBins=HConfig.GetTH1D(Name+"_etaE_manyBins","etaE_manyBins",50,-2.5,2.5,"#eta_{e}");
  etaMu_manyBins=HConfig.GetTH1D(Name+"_etaMu_manyBins","etaMu_manyBins",50,-2.5,2.5,"#eta_{#mu}");

  mtmu_metgr30=HConfig.GetTH1D(Name+"_mtmu_metgr30","mtmu_metgr30",40,0.,200.,"m_{t}^{#mu} / GeV");
  mtmu_metsm30=HConfig.GetTH1D(Name+"_mtmu_metsm30","mtmu_metsm30",40,0.,200.,"m_{t}^{#mu} / GeV");
  jet1E=HConfig.GetTH1D(Name+"_jet1E","jet1E",50,0.,5.,"#Delta(jet1,e)");
  jet2E=HConfig.GetTH1D(Name+"_jet2E","jet2E",50,0.,5.,"#Delta(jet2,e)");
  jet1Mu=HConfig.GetTH1D(Name+"_jet1Mu","jet1Mu",50,0.,5.,"#Delta(jet1,#mu)");
  jet2Mu=HConfig.GetTH1D(Name+"_jet2Mu","jet2Mu",50,0.,5.,"#Delta(jet2,#mu)");

  Dxy_trig=HConfig.GetTH1D(Name+"_Dxy_trig","Dxy_trig",50,0.,1.,"d_{xy}");
  Dz_trig=HConfig.GetTH1D(Name+"_Dz_trig","Dz_trig",50,0.,1.,"d_{z}");
  Dxy_nontrig=HConfig.GetTH1D(Name+"_Dxy_nontrig","Dxy_nontrig",50,0.,1.,"d_{xy}");
  Dz_nontrig=HConfig.GetTH1D(Name+"_Dz_nontrig","Dz_nontrig",50,0.,1.,"d_{z}");
  Dxy_trignoip=HConfig.GetTH1D(Name+"_Dxy_trignoip","Dxy_trignoip",50,0.,1.,"d_{xy}");
  Dz_trignoip=HConfig.GetTH1D(Name+"_Dz_trignoip","Dz_trignoip",50,0.,1.,"d_{z}");
  eta_mu_e=HConfig.GetTH2D(Name+"_eta_mu_e","eta_mu_e",20,-2.5,2.5,20,-2.5,2.5,"#eta_{#mu}","#eta_{e}");
  pt_vs_eta_mu=HConfig.GetTH2D(Name+"_pt_vs_eta_mu","pt_vs_eta_mu",40,0.,100.,20,-2.5,2.5,"p_{T}^{#mu} / GeV","#eta_{#mu}");
  pt_vs_eta_e=HConfig.GetTH2D(Name+"_pt_vs_eta_e","pt_vs_eta_e",40,0.,100.,20,-2.5,2.5,"p_{T}^{e} / GeV","#eta_{e}");

  Selection::ConfigureHistograms();
  HConfig.GetHistoInfo(types,CrossSectionandAcceptance,legend,colour);
  for(int i=0;i<CrossSectionandAcceptance.size();i++){
    std::cout << i << " CrossSectionandAcceptance " << CrossSectionandAcceptance.at(i) << std::endl;
  }
}




void  ZtoEMu::Store_ExtraDist(){
 Extradist1d.push_back(&RelIsoE);
 Extradist1d.push_back(&RelIsoMu);
 Extradist1d.push_back(&EPt);
 Extradist1d.push_back(&EEt);
 Extradist1d.push_back(&MuPt);
 Extradist1d.push_back(&mtMu);
 Extradist1d.push_back(&mtE);
 Extradist1d.push_back(&NJets);
 Extradist1d.push_back(&NJetsLoose);
 Extradist1d.push_back(&NJetsMedium);
 Extradist1d.push_back(&NJetsTight);
 Extradist1d.push_back(&PUJetId);
 Extradist1d.push_back(&etaE);
 Extradist1d.push_back(&etaMu);
 Extradist1d.push_back(&jetsum);
 Extradist1d.push_back(&chargesum);
 Extradist1d.push_back(&drmue);
 Extradist1d.push_back(&deltaphi);
 Extradist1d.push_back(&ptbal);
 Extradist1d.push_back(&chargesumsigned);
 Extradist1d.push_back(&FirstJetPt);
 Extradist1d.push_back(&SecondJetPt);
 
 Extradist1d.push_back(&invmass_zmass);
 Extradist1d.push_back(&invmass_ptbalance);
 Extradist1d.push_back(&invmass_mtmu);
 Extradist1d.push_back(&invmass_jetveto);
 Extradist1d.push_back(&invmass_vetos);
 Extradist1d.push_back(&invmass_only_object_id);

 Extradist1d.push_back(&invmass_zmass_m);
 Extradist1d.push_back(&invmass_ptbalance_m);
 Extradist1d.push_back(&invmass_mtmu_m);
 Extradist1d.push_back(&invmass_jetveto_m);
 Extradist1d.push_back(&invmass_vetos_m);
 Extradist1d.push_back(&invmass_only_object_id_m);

 Extradist1d.push_back(&invmass_dremu_only);
 Extradist1d.push_back(&invmass_dimuon_only);
 Extradist1d.push_back(&invmass_trilepton_only);
 Extradist1d.push_back(&invmass_charge_only);
 Extradist1d.push_back(&invmass_jetveto_only);
 Extradist1d.push_back(&invmass_mtmu_only);
 Extradist1d.push_back(&invmass_ptbal_only);

 Extradist1d.push_back(&nm0_met);
 Extradist1d.push_back(&nm0_jetsum);
 Extradist1d.push_back(&nm0_onejet);
 Extradist1d.push_back(&nm0_mtmu);
 Extradist1d.push_back(&nm0_ptbalance);
 
 Extradist1d.push_back(&NPV);
 Extradist1d.push_back(&NPV3d);
 Extradist1d.push_back(&NPVfine);
 Extradist1d.push_back(&evtweight);
 
 Extradist1d.push_back(&met);
 Extradist1d.push_back(&met_uncorr);
 Extradist1d.push_back(&onejet);
 Extradist1d.push_back(&mte_mtmu);
 Extradist1d.push_back(&NbJets);
 Extradist1d.push_back(&NbJetsVtxL);
 Extradist1d.push_back(&NbJetsVtxM);
 Extradist1d.push_back(&NbJetsVtxT);

 Extradist1d.push_back(&zpt);
 Extradist1d.push_back(&zeta);
 Extradist1d.push_back(&zmass);
 Extradist1d.push_back(&leadingjet_pt);
 Extradist1d.push_back(&subleadingjet_pt);
 Extradist1d.push_back(&leadingjet_eta);
 Extradist1d.push_back(&subleadingjet_eta);
 Extradist1d.push_back(&jetsumcustom);

 Extradist1d.push_back(&ptbal_chargepass);
 Extradist1d.push_back(&ptbal_chargefail);

 Extradist1d.push_back(&etaE_offBins);
 Extradist1d.push_back(&etaE_manyBins);
 Extradist1d.push_back(&etaMu_offBins);
 Extradist1d.push_back(&etaMu_manyBins);
 Extradist1d.push_back(&mtmu_metgr30);
 Extradist1d.push_back(&mtmu_metsm30);
 Extradist1d.push_back(&jet1E);
 Extradist1d.push_back(&jet2E);
 Extradist1d.push_back(&jet1Mu);
 Extradist1d.push_back(&jet2Mu);

 Extradist1d.push_back(&Dxy_trig);
 Extradist1d.push_back(&Dz_trig);
 Extradist1d.push_back(&Dxy_nontrig);
 Extradist1d.push_back(&Dz_nontrig);
 Extradist1d.push_back(&Dxy_trignoip);
 Extradist1d.push_back(&Dz_trignoip);
 Extradist2d.push_back(&eta_mu_e);
 Extradist2d.push_back(&pt_vs_eta_mu);
 Extradist2d.push_back(&pt_vs_eta_e);

}

void  ZtoEMu::doEvent(){
  if(verbose)std::cout << "ZtoEMu::doEvent() START" << std::endl;
  unsigned int t;
  int id(Ntp->GetMCID());
  if(verbose)std::cout << "id: " << id << std::endl;
  if(!HConfig.GetHisto(Ntp->isData(),id,t)){ std::cout << "failed to find id" << std::endl; return;}
  
  // duplicate lorentz vectors
 /* std::vector<TLorentzVector> muons;
  std::vector<TLorentzVector> electrons;
  muons.clear();
  electrons.clear();
  //muonres=mures->GetRandom();
  //eleres = eres->GetRandom();
  TLorentzVector muon;
  TLorentzVector electron;
  for(unsigned i=0;i<Ntp->NMuons();i++){
	  muon = Ntp->Muon_p4(i);
	  //muon.SetPerp(Ntp->Muon_p4(i).Perp()+muonres);
	  //muon.SetPerp(Ntp->Muon_p4(i).Perp());
	  muons.push_back(muon);
  }
  for(unsigned i=0;i<Ntp->NElectrons();i++){
	  electron = Ntp->Electron_p4(i);
	  //electron.SetPerp(Ntp->Electron_p4(i).Perp()-eleres);
	  electrons.push_back(Ntp->Electron_p4(i));
  }*/

  ///////////////////////////////////////////////
  //
  // Trigger passed?
  //
  if(verbose)std::cout << "Trigger passed?" << std::endl;
  value.at(TriggerOk)=0;
  if(Ntp->TriggerAccept("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v") || Ntp->TriggerAccept("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v")){
	  value.at(TriggerOk)=1;
  }
  if(Ntp->GetMCID()==DataMCType::DY_emu_embedded)value.at(TriggerOk)=1;
  pass.at(TriggerOk)=(value.at(TriggerOk)==cut.at(TriggerOk));

  // Apply Selection
    
  ///////////////////////////////////////////////
  //
  // Vertex selection
  //
  if(verbose)std::cout << "Vertex selection" << std::endl;
  unsigned int nGoodVtx=0;
  int vertex = -1;
  for(unsigned i=0;i<Ntp->NVtx();i++){
	  if(isGoodVtx(i)){
		  if(vertex==-1)vertex=i;
		  nGoodVtx++;
	  }
  }
  value.at(PrimeVtx)=nGoodVtx;
  pass.at(PrimeVtx)=(value.at(PrimeVtx)>=cut.at(PrimeVtx));

  ///////////////////////////////////////////////
  //
  // Mu Cuts
  //
  if(verbose) std::cout << "Muon cuts" << std::endl;
  std::vector<unsigned int> GoodMuons;
  std::vector<unsigned int> Fakemuons;
  
  // muon ID cuts
  for(unsigned i=0;i<Ntp->NMuons();i++){
	  if(Ntp->Muon_p4(i).Pt()>mu_ptlow
			  && fabs(Ntp->Muon_p4(i).Eta())<mu_eta
			  && vertex>=0
			  && (matchTrigger(i,0.2,"HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v","muon") || matchTrigger(i,0.2,"HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v","muon") || Ntp->GetMCID()==DataMCType::DY_emu_embedded)
			  ){
		  if(doHiggsObjects){
			  if(isHiggsMuon(i,vertex)
					  && ((fabs(Ntp->Muon_p4(i).Eta())<1.479 && Muon_RelIso(i)<0.15) || (fabs(Ntp->Muon_p4(i).Eta())>=1.479 && Muon_RelIso(i)<0.10))
							  ){
				  GoodMuons.push_back(i);
			  }else if(isFakeMuon(i,vertex)
					  && (Ntp->isData() || Ntp->GetMCID()==DataMCType::DY_ee || Ntp->GetMCID()==DataMCType::DY_mumu || Ntp->GetMCID()==DataMCType::DY_tautau || Ntp->GetMCID()==DataMCType::DY_ll)
					  ){
				  Fakemuons.push_back(i);
				  GoodMuons.push_back(i);
			  }
		  }else{
			  if(isTightMuon(i,vertex)
					  && Muon_RelIso(i)<0.12
					  ){
				  GoodMuons.push_back(i);
			  }else if(doWWObjects
					  && isFakeMuon(i,vertex)
					  && (Ntp->isData() || Ntp->GetMCID()==DataMCType::DY_ee || Ntp->GetMCID()==DataMCType::DY_mumu || Ntp->GetMCID()==DataMCType::DY_tautau || Ntp->GetMCID()==DataMCType::DY_ll)
					  ){
				  Fakemuons.push_back(i);
				  GoodMuons.push_back(i);
			  }
		  }
	  }
  }
  
  value.at(NMu)=GoodMuons.size();
  pass.at(NMu)=(value.at(NMu)>=cut.at(NMu));
  
  unsigned int muidx(999);
  double hardestmu(0);
  if(GoodMuons.size()>1){
	  for(unsigned i=0;i<GoodMuons.size();i++){
		  if(Ntp->Muon_p4(GoodMuons.at(i)).Pt()>hardestmu){
			  hardestmu = Ntp->Muon_p4(GoodMuons.at(i)).Pt();
			  muidx = GoodMuons.at(i);
		  }
	  }
  }
  if(GoodMuons.size()==1){muidx=GoodMuons.at(0);}

  ///////////////////////////////////////////////
  //
  // E Cuts
  //
  if(verbose) std::cout << "electrons cuts" << std::endl;
  std::vector<unsigned int> GoodElectrons;
  std::vector<unsigned int> Fakeelectrons;
  bool hasMuonTrack = false;
  bool matchRecoMuon = false;
  
  // electron ID cuts (eta-dependent MVA or simple cut-based)
  for(unsigned i=0;i<Ntp->NElectrons();i++){
	  hasMuonTrack = false;
	  matchRecoMuon = false;
	  if(Ntp->Electron_p4(i).Et()>e_ptlow
			  && fabs(Ntp->Electron_supercluster_eta(i))<e_eta
			  && vertex>=0
			  && (matchTrigger(i,0.2,"HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v","electron") || matchTrigger(i,0.2,"HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v","electron") || Ntp->GetMCID()==DataMCType::DY_emu_embedded)
			  ){
		  // don't use electrons with tracks matching those of selected muons
		  for(unsigned j=0;j<GoodMuons.size();j++){
			  if(Ntp->Electron_Track_idx(i)==Ntp->Muon_Track_idx(GoodMuons.at(j))) hasMuonTrack = true;
		  }
		  if(hasMuonTrack) continue;
		  // no overlapping reco muons
		  for(unsigned j=0;j<Ntp->NMuons();j++){
			  if(j==muidx)continue;
			  if(Ntp->Electron_p4(i).DeltaR(Ntp->Muon_p4(j))<0.3) matchRecoMuon = true;
		  }
		  if(matchRecoMuon) continue;
		  if(doHiggsObjects){
			  if(isHiggsElectron(i,vertex)
					  && ((fabs(Ntp->Electron_supercluster_eta(i))<1.479 && Electron_RelIso(i)<0.15) || (fabs(Ntp->Electron_supercluster_eta(i))>=1.479 && Electron_RelIso(i)<0.10))
					  ){
				  GoodElectrons.push_back(i);
			  }else if(isFakeElectron(i,vertex)
					  && (Ntp->isData() || Ntp->GetMCID()==DataMCType::DY_ee || Ntp->GetMCID()==DataMCType::DY_mumu || Ntp->GetMCID()==DataMCType::DY_tautau || Ntp->GetMCID()==DataMCType::DY_ll)
					  ){
				  Fakeelectrons.push_back(i);
				  GoodElectrons.push_back(i);
			  }
		  }else{
			  if(isWWElectron(i,vertex)
					  && Electron_RelIso(i)<0.15
					  ){
				  GoodElectrons.push_back(i);
			  }else if(doWWObjects
					  && isFakeElectron(i,vertex)
					  && (Ntp->isData() || Ntp->GetMCID()==DataMCType::DY_ee || Ntp->GetMCID()==DataMCType::DY_mumu || Ntp->GetMCID()==DataMCType::DY_tautau || Ntp->GetMCID()==DataMCType::DY_ll)
					  ){
				  Fakeelectrons.push_back(i);
				  GoodElectrons.push_back(i);
			  }
		  }
	  }
  }
  
  value.at(NE)=GoodElectrons.size();
  pass.at(NE)=(value.at(NE)>=cut.at(NE));
  
  unsigned int eidx(999);
  double hardeste(0);
  if(GoodElectrons.size()>1){
	  for(unsigned i=0;i<GoodElectrons.size();i++){
		  if(Ntp->Electron_p4(GoodElectrons.at(i)).Et()>hardeste){
			  hardeste = Ntp->Electron_p4(GoodElectrons.at(i)).Et();
			  eidx = GoodElectrons.at(i);
		  }
	  }
  }
  if(GoodElectrons.size()==1){eidx=GoodElectrons.at(0);}
  
  ///////////////////////////////////////////////
  //
  // pt thresholds
  //
  if(verbose) std::cout << "Setting pt thresholds" << std::endl;
  bool passembed = false;
  bool leadingmu = false;
  value.at(ptthreshold)=0;
  if(muidx!=999 && eidx!=999){
	  value.at(ptthreshold)=1;
	  if(Ntp->Muon_p4(muidx).Pt()<=mu_ptlow || Ntp->Electron_p4(eidx).Et()<=e_ptlow) value.at(ptthreshold)=0;
	  if(Ntp->Muon_p4(muidx).Pt()<mu_pthigh && Ntp->Electron_p4(eidx).Et()<e_pthigh) value.at(ptthreshold)=0;
	  if(value.at(ptthreshold)==1 && Ntp->GetMCID()==DataMCType::DY_emu_embedded) passembed = true;
	  if(Ntp->Muon_p4(muidx).Pt()<mu_pthigh){
		  if(!Ntp->TriggerAccept("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v")) value.at(ptthreshold)=0;
	  }else if(Ntp->Electron_p4(eidx).Et()<e_pthigh){
		  if(!Ntp->TriggerAccept("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v")) value.at(ptthreshold)=0;
		  else leadingmu = true;
	  }
	  if(passembed) value.at(ptthreshold)=1;
  }
  pass.at(ptthreshold)=(value.at(ptthreshold)==cut.at(ptthreshold));

  ///////////////////////////////////////////////
  //
  // m(ll)
  //
  if(verbose) std::cout << "m(ll)" << std::endl;
  value.at(mll)=mmin+1;
  if(muidx!=999 && eidx!=999){
	  value.at(mll)=(Ntp->Muon_p4(muidx)+Ntp->Electron_p4(eidx)).M();
  }
  pass.at(mll)=(value.at(mll)>cut.at(mll));

  ///////////////////////////////////////////////
  //
  // dR(e,mu)
  //
  if(verbose) std::cout << "dR(e,mu)" << std::endl;
  value.at(drEMu)=0;
  if(muidx!=999 && eidx!=999){
	  value.at(drEMu)=Ntp->Muon_p4(muidx).DeltaR(Ntp->Electron_p4(eidx));
  }
  pass.at(drEMu)=(value.at(drEMu)>cut.at(drEMu));

  ///////////////////////////////////////////////
  //
  // Di Muon Veto
  //
  if(verbose)std::cout << "dimuon veto" << std::endl;
  unsigned int dimu(0);
  if(eidx!=999 && muidx!=999){
	  for(unsigned i=0;i<Ntp->NMuons();i++){
		  if(i==muidx) continue;
		  if(Ntp->Muon_p4(i).Pt()<3) continue;
		  if(fabs(Ntp->Muon_p4(i).Eta())>2.4) continue;
		  if(Ntp->Electron_p4(eidx).DeltaR(Ntp->Muon_p4(i))>0.3) continue;
		  dimu++;
	  }
  }
  value.at(diMuonVeto)=dimu;
  pass.at(diMuonVeto)=(value.at(diMuonVeto)==cut.at(diMuonVeto));
  
  ///////////////////////////////////////////////
  //
  // Tri Lepton Veto
  //
  if(verbose)std::cout << "trilepton veto" << std::endl;
  unsigned int trilep(0);
  if(eidx!=999 && muidx!=999){
	  for(unsigned i=0;i<Ntp->NMuons();i++){
		  if(i==muidx) continue;
		  if(vertex<0) continue;
		  if(Ntp->Muon_p4(i).Pt()<10) continue;
		  if(fabs(Ntp->Muon_p4(i).Eta())>2.4) continue;
		  if(!isTightMuon(i,vertex)) continue;
		  if(Muon_RelIso(i)>0.3) continue;
		  if(doHiggsObjects){
			  if(dxy(Ntp->Muon_p4(i),Ntp->Muon_Poca(i),Ntp->Vtx(vertex))<0.045
					  && dz(Ntp->Muon_p4(i),Ntp->Muon_Poca(i),Ntp->Vtx(vertex))<0.2
					  ){
				  trilep++;
			  }
		  }else{
			  trilep++;
		  }
	  }
	  for(unsigned i=0;i<Ntp->NElectrons();i++){
		  if(i==eidx) continue;
		  if(vertex<0) continue;
		  if(Ntp->Electron_p4(i).Et()<10) continue;
		  if(fabs(Ntp->Electron_supercluster_eta(i))>2.5) continue;
		  if(doHiggsObjects){
			  if(isHiggsElectron(i,vertex)
					  && Electron_RelIso(i)<0.3
					  && dxy(Ntp->Electron_p4(i),Ntp->Electron_Poca(i),Ntp->Vtx(vertex))<0.045
					  && dz(Ntp->Electron_p4(i),Ntp->Electron_Poca(i),Ntp->Vtx(vertex))<0.2
					  ){
				  trilep++;
			  }
		  }else{
			  if(isWWElectron(i,vertex)
					  && Electron_RelIso(i)<0.3
					  ){
				  trilep++;
			  }
		  }
	  }
  }
  value.at(triLeptonVeto)=trilep;
  pass.at(triLeptonVeto)=(value.at(triLeptonVeto)==cut.at(triLeptonVeto));
  
  ///////////////////////////////////////////////
  //
  // charge cut
  //
  if(verbose) std::cout << "Charge cut" << std::endl;
  value.at(charge)=-1;
  if(eidx!=999 && muidx!=999){
	  value.at(charge)=Ntp->Electron_Charge(eidx)*Ntp->Muon_Charge(muidx);
	  if(Ntp->Electron_Charge(eidx)==-999 || Ntp->Muon_Charge(muidx)==-999) value.at(charge)=1.;
  }
  pass.at(charge)=(value.at(charge)<cut.at(charge));

  ////////////////////////////////////////////////
  //
  // QCD
  //
  if(verbose) std::cout << "QCD" << std::endl;
  fakeRate = 1.;
  bool fakemu = false;
  bool fakee = false;
  if(doHiggsObjects || doWWObjects){
	  for(unsigned i=0;i<Fakemuons.size();i++){
		  if(Fakemuons.at(i)==muidx){
			  fakemu=true;
			  if(doHiggsObjects) fakeRateMu = Fakerate(Ntp->Muon_p4(muidx),MuonFakeRate,"muon");
			  else if(doWWObjects) fakeRateMu = FakerateWW(muidx,"muon");
			  break;
		  }
	  }
	  for(unsigned i=0;i<Fakeelectrons.size();i++){
		  if(Fakeelectrons.at(i)==eidx){
			  fakee=true;
			  if(doHiggsObjects) fakeRateE = Fakerate(Ntp->Electron_p4(eidx),ElectronFakeRate,"electron");
			  else if(doWWObjects) fakeRateE = FakerateWW(eidx,"electron");
			  break;
		  }
	  }
	  if(pass.at(charge)
			  && (Ntp->isData() || Ntp->GetMCID()==DataMCType::DY_ee || Ntp->GetMCID()==DataMCType::DY_mumu || Ntp->GetMCID()==DataMCType::DY_tautau || Ntp->GetMCID()==DataMCType::DY_ll)
			  ){
		  if(fakemu || fakee) fakeRate = 0.;
	  }
	  if(!pass.at(charge)
			  && Ntp->isData()
			  ){
		  if(fakemu && !fakee){
			  fakeRate = fakeRateMu;
			  if(!HConfig.GetHisto(!Ntp->isData(),DataMCType::QCD,t)){ std::cout << "failed to find id "<< DataMCType::QCD <<std::endl; return;}
			  pass.at(charge)=true;
		  }else if(fakee && !fakemu){
			  fakeRate = fakeRateE;
			  if(!HConfig.GetHisto(!Ntp->isData(),DataMCType::QCD,t)){ std::cout << "failed to find id "<< DataMCType::QCD <<std::endl; return;}
			  pass.at(charge)=true;
		  }else if(fakemu && fakee){
			  fakeRate = fakeRateMu*fakeRateE;
			  if(!HConfig.GetHisto(!Ntp->isData(),DataMCType::QCD,t)){ std::cout << "failed to find id "<< DataMCType::QCD <<std::endl; return;}
			  pass.at(charge)=true;
		  }
	  }
	  if(!pass.at(charge)
			  && !Ntp->isData()
			  && (Ntp->GetMCID()==DataMCType::DY_ee || Ntp->GetMCID()==DataMCType::DY_mumu || Ntp->GetMCID()==DataMCType::DY_tautau || Ntp->GetMCID()==DataMCType::DY_ll)
			  ){
		  if(fakemu && !fakee){
			  fakeRate = -fakeRateMu;
			  if(!HConfig.GetHisto(Ntp->isData(),DataMCType::QCD,t)){ std::cout << "failed to find id "<< DataMCType::QCD <<std::endl; return;}
			  pass.at(charge)=true;
		  }else if(fakee && !fakemu){
			  fakeRate = -fakeRateE;
			  if(!HConfig.GetHisto(Ntp->isData(),DataMCType::QCD,t)){ std::cout << "failed to find id "<< DataMCType::QCD <<std::endl; return;}
			  pass.at(charge)=true;
		  }else if(fakemu && fakee){
			  fakeRate = -fakeRateMu*fakeRateE;
			  if(!HConfig.GetHisto(Ntp->isData(),DataMCType::QCD,t)){ std::cout << "failed to find id "<< DataMCType::QCD <<std::endl; return;}
			  pass.at(charge)=true;
		  }
	  }
	  if(fabs(fakeRate)>0 && fabs(fakeRate)<1) fakeRate*=0.83;
  }


  ///////////////////////////////////////////////
  //
  // jet veto
  //
  if(verbose)std::cout << "jet veto" << std::endl;
  if(verbose)std::cout << "Cleaning jets" << std::endl;
  bool etrackjet(false);
  bool mutrackjet(false);
  std::vector<int> jetsfromvtx;

  if(verbose)std::cout << "Finding jets from vtx" << std::endl;
  for(unsigned i=0;i<Ntp->NPFJets();i++){
	  mutrackjet = false;
	  etrackjet = false;
	  // clean jets against signal objects
	  if(Ntp->PFJet_p4(i).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(i).Eta(),Ntp->RunNumber())<20) continue;
	  if(fabs(Ntp->PFJet_p4(i).Eta())>jet_eta) continue; // TODO: restrict eta to 2.5?
	  if(muidx!=999){
		  if(Ntp->PFJet_p4(i).DeltaR(Ntp->Muon_p4(muidx))<0.3) mutrackjet = true;
	  }
	  if(eidx!=999){
		  if(Ntp->PFJet_p4(i).DeltaR(Ntp->Electron_p4(eidx))<0.3) etrackjet = true;
	  }
	  if(etrackjet || mutrackjet) continue;
	  // find jets from vertex: use pileup jet id for jets with pt>20 GeV
	  if(Ntp->PFJet_PUJetID_tightWP(i)>0.5) jetsfromvtx.push_back(i); // TODO: choose other WP?
  }

  if(verbose)std::cout<< "Find two highest pt jets" << std::endl;
  int firstjet_idx=-1;
  int secondjet_idx=-1;
  double initialpt=0.;

  // loop over jets from selected vertex & find the two jets with the highest pt
  for(unsigned i=0;i<jetsfromvtx.size();i++){
	  if(Ntp->PFJet_p4(jetsfromvtx.at(i)).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(jetsfromvtx.at(i)).Eta(),Ntp->RunNumber())>initialpt){
		  initialpt=Ntp->PFJet_p4(jetsfromvtx.at(i)).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(jetsfromvtx.at(i)).Eta(),Ntp->RunNumber());
		  firstjet_idx=jetsfromvtx.at(i);
	  }
  }
  initialpt=0.;
  for(unsigned i=0;i<jetsfromvtx.size();i++){
	  if(jetsfromvtx.size()>1 && firstjet_idx!=-1
			  && Ntp->PFJet_p4(jetsfromvtx.at(i)).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(jetsfromvtx.at(i)).Eta(),Ntp->RunNumber())>initialpt
			  && Ntp->PFJet_p4(jetsfromvtx.at(i)).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(jetsfromvtx.at(i)).Eta(),Ntp->RunNumber())<Ntp->PFJet_p4(firstjet_idx).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(firstjet_idx).Eta(),Ntp->RunNumber())
			  ){
		  initialpt=Ntp->PFJet_p4(jetsfromvtx.at(i)).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(jetsfromvtx.at(i)).Eta(),Ntp->RunNumber());
		  secondjet_idx=jetsfromvtx.at(i);
	  }
  }

  if(verbose)std::cout << "applying veto" << std::endl;
  value.at(jetVeto)=0;
  if(jetsfromvtx.size()>1 && firstjet_idx!=-1 && secondjet_idx!=-1){
	  value.at(jetVeto)=Ntp->PFJet_p4(firstjet_idx).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(firstjet_idx).Eta(),Ntp->RunNumber())+Ntp->PFJet_p4(secondjet_idx).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(secondjet_idx).Eta(),Ntp->RunNumber());
  }else if(jetsfromvtx.size()==1 && firstjet_idx!=-1){
	  value.at(jetVeto)=Ntp->PFJet_p4(firstjet_idx).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(firstjet_idx).Eta(),Ntp->RunNumber());
	  cut.at(jetVeto)=40;
  }
  /*int nb(0),nj(0);
  for(unsigned i=0;i<Ntp->NPFJets();i++){
	  if(Ntp->PFJet_bDiscriminator(i)>csvl) nb++;
	  if(Ntp->PFJet_PUJetID_looseWP(i)>0.5) nj++;
  }
  pass.at(jetVeto)=(nb==0 && nj<4);*/
  pass.at(jetVeto)=(value.at(jetVeto)<cut.at(jetVeto));
  
  ///////////////////////////////////////////////
  //
  // Mt Mu cut
  //
  if(verbose) std::cout << "Mt Mu cut" << std::endl;
  value.at(MtMu)=0.;
  if(muidx!=999){
	  value.at(MtMu)=sqrt(2*Ntp->Muon_p4(muidx).Pt()*Ntp->MET_CorrT0pcT1Txy_et()*(1-cosphi2d(Ntp->Muon_p4(muidx).Px(),Ntp->Muon_p4(muidx).Py(),Ntp->MET_CorrT0pcT1Txy_ex(),Ntp->MET_CorrT0pcT1Txy_ey())));
  }
  pass.at(MtMu)=(value.at(MtMu)<cut.at(MtMu));
  
  ///////////////////////////////////////////////
  //
  // Pt balance cut
  //
  if(verbose) std::cout << "pt balance cut" << std::endl;
  value.at(ptBalance)=0.;
  if(muidx!=999 && eidx!=999){
	  value.at(ptBalance) = (Ntp->Muon_p4(muidx)+Ntp->Electron_p4(eidx)).Pt();
	  //if(!useMadgraphZ && (Ntp->GetMCID()==DataMCType::DY_ee || Ntp->GetMCID()==DataMCType::DY_mumu || Ntp->GetMCID()==DataMCType::DY_tautau)) value.at(ptBalance)*=PowhegReweight(value.at(ptBalance));
	  //if(Ntp->GetMCID()==40)value.at(ptBalance)*=ZPtReweight(value.at(ptBalance));
  }
  pass.at(ptBalance)=(value.at(ptBalance)<cut.at(ptBalance));

  ///////////////////////////////////////////////
  //
  // Invariant mass cut
  //
  if(verbose) std::cout << "Invariant mass cut" << std::endl;
  value.at(ZMassmax)=zmax+1.;
  value.at(ZMassmin)=zmin-1.;
  if(eidx!=999 && muidx!=999){
	  value.at(ZMassmax)=(Ntp->Muon_p4(muidx)+Ntp->Electron_p4(eidx)).M();
	  value.at(ZMassmin)=(Ntp->Muon_p4(muidx)+Ntp->Electron_p4(eidx)).M();
  }
  pass.at(ZMassmax)=(value.at(ZMassmax)<cut.at(ZMassmax));
  pass.at(ZMassmin)=(value.at(ZMassmin)>cut.at(ZMassmin));

  ///////////////////////////////////////////////
  //
  // Weights
  //
  if(verbose) std::cout << "do weights" << std::endl;
  double wobs(1),w(1);
  if(!Ntp->isData() && Ntp->GetMCID()!=DataMCType::DY_emu_embedded){
    w*=Ntp->PUWeight()*fakeRate;
    if(pass.at(NE)){
    	if(doHiggsObjects){
    		w*=ElectronIDeff(eidx,"Higgs")*ElectronTriggerEff(eidx);
    	}else{
    		w*=ElectronIDeff(eidx,"Trig");
    	}
    }
    if(pass.at(NMu)){
    	if(doHiggsObjects){
    		w*=MuonHiggsIDeff(muidx)*MuonTriggerEff(muidx);
    	}else{
    		w*=MuonIDeff(muidx)*TrackingEff(Ntp->Muon_p4(muidx).Eta());
    	}
    }
    if(pass.at(TriggerOk)
    		&& pass.at(NMu)
    		&& pass.at(NE)
    		&& !doHiggsObjects
    		){
    	if(leadingmu) w*=TriggerEff(muidx,eidx,"Mu17_Ele8");
    	else w*=TriggerEff(muidx,eidx,"Mu8_Ele17");
    }
    if(pass.at(NMu)
    		&& pass.at(NE)
    		&& !useMadgraphZ
    		){
    	//if(Ntp->GetMCID()==DataMCType::DY_ee || Ntp->GetMCID()==DataMCType::DY_mumu || Ntp->GetMCID()==DataMCType::DY_tautau)w*=PowhegReweight((Ntp->Muon_p4(muidx)+Ntp->Electron_p4(eidx)).Pt());
    }
    if(verbose)std::cout << "void  ZtoEMu::doEvent() k" << w << " " << wobs << std::endl;
  }
  else if(!Ntp->isData() && Ntp->GetMCID()==DataMCType::DY_emu_embedded){
	  w*=Ntp->EmbeddedWeight();
	  if(pass.at(NE)) w*=ElectronEmbeddedEff(eidx);
  }
  else{w=1*fakeRate;wobs=1;}
  if(verbose)std::cout << "w=" << w << " " << wobs << " " << w*wobs << std::endl;
  bool status=AnalysisCuts(t,w,wobs);
  if(verbose)std::cout << "status: " << status << std::endl;
  ///////////////////////////////////////////////
  //
  // Add plots
  //
  if(verbose) std::cout << "add plots" << std::endl;
  if(pass.at(TriggerOk)
		  && pass.at(PrimeVtx)
		  ){
	  for(unsigned i=0;i<Ntp->NElectrons();i++){
		  if(Ntp->Electron_p4(i).Et()>e_ptlow
			  && fabs(Ntp->Electron_supercluster_eta(i))<e_eta
			  && (matchTrigger(i,0.2,"HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v","electron") || matchTrigger(i,0.2,"HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v","electron"))
			  && vertex>=0
			  ){
			  if(isMVATrigElectron(i)){
				  Dxy_trig.at(t).Fill(dxy(Ntp->Electron_p4(i),Ntp->Electron_Poca(i),Ntp->Vtx(vertex)),w);
				  Dz_trig.at(t).Fill(dz(Ntp->Electron_p4(i),Ntp->Electron_Poca(i),Ntp->Vtx(vertex)),w);
			  }
			  double mvapt = Ntp->Electron_p4(i).Pt();
			  double mvaeta = fabs(Ntp->Electron_supercluster_eta(i));
			  bool isnontrigele = true;
			  if(mvapt<7.) isnontrigele = false;
			  if(mvaeta>2.5) isnontrigele = false;
			  if(Ntp->Electron_numberOfMissedHits(i)>1) isnontrigele = false;
			  if(mvapt>7. && mvapt<10.){
				  if(mvaeta<0.8 && Ntp->Electron_MVA_NonTrig_discriminator(i)<=0.47) isnontrigele = false;
				  else if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_NonTrig_discriminator(i)<=0.004) isnontrigele = false;
				  else if(mvaeta>=1.479 && mvaeta<2.5 && Ntp->Electron_MVA_NonTrig_discriminator(i)<=0.295) isnontrigele = false;
			  }
			  if(mvapt>=10.){
				  if(mvaeta<0.8 && Ntp->Electron_MVA_NonTrig_discriminator(i)<=-0.34) isnontrigele = false;
				  else if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_NonTrig_discriminator(i)<=-0.65) isnontrigele = false;
				  else if(mvaeta>=1.479 && mvaeta<2.5 && Ntp->Electron_MVA_NonTrig_discriminator(i)<=0.6) isnontrigele = false;
			  }
			  if(isnontrigele){
				  Dxy_nontrig.at(t).Fill(dxy(Ntp->Electron_p4(i),Ntp->Electron_Poca(i),Ntp->Vtx(vertex)),w);
				  Dz_nontrig.at(t).Fill(dz(Ntp->Electron_p4(i),Ntp->Electron_Poca(i),Ntp->Vtx(vertex)),w);
			  }
			  if(isMVATrigNoIPElectron(i)){
				  Dxy_trignoip.at(t).Fill(dxy(Ntp->Electron_p4(i),Ntp->Electron_Poca(i),Ntp->Vtx(vertex)),w);
				  Dz_trignoip.at(t).Fill(dz(Ntp->Electron_p4(i),Ntp->Electron_Poca(i),Ntp->Vtx(vertex)),w);
			  }
		  }
	  }
  }
  if(pass.at(TriggerOk)
		  && pass.at(PrimeVtx)
		  && pass.at(NMu)
		  && pass.at(NE)
		  && pass.at(ptthreshold)
		  && pass.at(mll)
		  //&& pass.at(charge)
		  ){
	  // often needed variables
	  double m = (Ntp->Muon_p4(muidx)+Ntp->Electron_p4(eidx)).M();
	  double dp = Ntp->Muon_p4(muidx).DeltaPhi(Ntp->Electron_p4(eidx))/TMath::Pi();
	  if(dp<0)dp+=2;

	  // electron related histograms
	  EPt.at(t).Fill(Ntp->Electron_p4(eidx).Pt(),w);
	  EEt.at(t).Fill(Ntp->Electron_p4(eidx).Et(),w);
	  etaE.at(t).Fill(Ntp->Electron_supercluster_eta(eidx),w);
	  etaE_offBins.at(t).Fill(Ntp->Electron_supercluster_eta(eidx),w);
	  etaE_manyBins.at(t).Fill(Ntp->Electron_supercluster_eta(eidx),w);
	  mtE.at(t).Fill(sqrt(2*Ntp->Electron_p4(eidx).Pt()*Ntp->MET_CorrT0pcT1Txy_et()*(1-cosphi2d(Ntp->Electron_p4(eidx).Px(),Ntp->Electron_p4(eidx).Py(),Ntp->MET_CorrT0pcT1Txy_ex(),Ntp->MET_CorrT0pcT1Txy_ey()))),w);
	  RelIsoE.at(t).Fill(Electron_RelIso(eidx),w);
	  pt_vs_eta_e.at(t).Fill(Ntp->Electron_p4(eidx).Pt(),Ntp->Electron_supercluster_eta(eidx),w);

	  // muon related histograms
	  MuPt.at(t).Fill(Ntp->Muon_p4(muidx).Pt(),w);
	  etaMu.at(t).Fill(Ntp->Muon_p4(muidx).Eta(),w);
	  etaMu_offBins.at(t).Fill(Ntp->Muon_p4(muidx).Eta(),w);
	  etaMu_manyBins.at(t).Fill(Ntp->Muon_p4(muidx).Eta(),w);
	  mtMu.at(t).Fill(sqrt(2*Ntp->Muon_p4(muidx).Pt()*Ntp->MET_CorrT0pcT1Txy_et()*(1-cosphi2d(Ntp->Muon_p4(muidx).Px(),Ntp->Muon_p4(muidx).Py(),Ntp->MET_CorrT0pcT1Txy_ex(),Ntp->MET_CorrT0pcT1Txy_ey()))),w);
	  if(Ntp->MET_CorrT0pcT1Txy_et()>=30.)mtmu_metgr30.at(t).Fill(sqrt(2*Ntp->Muon_p4(muidx).Pt()*Ntp->MET_CorrT0pcT1Txy_et()*(1-cosphi2d(Ntp->Muon_p4(muidx).Px(),Ntp->Muon_p4(muidx).Py(),Ntp->MET_CorrT0pcT1Txy_ex(),Ntp->MET_CorrT0pcT1Txy_ey()))),w);
	  if(Ntp->MET_CorrT0pcT1Txy_et()<30.)mtmu_metsm30.at(t).Fill(sqrt(2*Ntp->Muon_p4(muidx).Pt()*Ntp->MET_CorrT0pcT1Txy_et()*(1-cosphi2d(Ntp->Muon_p4(muidx).Px(),Ntp->Muon_p4(muidx).Py(),Ntp->MET_CorrT0pcT1Txy_ex(),Ntp->MET_CorrT0pcT1Txy_ey()))),w);
	  RelIsoMu.at(t).Fill(Muon_RelIso(muidx),w);
	  pt_vs_eta_mu.at(t).Fill(Ntp->Muon_p4(muidx).Pt(),Ntp->Muon_p4(muidx).Eta(),w);

	  // histograms related to combination
	  eta_mu_e.at(t).Fill(Ntp->Muon_p4(muidx).Eta(),Ntp->Electron_supercluster_eta(eidx),w);
	  drmue.at(t).Fill(Ntp->Muon_p4(muidx).DeltaR(Ntp->Electron_p4(eidx)),w);
	  met.at(t).Fill(Ntp->MET_CorrT0pcT1Txy_et(),w);
	  met_uncorr.at(t).Fill(Ntp->MET_Uncorr_et(),w);
	  deltaphi.at(t).Fill(dp,w);
	  chargesum.at(t).Fill(fabs(Ntp->Muon_Charge(muidx)+Ntp->Electron_Charge(eidx)),w);
	  chargesumsigned.at(t).Fill(Ntp->Muon_Charge(muidx)+Ntp->Electron_Charge(eidx),w);
	  ptbal.at(t).Fill((Ntp->Muon_p4(muidx)+Ntp->Electron_p4(eidx)).Pt(),w);

	  if(jetsfromvtx.size()==1 && firstjet_idx!=-1){
		  onejet.at(t).Fill(Ntp->PFJet_p4(firstjet_idx).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(firstjet_idx).Eta(),Ntp->RunNumber()),w);
	  }
	  if(jetsfromvtx.size()>1 && firstjet_idx!=-1 && secondjet_idx!=-1){
		  jetsum.at(t).Fill(Ntp->PFJet_p4(firstjet_idx).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(firstjet_idx).Eta(),Ntp->RunNumber())+Ntp->PFJet_p4(secondjet_idx).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(secondjet_idx).Eta(),Ntp->RunNumber()),w);
	  }
	  if(pass.at(MtMu))mte_mtmu.at(t).Fill(sqrt(2*Ntp->Electron_p4(eidx).Pt()*Ntp->MET_CorrT0pcT1Txy_et()*(1-cosphi2d(Ntp->Electron_p4(eidx).Px(),Ntp->Electron_p4(eidx).Py(),Ntp->MET_CorrT0pcT1Txy_ex(),Ntp->MET_CorrT0pcT1Txy_ey()))),w);

	  std::vector<unsigned int> loosejets;
	  std::vector<unsigned int> mediumjets;
	  std::vector<unsigned int> tightjets;

	  for(unsigned int i=0;i<Ntp->NPFJets();i++){
		  if(Ntp->PFJet_PUJetID_looseWP(i)>0.5) loosejets.push_back(i);
		  if(Ntp->PFJet_PUJetID_mediumWP(i)>0.5) mediumjets.push_back(i);
		  if(Ntp->PFJet_PUJetID_tightWP(i)>0.5) tightjets.push_back(i);
	  }

	  NJets.at(t).Fill(Ntp->NPFJets(),w);
	  NJetsLoose.at(t).Fill(loosejets.size(),w);
	  NJetsMedium.at(t).Fill(mediumjets.size(),w);
	  NJetsTight.at(t).Fill(tightjets.size(),w);
	  int nbjets = 0;
	  int nbjetsvtxl = 0;
	  int nbjetsvtxm = 0;
	  int nbjetsvtxt = 0;
	  for(unsigned i=0;i<Ntp->NPFJets();i++){
		  PUJetId.at(t).Fill(Ntp->PFJet_PUJetID_discr(i),w);
		  if(Ntp->PFJet_bDiscriminator(i)>csvl){
			  nbjets++;
			  if(Ntp->PFJet_PUJetID_looseWP(i)>0.5){
				  nbjetsvtxl++;
				  if(Ntp->PFJet_bDiscriminator(i)>csvm){
					  nbjetsvtxm++;
					  if(Ntp->PFJet_bDiscriminator(i)>csvt){
						  nbjetsvtxt++;
					  }
				  }
			  }
		  }
	  }
	  NbJets.at(t).Fill(nbjets,w);
	  NbJetsVtxL.at(t).Fill(nbjetsvtxl,w);
	  NbJetsVtxM.at(t).Fill(nbjetsvtxm,w);
	  NbJetsVtxT.at(t).Fill(nbjetsvtxt,w);

	  if(Ntp->NPFJets()>0){
		  jet1E.at(t).Fill(Ntp->Electron_p4(eidx).DeltaR(Ntp->PFJet_p4(0)),w);
		  jet1Mu.at(t).Fill(Ntp->Muon_p4(muidx).DeltaR(Ntp->PFJet_p4(0)),w);
		  if(Ntp->NPFJets()>1){
			  jet2E.at(t).Fill(Ntp->Electron_p4(eidx).DeltaR(Ntp->PFJet_p4(1)),w);
			  jet2Mu.at(t).Fill(Ntp->Muon_p4(muidx).DeltaR(Ntp->PFJet_p4(1)),w);
		  }
	  }

	  if(jetsfromvtx.size()>0 && firstjet_idx!=-1){
		  FirstJetPt.at(t).Fill(Ntp->PFJet_p4(firstjet_idx).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(firstjet_idx).Eta(),Ntp->RunNumber()),w);
	  }
	  if(jetsfromvtx.size()>1 && firstjet_idx!=-1 && secondjet_idx!=-1){
		  SecondJetPt.at(t).Fill(Ntp->PFJet_p4(secondjet_idx).Pt()*rundependentJetPtCorrection(Ntp->PFJet_p4(secondjet_idx).Eta(),Ntp->RunNumber()),w);
	  }

	  if(pass.at(charge)) ptbal_chargepass.at(t).Fill((Ntp->Muon_p4(muidx)+Ntp->Electron_p4(eidx)).Pt(),w);
	  if(!pass.at(charge)) ptbal_chargefail.at(t).Fill((Ntp->Muon_p4(muidx)+Ntp->Electron_p4(eidx)).Pt(),w);

	  NPV.at(t).Fill(Ntp->NVtx(),w);
	  if(!Ntp->isData()){
		  evtweight.at(t).Fill(Ntp->PUWeight());
		  if(Ntp->PUWeight()>0.)NPV3d.at(t).Fill(Ntp->NVtx(),w*Ntp->PUWeight3D()/Ntp->PUWeight());
		  if(Ntp->PUWeight()>0.)NPVfine.at(t).Fill(Ntp->NVtx(),w*Ntp->PUWeightFineBins()/Ntp->PUWeight());
	  }else{
		  NPV3d.at(t).Fill(Ntp->NVtx(),w);
		  NPVfine.at(t).Fill(Ntp->NVtx(),w);
	  }

	  if(Ntp->GetMCID()==30 || Ntp->GetMCID()==31 || Ntp->GetMCID()==32 || Ntp->GetMCID()==33 || Ntp->GetMCID()==40){
		  for(unsigned i=0;i<Ntp->NMCParticles();i++){
			  if(Ntp->MCParticle_pdgid(i)==23){
				  if(Ntp->GetMCID()==DataMCType::DY_ee || Ntp->GetMCID()==DataMCType::DY_mumu || Ntp->GetMCID()==DataMCType::DY_tautau){
					  zpt.at(t).Fill(Ntp->MCParticle_p4(i).Pt()*PowhegReweight(Ntp->MCParticle_p4(i).Pt()));
				  }else{
					  zpt.at(t).Fill(Ntp->MCParticle_p4(i).Pt());
				  }
				  zeta.at(t).Fill(Ntp->MCParticle_p4(i).Eta());
				  zmass.at(t).Fill(Ntp->MCParticle_p4(i).M());
			  }
		  }
		  if(jetsfromvtx.size()>0){
			  leadingjet_pt.at(t).Fill(Ntp->PFJet_p4(firstjet_idx).Pt());
			  leadingjet_eta.at(t).Fill(Ntp->PFJet_p4(firstjet_idx).Eta());
		  }
		  if(jetsfromvtx.size()>1){
			  subleadingjet_pt.at(t).Fill(Ntp->PFJet_p4(secondjet_idx).Pt());
			  subleadingjet_eta.at(t).Fill(Ntp->PFJet_p4(secondjet_idx).Eta());
			  jetsumcustom.at(t).Fill(Ntp->PFJet_p4(firstjet_idx).Pt()+Ntp->PFJet_p4(secondjet_idx).Pt());
		  }
	  }

	  if(verbose)std::cout << "invariant mass with different cuts applied" << std::endl;

	  invmass_only_object_id.at(t).Fill(m,w);
	  invmass_only_object_id_m.at(t).Fill(m,w);
	  if(pass.at(drEMu)
			  && pass.at(diMuonVeto)
			  && pass.at(triLeptonVeto)
			  && pass.at(charge)
			  ){
		  invmass_vetos.at(t).Fill(m,w);
		  invmass_vetos_m.at(t).Fill(m,w);
		  if(pass.at(jetVeto)){
			  invmass_jetveto.at(t).Fill(m,w);
			  invmass_jetveto_m.at(t).Fill(m,w);
			  if(pass.at(MtMu)){
				  invmass_mtmu.at(t).Fill(m,w);
				  invmass_mtmu_m.at(t).Fill(m,w);
				  if(pass.at(ptBalance)){
					  invmass_ptbalance.at(t).Fill(m,w);
					  invmass_ptbalance_m.at(t).Fill(m,w);
					  nm0_met.at(t).Fill(Ntp->MET_CorrT0pcT1_et(),w);
					  nm0_mtmu.at(t).Fill(sqrt(2*Ntp->Muon_p4(muidx).Pt()*Ntp->MET_CorrT0pcT1Txy_et()*(1-cosphi2d(Ntp->Muon_p4(muidx).Px(),Ntp->Muon_p4(muidx).Py(),Ntp->MET_CorrT0pcT1Txy_ex(),Ntp->MET_CorrT0pcT1Txy_ey()))),w);
					  nm0_ptbalance.at(t).Fill((Ntp->Muon_p4(muidx)+Ntp->Electron_p4(eidx)).Pt(),w);
					  if(pass.at(ZMassmax)
							  && pass.at(ZMassmin)){
						  invmass_zmass.at(t).Fill(m,w);
						  invmass_zmass_m.at(t).Fill(m,w);
					  }
				  }
			  }
		  }
	  }
	  if(pass.at(drEMu)) invmass_dremu_only.at(t).Fill(m,w);
	  if(pass.at(diMuonVeto)) invmass_dimuon_only.at(t).Fill(m,w);
	  if(pass.at(triLeptonVeto)) invmass_trilepton_only.at(t).Fill(m,w);
	  if(pass.at(charge)) invmass_charge_only.at(t).Fill(m,w);
	  if(pass.at(jetVeto)) invmass_jetveto_only.at(t).Fill(m,w);
	  if(pass.at(MtMu)) invmass_mtmu_only.at(t).Fill(m,w);
	  if(pass.at(ptBalance)) invmass_ptbal_only.at(t).Fill(m,w);
  }

  if(verbose)std::cout << "ZtoEMu::doEvent() doEvent END" << std::endl;
}

//////////////////////
//
// Utility functions
//

bool ZtoEMu::isGoodVtx(unsigned int idx){
	if(fabs(Ntp->Vtx(idx).z())>=24) return false;
	if(Ntp->Vtx(idx).Perp()>=2) return false;
	if(Ntp->Vtx_ndof(idx)<=4) return false;
	if(Ntp->Vtx_isFake(idx)!=0) return false;
	return true;
}

double ZtoEMu::ZPtReweight(double zpt){
	double weight = 1;
	if(zpt<2.5)	weight/=1.0483;
	else if(zpt>=2.5 && zpt<5.) weight/=1.02656;
	else if(zpt>=5. && zpt<7.5)	weight/=1.02578;
	else if(zpt>=7.5 && zpt<10.) weight/=1.06374;
	else if(zpt>=10. && zpt<12.5) weight/=0.976617;
	else if(zpt>=12.5 && zpt<15.) weight/=0.912292;
	else if(zpt>=15. && zpt<17.5) weight/=0.981654;
	else if(zpt>=17.5 && zpt<20.) weight/=0.948347;
	else if(zpt>=20. && zpt<22.5) weight/=0.987479;
	else if(zpt>=22.5 && zpt<25.) weight/=0.960792;
	else if(zpt>=25. && zpt<27.5) weight/=0.863612;
	else if(zpt>=27.5 && zpt<30.) weight/=1.00904;
	return weight;
}

double ZtoEMu::PowhegReweight(double zpt){
	double weight = 1.;
	weight = ZptCorrection->GetBinContent(ZptCorrection->FindFixBin(zpt));
	return weight;
}

double ZtoEMu::ElectronMassScale(unsigned int idx){
	double ept = Ntp->Electron_p4(idx).Pt();
	double eeta = fabs(Ntp->Electron_supercluster_eta(idx));
	double corr = 0;
	if(eeta<1.48){
		if(ept<12) corr = 0.008;
		else if(ept>=12 && ept<16) corr = 0.009;
		else if(ept>=16 && ept<20) corr = 0.08;
		else if(ept>=20 && ept<24) corr = 0.006;
		else if(ept>=24 && ept<28) corr = 0.004;
		else if(ept>=28 && ept<32) corr = 0.003;
		else if(ept>=32 && ept<36) corr = 0.002;
		else if(ept>=36) corr = 0.001;
	}else if(eeta>1.48){
		if(ept<12) corr = 0.018;
		else if(ept>=12 && ept<16)	corr = 0.0135;
		else if(ept>=16 && ept<20)	corr = 0.08;
		else if(ept>=20 && ept<24)	corr = 0.0075;
		else if(ept>=24 && ept<28)	corr = 0.0055;
		else if(ept>=28 && ept<32)	corr = 0.004;
		else if(ept>=32 && ept<36)	corr = 0.002;
		else if(ept>=36) corr = 0.001;
	}
	return corr;
}

double ZtoEMu::rundependentJetPtCorrection(double jeteta, int runnumber){
	if(!Ntp->isData() && Ntp->GetMCID()!=DataMCType::DY_emu_embedded) return 1.;
	const double corrs[5] = {0.0, -0.454e-6, -0.952e-6, 1.378e-6, 0.0};
	const int run0 = 201000;
	double eta = fabs(jeteta);
	double corr = 0.;
	if(eta<1.3) corr = corrs[0];
	else if(eta<2.0) corr = corrs[1];
	else if(eta<2.5) corr = corrs[2];
	else if(eta<3.0) corr = corrs[3];
	else if(eta<5.0) corr = corrs[4];
	return (1.+corr*(runnumber-run0));
}

double ZtoEMu::CorrectJER(unsigned int idx){
	double sf = 1.;
	double c = JetEnergyResolutionCorr(Ntp->PFJet_p4(idx).Eta());
	if(GenJet(idx)==TLorentzVector(0.,0.,0.,0.)) std::cout << "ZtoEMu::CorrectJER - jet could not be matched to generator particles" << std::endl;
	else sf = std::max(0.,c+(1-c)*GenJet(idx).Pt()/Ntp->PFJet_p4(idx).Pt());
	return sf;
}

// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution
double ZtoEMu::JetEnergyResolutionCorr(double jeteta){
	double eta = fabs(jeteta);
	double corr = 1.;
	if(eta<0.5) corr = 1.079;
	else if(eta<1.1) corr = 1.099;
	else if(eta<1.7) corr = 1.121;
	else if(eta<2.3) corr = 1.208;
	else if(eta<2.8) corr = 1.254;
	else if(eta<3.2) corr = 1.395;
	else if(eta<5.0) corr = 1.056;
	return corr;
}

// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution
double ZtoEMu::JetEnergyResolutionCorrErr(double jeteta){
	double eta = fabs(jeteta);
	double err = 0.;
	if(eta<0.5) err = 0.026;
	else if(eta<1.1) err = 0.027;
	else if(eta<1.7) err = 0.029;
	else if(eta<2.3) err = 0.046;
	else if(eta<2.8) err = 0.062;
	else if(eta<3.2) err = 0.063;
	else if(eta<5.0) err = 0.191;
	return err;
}

TLorentzVector ZtoEMu::GenJet(unsigned int recjet){
	TLorentzVector genjet(0.,0.,0.,0.);
	// TODO: once new nTuples available, switch to GenJetNoNu
	for(unsigned i=0;i<Ntp->NMCParticles();i++){
		if(Ntp->MCParticle_p4(i).Vect()!=TVector3(0.,0.,0.)
				&& Ntp->PFJet_p4(recjet).DeltaR(Ntp->MCParticle_p4(i))<0.1
				&& fabs(Ntp->PFJet_p4(recjet).Pt()-Ntp->MCParticle_p4(i).Pt())<0.1
				){
			genjet = Ntp->MCParticle_p4(i);
		}
	}
	return genjet;
}

double ZtoEMu::calculatePzeta(int muiterator, int eiterator){
  double pex=Ntp->Electron_p4(eiterator).Px();
  double pey=Ntp->Electron_p4(eiterator).Py();
  double pmux=Ntp->Muon_p4(muiterator).Px();
  double pmuy=Ntp->Muon_p4(muiterator).Py();
  double phie=Ntp->Electron_p4(eiterator).Phi();
  double phimu=Ntp->Muon_p4(muiterator).Phi();
  double combpt=TMath::Sqrt(pow(pex+pmux,2)+pow(pey+pmuy,2));
  double aemu=TMath::ACos(pmux*pex+pmuy*pey/(Ntp->Muon_p4(muiterator).Pt()*Ntp->Electron_p4(eiterator).Pt()));
  double phismall = 0.;
  if(phie<phimu && fabs(phie-phimu)<TMath::Pi()) phismall=phie;
  else if(phimu<phie && fabs(phie-phimu)>TMath::Pi())phismall=phie;
  else if(phie<phimu && fabs(phie-phimu)>TMath::Pi())phismall=phimu;
  else if(phimu<phie && fabs(phie-phimu)<TMath::Pi())phismall=phimu;
  double beta=TMath::ACos(((pex+pmux)*TMath::Cos(phismall+0.5*aemu)+(pey+pmuy)*TMath::Sin(phismall+0.5*aemu))/combpt);
  double gamma=TMath::ACos((Ntp->MET_CorrT0pcT1_ex()*TMath::Cos(phismall+0.5*aemu)+Ntp->MET_CorrT0pcT1_ey()*TMath::Sin(phismall+0.5*aemu))/Ntp->MET_CorrT0pcT1_et());
  if(Ntp->MET_CorrT0pcT1_phi()>(phismall+0.5*aemu+0.5*TMath::Pi()) && Ntp->MET_CorrT0pcT1_phi()<(phismall+0.5*aemu+1.5*TMath::Pi()))gamma*=-1;
  double pvis=TMath::Sin(beta)*combpt;
  double pmiss=TMath::Sin(gamma)*Ntp->MET_CorrT0pcT1_et();
  return pmiss-pvis;
}

double ZtoEMu::calculatePzetaDQM(int muiterator, int eiterator){
	double cosPhi1 = TMath::Cos(Ntp->Electron_p4(eiterator).Phi());
	double sinPhi1 = TMath::Sin(Ntp->Electron_p4(eiterator).Phi());
	double cosPhi2 = TMath::Cos(Ntp->Muon_p4(muiterator).Phi());
	double sinPhi2 = TMath::Sin(Ntp->Muon_p4(muiterator).Phi());
	double zetaX = cosPhi1 + cosPhi2;
	double zetaY = sinPhi1 + sinPhi2;
	double zetaR = TMath::Sqrt(zetaX*zetaX + zetaY*zetaY);
	if(zetaR>0.){
		zetaX/=zetaR;
		zetaY/=zetaR;
	}
	double pxVis=Ntp->Electron_p4(eiterator).Px()+Ntp->Muon_p4(muiterator).Px();
	double pyVis=Ntp->Electron_p4(eiterator).Py()+Ntp->Muon_p4(muiterator).Py();
	double pZetaVis=pxVis*zetaX+pyVis*zetaY;
	double px=pxVis+Ntp->MET_CorrT0pcT1_ex();
	double py=pyVis+Ntp->MET_CorrT0pcT1_ey();
	double pZeta=px*zetaX+py*zetaY;
	return pZeta-1.5*pZetaVis;
}

double ZtoEMu::cosphi2d(double px1, double py1, double px2, double py2){
	return (px1*px2+py1*py2)/(sqrt(pow(px1,2)+pow(py1,2))*sqrt(pow(px2,2)+pow(py2,2)));
}

double ZtoEMu::cosphi3d(TVector3 vec1, TVector3 vec2){
	return (vec1.Dot(vec2))/vec1.Mag()/vec2.Mag();
}

bool ZtoEMu::jetFromVtx(std::vector<int> vtx_track_idx, int leadingtrack_idx){
	for(unsigned i=0;i<vtx_track_idx.size();i++){
		if(vtx_track_idx.at(i)==leadingtrack_idx)return true;
	}
	return false;
}

double ZtoEMu::dxy(TLorentzVector fourvector, TVector3 poca, TVector3 vtx){
	return fabs((-(poca.X()-vtx.X())*fourvector.Py()+(poca.Y()-vtx.Y())*fourvector.Px())/fourvector.Pt());
}

double ZtoEMu::dz(TLorentzVector fourvector, TVector3 poca, TVector3 vtx){
	return fabs(poca.Z()-vtx.Z()-((poca.X()-vtx.X())*fourvector.Px()+(poca.Y()-vtx.Y())*fourvector.Py())*fourvector.Pz()/pow(fourvector.Pt(),2));
}

double ZtoEMu::vertexSignificance(TVector3 vec, unsigned int vertex){
	if(vertex>=0 && vertex<Ntp->NVtx()){
		const float elm[3] = {(vec.X()-Ntp->Vtx(vertex).X()),(vec.Y()-Ntp->Vtx(vertex).Y()),(vec.Z()-Ntp->Vtx(vertex).Z())};
		TVectorF diff(3,elm);
		TMatrixF M(Ntp->Vtx_Cov(vertex));
		if(M.IsValid()){
			double mag = diff.Norm2Sqr();
			double sim = M.Similarity(diff);
			return mag/sqrt(sim);
		}
	}
	return 999;
}

bool ZtoEMu::matchTrigger(unsigned int idx, double dr, std::string trigger, std::string object){
	unsigned int id = 0;
	TLorentzVector particle(0.,0.,0.,0.);
	TLorentzVector triggerObj(0.,0.,0.,0.);
	if(object=="electron"){
		id = 82;
		particle = Ntp->Electron_p4(idx);
	}
	if(object=="muon"){
		id = 83;
		particle = Ntp->Muon_p4(idx);
	}
	for(unsigned i=0;i<Ntp->NHLTTrigger_objs();i++){
		if(Ntp->HLTTrigger_objs_trigger(i).find(trigger) != string::npos){
			for(unsigned j=0;j<Ntp->NHLTTrigger_objs(i);j++){
				if(Ntp->HLTTrigger_objs_Id(i,j)==id){
					triggerObj.SetPtEtaPhiE(Ntp->HLTTrigger_objs_Pt(i,j),
							Ntp->HLTTrigger_objs_Eta(i,j),
							Ntp->HLTTrigger_objs_Phi(i,j),
							Ntp->HLTTrigger_objs_E(i,j));
				}
				if(triggerObj.Pt()>0.
						&& particle.Pt()>0.
						&& particle.DeltaR(triggerObj)<dr) return true;
			}
		}
	}
	return false;
}

int ZtoEMu::matchTruth(TLorentzVector tvector){
	double testdr=1.;
	int pdgid = 0;
	for(unsigned i=0;i<Ntp->NMCParticles();i++){
		if(Ntp->MCParticle_p4(i).Pt()>0.){
			if(tvector.DeltaR(Ntp->MCParticle_p4(i))<testdr){
				testdr = tvector.DeltaR(Ntp->MCParticle_p4(i));
				pdgid = Ntp->MCParticle_pdgid(i);
			}
		}
	}
	return pdgid;
}

bool ZtoEMu::matchTruth(TLorentzVector tvector, int pid, double dr){
	for(unsigned i=0;i<Ntp->NMCParticles();i++){
		if(Ntp->MCParticle_p4(i).Pt()>0.){
			if(fabs(Ntp->MCParticle_pdgid(i))==pid){
				if(tvector.DeltaR(Ntp->MCParticle_p4(i))<dr) return true;
			}
		}
	}
	return false;
}

int ZtoEMu::findBin(TGraphAsymmErrors* graph, double xval){
	int bin = -1;
	for(unsigned i=0;i<graph->GetN()-1;i++){
		if(fabs(graph->GetX()[i]-xval)<fabs(graph->GetX()[i+1]-xval)) bin = i;
		else if(fabs(graph->GetX()[i]-xval)>=fabs(graph->GetX()[i+1]-xval)) bin = i+1;
	}
	return bin;
}

//////////////////////////////
//
// Muon related functions
//

bool ZtoEMu::isTightMuon(unsigned int idx){
	if(!Ntp->Muon_isGlobalMuon(idx)) return false;
	if(!Ntp->Muon_isPFMuon(idx)) return false;
	if(Ntp->Muon_normChi2(idx)>=10.) return false;
	if(Ntp->Muon_hitPattern_numberOfValidMuonHits(idx)<=0) return false;
	if(Ntp->Muon_numberOfMatchedStations(idx)<=1) return false;
	if(Ntp->Muon_numberofValidPixelHits(idx)<=0) return false;
	if(Ntp->Muon_trackerLayersWithMeasurement(idx)<=5) return false;
	return true;
}

bool ZtoEMu::isTightMuon(unsigned int idx, unsigned int vtx){
	if(vtx<0 || vtx>=Ntp->NVtx()) return false;
	if(!isTightMuon(idx)) return false;
	if(dxy(Ntp->Muon_p4(idx),Ntp->Muon_Poca(idx),Ntp->Vtx(vtx))>=0.2) return false;
	if(dz(Ntp->Muon_p4(idx),Ntp->Muon_Poca(idx),Ntp->Vtx(vtx))>=0.5) return false;
	return true;
}

bool ZtoEMu::isHiggsMuon(unsigned int idx, unsigned int vtx){
	if(vtx<0 || vtx>=Ntp->NVtx()) return false;
	if(!isTightMuon(idx)) return false;
	if(dxy(Ntp->Muon_p4(idx),Ntp->Muon_Poca(idx),Ntp->Vtx(vtx))>=0.02) return false;
	if(dz(Ntp->Muon_p4(idx),Ntp->Muon_Poca(idx),Ntp->Vtx(vtx))>=0.1) return false;
	return true;
}

bool ZtoEMu::isLooseMuon(unsigned int idx){
	if(!Ntp->Muon_isPFMuon(idx)) return false;
	if(!(Ntp->Muon_isGlobalMuon(idx) || Ntp->Muon_isTrackerMuon(idx))) return false;
	return true;
}

bool ZtoEMu::isFakeMuon(unsigned int idx){
	if(!Ntp->Muon_isGlobalMuon(idx)) return false;
	if(Ntp->Muon_p4(idx).Pt()<=10) return false;
	if(fabs(Ntp->Muon_p4(idx).Eta())>2.4) return false;
	if(Ntp->Muon_p4(idx).Pt()<=20){
		if(Ntp->Muon_sumPt03(idx)>=8.) return false;
		if(Ntp->Muon_emEt03(idx)>=8.) return false;
		if(Ntp->Muon_hadEt03(idx)>=8.) return false;
	}
	if(Ntp->Muon_p4(idx).Pt()>20){
		if(Ntp->Muon_sumPt03(idx)/Ntp->Muon_p4(idx).Pt()>=0.4) return false;
		if(Ntp->Muon_emEt03(idx)/Ntp->Muon_p4(idx).Pt()>=0.4) return false;
		if(Ntp->Muon_hadEt03(idx)/Ntp->Muon_p4(idx).Pt()>=0.4) return false;
	}
	return true;
}

bool ZtoEMu::isFakeMuon(unsigned int idx, unsigned int vtx){
	if(vtx<0 || vtx>=Ntp->NVtx()) return false;
	if(!isFakeMuon(idx)) return false;
	if(dxy(Ntp->Muon_p4(idx),Ntp->Muon_Poca(idx),Ntp->Vtx(vtx))>=0.2) return false;
	if(dz(Ntp->Muon_p4(idx),Ntp->Muon_Poca(idx),Ntp->Vtx(vtx))>=0.1) return false;
	return true;
}

double ZtoEMu::Muon_RelIso(unsigned int idx){
	return (Ntp->Muon_sumChargedHadronPt04(idx)+std::max(0.,Ntp->Muon_sumNeutralHadronEt04(idx)+Ntp->Muon_sumPhotonEt04(idx)-0.5*Ntp->Muon_sumPUPt04(idx)))/Ntp->Muon_p4(idx).Pt();
}

//////////////////////////////
//
// Electron related functions
//

bool ZtoEMu::isTrigPreselElectron(unsigned int idx){
	if(fabs(Ntp->Electron_supercluster_eta(idx))>2.5) return false;
	if(Ntp->Electron_numberOfMissedHits(idx)>0) return false;
	if(Ntp->Electron_Gsf_dr03TkSumPt(idx)/Ntp->Electron_p4(idx).Pt()>0.2) return false;
	if(Ntp->Electron_Gsf_dr03HcalTowerSumEt(idx)/Ntp->Electron_p4(idx).Pt()>0.2) return false;
	if(fabs(Ntp->Electron_supercluster_eta(idx))<1.479){
		if(Ntp->Electron_sigmaIetaIeta(idx)>0.014) return false;
		if(Ntp->Electron_hadronicOverEm(idx)>0.15) return false;
	}else{
		if(Ntp->Electron_sigmaIetaIeta(idx)>0.035) return false;
		if(Ntp->Electron_hadronicOverEm(idx)>0.1) return false;
	}
	return true;
}

bool ZtoEMu::isTrigNoIPPreselElectron(unsigned int idx){
	if(fabs(Ntp->Electron_supercluster_eta(idx))>2.5) return false;
	if(Ntp->Electron_numberOfMissedHits(idx)>0) return false;
	if(Ntp->Electron_Gsf_dr03TkSumPt(idx)/Ntp->Electron_p4(idx).Pt()>0.2) return false;
	if(Ntp->Electron_Gsf_dr03HcalTowerSumEt(idx)/Ntp->Electron_p4(idx).Pt()>0.2) return false;
	if(fabs(Ntp->Electron_supercluster_eta(idx))<1.479){
		if(Ntp->Electron_sigmaIetaIeta(idx)>0.01) return false;
		if(Ntp->Electron_hadronicOverEm(idx)>0.12) return false;
		if(fabs(Ntp->Electron_Gsf_deltaEtaSuperClusterTrackAtVtx(idx))>0.007) return false;
		if(fabs(Ntp->Electron_Gsf_deltaPhiSuperClusterTrackAtVtx(idx))>0.15) return false;
	}else{
		if(Ntp->Electron_sigmaIetaIeta(idx)>0.03) return false;
		if(Ntp->Electron_hadronicOverEm(idx)>0.1) return false;
		if(fabs(Ntp->Electron_Gsf_deltaEtaSuperClusterTrackAtVtx(idx))>0.009) return false;
		if(fabs(Ntp->Electron_Gsf_deltaPhiSuperClusterTrackAtVtx(idx))>0.1) return false;
	}
	return true;
}

bool ZtoEMu::isMVATrigNoIPElectron(unsigned int idx){
	double mvapt = Ntp->Electron_p4(idx).Pt();
	double mvaeta = fabs(Ntp->Electron_supercluster_eta(idx));
	if(mvaeta>2.5) return false;
	if(Ntp->Electron_HasMatchedConversions(idx)) return false;
	if(Ntp->Electron_numberOfMissedHits(idx)>0) return false;
	if(!isTrigNoIPPreselElectron(idx)) return false;
	if(mvaeta<1.479 && Electron_RelIso(idx)>0.15) return false;// TODO: correct isolation cuts?
	if(mvaeta>=1.479 && Electron_RelIso(idx)>0.10) return false;
	if(mvapt<20){
		if(mvaeta<0.8 && Ntp->Electron_MVA_TrigNoIP_discriminator(idx)<=-0.5375) return false;
		if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_TrigNoIP_discriminator(idx)<=-0.375) return false;
		if(mvaeta>=1.479 && Ntp->Electron_MVA_TrigNoIP_discriminator(idx)<=-0.025) return false;
	}
	if(mvapt>=20){
		if(mvaeta<0.8 && Ntp->Electron_MVA_TrigNoIP_discriminator(idx)<=0.325) return false;
		if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_TrigNoIP_discriminator(idx)<=0.775) return false;
		if(mvaeta>=1.479 && Ntp->Electron_MVA_TrigNoIP_discriminator(idx)<=0.775) return false;
	}
	return true;
}

bool ZtoEMu::isMVANonTrigElectron(unsigned int idx, unsigned int vtx){
	double mvapt = Ntp->Electron_p4(idx).Pt();
	double mvaeta = fabs(Ntp->Electron_supercluster_eta(idx));
	if(mvapt<7.) return false;
	if(mvaeta>2.5) return false;
	if(Ntp->Electron_numberOfMissedHits(idx)>1) return false;
	if(vertexSignificance(Ntp->Electron_Poca(idx),vtx)>=4) return false;
	if(mvapt>7. && mvapt<10.){
		if(mvaeta<0.8 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=0.47) return false;
		if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=0.004) return false;
		if(mvaeta>=1.479 && mvaeta<2.5 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=0.295) return false;
	}
	if(mvapt>=10.){
		if(mvaeta<0.8 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=-0.34) return false;
		if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=-0.65) return false;
		if(mvaeta>=1.479 && mvaeta<2.5 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=0.6) return false;
	}
	return true;
}

bool ZtoEMu::isHiggsElectron(unsigned int idx, unsigned int vtx){
	double mvapt = Ntp->Electron_p4(idx).Pt();
	double mvaeta = fabs(Ntp->Electron_supercluster_eta(idx));
	if(mvaeta>2.5) return false;
	if(Ntp->Electron_numberOfMissedHits(idx)>0) return false;
	if(Ntp->Electron_HasMatchedConversions(idx)) return false;
	if(dxy(Ntp->Electron_p4(idx),Ntp->Electron_Poca(idx),Ntp->Vtx(vtx))>=0.02) return false;
	if(dz(Ntp->Electron_p4(idx),Ntp->Electron_Poca(idx),Ntp->Vtx(vtx))>=0.1) return false;
	if(mvapt<20.){
		if(mvaeta<0.8 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=0.925) return false;
		if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=0.915) return false;
		if(mvaeta>=1.479 && mvaeta<2.5 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=0.965) return false;
	}
	if(mvapt>=20.){
		if(mvaeta<0.8 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=0.905) return false;
		if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=0.955) return false;
		if(mvaeta>=1.479 && mvaeta<2.5 && Ntp->Electron_MVA_NonTrig_discriminator(idx)<=0.975) return false;
	}
	return true;
}

bool ZtoEMu::isWWElectron(unsigned int idx, unsigned int vtx){
	double mvapt = Ntp->Electron_p4(idx).Pt();
	double mvaeta = fabs(Ntp->Electron_supercluster_eta(idx));
	if(mvapt<10.) return false;
	if(mvaeta>2.5) return false;
	if(!isFakeElectron(idx,vtx)) return false;
	if(mvapt>10. && mvapt<20.){
		if(mvaeta<0.8 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.00) return false;
		if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.10) return false;
		if(mvaeta>=1.479 && mvaeta<2.5 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.62) return false;
	}
	if(mvapt>=20.){
		if(mvaeta<0.8 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.94) return false;
		if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.85) return false;
		if(mvaeta>=1.479 && mvaeta<2.5 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.92) return false;
	}
	return true;
}

bool ZtoEMu::isMVATrigElectron(unsigned int idx){
	double mvapt = Ntp->Electron_p4(idx).Pt();
	double mvaeta = fabs(Ntp->Electron_supercluster_eta(idx));
	if(mvapt<10.) return false;
	if(mvaeta>2.5) return false;
	if(Ntp->Electron_numberOfMissedHits(idx)>0) return false;
	if(Ntp->Electron_HasMatchedConversions(idx)) return false;
	if(!isTrigPreselElectron(idx)) return false;
	if(mvapt>10. && mvapt<20.){
		if(mvaeta<0.8 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.00) return false;
		if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.10) return false;
		if(mvaeta>=1.479 && mvaeta<2.5 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.62) return false;
	}
	if(mvapt>=20.){
		if(mvaeta<0.8 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.94) return false;
		if(mvaeta>=0.8 && mvaeta<1.479 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.85) return false;
		if(mvaeta>=1.479 && mvaeta<2.5 && Ntp->Electron_MVA_Trig_discriminator(idx)<=0.92) return false;
	}
	return true;
}

bool ZtoEMu::isTightElectron(unsigned int idx){
	if(Ntp->Electron_HasMatchedConversions(idx)) return false;
	if(Ntp->Electron_numberOfMissedHits(idx)>0) return false;
	if(Electron_RelIso(idx)>=0.1) return false;
	if(fabs(1/Ntp->Electron_ecalEnergy(idx)-1/Ntp->Electron_trackMomentumAtVtx(idx))>=0.05) return false;
	if(fabs(Ntp->Electron_supercluster_eta(idx))<=1.479){
		if(Ntp->Electron_Gsf_deltaEtaSuperClusterTrackAtVtx(idx)>=0.004) return false;
		if(Ntp->Electron_Gsf_deltaPhiSuperClusterTrackAtVtx(idx)>=0.03) return false;
		if(Ntp->Electron_sigmaIetaIeta(idx)>=0.01) return false;
		if(Ntp->Electron_hadronicOverEm(idx)>=0.12) return false;
	}
	if(fabs(Ntp->Electron_supercluster_eta(idx))>1.479 && fabs(Ntp->Electron_supercluster_eta(idx))<2.5){
		if(Ntp->Electron_Gsf_deltaEtaSuperClusterTrackAtVtx(idx)>=0.005) return false;
		if(Ntp->Electron_Gsf_deltaPhiSuperClusterTrackAtVtx(idx)>=0.02) return false;
		if(Ntp->Electron_sigmaIetaIeta(idx)>=0.03) return false;
		if(Ntp->Electron_hadronicOverEm(idx)>=0.10) return false;
		if(Ntp->Electron_p4(idx).Pt()<20 && Electron_RelIso(idx)>=0.07) return false;
	}
	return true;
}

bool ZtoEMu::isTightElectron(unsigned int idx, unsigned int vtx){
	if(vtx<0 || vtx>=Ntp->NVtx()) return false;
	if(!isTightElectron(idx)) return false;
	if(dxy(Ntp->Electron_p4(idx),Ntp->Electron_Poca(idx),Ntp->Vtx(vtx))>=0.02) return false;
	if(dz(Ntp->Electron_p4(idx),Ntp->Electron_Poca(idx),Ntp->Vtx(vtx))>=0.1) return false;
	return true;
}

bool ZtoEMu::isFakeElectron(unsigned int idx){
	if(Ntp->Electron_p4(idx).Pt()<10) return false;
	if(fabs(Ntp->Electron_supercluster_eta(idx))>2.5) return false;
	if(Ntp->Electron_HasMatchedConversions(idx)) return false;
	if(Ntp->Electron_numberOfMissedHits(idx)>0) return false;
	if(Ntp->Electron_tkSumPt03(idx)/Ntp->Electron_p4(idx).Pt()>0.2) return false;
	if(std::max(Ntp->Electron_ecalRecHitSumEt03(idx)-1.,0.)/Ntp->Electron_p4(idx).Pt()>0.2) return false;
	if((Ntp->Electron_hcalDepth1TowerSumEt03(idx)+Ntp->Electron_hcalDepth2TowerSumEt03(idx))/Ntp->Electron_p4(idx).Pt()>0.2) return false;
	if(fabs(Ntp->Electron_supercluster_eta(idx))<1.479){
		if(Ntp->Electron_sigmaIetaIeta(idx)>0.01) return false;
		if(Ntp->Electron_Gsf_deltaPhiSuperClusterTrackAtVtx(idx)>0.15) return false;
		if(Ntp->Electron_Gsf_deltaEtaSuperClusterTrackAtVtx(idx)>0.007) return false;
		if(Ntp->Electron_hadronicOverEm(idx)>0.12) return false;
	}
	if(fabs(Ntp->Electron_supercluster_eta(idx))>=1.479){
		if(Ntp->Electron_sigmaIetaIeta(idx)>0.03) return false;
		if(Ntp->Electron_Gsf_deltaPhiSuperClusterTrackAtVtx(idx)>0.10) return false;
		if(Ntp->Electron_Gsf_deltaEtaSuperClusterTrackAtVtx(idx)>0.009) return false;
		if(Ntp->Electron_hadronicOverEm(idx)>0.10) return false;
	}
	return true;
}

bool ZtoEMu::isFakeElectron(unsigned int idx, unsigned int vtx){
	if(vtx<0 || vtx>=Ntp->NVtx()) return false;
	if(!isFakeElectron(idx)) return false;
	if(dz(Ntp->Electron_p4(idx),Ntp->Electron_Poca(idx),Ntp->Vtx(vtx))>0.1) return false;
	if(dxy(Ntp->Electron_p4(idx),Ntp->Electron_Poca(idx),Ntp->Vtx(vtx))>0.02) return false;
	return true;
}

double ZtoEMu::Electron_RelIso(unsigned int idx){
	return (Ntp->Electron_chargedHadronIso(idx)+std::max((double)0.,Ntp->Electron_neutralHadronIso(idx)+Ntp->Electron_photonIso(idx)-Ntp->RhoIsolationAllInputTags()*Electron_Aeff_R04(Ntp->Electron_supercluster_eta(idx))))/Ntp->Electron_p4(idx).Pt();
}

double ZtoEMu::Electron_Aeff_R04(double Eta){
	double eta=fabs(Eta);
	if(eta>=0. && eta<1.) return 0.208;
	else if(eta>=1. && eta<1.479) return 0.209;
	else if(eta>=1.479 && eta<2.) return 0.115;
	else if(eta>=2. && eta<2.2) return 0.143;
	else if(eta>=2.2 && eta<2.3) return 0.183;
	else if(eta>=2.3 && eta<2.4) return 0.194;
	else if(eta>=2.4) return 0.261;
}

double ZtoEMu::Electron_Aeff_R03(double Eta){
	double eta=fabs(Eta);
	if(eta>=0. && eta<1.) return 0.13;
	else if(eta>=1. && eta<1.479) return 0.14;
	else if(eta>=1.479 && eta<2.) return 0.07;
	else if(eta>=2. && eta<2.2) return 0.09;
	else if(eta>=2.2 && eta<2.3) return 0.11;
	else if(eta>=2.3 && eta<2.4) return 0.11;
	else if(eta>=2.4) return 0.14;
}

bool ZtoEMu::isLooseElectron(unsigned int idx){
	if(fabs(Ntp->Electron_supercluster_eta(idx))<=1.479){ //barrel
		if(Ntp->Electron_Gsf_deltaEtaSuperClusterTrackAtVtx(idx)<0.007 &&
				Ntp->Electron_Gsf_deltaPhiSuperClusterTrackAtVtx(idx)<0.15 &&
				Ntp->Electron_sigmaIetaIeta(idx)<0.01 &&
				Ntp->Electron_hadronicOverEm(idx)<0.12 &&
				fabs(1/Ntp->Electron_ecalEnergy(idx)-1/Ntp->Electron_trackMomentumAtVtx(idx))<0.05 &&
				Electron_RelIso(idx)<0.15 &&
				!Ntp->Electron_HasMatchedConversions(idx) &&
				Ntp->Electron_numberOfMissedHits(idx)<=1
				){
			return true;
		}
	}else if(fabs(Ntp->Electron_supercluster_eta(idx))>1.479 && fabs(Ntp->Electron_supercluster_eta(idx))<2.5){ //endcaps
		if(Ntp->Electron_Gsf_deltaEtaSuperClusterTrackAtVtx(idx)<0.009 &&
				Ntp->Electron_Gsf_deltaPhiSuperClusterTrackAtVtx(idx)<0.10 &&
				Ntp->Electron_sigmaIetaIeta(idx)<0.03 &&
				Ntp->Electron_hadronicOverEm(idx)<0.10 &&
				fabs(1/Ntp->Electron_ecalEnergy(idx)-1/Ntp->Electron_trackMomentumAtVtx(idx))<0.05 &&
				!Ntp->Electron_HasMatchedConversions(idx) &&
				Ntp->Electron_numberOfMissedHits(idx)<=1
				){
			if(Ntp->Electron_p4(idx).Pt()>=20.0 && Electron_RelIso(idx)<0.15){
				return true;
			}else if(Ntp->Electron_p4(idx).Pt()<20.0 && Electron_RelIso(idx)<0.10){
				return true;
			}
		}
	}
	return false;
}

//////////////////////////////
//
// Trigger & ID efficiencies
//

double ZtoEMu::MuonIDeff(unsigned int idx){
	double pt = Ntp->Muon_p4(idx).Pt();
	double eta = fabs(Ntp->Muon_p4(idx).Eta());
	double eff = 1.;
	// TODO: fix me
	/*if(pt<20) pt=20;
	if(pt>100) pt=100;
	if(eta<0.9) eff = MuIdEff09->Eval(pt)*MuIsoEff09->Eval(pt);
	if(eta>=0.9 && eta<1.2) eff = MuIdEff12->Eval(pt)*MuIsoEff12->Eval(pt);
	if(eta>=1.2 && eta<2.1) eff = MuIdEff21->Eval(pt)*MuIsoEff21->Eval(pt);
	if(eta>=2.1 && eta<2.4) eff = MuIdEff24->Eval(pt)*MuIsoEff24->Eval(pt);*/

	// eff_total = eff_Id*eff_Iso
	//if(pt<10) pt=10;
	//if(pt>300) pt=300;
	if(pt>=10. && pt<20.){
		if(eta<0.9) eff = 0.970*0.940;
		if(eta>=0.9 && eta<1.2) eff = 1.002*0.948;
		if(eta>=1.2 && eta<2.1) eff = 1.018*0.972;
		if(eta>=2.1 && eta<2.4) eff = 1.005*1.117;
	}
	if(pt>=20. && pt<25.){
		if(eta<0.9) eff = 0.989*0.977;
		if(eta>=0.9 && eta<1.2) eff = 0.994*0.986;
		if(eta>=1.2 && eta<2.1) eff = 1.000*0.990;
		if(eta>=2.1 && eta<2.4) eff = 0.998*1.116;
	}
	if(pt>=25. && pt<30.){
		if(eta<0.9) eff = 0.992*0.996;
		if(eta>=0.9 && eta<1.2) eff = 0.995*1.000;
		if(eta>=1.2 && eta<2.1) eff = 0.998*1.003;
		if(eta>=2.1 && eta<2.4) eff = 0.996*1.097;
	}
	if(pt>=30. && pt<35.){
		if(eta<0.9) eff = 0.993*0.993;
		if(eta>=0.9 && eta<1.2) eff = 0.993*1.000;
		if(eta>=1.2 && eta<2.1) eff = 0.997*1.004;
		if(eta>=2.1 && eta<2.4) eff = 1.001*1.075;
	}
	if(pt>=35. && pt<40.){
		if(eta<0.9) eff = 0.994*0.994;
		if(eta>=0.9 && eta<1.2) eff = 0.992*0.999;
		if(eta>=1.2 && eta<2.1) eff = 0.996*1.002;
		if(eta>=2.1 && eta<2.4) eff = 0.993*1.061;
	}
	if(pt>=40. && pt<50.){
		if(eta<0.9) eff = 0.992*0.994;
		if(eta>=0.9 && eta<1.2) eff = 0.992*0.999;
		if(eta>=1.2 && eta<2.1) eff = 0.996*1.001;
		if(eta>=2.1 && eta<2.4) eff = 0.995*1.034;
	}
	if(pt>=50. && pt<60.){
		if(eta<0.9) eff = 0.991*0.996;
		if(eta>=0.9 && eta<1.2) eff = 0.995*0.998;
		if(eta>=1.2 && eta<2.1) eff = 0.995*1.000;
		if(eta>=2.1 && eta<2.4) eff = 0.994*1.025;
	}
	if(pt>=60. && pt<90.){
		if(eta<0.9) eff = 0.989*0.999;
		if(eta>=0.9 && eta<1.2) eff = 0.990*0.999;
		if(eta>=1.2 && eta<2.1) eff = 0.992*1.001;
		if(eta>=2.1 && eta<2.4) eff = 0.989*1.015;
	}
	if(pt>=90. && pt<140.){
		if(eta<0.9) eff = 1.004*1.000;
		if(eta>=0.9 && eta<1.2) eff = 1.009*1.001;
		if(eta>=1.2 && eta<2.1) eff = 1.023*0.999;
		if(eta>=2.1 && eta<2.4) eff = 1.060*1.008;
	}
	if(pt>=140. && pt<300.){
		if(eta<0.9) eff = 1.019*0.999;
		if(eta>=0.9 && eta<1.2) eff = 1.011*1.002;
		if(eta>=1.2 && eta<2.1) eff = 0.975*0.996;
		if(eta>=2.1 && eta<2.4) eff = 0.891*1.011;
	}
	return eff;
}

double ZtoEMu::MuonIDerrUp(unsigned int idx){
	double pt = Ntp->Muon_p4(idx).Pt();
	double eta = fabs(Ntp->Muon_p4(idx).Eta());
	double err = 0.;
	// TODO: fix me
	if(pt<20) pt=20;
	if(eta<0.9){
		err = sqrt(pow(MuIsoEff09->Eval(pt)*MuIdEff09->GetErrorYhigh(findBin(MuIdEff09,pt)),2)
				+pow(MuIdEff09->Eval(pt)*MuIsoEff09->GetErrorYhigh(findBin(MuIsoEff09,pt)),2));
	}
	if(eta>=0.9 && eta<1.2){
		err = sqrt(pow(MuIsoEff12->Eval(pt)*MuIdEff12->GetErrorYhigh(findBin(MuIdEff12,pt)),2)
				+pow(MuIdEff12->Eval(pt)*MuIsoEff12->GetErrorYhigh(findBin(MuIsoEff12,pt)),2));
	}
	if(eta>=1.2 && eta<2.1){
		err = sqrt(pow(MuIsoEff21->Eval(pt)*MuIdEff21->GetErrorYhigh(findBin(MuIdEff21,pt)),2)
				+pow(MuIdEff21->Eval(pt)*MuIsoEff21->GetErrorYhigh(findBin(MuIsoEff21,pt)),2));
	}
	if(eta>=2.1 && eta<2.4){
		err = sqrt(pow(MuIsoEff24->Eval(pt)*MuIdEff24->GetErrorYhigh(findBin(MuIdEff24,pt)),2)
				+pow(MuIdEff24->Eval(pt)*MuIsoEff24->GetErrorYhigh(findBin(MuIsoEff24,pt)),2));
	}
	return err;
}

double ZtoEMu::MuonIDerrDown(unsigned int idx){
	double pt = Ntp->Muon_p4(idx).Pt();
	double eta = fabs(Ntp->Muon_p4(idx).Eta());
	double err = 0.;
	// TODO: fix me
	if(pt<20) pt=20;
	if(eta<0.9){
		err = sqrt(pow(MuIsoEff09->Eval(pt)*MuIdEff09->GetErrorYlow(findBin(MuIdEff09,pt)),2)
				+pow(MuIdEff09->Eval(pt)*MuIsoEff09->GetErrorYlow(findBin(MuIsoEff09,pt)),2));
	}
	if(eta>=0.9 && eta<1.2){
		err = sqrt(pow(MuIsoEff12->Eval(pt)*MuIdEff12->GetErrorYlow(findBin(MuIdEff12,pt)),2)
				+pow(MuIdEff12->Eval(pt)*MuIsoEff12->GetErrorYlow(findBin(MuIsoEff12,pt)),2));
	}
	if(eta>=1.2 && eta<2.1){
		err = sqrt(pow(MuIsoEff21->Eval(pt)*MuIdEff21->GetErrorYlow(findBin(MuIdEff21,pt)),2)
				+pow(MuIdEff21->Eval(pt)*MuIsoEff21->GetErrorYlow(findBin(MuIsoEff21,pt)),2));
	}
	if(eta>=2.1 && eta<2.4){
		err = sqrt(pow(MuIsoEff24->Eval(pt)*MuIdEff24->GetErrorYlow(findBin(MuIdEff24,pt)),2)
				+pow(MuIdEff24->Eval(pt)*MuIsoEff24->GetErrorYlow(findBin(MuIsoEff24,pt)),2));
	}
	return err;
}

double ZtoEMu::MuonHiggsIDeff(unsigned int idx){
	double pt = Ntp->Muon_p4(idx).Pt();
	double eta = fabs(Ntp->Muon_p4(idx).Eta());
	double eff = 1.;
	if(pt>10 && pt<=15){
		if(eta>=0 && eta<0.8){
			eff = 0.9771;
		}else if(eta>=0.8 && eta<1.2){
			eff = 0.9746;
		}else if(eta>=1.2 && eta<1.6){
			eff = 0.9644;
		}else if(eta>=1.6 && eta<2.1){
			eff = 0.9891;
		}
	}else if(pt>15 && pt<=20){
		if(eta>=0 && eta<0.8){
			eff = 0.9548;
		}else if(eta>=0.8 && eta<1.2){
			eff = 0.9701;
		}else if(eta>=1.2 && eta<1.6){
			eff = 0.9766;
		}else if(eta>=1.6 && eta<2.1){
			eff = 0.9892;
		}
	}else if(pt>20 && pt<=25){
		if(eta>=0 && eta<0.8){
			eff = 0.9648;
		}else if(eta>=0.8 && eta<1.2){
			eff = 0.9836;
		}else if(eta>=1.2 && eta<1.6){
			eff = 0.9820;
		}else if(eta>=1.6 && eta<2.1){
			eff = 0.9909;
		}
	}else if(pt>25 && pt<=30){
		if(eta>=0 && eta<0.8){
			eff = 0.9676;
		}else if(eta>=0.8 && eta<1.2){
			eff = 0.9817;
		}else if(eta>=1.2 && eta<1.6){
			eff = 0.9886;
		}else if(eta>=1.6 && eta<2.1){
			eff = 0.9883;
		}
	}else if(pt>30 && pt<=35){
		if(eta>=0 && eta<0.8){
			eff = 0.9883;
		}else if(eta>=0.8 && eta<1.2){
			eff = 0.9833;
		}else if(eta>=1.2 && eta<1.6){
			eff = 0.9910;
		}else if(eta>=1.6 && eta<2.1){
			eff = 0.9900;
		}
	}else if(pt>35){
		if(eta>=0 && eta<0.8){
			eff = 0.9826;
		}else if(eta>=0.8 && eta<1.2){
			eff = 0.9841;
		}else if(eta>=1.2 && eta<1.6){
			eff = 0.9900;
		}else if(eta>=1.6 && eta<2.1){
			eff = 0.9886;
		}
	}
	return eff;
}

double ZtoEMu::MuonTriggerEff(unsigned int idx){
	double pt = Ntp->Muon_p4(idx).Pt();
	double eta = fabs(Ntp->Muon_p4(idx).Eta());
	if(pt>10 && pt<=15){
		if(eta>=0 && eta<0.8){
			return 0.9829;
		}else if(eta>=0.8 && eta<1.2){
			return 0.9745;
		}else if(eta>=1.2 && eta<1.6){
			return 0.9943;
		}else if(eta>=1.6 && eta<2.1){
			return 0.9158;
		}
	}else if(pt>15 && pt<=20){
		if(eta>=0 && eta<0.8){
			return 0.9850;
		}else if(eta>=0.8 && eta<1.2){
			return 0.9852;
		}else if(eta>=1.2 && eta<1.6){
			return 0.9743;
		}else if(eta>=1.6 && eta<2.1){
			return 0.9333;
		}
	}else if(pt>20 && pt<=25){
		if(eta>=0 && eta<0.8){
			return 0.9951;
		}else if(eta>=0.8 && eta<1.2){
			return 0.9610;
		}else if(eta>=1.2 && eta<1.6){
			return 0.9716;
		}else if(eta>=1.6 && eta<2.1){
			return 0.9459;
		}
	}else if(pt>25 && pt<=30){
		if(eta>=0 && eta<0.8){
			return 0.9869;
		}else if(eta>=0.8 && eta<1.2){
			return 0.9779;
		}else if(eta>=1.2 && eta<1.6){
			return 0.9665;
		}else if(eta>=1.6 && eta<2.1){
			return 0.9501;
		}
	}else if(pt>30 && pt<=35){
		if(eta>=0 && eta<0.8){
			return 0.9959;
		}else if(eta>=0.8 && eta<1.2){
			return 0.9881;
		}else if(eta>=1.2 && eta<1.6){
			return 0.9932;
		}else if(eta>=1.6 && eta<2.1){
			return 0.9391;
		}
	}else if(pt>35){
		if(eta>=0 && eta<0.8){
			return 0.9986;
		}else if(eta>=0.8 && eta<1.2){
			return 0.9540;
		}else if(eta>=1.2 && eta<1.6){
			return 0.9549;
		}else if(eta>=1.6 && eta<2.1){
			return 0.9386;
		}
	}
	return 1.;
}

double ZtoEMu::MuonTriggerErr(unsigned int idx){
	double pt = Ntp->Muon_p4(idx).Pt();
	double eta = fabs(Ntp->Muon_p4(idx).Eta());
	if(pt>10 && pt<=15){
		if(eta>=0 && eta<0.8){
			return 0.0058;
		}else if(eta>=0.8 && eta<1.2){
			return 0.0124;
		}else if(eta>=1.2 && eta<1.6){
			return 0.0164;
		}else if(eta>=1.6 && eta<2.1){
			return 0.0176;
		}
	}else if(pt>15 && pt<=20){
		if(eta>=0 && eta<0.8){
			return 0.0056;
		}else if(eta>=0.8 && eta<1.2){
			return 0.0171;
		}else if(eta>=1.2 && eta<1.6){
			return 0.0179;
		}else if(eta>=1.6 && eta<2.1){
			return 0.0162;
		}
	}else if(pt>20 && pt<=25){
		if(eta>=0 && eta<0.8){
			return 0.0060;
		}else if(eta>=0.8 && eta<1.2){
			return 0.0116;
		}else if(eta>=1.2 && eta<1.6){
			return 0.0141;
		}else if(eta>=1.6 && eta<2.1){
			return 0.0159;
		}
	}else if(pt>25 && pt<=30){
		if(eta>=0 && eta<0.8){
			return 0.0074;
		}else if(eta>=0.8 && eta<1.2){
			return 0.0187;
		}else if(eta>=1.2 && eta<1.6){
			return 0.0184;
		}else if(eta>=1.6 && eta<2.1){
			return 0.0251;
		}
	}else if(pt>30 && pt<=35){
		if(eta>=0 && eta<0.8){
			return 0.0085;
		}else if(eta>=0.8 && eta<1.2){
			return 0.0227;
		}else if(eta>=1.2 && eta<1.6){
			return 0.0271;
		}else if(eta>=1.6 && eta<2.1){
			return 0.0307;
		}
	}else if(pt>35){
		if(eta>=0 && eta<0.8){
			return 0.0087;
		}else if(eta>=0.8 && eta<1.2){
			return 0.0165;
		}else if(eta>=1.2 && eta<1.6){
			return 0.0211;
		}else if(eta>=1.6 && eta<2.1){
			return 0.0209;
		}
	}
	return 0.;
}

double ZtoEMu::ElectronIDeff(unsigned int idx, std::string id){
	if(id=="Trig") return ElectronTrigIDeff(idx);
	if(id=="NonTrig") return ElectronNonTrigIDeff(idx);
	if(id=="Higgs") return ElectronHiggsIDeff(idx);
	return 1.;
}

double ZtoEMu::ElectronIDerr(unsigned int idx, std::string id){
	if(id=="Trig") return ElectronTrigIDerr(idx);
	if(id=="NonTrig") return ElectronNonTrigIDerr(idx);
	return 0.;
}

double ZtoEMu::ElectronTrigIDeff(unsigned int idx){
	double pt = Ntp->Electron_p4(idx).Pt();
	if(pt>=200) pt = 199;
	double eta = fabs(Ntp->Electron_supercluster_eta(idx));
	double eff = 1.;
	if(eta<2.5){
		eff = ElectronTrigEff->GetBinContent(ElectronTrigEff->FindFixBin(eta,pt));
		eff*=ElectronNonTrigRecoEff->GetBinContent(ElectronNonTrigRecoEff->FindFixBin(pt,Ntp->Electron_supercluster_eta(idx)))/ElectronNonTrigEff->GetBinContent(ElectronNonTrigEff->FindFixBin(pt,Ntp->Electron_supercluster_eta(idx)));
	}
	return eff;
}

double ZtoEMu::ElectronTrigIDerr(unsigned int idx){
	double pt = Ntp->Electron_p4(idx).Pt();
	double eta = fabs(Ntp->Electron_supercluster_eta(idx));
	double err = 0.;
	if(eta<2.5){
		err = ElectronTrigEff->GetBinError(ElectronTrigEff->FindFixBin(eta,pt));
	}
	return err;
}

double ZtoEMu::ElectronNonTrigIDeff(unsigned int idx){
	double pt = Ntp->Electron_p4(idx).Pt();
	double eta = Ntp->Electron_supercluster_eta(idx);
	double eff = 1.;
	if(fabs(eta)<2.5){
		eff = ElectronNonTrigEff->GetBinContent(ElectronNonTrigEff->FindFixBin(pt,eta));
	}
	return eff;
}

double ZtoEMu::ElectronNonTrigIDerr(unsigned int idx){
	double pt = Ntp->Electron_p4(idx).Pt();
	double eta = Ntp->Electron_supercluster_eta(idx);
	double err = 0.;
	if(fabs(eta)<2.5){
		err = ElectronNonTrigEff->GetBinError(ElectronNonTrigEff->FindFixBin(pt,eta));
	}
	return err;
}

double ZtoEMu::ElectronHiggsIDeff(unsigned int idx){
	double pt = Ntp->Electron_p4(idx).Pt();
	double eta = fabs(Ntp->Electron_supercluster_eta(idx));
	double eff = 1.;
	if(pt>10 && pt<=15){
		if(eta>=0 && eta<0.8){
			eff = 0.7654;
		}else if(eta>=0.8 && eta<1.5){
			eff = 0.7693;
		}else if(eta>=1.5 && eta<2.3){
			eff = 0.5719;
		}
	}else if(pt>15 && pt<=20){
		if(eta>=0 && eta<0.8){
			eff = 0.8394;
		}else if(eta>=0.8 && eta<1.5){
			eff = 0.8457;
		}else if(eta>=1.5 && eta<2.3){
			eff = 0.7024;
		}
	}else if(pt>20 && pt<=25){
		if(eta>=0 && eta<0.8){
			eff = 0.8772;
		}else if(eta>=0.8 && eta<1.5){
			eff = 0.8530;
		}else if(eta>=1.5 && eta<2.3){
			eff = 0.7631;
		}
	}else if(pt>25 && pt<=30){
		if(eta>=0 && eta<0.8){
			eff = 0.9006;
		}else if(eta>=0.8 && eta<1.5){
			eff = 0.8874;
		}else if(eta>=1.5 && eta<2.3){
			eff = 0.8092;
		}
	}else if(pt>30 && pt<=35){
		if(eta>=0 && eta<0.8){
			eff = 0.9261;
		}else if(eta>=0.8 && eta<1.5){
			eff = 0.9199;
		}else if(eta>=1.5 && eta<2.3){
			eff = 0.8469;
		}
	}else if(pt>35){
		if(eta>=0 && eta<0.8){
			eff = 0.9514;
		}else if(eta>=0.8 && eta<1.5){
			eff = 0.9445;
		}else if(eta>=1.5 && eta<2.3){
			eff = 0.9078;
		}
	}
	return eff;
}

double ZtoEMu::ElectronTriggerEff(unsigned int idx){
	double pt = Ntp->Electron_p4(idx).Pt();
	double eta = fabs(Ntp->Electron_supercluster_eta(idx));
	if(pt>10 && pt<=15){
		if(eta>=0 && eta<0.8){
			return 0.9548;
		}else if(eta>=0.8 && eta<1.5){
			return 0.9015;
		}else if(eta>=1.5 && eta<2.3){
			return 0.9017;
		}
	}else if(pt>15 && pt<=20){
		if(eta>=0 && eta<0.8){
			return 0.9830;
		}else if(eta>=0.8 && eta<1.5){
			return 0.9672;
		}else if(eta>=1.5 && eta<2.3){
			return 0.9463;
		}
	}else if(pt>20 && pt<=25){
		if(eta>=0 && eta<0.8){
			return 0.9707;
		}else if(eta>=0.8 && eta<1.5){
			return 0.9731;
		}else if(eta>=1.5 && eta<2.3){
			return 0.9691;
		}
	}else if(pt>25 && pt<=30){
		if(eta>=0 && eta<0.8){
			return 0.9768;
		}else if(eta>=0.8 && eta<1.5){
			return 0.9870;
		}else if(eta>=1.5 && eta<2.3){
			return 0.9727;
		}
	}else if(pt>30 && pt<=35){
		if(eta>=0 && eta<0.8){
			return 1.0047;
		}else if(eta>=0.8 && eta<1.5){
			return 0.9891;
		}else if(eta>=1.5 && eta<2.3){
			return 0.9858;
		}
	}else if(pt>35){
		if(eta>=0 && eta<0.8){
			return 1.0063;
		}else if(eta>=0.8 && eta<1.5){
			return 1.0047;
		}else if(eta>=1.5 && eta<2.3){
			return 1.0015;
		}
	}
	return 1.;
}

double ZtoEMu::ElectronTriggerErr(unsigned int idx){
	double pt = Ntp->Electron_p4(idx).Pt();
	double eta = fabs(Ntp->Electron_supercluster_eta(idx));
	if(pt>10 && pt<=15){
		if(eta>=0 && eta<0.8){
			return 0.0197;
		}else if(eta>=0.8 && eta<1.5){
			return 0.0205;
		}else if(eta>=1.5 && eta<2.3){
			return 0.0470;
		}
	}else if(pt>15 && pt<=20){
		if(eta>=0 && eta<0.8){
			return 0.0115;
		}else if(eta>=0.8 && eta<1.5){
			return 0.0113;
		}else if(eta>=1.5 && eta<2.3){
			return 0.0212;
		}
	}else if(pt>20 && pt<=25){
		if(eta>=0 && eta<0.8){
			return 0.0087;
		}else if(eta>=0.8 && eta<1.5){
			return 0.0083;
		}else if(eta>=1.5 && eta<2.3){
			return 0.0149;
		}
	}else if(pt>25 && pt<=30){
		if(eta>=0 && eta<0.8){
			return 0.0084;
		}else if(eta>=0.8 && eta<1.5){
			return 0.0083;
		}else if(eta>=1.5 && eta<2.3){
			return 0.0162;
		}
	}else if(pt>30 && pt<=35){
		if(eta>=0 && eta<0.8){
			return 0.0100;
		}else if(eta>=0.8 && eta<1.5){
			return 0.0111;
		}else if(eta>=1.5 && eta<2.3){
			return 0.0112;
		}
	}else if(pt>35){
		if(eta>=0 && eta<0.8){
			return 0.0078;
		}else if(eta>=0.8 && eta<1.5){
			return 0.0073;
		}else if(eta>=1.5 && eta<2.3){
			return 0.0135;
		}
	}
	return 0.;
}

double ZtoEMu::ElectronEmbeddedEff(unsigned int idx){
	double pt = Ntp->Electron_p4(idx).Pt();
	double eta = fabs(Ntp->Electron_supercluster_eta(idx));
	double eff = 1.;
	double xAxis[10] = {10,15,20,25,30,40,55,70,100,200};
	double yAxis[4] = {0,0.8,1.479,2.5};
	TH2D hPtEtaSFL("hPtEtaSFL","",9,xAxis,3,yAxis);
	hPtEtaSFL.SetBinContent(12,0.81);
	hPtEtaSFL.SetBinContent(13,0.91);
	hPtEtaSFL.SetBinContent(14,0.95);
	hPtEtaSFL.SetBinContent(15,0.96);
	hPtEtaSFL.SetBinContent(16,0.97);
	hPtEtaSFL.SetBinContent(17,0.98);
	hPtEtaSFL.SetBinContent(18,0.99);
	hPtEtaSFL.SetBinContent(19,0.98);
	hPtEtaSFL.SetBinContent(20,0.99);
	hPtEtaSFL.SetBinContent(21,0.98);
	hPtEtaSFL.SetBinContent(23,0.78);
	hPtEtaSFL.SetBinContent(24,0.89);
	hPtEtaSFL.SetBinContent(25,0.92);
	hPtEtaSFL.SetBinContent(26,0.94);
	hPtEtaSFL.SetBinContent(27,0.94);
	hPtEtaSFL.SetBinContent(28,0.97);
	hPtEtaSFL.SetBinContent(29,0.97);
	hPtEtaSFL.SetBinContent(30,0.99);
	hPtEtaSFL.SetBinContent(31,1.00);
	hPtEtaSFL.SetBinContent(32,1.00);
	hPtEtaSFL.SetBinContent(34,0.46);
	hPtEtaSFL.SetBinContent(35,0.66);
	hPtEtaSFL.SetBinContent(36,0.73);
	hPtEtaSFL.SetBinContent(37,0.80);
	hPtEtaSFL.SetBinContent(38,0.83);
	hPtEtaSFL.SetBinContent(39,0.86);
	hPtEtaSFL.SetBinContent(40,0.88);
	hPtEtaSFL.SetBinContent(41,0.91);
	hPtEtaSFL.SetBinContent(42,0.93);
	hPtEtaSFL.SetBinContent(43,1.00);

	if(pt>199.99) pt = 199.9;
	if(eta>2.49) eta = 2.49;
	if(pt<10.) eff = 0.;
	eff = hPtEtaSFL.GetBinContent(hPtEtaSFL.FindFixBin(pt,eta));

	return eff;
}

double ZtoEMu::TriggerEff(unsigned int muid, unsigned int eid, TString path){
	double eff = 1.;
	if(path.Contains("Mu17_Ele8")){
		eff = SingleMu(muid) + (1-SingleMu(muid))*SingleEle(eid)
				+ (1-SingleMu(muid))*(1-SingleEle(eid))*
				(DoubleMuLeading(muid)*DoubleEleTrailing(eid) + (1-DoubleMuLeading(muid)*DoubleEleTrailing(eid))*DoubleEleLeading(eid)*DoubleMuTrailing(muid));
	}
	if(path.Contains("Mu8_Ele17")){
		eff = SingleEle(eid) + (1-SingleEle(eid))*SingleMu(muid)
				+ (1-SingleEle(eid))*(1-SingleMu(muid))*
				(DoubleEleLeading(eid)*DoubleMuTrailing(muid) + (1-DoubleEleLeading(eid)*DoubleMuTrailing(muid))*DoubleMuLeading(muid)*DoubleEleTrailing(eid));
	}
	return eff;
}

double ZtoEMu::SingleEle(unsigned int idx){
	double pt = Ntp->Electron_p4(idx).Pt();
	double eta = fabs(Ntp->Electron_supercluster_eta(idx));
	double eff = 1.;
	if(pt>=60.)pt=55.;
	if(eta<1.5) eff = SingleEle15->GetBinContent(SingleEle15->FindFixBin(pt));
	if(eta>=1.5 && eta<2.5) eff = SingleEle25->GetBinContent(SingleEle25->FindFixBin(pt));
	return eff;
}

double ZtoEMu::DoubleEleLeading(unsigned int idx){
	double pt = Ntp->Electron_p4(idx).Pt();
	double eta = fabs(Ntp->Electron_supercluster_eta(idx));
	double eff = 1.;
	if(pt>=60.)pt=55.;
	if(eta<1.5) eff = DoubleEleLead15->GetBinContent(DoubleEleLead15->FindFixBin(pt));
	if(eta>=1.5 && eta<2.5) eff = DoubleEleLead25->GetBinContent(DoubleEleLead25->FindFixBin(pt));
	return eff;
}

double ZtoEMu::DoubleEleTrailing(unsigned int idx){
	double pt = Ntp->Electron_p4(idx).Pt();
	double eta = fabs(Ntp->Electron_supercluster_eta(idx));
	double eff = 1.;
	if(pt>=60.)pt=55.;
	if(eta<1.5) eff = DoubleEleTrail15->GetBinContent(DoubleEleTrail15->FindFixBin(pt));
	if(eta>=1.5 && eta<2.5) eff = DoubleEleTrail25->GetBinContent(DoubleEleTrail25->FindFixBin(pt));
	return eff;
}

double ZtoEMu::SingleMu(unsigned int idx){
	double pt = Ntp->Muon_p4(idx).Pt();
	double eta = fabs(Ntp->Muon_p4(idx).Eta());
	double eff = 1.;
	if(pt>=60.)pt=55.;
	if(eta<0.8) eff = SingleMu08->GetBinContent(SingleMu08->FindFixBin(pt));
	if(eta>=0.8 && eta<1.2) eff = SingleMu12->GetBinContent(SingleMu12->FindFixBin(pt));
	if(eta>=1.2 && eta<2.1) eff = SingleMu21->GetBinContent(SingleMu21->FindFixBin(pt));
	if(eta>=2.1 && eta<2.5) eff = SingleMu25->GetBinContent(SingleMu25->FindFixBin(pt));
	return eff;
}

double ZtoEMu::DoubleMuLeading(unsigned int idx){
	double pt = Ntp->Muon_p4(idx).Pt();
	double eta = fabs(Ntp->Muon_p4(idx).Eta());
	double eff = 1.;
	if(pt>=60.)pt=55.;
	if(eta<1.2) eff = DoubleMuLead12->GetBinContent(DoubleMuLead12->FindFixBin(pt));
	if(eta>=1.2 && eta<2.1) eff = DoubleMuLead21->GetBinContent(DoubleMuLead21->FindFixBin(pt));
	if(eta>=2.1 && eta<2.5) eff = DoubleMuLead25->GetBinContent(DoubleMuLead25->FindFixBin(pt));
	return eff;
}

double ZtoEMu::DoubleMuTrailing(unsigned int idx){
	double pt = Ntp->Muon_p4(idx).Pt();
	double eta = fabs(Ntp->Muon_p4(idx).Eta());
	double eff = 1.;
	if(pt>=60.)pt=55.;
	if(eta<1.2) eff = DoubleMuTrail12->GetBinContent(DoubleMuTrail12->FindFixBin(pt));
	if(eta>=1.2 && eta<2.1) eff = DoubleMuTrail21->GetBinContent(DoubleMuTrail21->FindFixBin(pt));
	if(eta>=2.1 && eta<2.5) eff = DoubleMuTrail25->GetBinContent(DoubleMuTrail25->FindFixBin(pt));
	return eff;
}

double ZtoEMu::TrackingEff(double eta){
	double eff = 1.;
	if(eta>=-2.4 && eta<-2.2) eff = 0.9861;
	if(eta>=-2.2 && eta<-2.0) eff = 0.9901;
	if(eta>=-2.0 && eta<-1.8) eff = 0.9946;
	if(eta>=-1.8 && eta<-1.6) eff = 0.9966;
	if(eta>=-1.6 && eta<-1.4) eff = 0.9964;
	if(eta>=-1.4 && eta<-1.2) eff = 0.9970;
	if(eta>=-1.2 && eta<-1.0) eff = 0.9973;
	if(eta>=-1.0 && eta<-0.8) eff = 0.9977;
	if(eta>=-0.8 && eta<-0.6) eff = 0.9980;
	if(eta>=-0.6 && eta<-0.4) eff = 0.9981;
	if(eta>=-0.4 && eta<-0.2) eff = 0.9974;
	if(eta>=-0.2 && eta<0.0) eff = 0.9967;
	if(eta>=0.0 && eta<0.2) eff = 0.9960;
	if(eta>=0.2 && eta<0.4) eff = 0.9978;
	if(eta>=0.4 && eta<0.6) eff = 0.9977;
	if(eta>=0.6 && eta<0.8) eff = 0.9975;
	if(eta>=0.8 && eta<1.0) eff = 0.9976;
	if(eta>=1.0 && eta<1.2) eff = 0.9964;
	if(eta>=1.2 && eta<1.4) eff = 0.9970;
	if(eta>=1.4 && eta<1.6) eff = 0.9948;
	if(eta>=1.6 && eta<1.8) eff = 0.9977;
	if(eta>=1.8 && eta<2.0) eff = 0.9974;
	if(eta>=2.0 && eta<2.2) eff = 0.9917;
	if(eta>=2.2 && eta<2.4) eff = 0.9806;
	return eff;
}

//////////////////////////////
//
// Calculate fakerate
//

double ZtoEMu::Fakerate(TLorentzVector vec, TH2D *fakeRateHist, std::string type){
	
	double eta1, eta2;
	int ptbin = 0;
	int etabin = 0;
	double FakePt = vec.Pt();
	double FakeEta = vec.Eta();
	double fakerate = 0.;
	
	if(type=="muon"){
		eta1 = 2.1;
		eta2 = 1.2;
	}else if(type=="electron"){
		eta1 = 2.;
		eta2 = 1.479;
	}
	
	if(FakePt<15.)ptbin = 1;
	else if(FakePt>=15. && FakePt<20.)ptbin = 2;
	else if(FakePt>=20. && FakePt<25.)ptbin = 3;
	else if(FakePt>=25. && FakePt<30.)ptbin = 4;
	else if(FakePt>=30.)ptbin = 5;
	
	if(FakeEta<-eta1)etabin = 1;
	else if(FakeEta>=-eta1 && FakeEta<-eta2)etabin = 2;
	else if(FakeEta>=-eta2 && FakeEta<-0.8)etabin = 3;
	else if(FakeEta>=-0.8 && FakeEta<0.)etabin = 4;
	else if(FakeEta>=0. && FakeEta<0.8)etabin = 5;
	else if(FakeEta>=0.8 && FakeEta<eta2)etabin = 6;
	else if(FakeEta>=eta2 && FakeEta<eta1)etabin = 7;
	else if(FakeEta>=eta1)etabin = 8;
	
	if(ptbin==0 || etabin==0){
		fakerate = 0;
	}else{
		fakerate = fakeRateHist->GetBinContent(ptbin,etabin);
	}
	
	return fakerate/(1-fakerate);
}

double ZtoEMu::FakerateWW(unsigned int idx, std::string type){
	double fakerate = 0.;
	double pt(0),eta(0);
	if(type=="muon"){
		pt = Ntp->Muon_p4(idx).Pt();
		eta = fabs(Ntp->Muon_p4(idx).Eta());
		if(eta<=1.){
			if(pt>10. && pt<=15.) fakerate = 0.131;
			if(pt>15. && pt<=20.) fakerate = 0.143;
			if(pt>20. && pt<=25.) fakerate = 0.198;
			if(pt>25. && pt<=30.) fakerate = 0.182;
			if(pt>30.) fakerate = 0.170;
		}
		if(eta>1. && eta<=1.479){
			if(pt>10. && pt<=15.) fakerate = 0.154;
			if(pt>15. && pt<=20.) fakerate = 0.191;
			if(pt>20. && pt<=25.) fakerate = 0.239;
			if(pt>25. && pt<=30.) fakerate = 0.228;
			if(pt>30.) fakerate = 0.244;
		}
		if(eta>1.479 && eta<=2.){
			if(pt>10. && pt<=15.) fakerate = 0.194;
			if(pt>15. && pt<=20.) fakerate = 0.235;
			if(pt>20. && pt<=25.) fakerate = 0.221;
			if(pt>25. && pt<=30.) fakerate = 0.195;
			if(pt>30.) fakerate = 0.195;
		}
		if(eta>2. && eta<=2.5){
			if(pt>10. && pt<=15.) fakerate = 0.241;
			if(pt>15. && pt<=20.) fakerate = 0.308;
			if(pt>20. && pt<=25.) fakerate = 0.271;
			if(pt>25. && pt<=30.) fakerate = 0.287;
			if(pt>30.) fakerate = 0.289;
		}
	}else if(type=="electron"){
		pt = Ntp->Electron_p4(idx).Pt();
		eta = fabs(Ntp->Electron_supercluster_eta(idx));
		if(eta<=1.){
			if(pt>10. && pt<=15.) fakerate = 0.045;
			if(pt>15. && pt<=20.) fakerate = 0.044;
			if(pt>20. && pt<=25.) fakerate = 0.041;
			if(pt>25. && pt<=30.) fakerate = 0.059;
			if(pt>30.) fakerate = 0.084;
		}
		if(eta>1. && eta<=1.479){
			if(pt>10. && pt<=15.) fakerate = 0.033;
			if(pt>15. && pt<=20.) fakerate = 0.049;
			if(pt>20. && pt<=25.) fakerate = 0.064;
			if(pt>25. && pt<=30.) fakerate = 0.101;
			if(pt>30.) fakerate = 0.111;
		}
		if(eta>1.479 && eta<=2.){
			if(pt>10. && pt<=15.) fakerate = 0.008;
			if(pt>15. && pt<=20.) fakerate = 0.017;
			if(pt>20. && pt<=25.) fakerate = 0.025;
			if(pt>25. && pt<=30.) fakerate = 0.041;
			if(pt>30.) fakerate = 0.058;
		}
		if(eta>2. && eta<=2.5){
			if(pt>10. && pt<=15.) fakerate = 0.021;
			if(pt>15. && pt<=20.) fakerate = 0.017;
			if(pt>20. && pt<=25.) fakerate = 0.025;
			if(pt>25. && pt<=30.) fakerate = 0.043;
			if(pt>30.) fakerate = 0.066;
		}
	}
	return fakerate/(1-fakerate);
}

double ZtoEMu::FakerateWWerror(unsigned int idx, std::string type){
	double error = 0.;
	double pt(0),eta(0);
	if(type=="muon"){
		pt = Ntp->Muon_p4(idx).Pt();
		eta = fabs(Ntp->Muon_p4(idx).Eta());
		if(eta<=1.){
			if(pt>10. && pt<=15.) error = 0.002;
			if(pt>15. && pt<=20.) error = 0.007;
			if(pt>20. && pt<=25.) error = 0.005;
			if(pt>25. && pt<=30.) error = 0.011;
			if(pt>30.) error = 0.021;
		}
		if(eta>1. && eta<=1.479){
			if(pt>10. && pt<=15.) error = 0.004;
			if(pt>15. && pt<=20.) error = 0.012;
			if(pt>20. && pt<=25.) error = 0.009;
			if(pt>25. && pt<=30.) error = 0.018;
			if(pt>30.) error = 0.036;
		}
		if(eta>1.479 && eta<=2.){
			if(pt>10. && pt<=15.) error = 0.005;
			if(pt>15. && pt<=20.) error = 0.016;
			if(pt>20. && pt<=25.) error = 0.011;
			if(pt>25. && pt<=30.) error = 0.022;
			if(pt>30.) error = 0.041;
		}
		if(eta>2. && eta<=2.5){
			if(pt>10. && pt<=15.) error = 0.009;
			if(pt>15. && pt<=20.) error = 0.027;
			if(pt>20. && pt<=25.) error = 0.021;
			if(pt>25. && pt<=30.) error = 0.045;
			if(pt>30.) error = 0.111;
		}
	}else if(type=="electron"){
		pt = Ntp->Electron_p4(idx).Pt();
		eta = fabs(Ntp->Electron_supercluster_eta(idx));
		if(eta<=1.){
			if(pt>10. && pt<=15.) error = 0.005;
			if(pt>15. && pt<=20.) error = 0.003;
			if(pt>20. && pt<=25.) error = 0.002;
			if(pt>25. && pt<=30.) error = 0.003;
			if(pt>30.) error = 0.006;
		}
		if(eta>1. && eta<=1.479){
			if(pt>10. && pt<=15.) error = 0.004;
			if(pt>15. && pt<=20.) error = 0.003;
			if(pt>20. && pt<=25.) error = 0.003;
			if(pt>25. && pt<=30.) error = 0.005;
			if(pt>30.) error = 0.009;
		}
		if(eta>1.479 && eta<=2.){
			if(pt>10. && pt<=15.) error = 0.002;
			if(pt>15. && pt<=20.) error = 0.001;
			if(pt>20. && pt<=25.) error = 0.002;
			if(pt>25. && pt<=30.) error = 0.003;
			if(pt>30.) error = 0.006;
		}
		if(eta>2. && eta<=2.5){
			if(pt>10. && pt<=15.) error = 0.005;
			if(pt>15. && pt<=20.) error = 0.002;
			if(pt>20. && pt<=25.) error = 0.002;
			if(pt>25. && pt<=30.) error = 0.003;
			if(pt>30.) error = 0.005;
		}
	}
	return error;
}

//////////////////////////////
//
// Finish function
//

void ZtoEMu::Finish(){
	/*for(unsigned i=0;i<15;i++){
		if(zpt.at(i).Integral()>0)zpt.at(i).Scale(1./zpt.at(i).Integral());
		if(zeta.at(i).Integral()>0)zeta.at(i).Scale(1./zeta.at(i).Integral());
		if(zmass.at(i).Integral()>0)zmass.at(i).Scale(1./zmass.at(i).Integral());
	}*/
	Selection::Finish();
}
