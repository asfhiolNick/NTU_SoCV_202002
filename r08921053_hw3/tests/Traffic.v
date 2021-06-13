module Traffic_Light (reset, clk, time_left, p1, p2, p3);
input reset, clk;
output [7:0] time_left;
output p1, p2, p3;

parameter RED_LIGHT    = 2'd0;
parameter GREEN_LIGHT  = 2'd1;
parameter YELLOW_LIGHT = 2'd2;

wire [5:0] RED_count;
wire [5:0] GREEN_count;
wire [2:0] YELLOW_count;
reg  [1:0] Light_Sign;
reg  [7:0] Counter;

assign RED_count    = 6'd60;
assign GREEN_count  = 6'd40;
assign YELLOW_count = 3'd5;

assign p1 = (time_left == 8'd255);
assign p2 = (Light_Sign == YELLOW_LIGHT);
assign p3 = !((Light_Sign == RED_LIGHT) || (Light_Sign == GREEN_LIGHT) || (Light_Sign == YELLOW_LIGHT));

assign time_left = Counter;

always @(posedge clk) begin
   if (!reset) begin
      Light_Sign <= RED_LIGHT;
      Counter <= 8'd0;
   end
   else begin
      case (Light_Sign)
         RED_LIGHT :
            Light_Sign <= (Counter == 8'd0) ? GREEN_LIGHT : RED_LIGHT;
         GREEN_LIGHT : 
            Light_Sign <= (Counter == 8'd0) ? YELLOW_LIGHT : GREEN_LIGHT;
         YELLOW_LIGHT : 
            Light_Sign <= (Counter == 8'd0) ? RED_LIGHT : YELLOW_LIGHT;
      endcase
      case (Light_Sign)
         RED_LIGHT :
            Counter <= (Counter == 8'd0) ? GREEN_count : Counter - 8'd1;
         GREEN_LIGHT : 
            Counter <= (Counter == 8'd0) ? YELLOW_count : Counter - 8'd1;
         YELLOW_LIGHT : 
            Counter <= (Counter == 8'd0) ? RED_count : Counter - 8'd1;
      endcase
   end
end
endmodule

