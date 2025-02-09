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


module NEXYS4_DDR #(parameter SIM = "NO") (
    input  CLK100MHZ,
    input  CPU_RESETN,
    input  [15:0] SW,
    output logic [15:0] LED,
    input  UART_TXD_IN,
    output UART_RXD_OUT
);

    // Parameters
    localparam UDM_BUS_TIMEOUT = (SIM == "YES") ? 100 : (1024*1024*100);
    localparam UDM_RTX_EXTERNAL_OVERRIDE = (SIM == "YES") ? "YES" : "NO";

    // Clocking and reset
    logic clk_gen, pll_locked, arst, srst, udm_reset;
    sys_clk sys_clk_inst (
        .clk_in1(CLK100MHZ),
        .reset(!CPU_RESETN),
        .clk_out1(clk_gen),
        .locked(pll_locked)
    );

    assign arst = !(CPU_RESETN & pll_locked);

    reset_cntrl reset_cntrl_inst (
        .clk_i(clk_gen),
        .arst_i(arst),
        .srst_o(srst)
    );

    // UDM Bus
    logic [31:0] udm_addr, udm_wdata, udm_rdata;
    logic [3:0] udm_be;
    logic udm_req, udm_we, udm_ack, udm_resp;

    udm #( 
        .BUS_TIMEOUT(UDM_BUS_TIMEOUT),
        .RTX_EXTERNAL_OVERRIDE(UDM_RTX_EXTERNAL_OVERRIDE)
    ) udm(
        .clk_i(clk_gen),
        .rst_i(srst),
        .rx_i(UART_TXD_IN),
        .tx_o(UART_RXD_OUT),
        .rst_o(udm_reset),
        .bus_req_o(udm_req),
        .bus_we_o(udm_we),
        .bus_addr_bo(udm_addr),
        .bus_be_bo(udm_be),
        .bus_wdata_bo(udm_wdata),
        .bus_ack_i(udm_ack),
        .bus_resp_i(udm_resp),
        .bus_rdata_bi(udm_rdata)
    );

    // Divider module instantiation
    logic [15:0] dividend, quotient, remainder;
    DivByConst13_comb divider_inst (
        .dividend(dividend),
        .quotient(quotient),
        .remainder(remainder)
    );

    // Address Map
    localparam CSR_DIVIDEND_ADDR  = 32'h10000000;
    localparam CSR_QUOTIENT_ADDR  = 32'h20000000;
    localparam CSR_REMAINDER_ADDR = 32'h20000004;

    // CSR handling and data transfer
    always @(posedge clk_gen) begin
        if (srst) begin
            dividend <= 16'b0;
        end else if (udm_req && udm_ack) begin
            if (udm_we) begin
                if (udm_addr == CSR_DIVIDEND_ADDR) dividend <= udm_wdata[15:0];
            end
        end
    end

    always_comb begin
        udm_ack = udm_req; // Always acknowledge requests
        udm_resp = 1'b0;
        udm_rdata = 32'b0;

        if (udm_req && !udm_we) begin
            case (udm_addr)
                CSR_QUOTIENT_ADDR: begin
                    udm_resp = 1'b1;
                    udm_rdata = {16'b0, quotient};
                end
                CSR_REMAINDER_ADDR: begin
                    udm_resp = 1'b1;
                    udm_rdata = {16'b0, remainder};
                end
            endcase
        end
    end

endmodule

