`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/03/2026 02:28:58 PM
// Design Name: 
// Module Name: layer
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


module layer#(

    parameter CHUNK = 64, //how big one chunk is
    parameter INPUTS_I = 784, //784, 256, 256
    parameter WIDTH_O = 256, //for each layer the output width changes
    localparam CHUNKS = (INPUTS_I + CHUNK -1) / CHUNK //how many chunks so 784 is 12

)(

    input logic clk, rst, start,
    input logic [INPUTS_I - 1: 0] input_bits,

    output logic [WIDTH_O - 1: 0] output_bits,
    output logic done
);

logic [CHUNKS*CHUNK-1:0] input_reg; 
logic last_neuron, last_chunk, enable_delay, clear;


logic [$clog2(INPUTS_I+1)-1: 0] count_t; //total neuron agreement count aggregate from the neuron module;
logic [$clog2(INPUTS_I + 1) -1:0] threshold [WIDTH_O - 1:0];
logic [$clog2(WIDTH_O)-1: 0] count_neurons;
logic [$clog2(CHUNKS)-1:0] curr_chunk;
logic [CHUNK-1:0] input_reg_chunk_delay;


logic [$clog2((WIDTH_O * CHUNKS)) - 1 : 0] addr;
neuron #(.CHUNK(CHUNK), .COUNTWIDTH($clog2(INPUTS_I))) neuron_n (.in(input_reg_chunk_delay), .weights(), .clk(clk), .clear(clear), .en(enable_delay), .count(count_t));


typedef enum logic [2:0] {

    IDLE, LOAD, CLEAR, RUN, WAIT, CHECK

} state_t;

state_t current_state, next_state;

assign last_neuron = (count_neurons == WIDTH_O-1);
assign clear  = (current_state == CLEAR);
assign last_chunk = (curr_chunk == CHUNKS - 1);

always_ff @(posedge clk) begin
    if (rst) begin
        current_state <= IDLE;
    end
    else
        current_state <= next_state;
end

always_comb begin
    next_state = current_state;
    case (current_state) 
        IDLE: 
            if (start) next_state <= LOAD;
        LOAD:
            next_state = CLEAR;
        CLEAR:
            next_state = RUN;
        RUN: begin
            if (last_chunk)
                next_state = WAIT;
        end 
        WAIT:
            //waits 1 clk cycle
            next_state = CHECK;
        CHECK: begin
            if(last_neuron)
                next_state = IDLE;
            else
                next_state = CLEAR;
        end
    endcase 
end


always_ff @(posedge clk) begin

    enable_delay <= (current_state == RUN);
    input_reg_chunk_delay <= (input_reg[curr_chunk*CHUNK+: CHUNK]);

end
always_ff @(posedge clk) begin

    case (current_state) 
        IDLE:
            //DO NOTHING OR MAKE IT RESET, BUT CLEAR SHOULD DO THAT 
            done <= 1'b0;
        LOAD: begin
            input_reg <= input_bits;
            count_neurons <= 1'b0;
        end 
        CLEAR:
            curr_chunk <= 0;
        RUN:
            curr_chunk <= curr_chunk + 1;
        WAIT:
            ;
        CHECK: begin
            //at that neuron agree or disagree for every neuron in a layer
            output_bits[count_neurons] <= (count_t >= threshold[count_neurons]);
            
            if (last_neuron)
                done <= 1'b1;
            else
                count_neurons <= count_neurons + 1;
        end
        default: ;
    endcase

end
endmodule
