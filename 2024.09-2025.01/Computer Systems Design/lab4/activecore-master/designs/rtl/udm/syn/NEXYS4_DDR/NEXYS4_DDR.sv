/*
 * NEXYS4_DDR.sv
 *
 *  Created on: 01.01.2020
 *      Author: Alexander Antonov <antonov.alex.alex@gmail.com>
 *     License: See LICENSE file for details
 */
 
interface MemSplit32 ();
    logic req;
    logic ack;
    logic [31:0] addr;
    logic we;
    logic [31:0] wdata;
    logic [3:0] be;
    logic resp;
    logic [31:0] rdata;

    modport Master  (output req, input ack, output addr, output we, output wdata, output be, input resp, input rdata);
    modport Slave   (input req, output ack, input addr, input we, input wdata, input be, output resp, output rdata);
endinterface


module NEXYS4_DDR
#( parameter SIM = "NO" )
(
    input 	CLK100MHZ
    , input   CPU_RESETN
    
    , input   [15:0] SW
    , output logic  [15:0] LED

    , input   UART_TXD_IN
    , output  UART_RXD_OUT
);

localparam UDM_BUS_TIMEOUT = (SIM == "YES") ? 100 : (1024*1024*100);
localparam UDM_RTX_EXTERNAL_OVERRIDE = (SIM == "YES") ? "YES" : "NO";

logic clk_gen;
logic pll_locked;

sys_clk sys_clk
(
    .clk_in1(CLK100MHZ)
    , .reset(!CPU_RESETN)
    , .clk_out1(clk_gen)
    , .locked(pll_locked)
);

logic arst;
assign arst = !(CPU_RESETN & pll_locked);

logic srst;
reset_sync reset_sync
(
    .clk_i(clk_gen),
    .arst_i(arst),
    .srst_o(srst)
);

logic udm_reset;

MemSplit32 udm_bus();

udm
#(
    .BUS_TIMEOUT(UDM_BUS_TIMEOUT)
    , .RTX_EXTERNAL_OVERRIDE(UDM_RTX_EXTERNAL_OVERRIDE)
) udm (
    .clk_i(clk_gen)
    , .rst_i(srst)

    , .rx_i(UART_TXD_IN)
    , .tx_o(UART_RXD_OUT)

    , .rst_o(udm_reset)
    
    , .bus_req_o(udm_bus.req)
    , .bus_we_o(udm_bus.we)
    , .bus_addr_bo(udm_bus.addr)
    , .bus_be_bo(udm_bus.be)
    , .bus_wdata_bo(udm_bus.wdata)
    , .bus_ack_i(udm_bus.ack)
    , .bus_resp_i(udm_bus.resp)
    , .bus_rdata_bi(udm_bus.rdata)
);

localparam CSR_LED_ADDR         = 32'h00000000;
localparam CSR_SW_ADDR          = 32'h00000004;
localparam TESTMEM_ADDR         = 32'h80000000;

localparam TESTMEM_WSIZE_POW    = 10;
localparam TESTMEM_WSIZE        = 2**TESTMEM_WSIZE_POW;

logic udm_testmem_enb;
assign udm_testmem_enb = ((udm_bus.addr >= TESTMEM_ADDR) && (udm_bus.addr < (TESTMEM_ADDR + (TESTMEM_WSIZE*4))));

logic [31:0] udm_testmem_rdata;

logic testmem_p1_we;
logic [TESTMEM_WSIZE_POW-1:0] testmem_p1_addr;
logic [31:0] testmem_p1_wdata;
logic [31:0] testmem_p1_rdata;

assign testmem_p1_we = 1'b0;
assign testmem_p1_addr = 0;
assign testmem_p1_wdata = 0;

ram_dual #(
    .init_type("none")
    , .init_data("nodata.hex")
    , .dat_width(32)
    , .adr_width(TESTMEM_WSIZE_POW)
    , .mem_size(TESTMEM_WSIZE)
) testmem (
    .clk(clk_gen)

    , .dat0_i(udm_bus.wdata)
    , .adr0_i(udm_bus.addr[31:2])
    , .we0_i(udm_bus.req && udm_bus.we && udm_testmem_enb)
    , .dat0_o(udm_testmem_rdata)

    , .dat1_i(testmem_p1_wdata)
    , .adr1_i(testmem_p1_addr)
    , .we1_i(testmem_p1_we)
    , .dat1_o(testmem_p1_rdata) 
);

assign udm_bus.ack = udm_bus.req;
logic udm_csr_resp, udm_testmem_resp;
logic [31:0] udm_csr_rdata;

logic [31:0] div_dividend [9:0];
logic [31:0] div_quotient[9:0];
logic [31:0] div_remainder[9:0];
logic rst;
logic rdy;

//DivByConst13_pipeline div_inst (
////    .clk_i(clk_gen),
//    .rst_i(rst),
//    .rdy(rdy),
//    .dividend(div_dividend),
//    .quotient(div_quotient),
//    .remainder(div_remainder)
//);

logic hls_module_done;
div13 div_inst (
    .ap_clk(clk_gen), 
    .ap_rst(srst), 
    .ap_start(1'b1), 
    .ap_done(hls_module_done), 
//    .ap_idle(), 
//    .ap_ready(), 

    .x_0(div_dividend[0]),
    .x_1(div_dividend[1]),
    .x_2(div_dividend[2]),
    .x_3(div_dividend[3]),
    .x_4(div_dividend[4]),
    .x_5(div_dividend[5]),
    .x_6(div_dividend[6]),
    .x_7(div_dividend[7]),
    .x_8(div_dividend[8]),
    .x_9(div_dividend[9]),


    .quotient_0(div_quotient[0]),
    .quotient_1(div_quotient[1]),
    .quotient_2(div_quotient[2]),
    .quotient_3(div_quotient[3]),
    .quotient_4(div_quotient[4]),
    .quotient_5(div_quotient[5]),
    .quotient_6(div_quotient[6]),
    .quotient_7(div_quotient[7]),
    .quotient_8(div_quotient[8]),
    .quotient_9(div_quotient[9]),

    .remainder_0(div_remainder[0]),
    .remainder_1(div_remainder[1]),
    .remainder_2(div_remainder[2]),
    .remainder_3(div_remainder[3]),
    .remainder_4(div_remainder[4]),
    .remainder_5(div_remainder[5]),
    .remainder_6(div_remainder[6]),
    .remainder_7(div_remainder[7]),
    .remainder_8(div_remainder[8]),
    .remainder_9(div_remainder[9])
);

always @(posedge clk_gen) begin
    udm_csr_resp <= 1'b0;
    udm_testmem_resp <= 1'b0;
    
    if (srst) begin
        LED <= 16'hffff;
        rst <= 0;
    end
    
  if (srst)
        begin 
        for (int i=0; i<10; i++) 
            begin 
            div_dividend[i] <= 0; 
            end 
    end else begin
        if (udm_bus.req && udm_bus.ack) begin
            if (udm_bus.we) begin
                if (udm_bus.addr == CSR_LED_ADDR) LED <= udm_bus.wdata;
                     if (udm_bus.addr[31:28] == 4'h1) div_dividend[udm_bus.addr[5:2]] <= 
udm_bus.wdata; 
                   
            end else begin
            
                rst <= 1;
                if (udm_bus.addr == CSR_LED_ADDR) begin
                    udm_csr_resp <= 1'b1;
                    udm_csr_rdata <= LED;
                end
                if (udm_bus.addr == CSR_SW_ADDR) begin
                    udm_csr_resp <= 1'b1;
                    udm_csr_rdata <= SW;
                end
                if (udm_bus.addr >= 32'h20000000 && udm_bus.addr < 32'h20000000 + 4*10) begin
                    udm_csr_resp <= 1'b1;
                    udm_csr_rdata <= div_quotient[udm_bus.addr[5:2]]; 
                end
                if (udm_bus.addr >= 32'h20000100 && udm_bus.addr < 32'h20000100 + 4*10) begin
                    udm_csr_resp <= 1'b1;
                    udm_csr_rdata <= div_remainder[udm_bus.addr[5:2]];
                end
                udm_testmem_resp <= udm_testmem_enb;
            end
        end
    end
end

assign udm_bus.resp = udm_csr_resp | udm_testmem_resp;
assign udm_bus.rdata = (udm_csr_rdata & {32{udm_csr_resp}}) | (udm_testmem_rdata & {32{udm_testmem_resp}});

endmodule
