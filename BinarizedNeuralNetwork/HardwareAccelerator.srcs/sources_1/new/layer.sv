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

    //for Layer 3
    parameter ARGMAX = 0, //for layer 3 0-9 argmax. But not on layer 1-2.

    //Files
    parameter WEIGHT_FILE = "layer1_weights.mem",
    parameter THRESH_FILE = "layer1_thresh.mem"

)(

    input logic clk, rst, start,
    input logic [INPUTS_I - 1: 0] input_bits,

    output logic [$clog2(WIDTH_O)-1:0] digit, //ideally at layer 3, 10 neurons, digits 0-9.
    output logic [WIDTH_O - 1: 0] output_bits,
    output logic done
);

//vars 
localparam CHUNKS   = (INPUTS_I + CHUNK - 1) / CHUNK;  
localparam DEPTH    = WIDTH_O * CHUNKS;                // BRAM rows
localparam COUNT_W  = $clog2(INPUTS_I + 1);            // total count
localparam NEURON_W = $clog2(WIDTH_O);                 // neuron counter
localparam CHUNK_W  = $clog2(CHUNKS);                  // chunk counter
localparam ADDR_W   = $clog2(DEPTH);                   // BRAM address


// SIGNAL WIRES
logic [CHUNKS*CHUNK-1:0] input_reg;
logic [CHUNK-1:0]        input_chunk_d;   // delayed input chunk
logic [CHUNK-1:0]        weights_chunk;   // from BRAM
logic [COUNT_W-1:0]      count_t;
logic [COUNT_W-1:0]      threshold [WIDTH_O];
logic [NEURON_W-1:0]     count_neurons;
logic [CHUNK_W-1:0]      curr_chunk;
logic [ADDR_W-1:0]       addr;
logic [COUNT_W - 1:0]    best_count; //for layer 3
logic last_neuron, last_chunk, enable_delay, clear;

//Weights
initial if (!ARGMAX) $readmemh(THRESH_FILE, threshold); //layer 3 no thresh.

assign addr = count_neurons * CHUNKS + curr_chunk;

weight_mem #(.WIDTH(CHUNK), .DEPTH(DEPTH), .FILE(WEIGHT_FILE)) wmem (
    .clk(clk), .addr(addr), .data(weights_chunk)
);

neuron #(.CHUNK(CHUNK), .COUNTWIDTH(COUNT_W)) neuron_n (
    .clk(clk), .clear(clear), .en(enable_delay),
    .in(input_chunk_d), .weights(weights_chunk), .count(count_t)
);

//State machine logic
typedef enum logic [2:0] { IDLE, LOAD, CLEAR, RUN, WAIT, CHECK } state_t;
state_t current_state, next_state;

assign last_neuron = (count_neurons == WIDTH_O-1);
assign clear  = (current_state == CLEAR);
assign last_chunk = (curr_chunk == CHUNKS - 1);

always_ff @(posedge clk) begin
    if (rst) current_state <= IDLE;
    else current_state <= next_state;
end

always_comb begin
    next_state = current_state;
    case (current_state) 
        IDLE: if (start) next_state = LOAD;
        LOAD: next_state = CLEAR;
        CLEAR: next_state = RUN;
        RUN: if (last_chunk) next_state = WAIT;
        WAIT: next_state = CHECK;
        CHECK: begin
            if(last_neuron)
                next_state = IDLE;
            else
                next_state = CLEAR;
        end
        default: next_state = IDLE;
    endcase 
end

//Delaying input reg and also enable because BRAM is clocked 1 cycle.
always_ff @(posedge clk) begin
    enable_delay <= (current_state == RUN);
    input_chunk_d <= input_reg[curr_chunk*CHUNK +: CHUNK];
end

always_ff @(posedge clk) begin
    if (rst) begin
        count_neurons <= 1'b0;
        curr_chunk <= 1'b0;
        done <= 1'b0;
    end else begin
        case (current_state) 
            IDLE: done <= 1'b0;
            LOAD: begin
                input_reg <= input_bits;
                count_neurons <= 1'b0;
                best_count <= 0;
                digit <= 0;
            end 
            CLEAR: curr_chunk <= 0;
            RUN: curr_chunk <= curr_chunk + 1;
            WAIT: ;
            CHECK: begin
                if (ARGMAX) begin
                    if (count_t > best_count) begin
                        best_count <= count_t; //new count
                        digit <= count_neurons; //and the neuron were on currnetly is the best now.
                    end
                end else begin
                    //at that neuron agree or disagree for every neuron in a layer
                    output_bits[count_neurons] <= (count_t >= threshold[count_neurons]);
                end

                if (last_neuron) done <= 1'b1;
                else count_neurons <= count_neurons + 1;
            end
            default: ;
        endcase
    end
end

endmodule

