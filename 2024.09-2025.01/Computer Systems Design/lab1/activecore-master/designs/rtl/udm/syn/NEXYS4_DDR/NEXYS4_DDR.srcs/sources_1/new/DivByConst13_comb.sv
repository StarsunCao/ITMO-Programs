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
    input  logic [15:0] dividend,  // ���뱻����
    output logic [15:0] quotient,  // �����
    output logic [15:0] remainder  // �������
);

    logic [15:0] divword;  // ��ʱ�����������𲽼���
    logic [3:0] divisor = 4'd13; // ����13
    integer i;

    always_comb begin
        quotient = 0;                           // ��ʼ����Ϊ0
        divword = dividend >> 12;               // ��ʼ��divword������ (dividend_size - divisor_size) = 16 - 4 = 12λ

        for (i = 0; i <= 12; i++) begin         // ����13�� (dividend_size - divisor_size + 1)
            quotient = quotient << 1;           // ������һλ
            if (divword >= divisor) begin
                divword = divword - divisor;    // ���� divword
                quotient = quotient | 1;        // �����λ����Ϊ1
            end
            if (i != 12) begin                  // ����������һ�ε���
                divword = (divword << 1) |      // divword ����һλ
                          ((dividend >> (11 - i)) & 1); // ��ȡ dividend �ĵ� 11-i λ�������ӵ� divword �����λ
            end
        end

        remainder = divword;  // ���յ� divword ������
    end

endmodule




