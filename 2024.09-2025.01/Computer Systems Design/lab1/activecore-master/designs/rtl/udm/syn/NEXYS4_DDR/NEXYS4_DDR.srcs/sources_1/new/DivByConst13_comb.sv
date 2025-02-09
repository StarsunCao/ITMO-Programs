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


module DivByConst13_comb (
    input  logic [15:0] dividend,  // 输入被除数
    output logic [15:0] quotient,  // 输出商
    output logic [15:0] remainder  // 输出余数
);

    logic [15:0] divword;  // 临时变量，用于逐步计算
    logic [3:0] divisor = 4'd13; // 除数13
    integer i;

    always_comb begin
        quotient = 0;                           // 初始化商为0
        divword = dividend >> 12;               // 初始化divword，右移 (dividend_size - divisor_size) = 16 - 4 = 12位

        for (i = 0; i <= 12; i++) begin         // 迭代13次 (dividend_size - divisor_size + 1)
            quotient = quotient << 1;           // 商左移一位
            if (divword >= divisor) begin
                divword = divword - divisor;    // 更新 divword
                quotient = quotient | 1;        // 商最低位设置为1
            end
            if (i != 12) begin                  // 如果不是最后一次迭代
                divword = (divword << 1) |      // divword 左移一位
                          ((dividend >> (11 - i)) & 1); // 提取 dividend 的第 11-i 位，并附加到 divword 的最低位
            end
        end

        remainder = divword;  // 最终的 divword 是余数
    end

endmodule




