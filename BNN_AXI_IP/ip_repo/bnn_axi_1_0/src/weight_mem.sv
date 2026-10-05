`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/04/2026 07:39:59 AM
// Design Name: BRAM weight memory
// Module Name: weight_mem
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

module weight_mem #(parameter WIDTH = 64, parameter DEPTH = 64, parameter FILE = "weights.mem")(clk, addr, data);
   
    input logic clk;
    input logic [$clog2(DEPTH)-1:0] addr;
    output logic [WIDTH-1:0] data;

    (* rom_style = "block" *) reg [WIDTH-1:0] ram [DEPTH-1:0]; //force BRAM

    initial $readmemb(FILE, ram);
    
    always_ff @(posedge clk)
    begin
        data <= ram[addr];
    end 

endmodule
