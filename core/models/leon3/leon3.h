// vim : set fileencoding=utf-8 expandtab noai ts=4 sw=4 :
/// @addtogroup mmu_cache
/// @{
/// @file mmu_cache.h
/// Class definition of LEON2/3 cache-subsystem consisting of instruction cache,
/// data cache, i/d localrams and memory management unit. The mmu_cache class
/// provides two TLM slave sockets for connecting the cpu and an AHB master
/// interface for connecting the processor bus.
///
/// @date 2010-2014
/// @copyright All rights reserved.
///            Any reproduction, use, distribution or disclosure of this
///            program, without the express, prior written consent of the 
///            authors is strictly prohibited.
/// @author Thomas Schuster
///

#ifndef Leon3_H_
#define Leon3_H_

#include "core/common/systemc.h"
#include "core/common/gs_config.h"
#include "core/common/amba.h"
//#include <tlm_1/tlm_req_rsp/tlm_channels/tlm_fifo/tlm_fifo.h>

#include <math.h>

#include "core/common/socrocket.h"
#include "core/common/signalkit.h"
#include "core/common/ahbmaster.h"
#include "core/common/clkdevice.h"

#include "core/common/verbose.h"
#include "core/models/leon3/mmucache/mmu_cache_base.h"
#include "core/models/leon3/mmucache/localram.h"

// LEON3
#include "core/models/leon3/intunit/processor.hpp"
#include "core/common/trapgen/debugger/GDBStub.hpp"
#include "core/common/trapgen/osEmulator/osEmulator.hpp"

/// @addtogroup mmu_cache MMU_Cache
/// @{


/// Top-level class of the memory sub-system for the TrapGen LEON3 simulator
class Leon3 :
  public mmu_cache_base,
  public leon3_funclt_trap::MemoryInterface {

  typedef leon3_funclt_trap::Processor_leon3_funclt LEON3;

 public:

  GC_HAS_CALLBACKS();
  SC_HAS_PROCESS(Leon3);
  SK_HAS_SIGNALS(Leon3);
  // TLM sockets
  // -----------

  /// @brief Constructor of the top-level class of the memory sub-system (caches and mmu).
  Leon3(
      ModuleName name = "",  ///< SystemC module name
      bool icen = true,                   ///< instruction cache enable
      uint32_t irepl = 1,                 ///< instruction cache replacement strategy
      uint32_t isets = 4,                 ///< number of instruction cache sets
      uint32_t ilinesize = 8,             ///< instruction cache line size (in bytes)
      uint32_t isetsize = 8,              ///< size of an instruction cache set (in kbytes)
      uint32_t isetlock = true,           ///< enable instruction cache locking
      uint32_t dcen = true,               ///< data cache enable
      uint32_t drepl = 1,                 ///< data cache replacement strategy
      uint32_t dsets = 2,                 ///< number of data cache sets
      uint32_t dlinesize = 4,             ///< data cache line size (in bytes)
      uint32_t dsetsize = 8,              ///< size of a data cache set (in kbytes)
      bool dsetlock = true,               ///< enable data cache locking
      bool dsnoop = true,                 ///< enable data cache snooping
      bool ilram = false,                 ///< enable instruction scratch pad
      uint32_t ilramsize = 0x000,         ///< size of the instruction scratch pad (in kbytes)
      uint32_t ilramstart = 0x000,        ///< start address of the instruction scratch pad
      uint32_t dlram = false,             ///< enable data scratch pad
      uint32_t dlramsize = 0x000,         ///< size of the data scratch pad (in kbytes)
      uint32_t dlramstart = 0x000,        ///< start address of the data scratch pad
      uint32_t cached = 0,                ///< fixed cacheability mask
      bool mmu_en = true,                 ///< mmu enable
      uint32_t itlb_num = 8,              ///< number of instruction TLBs
      uint32_t dtlb_num = 8,              ///< number of data TLBs
      uint32_t tlb_type = 0,              ///< split or shared instruction and data TLBs
      uint32_t tlb_rep = 1,               ///< TLB replacement strategy
      uint32_t mmupgsz = 0,               ///< MMU page size
      uint32_t hindex = 0,                ///< ID of the bus master
      bool pow_mon = false,               ///< Enable power monitoring
      AbstractionLayer ambaLayer = amba::amba_LT);  ///< Select LT or AT abstraction

      // Destructor
      ~Leon3();
      void init_generics();
      virtual void clkcng();
      gs::cnf::callback_return_type g_gdb_callback(gs::gs_param_base& changed_param, gs::cnf::callback_type reason);
      gs::cnf::callback_return_type g_history_callback(gs::gs_param_base& changed_param, gs::cnf::callback_type reason);
      gs::cnf::callback_return_type g_osemu_callback(gs::gs_param_base& changed_param, gs::cnf::callback_type reason) ;
      gs::cnf::callback_return_type g_args_callback(gs::gs_param_base& changed_param, gs::cnf::callback_type reason);

      virtual sc_dt::uint64 read_dword( const unsigned int & address, const unsigned int asi, const unsigned int flush, const unsigned int lock ) throw();
      virtual unsigned int read_word( const unsigned int & address , const unsigned int asi, const unsigned int flush, const unsigned int lock ) throw();
      virtual unsigned short int read_half( const unsigned int & address, const unsigned int asi, const unsigned int flush, const unsigned int lock ) throw();
      virtual unsigned char read_byte( const unsigned int & address, const unsigned int asi, const unsigned int flush, const unsigned int lock ) throw();
      virtual unsigned int read_instr( const unsigned int & address, const unsigned int flush) throw();
      virtual sc_dt::uint64 read_dword_dbg( const unsigned int & address ) throw();
      virtual unsigned int read_word_dbg( const unsigned int & address ) throw();
      virtual unsigned short int read_half_dbg( const unsigned int & address ) throw();
      virtual unsigned char read_byte_dbg( const unsigned int & address ) throw();
      virtual void write_dword( const unsigned int & address, sc_dt::uint64 datum, const unsigned int asi, const unsigned int flush, const unsigned int lock ) throw();
      virtual void write_word( const unsigned int & address, unsigned int datum, const unsigned int asi, const unsigned int flush, const unsigned int lock ) throw();
      virtual void write_half( const unsigned int & address, unsigned short int datum, const unsigned int asi, const unsigned int flush, const unsigned int lock ) throw();
      virtual void write_byte( const unsigned int & address, unsigned char datum, const unsigned int asi, const unsigned int flush, const unsigned int lock ) throw();
      virtual void write_dword_dbg( const unsigned int & address, sc_dt::uint64 datum ) throw();
      virtual void write_word_dbg( const unsigned int & address, unsigned int datum ) throw();
      virtual void write_half_dbg( const unsigned int & address, unsigned short int datum ) throw();
      virtual void write_byte_dbg( const unsigned int & address, unsigned char datum ) throw();
      virtual void lock();
      virtual void unlock();
      virtual void trigger_exception(unsigned int exception);

    LEON3 cpu;
    GDBStub<uint32_t> *debugger;
    OSEmulator<uint32_t> *osEmu;

    gs::cnf::gs_config<int> g_gdb;
    gs::cnf::gs_config<std::string> g_history;
    gs::cnf::gs_config<std::string> g_osemu;
    /// icache enable
    gs::cnf::gs_config<bool> g_icen;
    gs::cnf::gs_config<uint32_t> g_irepl;
    gs::cnf::gs_config<uint32_t> g_isets;
    gs::cnf::gs_config<uint32_t> g_ilinesize;
    gs::cnf::gs_config<uint32_t> g_isetsize;
    gs::cnf::gs_config<bool> g_isetlock;
    /// dcache enabled
    gs::cnf::gs_config<bool> g_dcen;
    gs::cnf::gs_config<uint32_t> g_drepl;
    gs::cnf::gs_config<uint32_t> g_dsets;
    gs::cnf::gs_config<uint32_t> g_dlinesize;
    gs::cnf::gs_config<uint32_t> g_dsetsize;
    gs::cnf::gs_config<bool> g_dsetlock;
    gs::cnf::gs_config<bool> g_dsnoop;
    /// instruction scratchpad enabled
    gs::cnf::gs_config<bool> g_ilram;

    gs::cnf::gs_config<uint32_t> g_ilramsize;
    /// instruction scratchpad starting address
    gs::cnf::gs_config<uint32_t> g_ilramstart;
    /// data scratchpad enabled
    gs::cnf::gs_config<bool> g_dlram;
    gs::cnf::gs_config<uint32_t> g_dlramsize;
    /// data scratchpad starting address
    gs::cnf::gs_config<uint32_t> g_dlramstart;
    /// enables fixed cacheability mask
    gs::cnf::gs_config<uint32_t> g_cached;
    /// mmu enabled
    gs::cnf::gs_config<bool> g_mmu_en;
    gs::cnf::gs_config<uint32_t> g_itlb_num;
    gs::cnf::gs_config<uint32_t> g_dtlb_num;
    gs::cnf::gs_config<uint32_t> g_tlb_type;
    gs::cnf::gs_config<uint32_t> g_tlb_rep;
    gs::cnf::gs_config<uint32_t> g_mmupgsz;
    gs::cnf::gs_config<uint32_t> g_hindex;
    gs::cnf::gs_config<std::vector<std::string> > g_args;
};

#endif //__MMU_CACHE_H__
/// @}