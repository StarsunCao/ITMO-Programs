`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/11/28 01:25:28
// Design Name: 
// Module Name: DivByConst13_comb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module DivByConst13_pipeline (
    input  logic clk_i,
    input  logic rst_i,
    input  logic [15:0] dividend [9:0],  // 输入：10个被除数
    output logic [15:0] quotient [9:0], // 输出：10个商
    output logic [15:0] remainder [9:0], // 输出：10个余数
    output logic rdy = 0
);

    localparam DIVIDEND_SIZE = 16;
    localparam DIVISOR = 13;
    localparam DIVISOR_SIZE = 4;

    logic [15:0] divword;
    logic [15:0] temp_quotient;
    integer i;
    
    always @(posedge clk_i or posedge rst_i) begin
        if (rst_i) begin
            for (int n = 0; n < 10; n++) begin
                divword = dividend[n] >> (DIVIDEND_SIZE - DIVISOR_SIZE);
                temp_quotient = 0; 
                for (i = 0; i < (DIVIDEND_SIZE - DIVISOR_SIZE + 1); i++) begin
                    temp_quotient = temp_quotient << 1;
                    if (divword >= DIVISOR) begin
                        divword = divword - DIVISOR;
                        temp_quotient = temp_quotient | 1;
                    end
                    if (i != (DIVIDEND_SIZE - DIVISOR_SIZE)) begin
                        divword = divword << 1;
                        divword[0] = dividend[n][DIVIDEND_SIZE - DIVISOR_SIZE - 1 - i];
                    end
                end
                quotient[n] <= temp_quotient;
                remainder[n] <= divword;
            end
            rdy <= 1; 
        end else begin
            for (int n = 0; n < 10; n++) begin
                quotient[n] = 16'h0000;
                remainder[n] <= 16'h0000;
            end   
        end
    end
endmodule










