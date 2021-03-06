// -*- C++ -*-
//
// Package:    PFCal/runPandora
// Class:      runPandora
// 
/**\class runPandora runPandora.cc PFCal/runPandora/plugins/runPandora.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//
// Original Author:  Andreas Psallidas
//         Created:  Mon, 11 Nov 2013 15:11:14 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Pandora/Pandora.h"
#include "Pandora/StatusCodes.h"
#include "Api/PandoraApi.h"
#include "TLorentzVector.h"
#include "Objects/ParticleFlowObject.h"
#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"
#include "Objects/Cluster.h"
#include "Objects/Track.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitFwd.h"

#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "DataFormats/Math/interface/Vector3D.h"
//DQM services for histogram
#include "DQMServices/Core/interface/DQMStore.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "PFCal/runPandora/interface/steerManager.h"

#include <TH1.h>
#include <TFile.h>
#include <TTree.h>

//
// class declaration
//
namespace reco {class PFRecHit;}

// namespace pandora {class Pandora;}

class runPandora : public edm::EDAnalyzer {
public:
  explicit runPandora(const edm::ParameterSet&);
  ~runPandora();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  static pandora::Pandora        *m_pPandora;

  void prepareTrack(math::XYZVector B_,const reco::RecoToSimCollection pRecoToSim,const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void prepareHits(edm::Handle<EcalRecHitCollection> ecalRecHitHandleEB,edm::Handle<HBHERecHitCollection> hcalRecHitHandleHBHE,edm::Handle<HGCeeRecHitCollection> HGCeeRecHitHandle,edm::Handle<HGChefRecHitCollection> HGChefRecHitHandle,edm::Handle<HGChebRecHitCollection> HGChebRecHitHandle, reco::Vertex& pv, const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void preparemcParticle(edm::Handle<std::vector<reco::GenParticle> > genpart);

  void preparePFO(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void prepareGeometry(const edm::EventSetup& iSetup);

  TrackingParticleRefVector getTpSiblings(TrackingParticleRef tp);
  TrackingParticleRefVector getTpDaughters(TrackingParticleRef tp);

  std::string _outputFileName;
  std::string m_pandoraSettingsXmlFile;

  std::string m_calibrationParameterFile;
  std::string m_energyWeightingFilename;

  std::string m_energyCorrMethod; //energy correction method


  void initPandoraCalibrParameters();
  void readCalibrParameterFile();
  void readEnergyWeight(); //FIXME part of calibration, to be merged to readCalibrParameterFile??
  void getLayerPropertiesEE (const HGCRecHit *eerh, int layer,
        double & nCellInteractionLengths, double & nCellRadiationLengths,
        double & absorberCorrectionEM, double & absorberCorrectionHAD
        );
  void getLayerPropertiesHEF (const HGCRecHit *eerh, int layer,
        double & nCellInteractionLengths, double & nCellRadiationLengths,
        double & absorberCorrectionEM, double & absorberCorrectionHAD
        );
  void getLayerPropertiesHEB (const HGCRecHit *eerh, int layer,
        double & nCellInteractionLengths, double & nCellRadiationLengths,
        double & absorberCorrectionEM, double & absorberCorrectionHAD
        );


  virtual double getEnergyWeight(int subDet, int layer, int showerType);


private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  virtual void resetVariables() {
     for (int il=0; il<100; il++) {
        m_hitEperLayer_EM [il] = 0.;
        m_hitEperLayer_HAD[il] = 0.;

        m_hitEperLayer_EM_EE [il] = 0.;
        m_hitEperLayer_EM_HEF[il] = 0.;
        m_hitEperLayer_EM_HEB[il] = 0.;
     }
  };

  steerManager * stm;

  //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

  // ----------member data ---------------------------

  // ----------access to event data
  edm::InputTag    inputTagEcalRecHitsEB_ ; 
  edm::InputTag    inputTagHcalRecHitsHBHE_ ; 
  edm::InputTag    inputTagHGCEErechit_;
  edm::InputTag    inputTagHGCHEFrechit_;
  edm::InputTag    inputTagHGCHEBrechit_;
  edm::InputTag    inputTagtPRecoTrackAsssociation_;
  edm::InputTag    inputTagGenParticles_;
  std::vector<edm::InputTag>  inputTagGeneralTracks_;
  TFile * file;
  TTree *mytree;
  double ene_track, ene_match_track,ene_match_em,ene_match_had, ene_had,ene_em,ene_match,mass_match,pid_match,pT_match,charge_match;
  double ene_true,mass_true,pid_true,pT_true,charge_true;
  int isDecBefCal;

  double RminVtxDaughter[2];
  double ZminVtxDaughter[2];
  int isDecayedBeforeCalo[2];

  TH1F * Epfos;
  TH1F * Egenpart;
  TH1F * Energy_res;

  TH1F * h_sumPfoE;
  TH1F * h_sumPfoEEM;
  TH1F * h_sumPfoEHad;
  TH1F * h_nbPFOs;

  TH1F * h_sumCaloE;
  TH1F * h_sumEcalEEM;
  TH1F * h_sumHcalEEM;
  TH1F * h_sumEcalEHad;
  TH1F * h_sumHcalEHad;


  TH1F * h_sumCaloEM;
  TH1F * h_sumCaloHad;

  TH1F * h_simDir_sumCaloEM ;//take only hits in sim part. direction
  TH1F * h_simDir_sumCaloHad;//take only hits in sim part. direction


  TH1F * h_MIP_EE ;
  TH1F * h_MIP_HEF;
  TH1F * h_MIP_HEB;
  TH1F * h_MIP_Corr_EE ;
  TH1F * h_MIP_Corr_HEF;
  TH1F * h_MIP_Corr_HEB;
  TH1F * h_MCp_Eta;
  TH1F * h_MCp_Phi;
  TH1F * h_hit_Eta;
  TH1F * h_hit_Phi;

  TH1F * h_hitEperLayer_EM ;
  TH1F * h_hitEperLayer_HAD;
  double * m_hitEperLayer_EM ;
  double * m_hitEperLayer_HAD;

  TH1F * h_hitEperLayer_EM_EE ;
  TH1F * h_hitEperLayer_EM_HEF;
  TH1F * h_hitEperLayer_EM_HEB;
  double * m_hitEperLayer_EM_EE ;
  double * m_hitEperLayer_EM_HEF;
  double * m_hitEperLayer_EM_HEB;


  TH2F * h2_Calo_EM_hcalEecalE;
  TH2F * h2_Calo_Had_hcalEecalE;

  TH2F * h2_EM_hcalEecalE;
  TH2F * h2_Had_hcalEecalE;



  //-------------- energy weighting ------------------
  double * layerSet_EE ;
  double * layerSet_HEF;
  double * layerSet_HEB;
  double * energyWeight_EM_EE ;
  double * energyWeight_EM_HEF;
  double * energyWeight_EM_HEB;
  double * energyWeight_Had_EE ;
  double * energyWeight_Had_HEF;
  double * energyWeight_Had_HEB;
  double  offSet_EM;
  double  offSet_Had;



  double m_firstMCpartEta;
  double m_firstMCpartPhi;
  double m_secondMCpartEta;
  double m_secondMCpartPhi;


  double m_Calibr_ADC2GeV_EE     ;
  double m_Calibr_ADC2GeV_HEF    ;
  double m_Calibr_ADC2GeV_HEB    ;

  double m_EM_addCalibrEE ;
  double m_EM_addCalibrHEF;
  double m_EM_addCalibrHEB;
  double m_HAD_addCalibrEE ;
  double m_HAD_addCalibrHEF;
  double m_HAD_addCalibrHEB;

  double m_hCalThresBarrel       ;
  double m_hCalThresEndCapHEF    ;
  double m_hCalThresEndCapHEB    ;
  double m_eCalThresBarrel       ;
  double m_eCalThresEndCap       ;

  double m_hCalMipThresBarrel    ;
  double m_hCalMipThresEndCapHEF ;
  double m_hCalMipThresEndCapHEB ;
  double m_eCalMipThresBarrel    ;
  double m_eCalMipThresEndCap    ;

  double m_eCalToMipEndCap       ;
  double m_eCalToMipBarrel       ;
  double m_hCalToMipEndCapHEF    ;
  double m_hCalToMipEndCapHEB    ;
  double m_hCalToMipBarrel       ;

  double m_eCalToEMGeVEndCap     ;
  double m_eCalToEMGeVBarrel     ;
  double m_hCalToEMGeVEndCapHEF  ;
  double m_hCalToEMGeVEndCapHEB  ;
  double m_hCalToEMGeVBarrel     ;

  double m_eCalToHadGeVEndCap    ;
  double m_eCalToHadGeVBarrel    ;
  double m_hCalToHadGeVEndCapHEF ;
  double m_hCalToHadGeVEndCapHEB ;
  double m_hCalToHadGeVBarrel    ;

  double m_muonToMip             ;

  bool firstEvent_ ; 
  short _debugLevel;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//
