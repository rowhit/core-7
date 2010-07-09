// Title:
//           AMBA APB Slave Device Adapter from CT to RTL
// ScssId:
// Origin:
// Author:   Rolf Meyer <rmeyer@ida.ing.tu-bs.de>
// Reviewed: Not Reviewd now
// Purpose:
// License:  GPL
//
#ifndef APB_CT_RTL_ADAPTER
#define APB_CT_RTL_ADAPTER

#include "adapters/APB_CT_RTL_Slave_Adapter.h"
class APB_CT_RTL : public sc_module {
  public:
    class ct : public APB_CT_RTL_Slave_Adapter<ct, 32>, public amba_slave_base {
      public:
        ct(sc_core::sc_module_name nm, sc_dt::uint64 base, sc_dt::uint64 size)
          : APB_CT_RTL_Slave_Adapter<ct, 32>(nm), m_base(base), m_size(size) {}

        sc_dt::uint64 get_base_addr()     { return m_base; }
        sc_dt::uint64 get_size()          { return m_size; }
      private:
        sc_dt::uint64                       m_base;
        sc_dt::uint64                       m_size;
    };

    SC_HAS_PROCESS(APB_CT_RTL);
    sc_core::sc_in_clk                      clk;
    sc_core::sc_out<bool>                   reset;

    sc_core::sc_in<apb_slv_out_type>        apbo;
    sc_core::sc_out<apb_slv_in_type>        apbi;

    sc_core::sc_in<sc_uint<32> >            pirqi;
    sc_core::sc_out<sc_uint<32> >           pirqo;
    sc_core::sc_out<sc_uint<32> >           pconfig_0;
    sc_core::sc_out<sc_uint<32> >           pconfig_1;
    sc_core::sc_out<sc_uint<16> >           pindex;

  private:
    sc_core::sc_signal<bool >               m_psel;
    sc_core::sc_signal<bool >               m_penable;
    sc_core::sc_signal<bool >               m_pwrite;
    sc_core::sc_signal<sc_dt::sc_uint<32> > m_paddr;
    sc_core::sc_signal<sc_dt::sc_uint<32> > m_pwdata;
    sc_core::sc_signal<bool >               m_pready;
    sc_core::sc_signal<sc_dt::sc_uint<32> > m_prdata;
    sc_core::sc_signal<bool >               m_pslverr;
  public:
    ct ct;

  public:
    APB_CT_RTL(sc_core::sc_module_name nm, sc_dt::uint64 base, sc_dt::uint64 size)
      : sc_module(nm), clk("CLOCK"), reset("RESET"), apbo("apbo"), apbi("apbi"),
        pirqi("GR_IRQ_IN"), pirqo("GR_IRQ_OUT"), pconfig_0("GR_CONFIG_0"),
        pconfig_1("GR_CONFIG_1"), pindex("GR_INDEX"), m_psel("APB_SELECT"),
        m_penable("APB_ENABLE"), m_pwrite("APB_WRITE"), m_paddr("APB_ADDRESS"),
        m_pwdata("APB_WRITE_DATA"), m_pready("APB_READY"), m_prdata("APB_READ_DATA"),
        m_pslverr("APB_SLAVE_ERROR"), ct("CT", base, size) {
      ct.m_clk(clk);
      ct.m_Reset(reset);
      ct.psel(m_psel);
      ct.penable(m_penable);
      ct.pwrite(m_pwrite);
      ct.paddr(m_paddr);
      ct.pwdata(m_pwdata);
      ct.pready(m_pready);   m_pready = 1;
      ct.prdata(m_prdata);
      ct.pslverr(m_pslverr); m_pslverr = 0;

      SC_THREAD(apbo_ctrl);
      sensitive << apbo;
      SC_THREAD(apbi_ctrl);
      sensitive << m_psel << m_penable << m_paddr << m_pwrite << m_pwdata << pirqi;
    }

    void apbo_ctrl() {
      while(1) {
        apb_slv_out_type val = apbo.read();
        m_prdata.write(val.prdata);
        pirqo.write(val.pirq);
        pconfig_0.write(val.pconfig[0]);
        pconfig_1.write(val.pconfig[1]);
        pindex.write(val.pindex);
        wait();
      }
    }

    void apbi_ctrl() {
      while(1) {
        apb_slv_in_type val;
        val.psel = ((bool)m_psel.read())? 0xFFFF : 0x0;
        val.penable = m_penable.read();
        val.paddr = m_paddr.read();
        val.pwrite = m_pwrite.read();
        val.pwdata = m_pwdata.read();
        val.pirq = pirqi.read();
        val.testen = false;
        val.scanen = false;
        val.testoen = false;
        apbi.write(val);
        wait();
      }
    }
};

#endif
