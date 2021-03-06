#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <TString.h>
#include <string>
#include <math.h>

#include <TROOT.h>
#include "TSystem.h"
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TNtuple.h>
#include <TMath.h>
#include <math.h>
#include <TH1.h>
#include <TH2.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include "TClonesArray.h"
#include <TAxis.h>
#include <cmath>
#include <TLorentzRotation.h>

#include <TCanvas.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <TGraphErrors.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TRandom.h>

#include <RooFit.h>
#include <RooGlobalFunc.h>
#include <RooCategory.h>
#include <RooGenericPdf.h>
#include <RooFFTConvPdf.h>
#include <RooWorkspace.h>
#include <RooBinning.h>
#include <RooHistPdf.h>
#include <RooProdPdf.h>
#include <RooAddPdf.h>
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooHist.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <RooConstVar.h>
#include "../../../cutsAndBin.h"

using namespace std;
using namespace RooFit;

int draw_yield_rap(TString szAA = "PA", int states =1)
{
  TFile *wf = new TFile(Form("Yield_Rap_%s_%ds.root",szAA.Data(),states),"recreate");
  
  /////////////////////////////////////////////////////////
  //// set style
  /////////////////////////////////////////////////////////
  
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  gStyle->SetTitleFillColor(0);
  gStyle->SetStatColor(0);

  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetFrameLineColor(kBlack);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);

  gStyle->SetTextSize(0.04);
  gStyle->SetTextFont(42);
  gStyle->SetLabelFont(42,"xyz");
  gStyle->SetTitleFont(42,"xyz");
  gStyle->SetTitleSize(0.048,"xyz");
  gStyle->SetTitleOffset(1.6,"y"); // KYO for yield

  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.12) ; 
  gStyle->SetPadLeftMargin(0.16) ; // KYO for yield
  
  /////////////////////////////////////////////////////////
  //// binning setting
  /////////////////////////////////////////////////////////
  
  int nPtBins=0;
  int nYBins=0;
  double* ptBin;
  double* yBin;
  int nCentBins=0;
  double* centBin;
  double* nPart;  // In order from peripheral to central 
  double* nColl;  // In order from central to peripheral 
  double* TAA;
  if ( states == 1 ) { 
    nPtBins = nPtBins1s;    ptBin = ptBin1s;
    nYBins = nYBins1S;    yBin = yBin1S; 
    nCentBins = nCentBins1s;  centBin = centBin1s; nPart = nPart1s; nColl = nColl1s; TAA = TAA1s;
  }
  else if ( states == 2 ) { 
    nPtBins = nPtBins2s;    ptBin = ptBin2s;
    nYBins = nYBins2S;    yBin = yBin2S; 
    nCentBins = nCentBins2s;  centBin = centBin2s; nPart = nPart2s; nColl = nColl2s; TAA = TAA2s;
  }
  else if ( states == 3 ) { 
    nPtBins = nPtBins3s;    ptBin = ptBin3s;
    nYBins = nYBins3S;    yBin = yBin3S; 
    nCentBins = nCentBins3s;  centBin = centBin3s; nPart = nPart3s; nColl = nColl3s; TAA = TAA3s;
  }

  int tmpBin = nYBins;
  
  const int nBin = tmpBin; // number of bin 
  const int nArrNum = nBin+1; // number of array
  double *binArr = yBin; // array
  cout << "nBin = " << nBin << endl;

  /////////////////////////////////////////////////////////
  //// Open RooDataFile
  /////////////////////////////////////////////////////////
 
  //file and ws
  TFile *fileIn[nBin];
  RooWorkspace* ws[nBin];
  // parameters 
  double nSig1s[nBin];
  double nSig1sErr[nBin];
  double nSig2s[nBin];
  double nSig2sErr[nBin];
  double nSig3s[nBin];
  double nSig3sErr[nBin];
  double nBkg[nBin];
  double nBkgErr[nBin];
  
  for (int ib =0; ib < nBin; ib ++ ) {
    //// read files
    if (szAA == "PP" ) { fileIn[ib]= new TFile(Form("/home/deathold/work/CMS/analysis/Upsilon_RAA/upsilonRAA5TeV/fitResults/Final_NomResult_170124/PAS_fitresults_upsilon_DoubleCB_%s_DATA_pt0.0-30.0_y%.1f-%.1f_muPt4.0.root",szAA.Data(),binArr[ib],binArr[ib+1])); }
    else if (szAA == "PA" ) { fileIn[ib]= new TFile(Form("/home/deathold/work/CMS/analysis/Upsilon_RpA/UpsilonpPb5TeV/RpA5.02TeV/Fitting/AllParmFree_SingleMu2.4/FitResults/AllParmFree_fitresults_upsilon_DoubleCB_5TeV_%s_DATA_pt0.0-30.0_y%.2f-%.2f_muPt4.0.root",szAA.Data(),binArr[ib],binArr[ib+1])); }
    else { cout << " Error ::: Select among PP and AA" << endl; return 0; }
    //cout << ib << "th file = " << fileIn[ib]->GetName() << endl;
    //if (fileIn[ib]->IsZombie()) { cout << "CANNOT open data root file\n"; return 1; }
    fileIn[ib]->cd();
    ws[ib]= (RooWorkspace*)fileIn[ib]->Get("workspace");
    //ws[ib]->Print();
    //// get parameters
    //cout << ws[ib]->var("nSig1s")->getVal() << endl;
    nSig1s[ib]=ws[ib]->var("nSig1s")->getVal();
    nSig1sErr[ib]=ws[ib]->var("nSig1s")->getError();
    nSig2s[ib]=ws[ib]->var("nSig2s")->getVal();
    nSig2sErr[ib]=ws[ib]->var("nSig2s")->getError();
    nSig3s[ib]=ws[ib]->var("nSig3s")->getVal();
    nSig3sErr[ib]=ws[ib]->var("nSig3s")->getError();
    nBkg[ib]=ws[ib]->var("nBkg")->getVal();
    nBkgErr[ib]=ws[ib]->var("nBkg")->getError();
    //cout << ib << "th nSig1s = " << nSig1s[ib] << endl;
    //cout << ib << "th nSig2s = " << nSig2s[ib] << endl;
    //cout << ib << "th nSig3s = " << nSig3s[ib] << endl;
    //cout << ib << "th nBkg = " << nBkg[ib] << endl;
  }
 
  //// histogram
  TH1D* h1_nSig1s = new TH1D("h1_nSig1s","h1_nSig1s;|y|;events",nBin,binArr); 
  TH1D* h1_nSig2s = new TH1D("h1_nSig2s","h1_nSig2s;|y|;events",nBin,binArr); 
  TH1D* h1_nSig3s = new TH1D("h1_nSig3s","h1_nSig3s;|y|;events",nBin,binArr); 
  TH1D* h1_nBkg = new TH1D("h1_nBkg","h1_nBkg;|y|;events",nBin,binArr); 
  
  for (int ib =0; ib < nBin; ib ++ ) {
    h1_nSig1s->SetBinContent(ib+1,nSig1s[ib]);   
    h1_nSig1s->SetBinError(ib+1,nSig1sErr[ib]);   
    h1_nSig2s->SetBinContent(ib+1,nSig2s[ib]);   
    h1_nSig2s->SetBinError(ib+1,nSig2sErr[ib]);   
    h1_nSig3s->SetBinContent(ib+1,nSig3s[ib]);   
    h1_nSig3s->SetBinError(ib+1,nSig3sErr[ib]);   
    h1_nBkg->SetBinContent(ib+1,nBkg[ib]);   
    h1_nBkg->SetBinError(ib+1,nBkgErr[ib]);   
  }

  //// normalization
  h1_nSig1s->Scale(1,"width");
  h1_nSig2s->Scale(1,"width");
  h1_nSig3s->Scale(1,"width");
  h1_nBkg->Scale(1,"width");

  //// actual draw
  TLatex* latex = new TLatex();
  latex->SetNDC();
  latex->SetTextAlign(12);
  latex->SetTextSize(0.04);
 
  TCanvas* c_nSig1s = new TCanvas("c_nSig1s","c_nSig1s",600,600);
  c_nSig1s->cd();
  gPad->SetLogy(1);  // KTO for yield
  h1_nSig1s->GetXaxis()->CenterTitle(1);
  h1_nSig1s->GetYaxis()->CenterTitle(1);
  h1_nSig1s->GetYaxis()->SetRangeUser(30,300000);
  h1_nSig1s->SetMarkerColor(kRed);
  h1_nSig1s->SetLineColor(kRed);
  h1_nSig1s->SetMarkerStyle(kFullCircle);
  h1_nSig1s->Draw("pe");
  h1_nBkg->SetMarkerColor(kBlue);
  h1_nBkg->SetLineColor(kBlue);
  h1_nBkg->SetMarkerStyle(kOpenCircle);
  h1_nBkg->Draw("pe same");
  latex->SetTextColor(kBlack);
  latex->DrawLatex(0.55,0.85,Form("%s",szAA.Data()));
  latex->SetTextColor(kRed);
  latex->DrawLatex(0.55,0.79,Form("#Upsilon(%dS) yields",states));
  latex->SetTextColor(kBlue);
  latex->DrawLatex(0.55,0.73,"Backgrounds");
  
  c_nSig1s->SaveAs(Form("yield/rap_nSig1s_%s_%dS.pdf",szAA.Data(),states));
  
  TCanvas* c_nSig2s = new TCanvas("c_nSig2s","c_nSig2s",600,600);
  c_nSig2s->cd();
//  gPad->SetLogy(1);  // KTO for yield
  h1_nSig2s->GetXaxis()->CenterTitle(1);
  h1_nSig2s->GetYaxis()->CenterTitle(1);
  if (szAA == "PP") h1_nSig2s->GetYaxis()->SetRangeUser(1,8000);
  else if (szAA == "AA") h1_nSig2s->GetYaxis()->SetRangeUser(1,800);
  h1_nSig2s->SetMarkerColor(kRed);
  h1_nSig2s->SetLineColor(kRed);
  h1_nSig2s->SetMarkerStyle(kFullCircle);
  h1_nSig2s->Draw("pe");
  latex->SetTextColor(kBlack);
  latex->DrawLatex(0.55,0.85,Form("%s",szAA.Data()));
  latex->SetTextColor(kRed);
  latex->DrawLatex(0.55,0.79,Form("#Upsilon(%dS) yields",states));
  c_nSig2s->SaveAs(Form("yield/rap_nSig2s_%s_%dS.pdf",szAA.Data(),states));
  
  TCanvas* c_nSig3s = new TCanvas("c_nSig3s","c_nSig3s",600,600);
  c_nSig3s->cd();
//  gPad->SetLogy(1);  // KTO for yield
  h1_nSig3s->GetXaxis()->CenterTitle(1);
  h1_nSig3s->GetYaxis()->CenterTitle(1);
//  h1_nSig3s->GetYaxis()->SetRangeUser(1,1000);
  if (szAA == "PP") h1_nSig3s->GetYaxis()->SetRangeUser(0,5000);
  else if (szAA == "AA") h1_nSig3s->GetYaxis()->SetRangeUser(0,100);
  h1_nSig3s->SetMarkerColor(kRed);
  h1_nSig3s->SetLineColor(kRed);
  h1_nSig3s->SetMarkerStyle(kFullCircle);
  h1_nSig3s->Draw("pe");
  latex->SetTextColor(kBlack);
  latex->DrawLatex(0.55,0.85,Form("%s",szAA.Data()));
  latex->SetTextColor(kRed);
  latex->DrawLatex(0.55,0.79,Form("#Upsilon(%dS) yields",states));
  c_nSig3s->SaveAs(Form("yield/rap_nSig3s_%s_%dS.pdf",szAA.Data(),states));
 
  wf->cd();

  h1_nBkg->Write();
  if(states==1) h1_nSig1s->Write();
  else if(states==2) h1_nSig2s->Write();
  else if(states==3) h1_nSig3s->Write();
  
 /* 
  TCanvas* c_nBkg = new TCanvas("c_nBkg","c_nBkg",600,600);
  c_nBkg->cd();
//  gPad->SetLogy(1);  // KTO for yield
  h1_nBkg->GetXaxis()->CenterTitle(1);
  h1_nBkg->GetYaxis()->CenterTitle(1);
  h1_nBkg->GetYaxis()->SetRangeUser(1,100000);
  h1_nBkg->SetMarkerColor(kRed);
  h1_nBkg->SetLineColor(kRed);
  h1_nBkg->SetMarkerStyle(kFullCircle);
  h1_nBkg->Draw("pe");
  latex->DrawLatex(0.45,0.83,Form("%s, binning for #Upsilon(%dS)",szAA.Data(),states));
  c_nBkg->SaveAs(Form("yield/rap_nBkg_%s_%dS.pdf",szAA.Data(),states));
*/

  return 0;

}

