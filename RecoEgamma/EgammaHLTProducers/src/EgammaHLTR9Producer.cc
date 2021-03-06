/** \class EgammaHLTR9Producer
 *
 *  \author Roberto Covarelli (CERN)
 *
 * $Id:
 *
 */

#include "RecoEgamma/EgammaHLTProducers/interface/EgammaHLTR9Producer.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateIsolation.h"

#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "RecoLocalCalo/EcalRecAlgos/interface/EcalSeverityLevelAlgo.h"

EgammaHLTR9Producer::EgammaHLTR9Producer(const edm::ParameterSet& config) : conf_(config)
{
 // use configuration file to setup input/output collection names
  recoEcalCandidateProducer_ = consumes<reco::RecoEcalCandidateCollection>(conf_.getParameter<edm::InputTag>("recoEcalCandidateProducer"));
  ecalRechitEBTag_ = conf_.getParameter< edm::InputTag > ("ecalRechitEB");
  ecalRechitEETag_ = conf_.getParameter< edm::InputTag > ("ecalRechitEE");
  ecalRechitEBToken_ = consumes<EcalRecHitCollection>(ecalRechitEBTag_);
  ecalRechitEEToken_ = consumes<EcalRecHitCollection>(ecalRechitEETag_);

  useSwissCross_   = conf_.getParameter< bool > ("useSwissCross");

  //register your products
  produces < reco::RecoEcalCandidateIsolationMap >();
}

EgammaHLTR9Producer::~EgammaHLTR9Producer()
{}

// ------------ method called to produce the data  ------------
void EgammaHLTR9Producer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  
  // Get the HLT filtered objects
  edm::Handle<reco::RecoEcalCandidateCollection> recoecalcandHandle;
  iEvent.getByToken(recoEcalCandidateProducer_,recoecalcandHandle);

  EcalClusterLazyTools lazyTools(iEvent, iSetup, ecalRechitEBTag_, ecalRechitEETag_);
  
  reco::RecoEcalCandidateIsolationMap r9Map;
   
  for(reco::RecoEcalCandidateCollection::const_iterator iRecoEcalCand = recoecalcandHandle->begin(); iRecoEcalCand != recoecalcandHandle->end(); iRecoEcalCand++){
    
    reco::RecoEcalCandidateRef recoecalcandref(recoecalcandHandle,iRecoEcalCand-recoecalcandHandle->begin());

    float r9 = -1;

    if (useSwissCross_){
      edm::Handle< EcalRecHitCollection > pEBRecHits;
      iEvent.getByToken(ecalRechitEBToken_, pEBRecHits);
      r9 = -1;
    }
    else{
    float e9 = lazyTools.e3x3( *(recoecalcandref->superCluster()->seed()) );
    if (e9 != 0 ) {r9 = lazyTools.eMax(*(recoecalcandref->superCluster()->seed())  )/e9;}
    }

    r9Map.insert(recoecalcandref, r9);
    
  }

  std::auto_ptr<reco::RecoEcalCandidateIsolationMap> R9Map(new reco::RecoEcalCandidateIsolationMap(r9Map));
  iEvent.put(R9Map);

}
