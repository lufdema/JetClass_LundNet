
//#include "classes/DelphesClasses.h"
//#include "external/ExRootAnalysis/ExRootTreeReader.h"
//#include "external/ExRootAnalysis/ExRootResult.h"


#include <iostream>
#include <fstream>
#include <sstream>
#include <omp.h> 
#include <vector>
#include <string>

#ifdef __MAKECINT__
#pragma link C++ class vector<vector<float> >+;
#endif

#include <regex>
#include <iterator>
#include <queue>

//The next libraries must be linked dependent of the location of root. In this case the root version used is the 6.32.08

#include "/home/felipe/root/include/TROOT.h"
#include "/home/felipe/root/include/TFile.h"
#include "/home/felipe/root/include/TTree.h"
#include "/home/felipe/root/include/TClonesArray.h"
#include "/home/felipe/root/include/TChain.h"
#include "/home/felipe/root/include/TObject.h"
#include "/home/felipe/root/include/TInterpreter.h"
#include "fastjet/PseudoJet.hh"
#include <string>
#include "LundGenerator.hh" // In external code, this should be fastjet/contrib/LundGenerator.hh
#include "LundJSON.hh"  // In external code, this should be fastjet/contrib/LundJSON.hh
using namespace std;
using namespace fastjet;
using std::begin;
using std::end;


// forward declaration to make things clearer


//-----------------------------------------------------------------------------------------------------------------------------------
void setBranches(vector<vector<contrib::LundDeclustering>>  &v_LundValues, TTree *Tree )
{
    gInterpreter->GenerateDictionary("vector <vector<int> >", "vector");
    gInterpreter->GenerateDictionary("vector <vector<float> >", "vector");
    gInterpreter->GenerateDictionary("vector <vector<double> >", "vector");
//-------------------------------------------------------------------------------------

  auto label = new int();
  
 

//-------------------------------------------------------------------------------------
// Definition of the varibles (the ones that going to be used in the train) and its branches


  std::vector<float>  m_massFromLJP;
  std::vector<float> m_jetLundPt1;
  std::vector<float> m_jetLundPt2 ;
  std::vector<float> m_jetLundDeltaR;
  std::vector<float> m_jetLundZ;
  std::vector<float> m_jetLundKt;
  std::vector<float> m_LRJ_m;
  std::vector<float> m_LRJ_m_jet;
  std::vector<float> m_LRJ_pt;
  std::vector<float> m_LRJ_pt_const;
  std::vector<int> m_jetLundIDParent1;
  std::vector<int> m_jetLundIDParent2;
 
  int LRJ_truthLabel;

    
    TBranch* bmfl= Tree->Branch("massFromLJP", &m_massFromLJP);
    TBranch* blp1=Tree->Branch("jetLundPt1", &m_jetLundPt1 );
    TBranch* blp2=Tree->Branch("jetLundPt2", &m_jetLundPt2);
    TBranch* bldr=Tree->Branch("jetLundDeltaR", &m_jetLundDeltaR);
    TBranch* blz=Tree->Branch("jetLundZ", &m_jetLundZ);
    TBranch* blkt=Tree->Branch("jetLundKt", &m_jetLundKt);
    TBranch* blm=Tree->Branch("m_LRJ_mass", &m_LRJ_m);
    TBranch* blm2=Tree->Branch("m_LRJ_mass_jet", &m_LRJ_m_jet);
    TBranch* blpt=Tree->Branch("m_LRJ_pt", &m_LRJ_pt);
    TBranch* blpt2=Tree->Branch("m_LRJ_pt_const", &m_LRJ_pt_const);
    TBranch* bidp1=Tree->Branch("jetLundIDParent1", &m_jetLundIDParent1);
    TBranch* bidp2=Tree->Branch("jetLundIDParent2", &m_jetLundIDParent2);

    TBranch* bltl=Tree->Branch("LRJ_truthLabel",&LRJ_truthLabel);
  

    
//--------------------------------------------------------------------------------------
 cout<<v_LundValues.size()<<endl;
 



for (unsigned int iJet = 0; iJet < v_LundValues.size(); iJet++) {
      Tree->GetEntry(iJet);



      std::vector<int> index_count_out;
      std::vector<int> selected_nodes_out;
      index_count_out.clear();
      selected_nodes_out.clear();
      
//The label for training, for now it going to be change manually depending if we have signal (1) eg: W,Top,Higgs, etc or background (0) 

      LRJ_truthLabel=1;
      
      for (unsigned int iLund = 0; iLund < 1; iLund++) {
      	if (v_LundValues.at(iJet).size()==0){
        	continue;
        	}
       	m_LRJ_pt.push_back(v_LundValues.at(iJet).at(iLund).pair().pt());
       	m_LRJ_m_jet.push_back(v_LundValues.at(iJet).at(iLund).pair().m());
     }

      //m_LRJ_pt.push_back(v_LundValues.at(iJet).at(iLund).pair().pt());
      
     
        
      for (unsigned int iLund = 0; iLund < v_LundValues.at(iJet).size(); iLund++) {
      
        
        m_jetLundPt1.push_back(v_LundValues.at(iJet).at(iLund).harder().pt());
        m_jetLundPt2.push_back(v_LundValues.at(iJet).at(iLund).softer().pt());
        m_jetLundDeltaR.push_back(v_LundValues.at(iJet).at(iLund).Delta());
        m_jetLundZ.push_back(v_LundValues.at(iJet).at(iLund).z());
        m_jetLundKt.push_back(v_LundValues.at(iJet).at(iLund).kt());
        m_LRJ_m.push_back(v_LundValues.at(iJet).at(iLund).pair().m());
        m_LRJ_pt_const.push_back(v_LundValues.at(iJet).at(iLund).pair().pt());
 
    

        

      }

       //--------------------------------------------------------------------------------
        for (unsigned int id=0; id < m_LRJ_pt_const.size(); id++) {

           int idp1 = -1;
           int idp2 = -1;

           auto itp1 = find(m_LRJ_pt_const.begin(), m_LRJ_pt_const.end(), m_jetLundPt1.at(id));
           auto itp2 = find(m_LRJ_pt_const.begin(), m_LRJ_pt_const.end(), m_jetLundPt2.at(id));

	   
           if (itp1 != m_LRJ_pt_const.end()) {
	     idp1 = itp1 - m_LRJ_pt_const.begin();
	    
             if (idp1 == id){ // in case second parent has a very very small pT        
	       idp1 = -1;
	     }

	     
           }
           
           if (itp2 != m_LRJ_pt_const.end()) {
	     idp2 = itp2 - m_LRJ_pt_const.begin();
           }
        
        m_jetLundIDParent1.push_back(idp1);
        m_jetLundIDParent2.push_back(idp2);
      }
	      
      //--------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


	

 	  
	
      
      
      

 
       
       bmfl->Fill(); 
       blp1->Fill(); 
       blp2->Fill(); 
       bldr->Fill(); 
       blz->Fill(); 
       blkt->Fill(); 
       blm->Fill(); 
       blm2->Fill(); 
       blpt->Fill(); 
       blpt2->Fill(); 
       bidp1->Fill(); 
       bidp2->Fill(); 
       bltl->Fill(); 
       m_massFromLJP.clear();
	m_jetLundPt1.clear();
	m_jetLundPt2.clear();
	m_jetLundDeltaR.clear();
	m_jetLundZ.clear();
	m_jetLundKt.clear();
	m_LRJ_m.clear();
	m_LRJ_m_jet.clear();
	m_LRJ_pt.clear();
	m_LRJ_pt_const.clear();
	m_jetLundIDParent1.clear();
	m_jetLundIDParent2.clear();

	

      ////////////////////////***********************************//////////////////////////////***************//////////////////////         
       
	
  
    }


        //for (int i = 0; i < 10; i++)
	//{
    	//for (int j = 0; j < m_jetLundPt1.at(i).size() ; j++)
    	//{
        //	cout<<"Jet_pt:"<<m_LRJ_pt.at(i).at(j)<<"pt_1:"<<m_jetLundPt1.at(i).at(j)<<"      "<<"pt_2:"<< m_jetLundPt2.at(i).at(j)<<endl;
    	//}
	//}

   Tree->Write("", TObject::kOverwrite);
}


//------------------------------------------------------------------------------

int main(){

  
  
  //string filename = "TTBar_000_to_maketrain.root";
  //string file_path("/home/felipe/fjcontrib-1.038/LundPlane/JC_json/");
  //file_path += filename.c_str();
  //ofstream outfile;
  //outfile.open(file_path);
  
  // opening the .root file and its trees
  
  string input_path;
  const char *inputFile;

  //For multiple file just put here the names and modify the for in the line 252 to run over all the files
   const char *inputfiles[]   = {"WToQQ_068.root", "WToQQ_069.root", "TTBar_002.root","TTBar_003.root","TTBar_004.root", "TTBar_005.root", "TTBar_006.root", "TTBar_007.root","TTBar_008.root", "TTBar_009.root"};
  
  
  for(int j=0; j<2; ++j)
  {
  
  inputFile = inputfiles[j];
  
  // Path of the .root files. In this case the program will rewrite the original file with all its varibles but adding
  // the news for the training
  
  input_path = "/home/felipe/fjcontrib-1.038/LundPlane/Training_Files/" ;
  input_path += inputFile;
  const char *file_to_open = input_path.c_str();
  TFile *fopen=new TFile(file_to_open, "UPDATE"); 
  TTree *tree=(TTree*)fopen->Get("tree"); 
  
  // Declaration of some varibles include the momentums of the contituents and it energy. Also the IsCharged to make the number 
  // of tracks for the training of the modified LundNet
  
  auto px = new std::vector<double>();
  auto py = new std::vector<double>();
  auto pz = new std::vector<double>();
  auto E = new std::vector<double>();
  auto IsCharged = new std::vector<float>();
  std::vector<int> m_LRJ_Nconst_Charged;
  
  cout<< inputFile << endl;
  //---------------------------------------------------------------------------------------------

  std::vector<vector<contrib::LundDeclustering>> v_LundValues;

   int ch=0;
  //vector<contrib::LundDeclustering> v_LundValues;
  //v_LundValues.clear();
  //---------------------------------------------------------------------------------------------
  
  int allEntries = tree->GetEntries();
  
  cout << "** Chain contains " << allEntries << " events" << endl;

  Long64_t entry;

  
  tree->SetBranchAddress("part_px", &px);
  tree->SetBranchAddress("part_py", &py);
  tree->SetBranchAddress("part_pz", &pz);
  tree->SetBranchAddress("part_energy", &E);
  tree->SetBranchAddress("part_charge", &IsCharged);
  TBranch* b1=tree->Branch("m_LRJ_Nconst_Charged", &m_LRJ_Nconst_Charged);
 
  
  // loop over the events in  the file to apply the anti-kt algorithm 
  
  
  for(entry = 0; entry <100000; ++entry)
  {
    // Load selected branches with data from specified event
    tree->GetEntry(entry);
    vector<PseudoJet> constituents;
    int size= px->size();
    int i;
    for (i=0; (i<size); ++i)
    {	
     
     constituents.push_back( PseudoJet( px[0][i], py[0][i], pz[0][i], E[0][i]));
     if (IsCharged[0][i] != 0){
       ch++;
       }
       
 
  }
  
    m_LRJ_Nconst_Charged.push_back(ch);  
    b1->Fill();
    ch=0;
    m_LRJ_Nconst_Charged.clear();
    // Cluster sequence with anti-kt in this case with R=1.0 but it could change since JetClass dataset is made with R+0.8. 
    // Also the ptmin in this case is 450GeV because the dataset have a cut in pt for large R jets with pt below 500GeV
  double R =1.0, ptmin = 500.0;
  JetDefinition jet_def(antikt_algorithm, R);
  ClusterSequence cs(constituents, jet_def);
  vector<PseudoJet> jets = sorted_by_pt(cs.inclusive_jets(ptmin));
  
  
  // De-cluster sequence with Cambridge Aachen
  contrib::LundGenerator lund;

//------------------------------------------------------------------------------------------------------------------- 

  
  for (unsigned ijet = 0; ijet < jets.size(); ijet++) {
    vector<contrib::LundDeclustering> lundvals = lund(jets[ijet]);
    v_LundValues.push_back(lundvals);
	
    }
//------------------------------------------------------------------------------------------------------------------- 

    }

 // Fill branches to update the files with the variables that eventually will be used to train the model
    setBranches(v_LundValues, tree);
    
    //tree->Fill();
    //tree->Write("", TObject::kOverwrite);
    //tree->Scan("jetLundPt1");
     //tree->Print();
     tree->Print();
     delete tree;
     delete fopen;
     
     }
   

  cout << "fin"<< endl ;
  
  return 0;
  

}

//------------------------------------------------------------------------------
