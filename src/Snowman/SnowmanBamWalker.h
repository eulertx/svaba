#ifndef SNOWMAN_SNOWMAN_BAM_WALKER_H__
#define SNOWMAN_SNOWMAN_BAM_WALKER_H__

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

#include "KmerFilter.h"
#include "SnowTools/BamWalker.h"
#include "SnowTools/STCoverage.h"
#include "SnowTools/BWAWrapper.h"
#include "SnowTools/Fractions.h"
#include "DBSnpFilter.h"

#include "htslib/khash.h"

#define GET_COVERAGE 1

typedef std::unordered_map<std::string, size_t> CigarMap;
using SnowTools::GRC;
using SnowTools::BamReadVector;
using SnowTools::STCoverage;
using SnowTools::BamRead;

class MateRegion: public SnowTools::GenomicRegion
{
 public:
  MateRegion() {}
  MateRegion (int32_t c, uint32_t p1, uint32_t p2, char s = '*') : SnowTools::GenomicRegion(c, p1, p2, s) {}
  size_t count = 0;// read count
  SnowTools::GenomicRegion partner;

};

typedef SnowTools::GenomicRegionCollection<MateRegion> MateRegionVector;

class SnowmanBamWalker: public SnowTools::BamWalker {
  
 public:
  
  SnowmanBamWalker() {}

 SnowmanBamWalker(const std::string& in) : SnowTools::BamWalker(in) {}
  
  SnowmanBamWalker(const std::string& in, SnowTools::BWAWrapper * b, int rlen, 
		  const std::string p, const SnowTools::GRC& bl) : 
  SnowTools::BamWalker(in), main_bwa(b), readlen(rlen), prefix(p), blacklist(bl) {  }


  SnowTools::BWAWrapper * main_bwa = nullptr;
  int readlen;  
  std::string prefix; // eg. tumor, normal
  SnowTools::GRC blacklist;
  
  void readBam(std::ofstream* log = nullptr, const SnowTools::DBSnpFilter* dbs = nullptr);

  void realignDiscordants(SnowTools::BamReadVector& reads);
  
  void filterMicrobial(SnowTools::BWAWrapper * b);
  
  void KmerCorrect();
  
  bool hasAdapter(const BamRead& r) const;
  
  bool addCigar(BamRead &r, const SnowTools::DBSnpFilter* d = nullptr);
  
  bool isDuplicate(BamRead &r);
  
  void subSampleToWeirdCoverage(double max_coverage);
  
  void calculateMateRegions();
  
  void removeRepeats();
  
  bool get_coverage = true;
  bool get_mate_regions = true;
 
  
  //ReadVec reads;
  BamReadVector reads;
  
  std::unordered_map<uint32_t, size_t> cig_pos;

  STCoverage cov, weird_cov, bad_cov, clip_cov;
  
  CigarMap cigmap;




  
  //SnowTools::GenomicRegion coverage_region;
  
  size_t max_weird_cov = 100;
  
  MateRegionVector mate_regions;
  
  SnowTools::ReadCount rc;
  

  
  size_t max_cov = 100;
  
  bool do_kmer_filtering = true;
  
  bool adapter_trim = true;

  //std::stringstream cigr;
 private:
  
  // private string stream. Initialize once here for speed

  
  // might want these in case we are looking for duplicates
  //std::unordered_map<std::string, bool> name_map;
  //std::unordered_map<std::string, bool> seq_map;
  std::unordered_set<std::string> seq_set;
  //std::unordered_set<int> seq_set;
  
  size_t m_limit = 0;
  
  uint32_t m_seed = 1337;
  
};

#endif
