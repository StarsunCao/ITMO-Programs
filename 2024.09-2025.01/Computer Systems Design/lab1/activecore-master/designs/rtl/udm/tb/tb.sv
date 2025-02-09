/*
 * tb.v
 *
 *  Created on: 17.10.2019
 *      Author: Alexander Antonov <antonov.alex.alex@gmail.com>
 *     License: See LICENSE file for details
 */

`timescale 1ns / 1ps

`define HALF_PERIOD          5                     // External 100 MHz
`define DIVIDER_115200       32'd8680
`define DIVIDER_19200        32'd52083
`define DIVIDER_9600         32'd104166
`define DIVIDER_4800         32'd208333
`define DIVIDER_2400         32'd416666

module tb ();

// Signal declarations
logic CLK_100MHZ, RST, rx;
logic [15:0] SW;
logic [15:0] LED;

always #`HALF_PERIOD CLK_100MHZ = ~CLK_100MHZ;

always #1000 SW = SW + 8'h1;

NEXYS4_DDR
#(
    .SIM("YES")
) DUT (
    .CLK100MHZ(CLK_100MHZ),
    .CPU_RESETN(!RST),
    .SW(SW),
    .LED(LED),
    .UART_TXD_IN(rx),
    .UART_RXD_OUT()
);

// Reset all
task RESET_ALL();
    begin
        CLK_100MHZ = 1'b0;
        RST = 1'b1;
        rx = 1'b1;
        #(`HALF_PERIOD/2);
        RST = 1;
        #(`HALF_PERIOD*6);
        RST = 0;
        while (DUT.srst) WAIT(10);
    end
endtask

// Wait for the specified periods
task WAIT(input logic [15:0] periods);
    begin
        integer i;
        for (i = 0; i < periods; i = i + 1)
            #(`HALF_PERIOD * 2);
    end
endtask

// UDM signal and driver
`define UDM_RX_SIGNAL rx
`define UDM_BLOCK DUT.udm
`include "udm.svh"
udm_driver udm = new();

// Main test procedure
localparam CSR_DIVIDEND_ADDR  = 32'h10000000; // Dividend input
localparam CSR_QUOTIENT_ADDR  = 32'h20000000; // Quotient output
localparam CSR_REMAINDER_ADDR = 32'h20000004; // Remainder output

integer dividend;  // Declare the dividend variable

initial
    begin
        $display("### SIMULATION STARTED ###");

        SW = 8'h30;
        RESET_ALL();
        WAIT(100);

        // Configure UDM
        udm.cfg(`DIVIDER_115200, 2'b00);
        udm.check();
        udm.hreset();
        WAIT(100);

        // Test Divider Module with different dividend values
        for (dividend = 0; dividend <= 900; dividend = dividend + 100) begin
            $display("-------- (Dividend = %0d) --------", dividend);
            
            // Write the dividend to the CSR register
            udm.wr32(CSR_DIVIDEND_ADDR, dividend);
            
            // Read quotient and remainder
            udm.rd32(CSR_QUOTIENT_ADDR);              // Read quotient
            udm.rd32(CSR_REMAINDER_ADDR);             // Read remainder

            // Wait for the results
            WAIT(100);
        end

        $display("### TEST PROCEDURE FINISHED ###");
        $stop;
    end
endmodule 
