`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/02/2026 05:30:53 PM
// Design Name: 
// Module Name: neuron
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


module neuron #(
    
    parameter CHUNK = 64,
    parameter COUNTWIDTH = 10
)(
    input logic [CHUNK-1:0] in,
    input logic [CHUNK-1:0] weights,

    input logic clk, clear, en,

    //2^10 1024, covers 784
    output logic [COUNTWIDTH-1:0] count
);

logic [CHUNK-1:0] xnor_wire; 
logic [$clog2(CHUNK+1)-1:0] ones; //upto 64 per fragment

assign xnor_wire = in ~^ weights;
assign ones = $countones(xnor_wire); //count matches

always_ff @(posedge clk) begin

    if (clear) begin //10, 11
        count <= 1'b0;
    end else if (en) begin //01
        count <= count + ones;
    end else //00
        count <= count; 
end
endmodule
