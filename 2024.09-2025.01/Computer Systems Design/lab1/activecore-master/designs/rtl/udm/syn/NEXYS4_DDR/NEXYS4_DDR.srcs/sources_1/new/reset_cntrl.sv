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
    input  logic clk_i,      // ʱ������
    input  logic arst_i,     // �첽��λ����
    output logic srst_o      // ͬ����λ���
);

    logic [2:0] srst_reg;    // �����Ĵ�������ͬ����λ�ź�

    always @(posedge clk_i or posedge arst_i) begin
        if (arst_i) begin
            srst_reg <= 3'b111;  // ��λʱͬ����λ�ź���Ϊ��
        end else begin
            srst_reg <= {srst_reg[1:0], 1'b0};  // ��λ�߼�����ͬ����λ
        end
    end

    assign srst_o = srst_reg[2];  // ������һ���Ĵ�����Ϊͬ����λ�ź�

endmodule

