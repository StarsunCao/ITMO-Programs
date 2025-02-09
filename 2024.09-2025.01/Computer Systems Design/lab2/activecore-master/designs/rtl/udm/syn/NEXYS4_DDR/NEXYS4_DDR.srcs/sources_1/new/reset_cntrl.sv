`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/11/28 01:34:49
// Design Name: 
// Module Name: reset_cntrl
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


module reset_cntrl (
    input  logic clk_i,      // 时钟输入
    input  logic arst_i,     // 异步复位输入
    output logic srst_o      // 同步复位输出
);

    logic [2:0] srst_reg;    // 三个寄存器用于同步复位信号

    always @(posedge clk_i or posedge arst_i) begin
        if (arst_i) begin
            srst_reg <= 3'b111;  // 复位时同步复位信号设为高
        end else begin
            srst_reg <= {srst_reg[1:0], 1'b0};  // 移位逻辑产生同步复位
        end
    end

    assign srst_o = srst_reg[2];  // 输出最后一级寄存器作为同步复位信号

endmodule

