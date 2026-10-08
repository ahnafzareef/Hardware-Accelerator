`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/08/2026 11:00:16 AM
// Design Name: 
// Module Name: tb_neuron
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


module tb_neuron;

localparam CHUNK = 64;
localparam COUNTWIDTH = 10;

logic [CHUNK-1:0] in;
logic [CHUNK-1:0] weights;
logic [COUNTWIDTH-1:0] count;

logic clk = 0, clear = 0, en = 0;

integer expected = 0, errors = 0; 


neuron #(.CHUNK(CHUNK), .COUNTWIDTH(COUNTWIDTH)) uut (
    .in(in), .weights(weights), .clk(clk), .clear(clear), 
    .en(en), .count(count)
);

//100 MHZ clock -> 10 ns period
always #5 clk = ~clk; //5 on 5 off, 10ns period.

task step(input c, input e, input logic [CHUNK-1:0] i, input logic [CHUNK-1:0] w);   
    //feeding the values for certain at negedge
    @(negedge clk);
    clear = c;
    en = e;
    in = i;
    weights = w;
   
    @(posedge clk); //this is when neuron does its stuff


    //if clear then 0, if en then add num matching bits
    if (c) expected = 0;
    else if (e) expected += $countones(~(i ^ w));


    //compare, after running step times, uut has output count and we have value expected, compare the two.
    //above they take 0 time, so wait #1 before reading count

    #1;
    if (count !== expected) begin
        errors++;
        $display("Error on %0t, count = %0d, expected = %0d", $time, count, expected);
    end
endtask

initial begin

    




    if (errors == 0) $display("No Errors!");
    else $display("There are %0d errors", errors);
    $finish;
end




endmodule
